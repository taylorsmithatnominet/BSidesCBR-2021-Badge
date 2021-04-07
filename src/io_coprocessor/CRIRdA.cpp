#include "CRIRdA.h"
#include <Arduino.h>
#include <BSidesBadge2020.h>
#include <BadgeLog.h>
#include <BSidesMessaging.h>
#include <IRBlaster.h>

static IRBlaster blaster = IRBlaster();

// IRdA queue
SerialMessageQueue irdaQueue;
// Serial bridge message queue
extern SerialMessageQueue serialBridgeQueue;

static SerialMessage msgRecv;

void checkIRdAComponent(){

    bool result = blaster.checkComponent();
    IRCheck resultMessage;
    resultMessage.checkResult = result;

    SerialMessage statusMessage;

    //Send back okay message
    if (!prepareMessage(statusMessage, IRdACheck, nextSequenceNumber(), &resultMessage, sizeof(resultMessage))) {
        log(LOG_ERROR, "Failed to prepare IRDA = Okay message");
    }

    //Send the message
    if (!enqueueMessage(statusMessage, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to enqueue %s message", messageType(statusMessage.header.type));
    }

}

static void handleIRmessage(void) {
    switch(msgRecv.header.type) {
        case IRdACheck:
            checkIRdAComponent();
            break;
        default:
            // unimplemented
            break;
    }
}

void CRIRdA::setup() {
    this->setupCoroutine(F("irda"));
}

int CRIRdA::runCoroutine() {
    COROUTINE_LOOP() {
        if (dequeueMessage(&msgRecv, irdaQueue)) {
            if (validateMessage(msgRecv)
                    && (msgRecv.header.type == IRdASend
                        || msgRecv.header.type == IRdARecv
                        || msgRecv.header.type == IRdACheck
                       )) {
                log(LOG_DEBUG, "Received IRdA message");
                handleIRmessage();
            }
        }
        COROUTINE_YIELD();
    }
}

