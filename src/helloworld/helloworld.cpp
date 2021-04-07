#include "helloworld.h"

#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeService.h>
#include <BadgeMenu.h>
#include <BSidesBadge2020.h>
#include <display.h>
#include <menu.h>

// your application - used to register callbacks from badge OS.
BadgeApp helloworld_app;

// OPTIONAL: your service - used to register callbacks from badge OS.
BadgeService helloworld_service;

// globals for your app
static bool first_loop = false;

void helloworld_begin(void)
{
    // add your application to the "Apps" menu
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("Helloworld");
    menu_item->set_select_event([](){
        // when your app is selected, this function is called.
        // we push the app to start it. that way we can "pop" back to previous application on exit.
        badge_os.push_app(helloworld_app); 
    });

    // add callback functions for your application.
    helloworld_app.set_begin([](){
        // when your app starts, this function is called
        Serial.println("HELLOWORLD APP: begin");
        
        // you can initialise your global variables here.
        first_loop = true;

        // OPTIONAL: this a good place to start your service.
        helloworld_service.start();
    });
    helloworld_app.set_loop([](){
        // this function is repeatedly called in a loop.
        // WARNING: do not block for long periods of time.

        // do stuff
        if ( first_loop )
        {
            // only want to draw once (you can decide how often you want to draw).
            // NOTE: to save power and not destroy eink, try to reduce number of drawing updates.
            first_loop = false;

            // good idea to fill the screen before drawing you application.
            Serial.println("HELLOWORLD APP: render started");
            display.fillScreen(DISPLAY_COLOR_BLACK);

            // use the following to draw the status bar
            // it returns the y co-ordinate under the status bar your application can use.
            // NOTE: you can instead use display_draw_background() to draw:
            //          - status bar.
            //          - top banner
            //          - bottom banner
            size_t y = display_draw_status_bar();

            // add a title for you application in the status bar
            // NOTE: do this AFTER, that way you don't get drawn over by the status bar.
            display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
            display.setCursor(8, 0);
            display.setTextSize(2, 2);
            display.print("Helloworld");

            // you can use other Adafruit APIs to draw as well
            // NOTE: remember, you can use your "y" variable to avoid drawing of background art.
            display.drawRect(2, y + 2, display.width() - 4, display.height()  - y - 4, DISPLAY_COLOR_RED);
            display.drawRect(16, y + 16, display.width()  - 32, display.height() - y - 32, DISPLAY_COLOR_WHITE);

            // once all your drawing is done, display it!
            Serial.println("HELLOWORLD APP: render done");
            display.display();
            Serial.println("HELLOWORLD APP: display done");
        }
    });
    helloworld_app.set_end([](){
        // when your application ends, this function is called
        Serial.println("HELLOWORLD APP: end");

        // OPTIONAL: this a good place to stop your service.
        helloworld_service.stop();
    });
    helloworld_app.set_key_event([](uint8_t key, bool key_down){
        // this function handles key presses.
        switch (key)
        {
            case BADGE_KEY_LEFT:
                // use this small snippet to exit your application.
                if ( key_down )
                {
                    badge_os.pop_app();
                }
                break;
            default:
                break;
        }
    });

    // OPTIONAL: implementing a service
    helloworld_service.set_name("HelloWd"); // OPTIONAL: if you add a name, it will appear in services menu.
    helloworld_service.set_begin([](){
        Serial.println("HELLOWORLD SERIVCE: begin");
    });
    helloworld_service.set_period_ms(1000); // OPTIONAL: time between service loop calls
    helloworld_service.set_loop([](){
        Serial.println("HELLOWORLD SERIVCE: loop call");
    });
    helloworld_service.set_end([](){
        Serial.println("HELLOWORLD SERIVCE: end");
    });
    badge_os.add_service(helloworld_service);  // services must be registered to badge OS.
}
