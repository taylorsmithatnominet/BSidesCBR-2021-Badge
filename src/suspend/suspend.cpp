#include "suspend.h"

#include <Arduino.h>
#include <BadgeLog.h>
#include <BSidesMessaging.h>
#include <BSidesBadge2020.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeService.h>
#include <esp_sleep.h>
#include <display.h>
#include <audio.h>

extern Adafruit_SSD1675BX   display;
extern SerialMessageQueue serialBridgeQueue;

// Suspend after a minute of no input
#define SUSPEND_AFTER_MS (60000U)
#define SUSPEND_GRACE (1000U)
// Latest key event, exposed from keyboard service
extern int latestKeyEvent;

BadgeApp     suspend_app;
BadgeService suspend_service;

// We use pullup resistors on our buttons
// All of these buttons must be set LOW before it will reboot
// ESP RTC ULP suspend only supports any high, or all low
static constexpr long long dpad_bitmask = (
        1ULL << BADGE_ENTER_GPIO
        );
static bool suspendPending = false;
static int suspendTime = 0;
static bool suspend_running = false;

void startSuspend() {
    suspendPending = true;
    log_dump();
    // Enqueue a suspend call to SAMD
    SerialMessage msg;
    if (!(prepareMessage(msg, SAMD_Suspend, nextSequenceNumber(), NULL, 0)
                && enqueueMessage(msg, serialBridgeQueue))) {
        log(LOG_ERROR, "Failed to tell SAMD to suspend");
    }
}

void suspend() {
    size_t y = display.height() / 6;
    display_draw_background(BG_BLACK);
    display.setMode(Adafruit_SSD1675BX::kModeBlackWhiteRed);
    display_draw_background(BG_SUSPEND);
    display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE);
    display.setTextSize(1, 1);
    display.setCursor(16, y); y+= 8 * 2;
    display.print("Badge suspended");
    display.setCursor(8, y); y+= 8;
    display.print("Press centre D-pad");
    display.setCursor(16, y);
    display.print("button to reboot");
    display.display();
    // Stop all tasks and services (this will also power down peripherals they are using)
    badge_os.end();
    // Power down peripherals
    // SerialBridge is left active -- causes panic if closed.  Will be set low upon suspend.
    // IMU is reset by cooperative end()s so far -- if this changes we will need to add it here
    audio_teardown();
    display.powerDown();
    esp_deep_sleep_start();
    // When woken, will reboot
}

void suspend_loop() {
    if (!suspendPending) {
        suspendTime = millis();
        if ((suspendTime - latestKeyEvent) > SUSPEND_AFTER_MS) {
            log(LOG_INFO, "Suspend timeout reached");
            startSuspend();
        }
    } else {
        if (millis() - suspendTime > SUSPEND_GRACE) {
            suspend();
        }
    }
}

void suspend_init(void) {
    suspend_app.set_begin([](){
        if (!suspend_running) {
            suspend_service.start();
        }
        startSuspend();
    });
    suspend_service.set_begin([](){
        // Record a latest key event so we don't suspend immediately
        latestKeyEvent = millis();
        // Setup to wake on D-pad button press being low
        esp_sleep_enable_ext1_wakeup(dpad_bitmask,ESP_EXT1_WAKEUP_ALL_LOW);
        suspend_running = true;
    });
    suspend_service.set_loop([](){
        suspend_loop();
    });
    suspend_service.set_end([](){
        suspend_running = false;
    });
    suspend_service.set_name("Suspend");
    suspend_service.set_period_ms(0);

    // Add service and start it
    badge_os.add_service(suspend_service);
    suspend_service.start();
}
