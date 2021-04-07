#include "CRRgbLed.h"
#include <Arduino.h>
#include <RgbDriver.h>
#include <BSidesMessaging.h>
#include <BadgeLog.h>

// TI LT5024 RGB LED driver
// Non-static because we need to be able to handle standby
// from serial bridge
RgbDriver rgbDriver(B0101000);

SerialMessageQueue rgbLedQueue;

static SerialMessage msgRecv;

int mapLEDIndex(int idx) {
    switch(idx) {
        case 0: return 1;
        case 1: return 0;
        case 2: return 7;
        case 3: return 6;
        case 4: return 5;
        case 5: return 4;
        case 6: return 3;
        case 7: return 2;
        default: return -1;
    }
}

void setLEDs(SerialMessage &msg) {
    RGBLEDState state = msg.data.payload.leds;
    for (int i = 0; i < 8; i++) {
        int idx = mapLEDIndex(i);
        log(LOG_DEBUG, "LED %d brightness %d r:%d g:%d b:%d", 
                idx, state.brightness[i], state.red[i], state.green[i], state.blue[i]);
        if (rgbDriver.setLedBrightness(idx, state.brightness[i])) {
            log(LOG_ERROR, "Failed to set LED %d brightness", idx);
        }
        if (rgbDriver.setLedColour(idx, state.red[i], state.green[i], state.blue[i])) {
            log(LOG_ERROR, "Failed to set LED %d colour", idx);
        }
    }
}

void CRRgbLed::setup(void) {
    // Initialise RGB LEDs
    rgbDriver.selectNormalMode();
    rgbDriver.setDeviceConfig(true, true, true, true, false, false);
    rgbDriver.setLedConfig(false, false, false, false, false, false, false, false);
    this->setupCoroutine(F("rgbled"));
}

int CRRgbLed::runCoroutine() {
    // Run LED loop
    COROUTINE_LOOP() {
        if (dequeueMessage(&msgRecv, rgbLedQueue)) {
            if (msgRecv.header.type == LEDSet) {
                log(LOG_DEBUG, "Received LED message");
                setLEDs(msgRecv);
            } else {
                log(LOG_ERROR, "Received %s message in LED coroutine", messageType(msgRecv.header.type));
            }
        }
        COROUTINE_YIELD();
    }
}
