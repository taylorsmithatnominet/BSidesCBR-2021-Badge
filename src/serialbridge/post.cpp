#include <cstdint>
#include <cstring>

#include "serialbridge.h"

#include <Arduino.h>

#include <BadgeKey.h>
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <DiagApp.h>
#include <SerialBridge.h>

/* Forward declare the delegate functions */
static void _begin(void);
static void _loop(void);

extern SerialMessageQueue serialBridgeQueue;
extern BridgeState  bridgeState;

// Used to track time elapsed since diagnostics started -- to fail out if no response from SAMD
static int timeSinceDiagnosticStart = 0;

#define DIAGNOSTIC_TIMEOUT (3000U)

/* Construct the diagnostic app with delegates and then expose a shared pointer
 * to it which is what the diagnostics app will link against.
 */
static DiagApp _serialbridge_post("Bridge", _begin, _loop, nullptr, nullptr);
std::shared_ptr<DiagApp> serialbridge_post{
    std::shared_ptr<DiagApp>{}, &_serialbridge_post
};

static void _begin(void) {
    // wait for diagnostics response
    timeSinceDiagnosticStart = millis();
}

static void _loop(void) {
    if (bridgeState == live){
        _serialbridge_post.test_complete(DiagAppStatus::kPass);
    } else if (millis() - timeSinceDiagnosticStart >= DIAGNOSTIC_TIMEOUT) {
        log(LOG_ERROR, "Failed to achieve a valid state of serial bridge");
        _serialbridge_post.test_complete(DiagAppStatus::kFail);
    }
}
