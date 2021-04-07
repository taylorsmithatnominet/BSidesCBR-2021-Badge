#include "CRCapsense.h"
#include <Arduino.h>
#include <BSidesBadge2020.h>
#include <BadgeLog.h>
#include <Adafruit_FreeTouch.h>
#include <BSidesMessaging.h>

// Serial bridge message queue
extern SerialMessageQueue serialBridgeQueue;
extern SerialMessageQueue rgbLedQueue;

SerialMessageQueue capsenseQueue;

// Capsense objects
static Adafruit_FreeTouch ft1 = Adafruit_FreeTouch(CP1, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft2 = Adafruit_FreeTouch(CP2, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft3 = Adafruit_FreeTouch(CP3, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft4 = Adafruit_FreeTouch(CP4, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft5 = Adafruit_FreeTouch(CP5, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft6 = Adafruit_FreeTouch(CP6, OVERSAMPLE_16, RESISTOR_0, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft7 = Adafruit_FreeTouch(CP7, OVERSAMPLE_1, RESISTOR_100K, FREQ_MODE_NONE);
static Adafruit_FreeTouch ft8 = Adafruit_FreeTouch(CP8, OVERSAMPLE_1, RESISTOR_0, FREQ_MODE_NONE);

static CapsenseState touchValues;
static CapsenseState newTouchValues;
static bool sendMsg = false;
// Serial message structure for sending/receiving
static SerialMessage msg;
// Assume SAMD has control over LED feedback of capsense touches unless we are disabled.
static bool feedbackEnabled = true;

void sendCapsenseMessage() {
    if (prepareMessage(msg, Capsense, nextSequenceNumber(), &touchValues, sizeof(touchValues))) {
        if (enqueueMessage (msg, serialBridgeQueue)) {
            log(LOG_DEBUG, "Sent capsense message");
        } else {
            log(LOG_ERROR, "Failed to send capsense message");
        }
    } else {
        log(LOG_ERROR, "Failed to prepare capsense message");
    }
    RGBLEDState state;
    memset((void*)&state, 0, sizeof(state));
    for (int i = 0; i < 8; i++) {
        if (touchValues.pads[i]) {
            state.brightness[i] = 0x20;
            state.blue[i] = 0xFF;
        }
    }
    if (!feedbackEnabled) {
        return;
    }
    if (prepareMessage(msg, LEDSet, nextSequenceNumber(), &state, sizeof(state))) {
        if (enqueueMessage (msg, rgbLedQueue)) {
            log(LOG_DEBUG, "Sent capsense message to leds");
        } else {
            log(LOG_ERROR, "Failed to send capsense message to leds");
        }
    } else {
        log(LOG_ERROR, "Failed to prepare LED message");
    }
}

void CRCapsense::setup() {
    // Initialise capacitive buttons
    ft1.begin();
    ft2.begin();
    ft3.begin();
    ft4.begin();
    ft5.begin();
    ft6.begin();
    ft7.begin();
    ft8.begin();
    this->setupCoroutine(F("capsense"));
}

int CRCapsense::runCoroutine() {
    COROUTINE_LOOP() {
        if(dequeueMessage(&msg, capsenseQueue)) {
            if (msg.header.type == CapsenseFeedbackEnable) {
                feedbackEnabled = msg.data.payload.feedback.enable;
                log(LOG_DEBUG, "Direct capsense feedback is %s", feedbackEnabled ? "enabled" : "disabled");
            } else {
                log(LOG_ERROR, "Bad message received at capsense");
            }
        }
        sendMsg = false;
        // Read capacitive touch buttons, divide by 1000 to get whether they were pressed
        newTouchValues.pads[0] = (ft1.measure()/1000) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[1] = (ft2.measure()/1000) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[2] = (ft3.measure()/1000) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[3] = (ft4.measure()/1000) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[4] = (ft5.measure()/1000) > 0;
        COROUTINE_YIELD();
        // pad 6 is very close to the power supply for the screen, so we've set it to be very insensitive
        // And even then it has close to zero change -- only a difference between 1015 and 1016.
        newTouchValues.pads[5] = (ft6.measure() - 1015) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[6] = (ft7.measure()/1000) > 0;
        COROUTINE_YIELD();
        newTouchValues.pads[7] = (ft8.measure()/1000) > 0;
        COROUTINE_YIELD();
        for (int i = 0; i < 8; i++) {
            if (newTouchValues.pads[i] != touchValues.pads[i]) {
                sendMsg = true;
            }
            touchValues.pads[i] = newTouchValues.pads[i];
        }
        if(sendMsg) {
            sendCapsenseMessage();
        }
        COROUTINE_YIELD();
    }
}

