#include "CRvBAT.h"
#include <avr/dtostrf.h>
#include <Arduino.h>
#include <BSidesBadge2020.h>
#include <BadgeLog.h>
#include <BSidesMessaging.h>

// IRdA queue
SerialMessageQueue vbatQueue;
// Serial bridge message queue
extern SerialMessageQueue serialBridgeQueue;

static SerialMessage msg;
static VBATValue vbat;

void CRvBAT::setup() {
    // This appears to be the default analog reference
    // but it doesn't hurt to explicitly set it
    analogReference(AR_DEFAULT); // AR_DEFAULT is a 3.3V reference
    pinMode(VBAT_MEAS, INPUT);
    this->setupCoroutine(F("vbat"));
}

int CRvBAT::runCoroutine() {
    COROUTINE_LOOP() {
        if (dequeueMessage(&msg, vbatQueue)) {
            if (validateMessage(msg)
                && msg.header.type == VBATRequest
               ) {
                log(LOG_DEBUG, "Received vBAT message");
                // Measure value, normalise based on VREF range, normalise based on voltage divider ((330k + 10k) / 10k)
                vbat.value = ((float)analogRead(VBAT_MEAS) * 3.3f * 34.0f) / 1023.0f;
                // grumble grumble Arduino printf formatting not supporting floats rhubarb rhubarb
                char array[8];
                dtostrf(vbat.value, 2, 2, array);
                log(LOG_INFO, "VBat is %sV", array);
                prepareMessage(msg,VBATResponse, nextSequenceNumber(), &vbat, sizeof(vbat));
                enqueueMessage(msg, serialBridgeQueue);
            }
        }
        COROUTINE_YIELD();
    }
}
