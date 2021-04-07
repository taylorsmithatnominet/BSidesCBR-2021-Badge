#include "simonsays.h"

#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeService.h>
#include <BadgeMenu.h>
#include <BSidesBadge2020.h>
#include <BadgeLog.h>
#include <BSidesMessaging.h>
#include <display.h>
#include <menu.h>

// application - used to register callbacks from badge OS.
BadgeApp simonsays_app;

// globals for simonsays app
// Reference to the serial bridge queue
extern SerialMessageQueue serialBridgeQueue;
// first run boolean
static bool first_loop = false;
// What level we're up to
static uint8_t level = 0;
static char levelText[256];
// Memory to hold the sequence of randomly-selected LED values
static uint8_t sequence[255] = {0};
static uint8_t printSequenceIdx = 0;
// message structure for sending
static SerialMessage msgToSend;

#define DELAY_REPEAT                 5000 // ms
#define DELAY_FLASH_ON                500 // ms
#define DELAY_BETWEEN_FLASHES         100 // ms
#define MAX_DELAY_BETWEEN_KEYPRESSES 5000 // ms
#define STANDARD_DELAY               1000 // ms
static int currTime = 0;
// Time the last event was fired
static int lastEventTime = 0;
// Time delta after which to fire the next event
static int nextEventTimeDelta = 0;
// Level event check for async capsense handling
static bool levelEventReached = false;
// Event callback
typedef void (*eventCallback)(void);
static eventCallback nextEvent = NULL;
// LED state
static RGBLEDState state;
#define LED_BRIGHTNESS 0x60

// PrintSequence variables
static bool printSequenceOn = false;
static bool canReceiveCapsense = false;

// Forward-declaration of level logic
void levelFail(void);
void levelPass(void);
void setLevelStart(void);
void flashLED(uint8_t idx);
void ledsOff(void);

// Convenience functions
void enableCapsenseFeedback(bool enable) {
    CapsenseFeedback feedback;
    feedback.enable = enable;
    if (!prepareMessage(msgToSend, CapsenseFeedbackEnable, nextSequenceNumber(), &feedback, sizeof(feedback))) {
        log(LOG_ERROR, "Failed to prepare feedback message from simonsays");
    }
    if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send feedback message from simonsays");
    }
}
void handleCapsense(uint8_t pad, bool pressed) {
    if (!canReceiveCapsense) {
        return;
    }
    // Record process time of event
    lastEventTime = millis();
    if (pressed) {
        // set up to fail if too long a delay
        nextEvent = levelFail;
        nextEventTimeDelta = MAX_DELAY_BETWEEN_KEYPRESSES;
        flashLED(pad);
        levelEventReached = false;
        if (pad == sequence[printSequenceIdx]) {
            printSequenceIdx++;
            if (printSequenceIdx > level) {
                levelEventReached = true;
                nextEvent = levelPass;
                nextEventTimeDelta = STANDARD_DELAY;
            }
        } else {
            log(LOG_DEBUG, "received pad %d, expected pad %d", pad, sequence[printSequenceIdx]);
            levelEventReached = true;
            nextEvent = levelFail;
            nextEventTimeDelta = STANDARD_DELAY;
            return;
        }
    } else {
        ledsOff();
        if (levelEventReached) {
            canReceiveCapsense = false;
        }
    }
}
void sendLEDMessage(const RGBLEDState &state) {
    log(LOG_DEBUG, "LEDs message");
    if (!prepareMessage(msgToSend, LEDSet, nextSequenceNumber(), &state, sizeof(state))) {
        log(LOG_ERROR, "Failed to prepare LED message from simonsays");
    }
    if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send LED message from simonsays");
    }
}
void ledsOff() {
    log(LOG_DEBUG, "LEDs off");
    // this will set LED structure to 0, forcing them to be disabled
    if (!prepareMessage(msgToSend, LEDSet, nextSequenceNumber(), NULL, 0)) {
        log(LOG_ERROR, "Failed to prepare LED message from simonsays");
    }
    if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send LED message from simonsays");
    }
}
void setLED(uint8_t idx, RGBLEDState &state) {
    switch (idx) {
        // Red
        case 0: state.red[idx] = 0xFF; break;
        // Green
        case 1: state.green[idx] = 0xFF; break;
        // Blue
        case 2: state.blue[idx] = 0xFF; break;
        // Yellow
        case 3: state.red[idx] = state.green[idx] = 0xFF; break;
        // Purple
        case 4: state.red[idx] = state.blue[idx] = 0xFF; break;
        // Cyan
        case 5: state.green[idx] = state.blue[idx] = 0xFF; break;
        // Orange
        case 6: state.red[idx] = 0xFF; state.green[idx] = 0x60; break;
        // White
        case 7: state.red[idx] = state.green[idx] = 0xA0; state.blue[idx] = 0xFF; break;
        // No LEDs are indexable past here
        default: log(LOG_ERROR, "Invalid index for LED"); return;
    }
    state.brightness[idx] = LED_BRIGHTNESS;
}
void ledsOn() {
    memset(&state, 0, sizeof(state));
    for (uint8_t i = 0; i < 8; i++) {
        setLED(i, state);
    }
    sendLEDMessage(state);
}
void passLEDs() {
    memset(&state, 0, sizeof(state));
    for (uint8_t i = 0; i < 8; i++) {
        state.brightness[i] = LED_BRIGHTNESS;
        state.green[i] = 0xFF;
    }
    sendLEDMessage(state);
}
void failLEDs() {
    memset(&state, 0, sizeof(state));
    for (uint8_t i = 0; i < 8; i++) {
        state.brightness[i] = LED_BRIGHTNESS;
        state.red[i] = 0xFF;
    }
    sendLEDMessage(state);
}
void setLevelText() {
    display.fillScreen(DISPLAY_COLOR_BLACK);
    size_t y = display_draw_status_bar();
    display.drawRect(2, y + 2, display.width() - 4, display.height()  - y - 4, DISPLAY_COLOR_RED);
    display.drawRect(16, y + 16, display.width()  - 32, display.height() - y - 32, DISPLAY_COLOR_WHITE);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(20, 40);
    display.setTextSize(1, 1);
    snprintf(levelText, 255, "Round %d", level + 1);
    display.print(levelText);
    size_t offset = 60;
    display.setCursor(28, offset); offset += 8;
    display.print("Wait for the"); display.setCursor(28, offset); offset += 8;
    display.print("pattern to"); display.setCursor(28, offset); offset += 16;
    display.print("complete!"); display.setCursor(28, offset); offset += 8;
    display.print("Then try to"); display.setCursor(28, offset); offset += 16;
    display.print("repeat it!"); display.setCursor(28, offset); offset += 8;
    display.print("The LEDs"); display.setCursor(28, offset); offset += 8;
    display.print("will flash"); display.setCursor(28, offset); offset += 8;
    display.print("green on"); display.setCursor(28, offset); offset += 8;
    display.print("success, or"); display.setCursor(28, offset); offset += 8;
    display.print("red if you"); display.setCursor(28, offset); offset += 8;
    display.print("make a"); display.setCursor(28, offset); offset += 16;
    display.print("mistake"); display.setCursor(28, offset); offset += 8;
    display.print("Press LEFT"); display.setCursor(28, offset); offset += 8;
    display.print("to exit at"); display.setCursor(28, offset); offset += 8;
    display.print("any time"); display.setCursor(28, offset); offset += 8;
    display.display();
}
void flashLED(uint8_t idx) {
    memset(&state, 0, sizeof(state));
    setLED(idx, state);
    sendLEDMessage(state);
}
void printSequence() {
    log(LOG_DEBUG, "print sequence (%s)", printSequenceOn? "ON" : "OFF");
    if (printSequenceIdx > level) {
        ledsOff();
        printSequenceIdx = 0;
        canReceiveCapsense = true;
        // Repeat sequence if capsense not discovered in time
        nextEvent = printSequence;
        nextEventTimeDelta = DELAY_REPEAT;
        return;
    }
    canReceiveCapsense = false;
    if (printSequenceOn) {
        log(LOG_DEBUG, "print sequence: LED %d", sequence[printSequenceIdx]);
        flashLED(sequence[printSequenceIdx]);
        printSequenceIdx++;
        nextEventTimeDelta = DELAY_FLASH_ON;
    } else {
        ledsOff();
        nextEventTimeDelta = DELAY_BETWEEN_FLASHES;
    }
    printSequenceOn = !printSequenceOn;
    nextEvent = printSequence;
}
void levelFail() {
    canReceiveCapsense = false;
    if (!printSequenceOn) {
        log(LOG_INFO, "Level %d failed", level + 1);
        failLEDs();
        printSequenceOn = true;
        // Reset back to level 1
        level = 0;
        nextEvent = levelFail;
        nextEventTimeDelta = STANDARD_DELAY;
    } else {
        ledsOff();
        setLevelStart();
    }
}
void levelPass() {
    canReceiveCapsense = false;
    if (!printSequenceOn) {
        log(LOG_INFO, "Level %d pass", level + 1);
        passLEDs();
        printSequenceOn = true;
        // increment level
        level++;
        nextEvent = levelPass;
        nextEventTimeDelta = STANDARD_DELAY;
    } else {
        ledsOff();
        setLevelStart();
    }
}
void levelStart() {
    canReceiveCapsense = false;
    ledsOff();
    setLevelText();
    // Set the next LED value
#ifdef __unix__
    sequence[level] = 0 + ((uint8_t)random() % (8  + 1 - 0));
#else
    sequence[level] = random(8);
#endif
    printSequenceIdx = 0;
    printSequenceOn = false;
    printSequence();
}
void setLevelStart() {
    log(LOG_DEBUG, "Set level start");
    lastEventTime = millis();
    nextEventTimeDelta = STANDARD_DELAY;
    nextEvent = levelStart;
}

void simonSaysGameLogic() {
    currTime = millis();
    if ((currTime - lastEventTime) > nextEventTimeDelta) {
        log(LOG_DEBUG, "event timeout reached");
        lastEventTime = currTime;
        if (nextEvent != NULL) {
            nextEvent();
        }
    }
}

// Top-level app code below here
void simonsays_begin(void)
{
    // add application to the "Games" menu
    BadgeMenuItem *menu_item = game_menu.create_menu_item();
    menu_item->set_name("Simon Says");
    menu_item->set_select_event([](){
        // when your app is selected, this function is called.
        // we push the app to start it. that way we can "pop" back to previous application on exit.
        badge_os.push_app(simonsays_app); 
    });

    // add callback functions for your application.
    simonsays_app.set_begin([](){
        first_loop = true;
        enableCapsenseFeedback(false);
        ledsOn();
        level = 0;
        setLevelStart();
    });

    simonsays_app.set_loop([](){
        // this function is repeatedly called in a loop.
        // WARNING: do not block for long periods of time.

        // do stuff
        if ( first_loop )
        {
            first_loop = false;
            setLevelText();
        }
        simonSaysGameLogic();
    });

    simonsays_app.set_end([](){
        // Switch off LED control on exit
        ledsOff();
        enableCapsenseFeedback(true);
    });

    simonsays_app.set_key_event([](uint8_t key, bool key_down){
        // this function handles key presses.
        log(LOG_DEBUG, "Key event: %s %s", BADGE_KEY_NAMES[key], key_down ? "press" : "release");
        switch (key)
        {
            case BADGE_KEY_LEFT:
                // use this small snippet to exit your application.
                if ( key_down )
                {
                    badge_os.pop_app();
                }
                break;
            // Handle capsense events
            case BADGE_CAPSENSE_1:
            case BADGE_CAPSENSE_2:
            case BADGE_CAPSENSE_3:
            case BADGE_CAPSENSE_4:
            case BADGE_CAPSENSE_5:
            case BADGE_CAPSENSE_6:
            case BADGE_CAPSENSE_7:
            case BADGE_CAPSENSE_8:
                handleCapsense(key - BADGE_CAPSENSE_1, key_down);
                break;
            default:
                break;
        }
    });

}
