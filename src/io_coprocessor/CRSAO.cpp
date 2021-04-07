#include "CRSAO.h"
#include <Arduino.h>
#include <BSidesBadge2020.h>
#include <BadgeLog.h>
#include <BSidesMessaging.h>

// SAO queue
SerialMessageQueue saoQueue;
// Serial bridge message queue
extern SerialMessageQueue serialBridgeQueue;

static SerialMessage msgRecv;

void CRSAO::setup() {
    this->setupCoroutine(F("sao"));
}

int CRSAO::runCoroutine() {
    COROUTINE_LOOP() {
        if (dequeueMessage(&msgRecv, saoQueue)) {
            if (validateMessage(msgRecv)
                    && (msgRecv.header.type == SAOWireSend
                        || msgRecv.header.type == SAOWireRequest
                        || msgRecv.header.type == SAOGPIOGet
                        || msgRecv.header.type == SAOGPIOSet
                       )) {
            log(LOG_DEBUG, "Received SAO message");
            }
        }
        COROUTINE_YIELD();
    }
}

