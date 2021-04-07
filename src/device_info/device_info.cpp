#include <cstdbool>
#include <cstdint>

#include <BadgeApp.h>
#include <BadgeKey.h>
#include <BadgeInfo.h>
#include <BadgeOs.h>
#include <BadgeAssets.h>
#include <BSidesMessaging.h>

#include <display.h>
#include <asset/battery_unknown_bmp.h>
#include <asset/battery_empty_bmp.h>
#include <asset/battery_low_bmp.h>
#include <asset/battery_mid_bmp.h>
#include <asset/battery_full_bmp.h>

#include "device_info.h"

extern VBATValue _vbat;

void device_info_begin(void) {
    for ( size_t i = 0; i < device_info_count(); i++ )
    {
        // write device info to serial at boot time
        char key_value[128];
        device_info_name(i, key_value, sizeof(key_value));
        Serial.print(key_value);
        Serial.print(": ");
        device_info_value(i, key_value, sizeof(key_value));
        Serial.print(key_value);
        Serial.print("\n");
        Serial.flush();
    }
    device_info_svc.set_name("sysinfo");
    device_info_svc.set_draw_icon([](uint16_t x, uint16_t y, uint16_t w, uint16_t h){
        x += (w / 2) - (ASSET_WIDTH(battery_unknown_bmp) / 2);
        y += (h / 2) - (ASSET_HEIGHT(battery_unknown_bmp) / 2);
#ifdef __unix__
        DRAW_ASSET(x, y, battery_unknown_bmp, display);
#endif // __unix
#ifdef ESP32
        // draw rough percentage based on approximate discharge voltages
        if (_vbat.value < 1.0 || _vbat.value >= 4.5) {
            // Out of range for battery, unknown status
            DRAW_ASSET(x, y, battery_unknown_bmp, display);
        } else if (_vbat.value > 3.7) {
            DRAW_ASSET(x, y, battery_full_bmp, display);
        } else if (_vbat.value > 3.5) {
            DRAW_ASSET(x, y, battery_mid_bmp, display);
        } else if (_vbat.value > 3.2) {
            DRAW_ASSET(x, y, battery_low_bmp, display);
        } else {
            DRAW_ASSET(x, y, battery_empty_bmp, display);
        }
#endif // ESP32
    });
    badge_os.add_service(device_info_svc);
    device_info_svc.start();
}

/* This app is hooked into the menu */
static void _begin(void);
static void _kh(uint8_t, bool);
BadgeApp device_info(_begin, nullptr, nullptr, _kh);

static void _begin(void) {
    size_t x, y;

#ifdef ESP32
    display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
#endif
    display.fillScreen(DISPLAY_COLOR_BLACK);
    y = display_draw_background(BG_DEFAULT);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(8, 0);
    display.setTextSize(2, 2);
    display.print("Status");

    x = 8;
    display.setCursor(x, y);
    display.setTextSize(1, 1);
    int16_t dy = 8 + 4;
    for ( size_t i = 0; i < device_info_count(); i++ )
    {
        char key_value[128];
        device_info_name(i, key_value, sizeof(key_value));
        display.setCursor(x, y + dy); dy += 8;
        display.print(key_value);
        display.print(":");
        device_info_value(i, key_value, sizeof(key_value));
        display.setCursor(x, y + dy); dy += 12;
        display.print(key_value);
    }
    display.display();
}
static void _kh(uint8_t key, bool down) {
    if (down && key == BADGE_KEY_LEFT) badge_os.pop_app();
}
