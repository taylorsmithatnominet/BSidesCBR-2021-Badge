
#include "irblaster_post.h"

#include <math.h>
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <BadgeMenu.h>
#include <BSidesBadge2020.h>
#include <BSidesMessaging.h>
#include <DiagApp.h>
#include <display.h>
#include <menu.h>

#define DIAGNOSTIC_TIMEOUT (1000U)

SerialMessageQueue irdaQueue;
extern SerialMessageQueue serialBridgeQueue;

static int timeSinceDiagnosticStart = 0;
static SerialMessage msg_to_send;

static void _begin(void);
static void _loop(void);

static DiagApp _irblaster_post("IR", _begin, _loop, nullptr, nullptr);
std::shared_ptr<DiagApp> irblaster_post{
    std::shared_ptr<DiagApp>{}, &_irblaster_post
};

static void _begin(void) {

    timeSinceDiagnosticStart = millis();

    if (!prepareMessage(msg_to_send, IRdACheck, nextSequenceNumber(), NULL, 0)) {
        log(LOG_ERROR, "Failed to prepare feedback message: IRdA Check");
    }

    if (!enqueueMessage(msg_to_send, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to send feedback message: IRdA Check");
    }

}

static void _loop(void) {

    static SerialMessage IRdACheckMessage;

    if (millis() - timeSinceDiagnosticStart >= DIAGNOSTIC_TIMEOUT) {
        log(LOG_ERROR, "Failed to detect IR Blaster");
        _irblaster_post.test_complete(DiagAppStatus::kFail);
    }else{

        if (dequeueMessage(&IRdACheckMessage, irdaQueue)) {
            bool okay = IRdACheckMessage.data.payload.ircheck.checkResult;

            if(okay == true){
                log(LOG_ERROR, "IR Blaster detected OK");
                _irblaster_post.test_complete(DiagAppStatus::kPass);
            }else{
                log(LOG_ERROR, "Failed to detect IR Blaster");
                 _irblaster_post.test_complete(DiagAppStatus::kFail);
            }
        }

    }

}


