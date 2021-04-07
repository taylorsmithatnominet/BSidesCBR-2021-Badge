#include <unordered_set>

#include "bootanimation.h"

#include <display.h>
#include <BadgeAssets.h>
#include <BadgeKey.h>
#include <BadgeLog.h>
#include <asset/bsidescbr_bmp.h>
#include <asset/penten_bmp.h>
#include <diagnostics.h>

BadgeApp        bootanimation_app;
BadgeService    bootanimation_service;

/* We have a well defined set of bootkey combinations to match against */
static const std::unordered_set<uint8_t> SKIP{BADGE_KEY_ENTER, BADGE_KEY_RIGHT};
static const std::unordered_set<uint8_t> DIAG{BADGE_KEY_UP, BADGE_KEY_DOWN};

void bootanimation_begin()
{
    bootanimation_service.set_loop([](){
        static uint8_t state = 0;
        bootanimation_service.set_period_ms(BOOT_IMAGES_DELAY_MS); // time between transistion
        switch (state)
        {
            case 0:
#ifdef ESP32
                display.setMode(Adafruit_SSD1675BX::kModeBlackWhiteRed);
#endif
                // Show image
                display.fillScreen(DISPLAY_COLOR_WHITE);
                DRAW_ASSET_CENTER(bsidescbr_bmp, display);
                // Show bootkey shortcuts
                display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE);
                display.setTextSize(1, 1);
                display.setCursor(8, 272);
                display.print("SKIP: enter + right");
                display.setCursor(8, 280);
                display.print("DIAG: up + down");
                display.display();
                break;
            case 1:
                display.fillScreen(DISPLAY_COLOR_WHITE);
                DRAW_ASSET_CENTER(penten_bmp, display);
                display.display();
                break;
            default:
                bootanimation_service.stop();
                badge_os.pop_app();
                break;
        }
        state++;
    });
    badge_os.add_service(bootanimation_service);
    bootanimation_app.set_begin([](){ bootanimation_service.start(); });
    /* We install a key event handler for bootkey combinations */
    bootanimation_app.set_key_event([](uint8_t key, bool key_down){
        static std::unordered_set<uint8_t> bootkeys;
        if (key_down) bootkeys.insert(key);
        else bootkeys.erase(key);
        /* Check against known bootkey combos */
        if (bootkeys == SKIP) {
            log(LOG_INFO, "Skipping boot animation");
            bootanimation_service.stop();
            /* Pop until we can't pop no more - we expect to want to skip to
             * the topmost registered app.
             */
            while (badge_os.pop_app());
        }
        else if (bootkeys == DIAG) {
            log(LOG_INFO, "Entering diagnostics mode");
            bootanimation_service.stop();
            while (badge_os.pop_app());
            badge_os.push_app(diagnostics_app);
        }
    });
}
