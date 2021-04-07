#include "rgbled.h"

#include <math.h>
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <BadgeMenu.h>
#include <BSidesBadge2020.h>
#include <BSidesMessaging.h>
#include <DiagApp.h>
#include <display.h>
#include <menu.h>

extern SerialMessageQueue serialBridgeQueue;

#ifdef ESP32
static uint8_t backup_display_mode;
#endif
#ifdef __unix__
#define esp_err_t int
#define ESP_OK 0
#define ESP_FAIL -1
#endif


static uint32_t g_last_time = 0;
static uint32_t g_cycle_idx = 0;
static SerialMessage msgToSend;


static void enableCapsenseFeedback(bool enable) {
    CapsenseFeedback feedback;
    feedback.enable = enable;
    if (!prepareMessage(msgToSend, CapsenseFeedbackEnable, nextSequenceNumber(), &feedback, sizeof(feedback))) {
        log(LOG_ERROR, "Failed to prepare feedback message");
    }
    if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send feedback message");
    }
}

static void _begin(void) {
#ifdef ESP32
    backup_display_mode = display.getMode();
    display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
#endif
    g_last_time = millis();
    g_cycle_idx = 0;
    enableCapsenseFeedback(false);
    // Write string to dsplay
    const char *state_desc = "R G B W K pattern";
    display.fillScreen(DISPLAY_COLOR_BLACK);
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(1, 1);
    display.setCursor(8, 32);
    display.print(state_desc);
    log(LOG_INFO, "%s", state_desc);
    display.setCursor(8, 48);
    display.print("PASS: Press RIGHT");
    display.setCursor(8, 64);
    display.print("FAIL: Press LEFT");
    display.display();
}

static RGBLEDState red_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x40;
        led_state.red[i] = 0xFF;
        led_state.green[i] = 0x00;
        led_state.blue[i] = 0x00;
    }
    return led_state;
}

static RGBLEDState green_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x40;
        led_state.red[i] = 0x00;
        led_state.green[i] = 0xFF;
        led_state.blue[i] = 0x00;
    }
    return led_state;
}

static RGBLEDState blue_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x40;
        led_state.red[i] = 0x00;
        led_state.green[i] = 0x00;
        led_state.blue[i] = 0xFF;
    }
    return led_state;
}

static RGBLEDState white_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x40;
        led_state.red[i] = 0xFF;
        led_state.green[i] = 0xFF;
        led_state.blue[i] = 0xFF;
    }
    return led_state;
}

static RGBLEDState off_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x00;
        led_state.red[i] = 0x00;
        led_state.green[i] = 0x00;
        led_state.blue[i] = 0x00;
    }
    return led_state;
}

static RGBLEDState clockwise_state(int32_t idx) {
    RGBLEDState led_state = {};
    switch(idx % 5) {
        case 0:
            led_state = red_state();
            break;
        case 1:
            led_state = green_state();
                break;
        case 2:
            led_state = blue_state();
            break;
        case 3:
            led_state = white_state();
            break;
        case 4:
        default:
            led_state = off_state();
            break;
    }
    return led_state;
}

static void _loop(void) {
    RGBLEDState led_state; 
    // Cycle every ~500ms
    int32_t time = millis();
    if (time - g_last_time > 500) {
        g_last_time = time;
        // Spin the LED cycle
        g_cycle_idx++;
        led_state = clockwise_state(g_cycle_idx);
        if (!prepareMessage(msgToSend, LEDSet, nextSequenceNumber(), &led_state, sizeof(led_state))) {
            log(LOG_ERROR, "Failed to prepare LED message");
        } else if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
            log(LOG_ERROR, "Failed to send LED message");
        }
    }
}

static void _end(void) {
#ifdef ESP32
    display.setMode(backup_display_mode);
#endif
    RGBLEDState led_state = off_state();
    if (!prepareMessage(msgToSend, LEDSet, nextSequenceNumber(), &led_state, sizeof(led_state))) {
        log(LOG_ERROR, "Failed to prepare LED message");
    } else if (!enqueueMessage(msgToSend, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send LED message");
    }
    enableCapsenseFeedback(true);
}

static void _keyh(uint8_t key, bool down) {
    if (!down) {
        return;
    }

    switch (key) {
    case BADGE_KEY_RIGHT:
        log(LOG_INFO, "RGB LED POST Successful");
        rgbled_post.test_complete(DiagAppStatus::kPass);
        break;
    case BADGE_KEY_LEFT:
        rgbled_post.test_complete(DiagAppStatus::kFail);
        break;
    }
}

DiagApp rgbled_post("RGB LED", _begin, _loop, _end, _keyh);
