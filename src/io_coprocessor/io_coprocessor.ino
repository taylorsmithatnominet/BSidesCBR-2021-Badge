#include <Arduino.h>
#include <BSidesBadge2020.h>
#include <SerialBridge.h>
#include "CRSerial.h"
#include "CRSerialBridge.h"
#include "CRCapsense.h"
#include "CRRgbLed.h"
#include "CRIRdA.h"
#include "CRSAO.h"
#include "CRvBAT.h"
#include <AceRoutine.h>

using namespace ace_routine;

CRSerial serial;
CRSerialBridge serialbridge;
CRCapsense capsense;
CRRgbLed rgbled;
CRIRdA irda;
CRSAO sao;
CRvBAT vbat;

void setup() {
    Serial.begin(115200);
    DebugSerial.begin(115200);
    // Initialise IR blaster
    pinMode(IR_RX, INPUT);
    pinMode(IR_TX, OUTPUT);
    pinMode(IR_STANDBY, OUTPUT);
    // Setup coroutines
    serial.setup();
    serialbridge.setup();
    capsense.setup();
    rgbled.setup();
    irda.setup();
    sao.setup();
    vbat.setup();
    CoroutineScheduler::setup();
}

void loop() {
    CoroutineScheduler::loop();
}
