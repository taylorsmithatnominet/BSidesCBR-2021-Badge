#include "ExampleDiagApp.h"

#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeLog.h>
#include <BSidesBadge2020.h>
#include <display.h>

// BadgeApp

void ExampleDiagApp::begin(void)
{
    log(LOG_INFO, "ExampleDiagApp: begin");

    // Draw instructions to the screen
    log(LOG_DEBUG, "ExampleDiagApp: render started");
    display.fillScreen(DISPLAY_COLOR_BLACK);
    // Title
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(2, 2);
    display.setCursor(4, 0);
    display.print(get_name().c_str());
    // Content
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(1, 1);
    display.setCursor(8, 32);
    display.print("This is an example.");
    display.setCursor(8, 48);
    display.print("PASS: Press UP");
    display.setCursor(8, 64);
    display.print("FAIL: Press DOWN");
    log(LOG_DEBUG, "ExampleDiagApp: render done");
    display.display();
    log(LOG_DEBUG, "ExampleDiagApp: display done");
}

void ExampleDiagApp::loop(void)
{
    log(LOG_DEBUG, "ExampleDiagApp: loop");
}

void ExampleDiagApp::end(void)
{
    log(LOG_INFO, "ExampleDiagApp: end");
}

// Press UP to set the example test as PASS
// Press DOWN to set the example test as FAIL
void ExampleDiagApp::key_event(uint8_t key, bool key_down)
{
    switch(key)
    {
        case BADGE_KEY_UP:
            if(key_down)
            {
                // RESULT: Pass
                log(LOG_INFO, "ExampleDiagApp: PASS");
                test_complete(DiagAppStatus::kPass);
            }
            break;
        case BADGE_KEY_DOWN:
            if(key_down)
            {
                // RESULT: Fail
                log(LOG_INFO, "ExampleDiagApp: FAIL");
                test_complete(DiagAppStatus::kFail);
            }
        default:
            break;
    }
}