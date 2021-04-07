#include "CRSerial.h"
#include <Arduino.h>
#include <BadgeLog.h>
#include <SerialBridge.h>

void CRSerial::setup(void) {
    Serial.begin(115200);
    DebugSerial.begin(115200);
    this->setupCoroutine(F("serial"));
}

static int lastMillis = 0;

int CRSerial::runCoroutine() {
    COROUTINE_LOOP() {
        COROUTINE_DELAY(500);
        log_dump();
        COROUTINE_YIELD();
    }
}
