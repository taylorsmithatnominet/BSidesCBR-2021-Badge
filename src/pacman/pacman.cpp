#include "pacman.h"

#include <display.h>
#include <BSidesBadge2020.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <menu.h>
#include <Pacman.h>
#ifdef ESP32
#include <soc/rtc.h>
#endif

pacman::Engine      pacman_game;
BadgeApp            pacman_app;

bool init_pacman = true;
bool exit_pacman = false;

#define PACMAN_SCREEN_UNKNOWN           0
#define PACMAN_SCREEN_TITLE             1
#define PACMAN_SCREEN_IN_GAME           2
#define PACMAN_SCREEN_GAME_OVER         3
uint8_t screen_detected_type = PACMAN_SCREEN_UNKNOWN;
uint8_t screen_type = PACMAN_SCREEN_UNKNOWN;

#ifdef ESP32
static rtc_cpu_freq_config_t backup_config;
static uint8_t backup_display_mode;
#endif

static void pacman_begin_once(void)
{
    if ( !init_pacman )
    {
        return;
    }
    init_pacman = false;
    pacman_game.SetBeginFrame([](){
        display.fillScreen(DISPLAY_COLOR_BLACK);
        // assume in-game unless special strings detected
        screen_detected_type = PACMAN_SCREEN_IN_GAME;
    });
    pacman_game.SetEndFrame([](){
#ifdef ESP32
        if ( screen_detected_type != screen_type )
        {
            // screen changed - change eink mode
            if ( screen_detected_type == PACMAN_SCREEN_IN_GAME )
            {
                // as fast as possible
                display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
            }
            else
            {
                // better picture
                display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
            }
        }
#endif
        if ( screen_detected_type == PACMAN_SCREEN_IN_GAME )
        {
            // show ever in-game frame
            display.display();
        }
        else if ( screen_detected_type != screen_type )
        {
            // screen changed - draw it once
            display.display();
        }
        screen_type = screen_detected_type;
    });
    pacman_game.SetGetTickCountMs([](){
#ifdef ESP32
        return (uint32_t)millis() >> 1;  // slow down time, eink & esp32 not that fast
#else
        return (uint32_t)millis();
#endif
    });
    pacman_game.SetDrawPixel([](int16_t x, int16_t y, uint16_t color){
        if ( color == 0x019F)
        {
            // small hack to show ghost's eyes on black/white eink
            color = 0x0000;
        }
        display.drawPixel(x, y, color);
    });
    pacman_game.SetDrawString8x8([](int16_t x, int16_t y, const char *text, uint16_t color){
        // not in-game as "PRESS ENTER" is displayed.
        if ( NULL != strstr(text, "PIN") )
        {
            // title screen ("PINKY" is displayed)
            screen_detected_type = PACMAN_SCREEN_TITLE;
        }
        if ( NULL != strstr(text, "SCORES") )
        {
            // game-over screen
            screen_detected_type = PACMAN_SCREEN_GAME_OVER;
        }
        display.setTextColor(color, DISPLAY_COLOR_BLACK);
        display.setTextSize(1, 1);
        while ( text && *text )
        {
            display.setCursor(x, y);
            display.print(text[0]);
            text++;
            x += 8;
        }
    });
    pacman_game.SetGameOver([](uint16_t score) {
        exit_pacman = true;
    });
    pacman_game.Initialize();
}

void pacman_begin(void)
{
    // add pacman to the game menu
    BadgeMenuItem *menu_item = game_menu.create_menu_item();
    menu_item->set_name("Pacman");
    menu_item->set_select_event([](){ badge_os.push_app(pacman_app); });

    // wrap pacman in application
    pacman_app.set_begin([](){
#ifdef ESP32
        // overclock esp32 to 240 MHz (max speed)
        rtc_clk_cpu_freq_get_config(&backup_config);
        rtc_cpu_freq_config_t config;
        rtc_clk_cpu_freq_mhz_to_config(240, &config);
        rtc_clk_cpu_freq_set_config(&config);

        // fastest display mode
        backup_display_mode = display.getMode();
        display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
#endif

        // don't know yet what screen is displayed
        screen_type = PACMAN_SCREEN_UNKNOWN;

        // pacman init
        exit_pacman = false;
        pacman_begin_once();
    });
    pacman_app.set_loop([](){
        pacman_game.RunOnce();
        if ( exit_pacman )
        {
            badge_os.pop_app();
        }
    });
    pacman_app.set_end([](){
        // pacman fini - do nothing, pacman is not a very good "port"

#ifdef ESP32
        // restore display mode
        display.setMode(backup_display_mode);

        // restore cpu freq
        rtc_clk_cpu_freq_set_config(&backup_config);
#endif
    });
    pacman_app.set_key_event([](uint8_t key, bool key_down){
        if ( !key_down )
        {
            return;
        }
        switch (key)
        {
            case BADGE_KEY_ENTER:
                pacman_game.KeyEvent(PM_SCANCODE_ENTER, true);
                pacman_app.loop();
                pacman_game.KeyEvent(PM_SCANCODE_ENTER, false);
                break;
            case BADGE_KEY_LEFT:
                pacman_game.KeyEvent(PM_SCANCODE_LEFT, true);
                pacman_app.loop();
                pacman_game.KeyEvent(PM_SCANCODE_LEFT, false);
                break;
            case BADGE_KEY_UP:
                pacman_game.KeyEvent(PM_SCANCODE_UP, true);
                pacman_app.loop();
                pacman_game.KeyEvent(PM_SCANCODE_UP, false);
                break;
            case BADGE_KEY_RIGHT:
                pacman_game.KeyEvent(PM_SCANCODE_RIGHT, true);
                pacman_app.loop();
                pacman_game.KeyEvent(PM_SCANCODE_RIGHT, false);
                break;
            case BADGE_KEY_DOWN:
                pacman_game.KeyEvent(PM_SCANCODE_DOWN, true);
                pacman_app.loop();
                pacman_game.KeyEvent(PM_SCANCODE_DOWN, false);
                break;
            default:
                break;
        }
    });
}
