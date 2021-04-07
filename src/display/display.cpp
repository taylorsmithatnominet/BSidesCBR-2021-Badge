#include "display.h"

#include <BadgeOs.h>
#include <BadgeService.h>
#include <BadgeAssets.h>
#include <asset/cyberpocalypse_bmp.h>
#include <asset/magpies_bmp.h>
#include <asset/magpie_suspend_bmp.h>
#include <asset/bsides_2021_bmp.h>
#include <asset/magpies_egg_bmp.h>

#ifdef ESP32
SPIClass SPI3(HSPI);
Adafruit_SSD1675BX  display(DISPLAY_WIDTH,
                            DISPLAY_HEIGHT,
                            EPD_DC,
                            EPD_RESET,
                            EPD_CS,
                            EPD_SRAM_CS,
                            EPD_BUSY,
                            &SPI3);
#endif

#ifdef __unix__
Adafruit_X11        x11_window(DISPLAY_WIDTH, DISPLAY_HEIGHT);
Adafruit_Cache      display(&x11_window, DISPLAY_WIDTH, DISPLAY_HEIGHT);
#endif

/* We define an app to show to 2020 interstitial before we draw the 2021 menu.
 * This is a no-op on the host build.
 */
BadgeApp display_2020_interstitial;

void display_begin(void)
{
    /* Display setup */
    display.setRotation(1);
#ifdef ESP32
    display.clearBuffer();
    display.begin();
#endif
#ifdef __unix__
    display.fillScreen(DISPLAY_COLOR_WHITE);
    display.display();
#endif
    /* 2020 interstitial setup */
    display_2020_interstitial.set_begin([](){
#ifdef ESP32
        /* We only bother doing anything on the ESP since the display blocks
         * for a bit and the effect is visually interesting.
         */
        display.setMode(Adafruit_SSD1675BX::kModeBlackWhiteRed);
        display_draw_background(BG_2020);
#endif
        badge_os.pop_app();
    });
}

/* Status bar is always a 16px high bar starting from the top */
#define STATUS_BAR_VOFF   (0U)
#define STATUS_BAR_HEIGHT (16U)
#define STATUS_ITEM_WIDTH (16U)

size_t display_draw_status_bar(void)
{
    uint16_t x = display.width();
    for ( BadgeService *service : badge_os.services() )
    {
        if ( service->running() && service->visible() )
        {
            x -= STATUS_ITEM_WIDTH;
            service->draw_icon(
                x, STATUS_BAR_VOFF, STATUS_ITEM_WIDTH, STATUS_BAR_HEIGHT
            );
        }
    }
    return STATUS_BAR_VOFF + STATUS_BAR_HEIGHT;
}

size_t display_draw_background(display_bg_e bg_id)
{
    /* Start drawing from the bottom of the freshly drawn status bar */
    size_t offset = display_draw_status_bar();

    switch (bg_id) {
    case BG_2020:
        DRAW_ASSET_OFFSET_TOP_CENTER(offset, cyberpocalypse_bmp, display);
        DRAW_ASSET_BOTTOM_CENTER(magpies_bmp, display);
        offset += ASSET_HEIGHT(cyberpocalypse_bmp);
        break;
    case BG_2021:
        DRAW_ASSET_OFFSET_TOP_CENTER(offset, bsides_2021_bmp, display);
        DRAW_ASSET_BOTTOM_CENTER(magpies_egg_bmp, display);
        offset += ASSET_HEIGHT(bsides_2021_bmp);
        break;
    case BG_SUSPEND:
        DRAW_ASSET_OFFSET_TOP_CENTER(offset, magpie_suspend_bmp, display);
        DRAW_ASSET_BOTTOM_CENTER(magpie_suspend_bmp, display);
        offset += ASSET_HEIGHT(magpie_suspend_bmp);
        break;
    case BG_BLACK:
        display.fillScreen(DISPLAY_COLOR_BLACK);
        break;
    case BG_RED:
        display.fillScreen(DISPLAY_COLOR_RED);
        break;
    case BG_WHITE:
    default:
        display.fillScreen(DISPLAY_COLOR_WHITE);
    }

    return offset;
}
