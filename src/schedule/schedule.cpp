#include "schedule.h"

#include <BSidesBadge2020.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <display.h>
#include <BadgeAssets.h>
#include <asset/schedule_bmp.h>

static void _begin(void);
static void _keyh(uint8_t key, bool down);
BadgeApp schedule_app(_begin, nullptr, nullptr, _keyh);

static void _begin(void){

    display_draw_background(BG_BLACK);
    (void)display_draw_status_bar();
    size_t offset = display.height() / 8 * 2;
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(8, 0);
    display.setTextSize(2, 2);
    display.print("Schedule");
    DRAW_ASSET_OFFSET_TOP_CENTER(offset, schedule_bmp, display);
    offset *= 2.5;
    display.setCursor(16, offset);
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(1, 1);
    display.print(
            "Scan the QR code\n\n"
            "      or visit:\n"
            "      https://\n"
            "   bsidescbr.com.au\n"
            "    /schedule.html\n\n"
            "  <- Back | Next ->"
    );
    display.display();
}

static void _keyh(uint8_t key, bool key_down){
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
        case BADGE_KEY_RIGHT:
            /* TODO: Reserved for further screens - falls through for now */
        default:
            break;
    }
}
