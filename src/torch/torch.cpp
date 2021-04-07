#include "torch.h"

#include <Arduino.h>
#include <BadgeOs.h>
#include <BadgeLog.h>
#include <BadgeMenu.h>
#include <BadgeTextViewer.h>
#include <BSidesBadge2020.h>
#include <BSidesMessaging.h>
#include <display.h>
#include <menu.h>

#define BRIGHTNESS_MAXIMUM (5U)

extern SerialMessageQueue serialBridgeQueue;
static SerialMessage msg_to_send;
static bool brightness_changed;
static uint8_t brightness;

static void _enable_capsense_feedback(bool enable) {
    CapsenseFeedback feedback;
    feedback.enable = enable;
    if (!prepareMessage(msg_to_send, CapsenseFeedbackEnable, nextSequenceNumber(), &feedback, sizeof(feedback))) {
        log(LOG_ERROR, "TORCH: failed to prepare feedback message: enable/disable capsense");
    }
    if (!enqueueMessage(msg_to_send, serialBridgeQueue)) {
        log(LOG_ERROR, "TORCH: failed to send feedback message: enable/disable capsense");
    }
}

static RGBLEDState _on_state(uint8_t brightness) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = brightness;
        led_state.red[i] = 0xFF;
        led_state.green[i] = 0xFF;
        led_state.blue[i] = 0xFF;
    }
    return led_state;
}

static RGBLEDState _off_state(void) {
    RGBLEDState led_state = {};
    for (int i = 0; i < 8; i++) {
        led_state.brightness[i] = 0x00;
        led_state.red[i] = 0x00;
        led_state.green[i] = 0x00;
        led_state.blue[i] = 0x00;
    }
    return led_state;
}

static void _set_torch_brightness(void) {
    log(LOG_INFO, "TORCH: setting brightness");
    bool success = true;
    RGBLEDState led_state = _on_state(255 / BRIGHTNESS_MAXIMUM * brightness);
    if (success) {
        if (!prepareMessage(msg_to_send, LEDSet, nextSequenceNumber(), &led_state, sizeof(led_state))) {
            log(LOG_ERROR, "TORCH: failed to prepare message: set brightness");
            success = false;
        }
    }
    if (success) {
        if (!enqueueMessage(msg_to_send, serialBridgeQueue)) {
            log(LOG_ERROR, "TORCH: failed to send message: set brightness");
            success = false;
        }
    }
}

static void _disable_torch(void) {
    log(LOG_INFO, "TORCH: disable");
    bool success = true;
    RGBLEDState led_state = _off_state();
    if (success) {
        if (!prepareMessage(msg_to_send, LEDSet, nextSequenceNumber(), &led_state, sizeof(led_state))) {
            log(LOG_ERROR, "TORCH: failed to prepare message: disable torch");
            success = false;
        }
    }
    if (success) {
        if (!enqueueMessage(msg_to_send, serialBridgeQueue)) {
            log(LOG_ERROR, "TORCH: failed to send message: disable torch");
            success = false;
        }
    }
}

static void _draw_torch_app(void) {
    log(LOG_INFO, "TORCH: drawing");
    // Clear the screen
    display.fillScreen(DISPLAY_COLOR_BLACK);
    // Status bar
    size_t status_bar_y = display_draw_status_bar();
    // Title
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(2, 2);
    display.setCursor(4, 0);
    display.print("TORCH");
    // Percentage
    int16_t outer_rect_x = 32;
    int16_t outer_rect_y = status_bar_y + 64;
    int16_t outer_rect_w = display.width() - 64;
    int16_t outer_rect_h = display.height() - status_bar_y - 128;
    int16_t outer_rect_r = 5;
    display.drawRoundRect(outer_rect_x, outer_rect_y - 5, outer_rect_w, outer_rect_h + 10, outer_rect_r, DISPLAY_COLOR_WHITE);
    for(int16_t i = 0; i <= BRIGHTNESS_MAXIMUM; i++) {
        display.drawFastHLine(outer_rect_x,
                              outer_rect_y + (outer_rect_h * i / BRIGHTNESS_MAXIMUM),
                              outer_rect_w,
                              DISPLAY_COLOR_WHITE);
    }
    // Brightness boxes
    if(brightness > 0) {
        display.fillRect(outer_rect_x,
                         outer_rect_y + (outer_rect_h - (outer_rect_h / BRIGHTNESS_MAXIMUM * brightness)),
                         outer_rect_w,
                         outer_rect_h / BRIGHTNESS_MAXIMUM * brightness,
                         DISPLAY_COLOR_WHITE);
    }
    display.display();
}

static void _increase_brightness(void) {
    if(brightness < BRIGHTNESS_MAXIMUM) {
        brightness++;
        brightness_changed = true;
        log(LOG_INFO, "TORCH: brightness increased to %u", brightness);
    }
}

static void _decrease_brightness(void) {
    if(brightness > 0) {
        brightness--;
        brightness_changed = true;
        log(LOG_INFO, "TORCH: brightness decreased to %u", brightness);
    }
}

static void _torch_begin(void) {
    log(LOG_INFO, "TORCH: begin");
    _enable_capsense_feedback(false);
    brightness = 1;
    brightness_changed = true;
}

static void _torch_loop(void) {
    if(brightness_changed) {
        brightness_changed = false;
        _set_torch_brightness();
        _draw_torch_app();
    }
}

static void _torch_end(void) {
    log(LOG_INFO, "TORCH: end");
    _enable_capsense_feedback(true);
    _disable_torch();
}

static void _torch_keyh(uint8_t key, bool down) {
    switch (key)
    {
    case BADGE_KEY_LEFT:
        if(down) {
            badge_os.pop_app();
        }
        break;
    case BADGE_KEY_UP:
        if(down) {
            _increase_brightness();
        }
        break;
    case BADGE_KEY_DOWN:
        if(down) {
            _decrease_brightness();
        }
        break;
    default:
        break;
    }
}

static BadgeApp torch_app(_torch_begin, _torch_loop, _torch_end, _torch_keyh);

void torch_begin(void) {
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("Torch");
    menu_item->set_select_event([](){ badge_os.push_app(torch_app); });
}