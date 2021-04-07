#include "snake.h"

#include <display.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <menu.h>
#include <snakec.h>
#ifdef ESP32
#include <soc/rtc.h>
#endif

#define SNAKE_GRID_WIDTH        20
#define SNAKE_GRID_HEIGHT       20
#define SNAKE_SQUARE_SIZE       6
#define SNAKE_GRID_OFFSET_X     2
#define SNAKE_GRID_OFFSET_Y     75
#define SNAKE_BOARDER_INDENT    2
#define SNAKE_BOARDER_WIDTH     ((SNAKE_GRID_WIDTH*SNAKE_SQUARE_SIZE)+(2*SNAKE_BOARDER_INDENT))
#define SNAKE_BOARDER_HEIGTH    ((SNAKE_GRID_HEIGHT*SNAKE_SQUARE_SIZE)+(2*SNAKE_BOARDER_INDENT))

BadgeApp            snake_app;

static uint8_t      snake_data[SNKC_CALC_DATA_SIZE(SNAKE_GRID_WIDTH, SNAKE_GRID_HEIGHT)];

#ifdef ESP32
static rtc_cpu_freq_config_t backup_config;
static uint8_t backup_display_mode;
#endif

void SNKC_API snakec_begin_frame(void *ctx)
{
    display.fillScreen(DISPLAY_COLOR_BLACK);
    size_t y = display_draw_background(BG_DEFAULT);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(8, 0);
    display.setTextSize(2, 2);
    char score_text[20];
    sprintf(score_text, "%8d", snkc_get_score(snake_data) * 100);
    display.print(score_text);
    display.drawRect(SNAKE_GRID_OFFSET_X, SNAKE_GRID_OFFSET_Y, SNAKE_BOARDER_WIDTH, SNAKE_BOARDER_HEIGTH, DISPLAY_COLOR_RED);
}

void SNKC_API snakec_end_frame(void *ctx)
{
    display.writeFastHLine(0, 16, display.width(), DISPLAY_COLOR_WHITE);
    display.display();
}

void SNKC_API snakec_draw_snake(void *ctx, int16_t x,int16_t y)
{
    x = SNAKE_GRID_OFFSET_X + SNAKE_BOARDER_INDENT + (x * SNAKE_SQUARE_SIZE);
    y = SNAKE_GRID_OFFSET_Y + SNAKE_BOARDER_INDENT + (y * SNAKE_SQUARE_SIZE);
    display.fillRect(x, y, SNAKE_SQUARE_SIZE, SNAKE_SQUARE_SIZE, DISPLAY_COLOR_WHITE);
}

void SNKC_API snakec_draw_apple(void *ctx, int16_t x,int16_t y)
{
    x = SNAKE_GRID_OFFSET_X + SNAKE_BOARDER_INDENT + (x * SNAKE_SQUARE_SIZE);
    y = SNAKE_GRID_OFFSET_Y + SNAKE_BOARDER_INDENT + (y * SNAKE_SQUARE_SIZE);
    display.fillCircle(x+((SNAKE_SQUARE_SIZE-1)/2), y+((SNAKE_SQUARE_SIZE-1)/2), ((SNAKE_SQUARE_SIZE-1)/2) + 1, DISPLAY_COLOR_RED);
}

int16_t SNKC_API snakec_random(void *ctx, int16_t min, int16_t max)
{
#ifdef __unix__
    return min + ((uint16_t)random() % (max  + 1 - min));
#else
    return random(min, max + 1);
#endif
}

void SNKC_API snakec_game_over(void *ctx, uint16_t score)
{
    badge_os.pop_app();
}

void snake_begin(void)
{
    // add snake to the game menu
    BadgeMenuItem *menu_item = game_menu.create_menu_item();
    menu_item->set_name("Snake");
    menu_item->set_select_event([](){ badge_os.push_app(snake_app); });

    // wrap snake in application
    snake_app.set_begin([](){
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

        // init snake
        snkc_init(snake_data, sizeof(snake_data));
        snkc_set_grid(snake_data, SNAKE_GRID_WIDTH, SNAKE_GRID_HEIGHT);
        snkc_set_draw_clear(snake_data, snakec_begin_frame, NULL);
        snkc_set_draw_snake(snake_data, snakec_draw_snake, NULL);
        snkc_set_draw_apple(snake_data, snakec_draw_apple, NULL);
        snkc_set_random(snake_data, snakec_random, NULL);
        snkc_set_game_over(snake_data, snakec_game_over, NULL);
        snkc_reset(snake_data);
    });
    snake_app.set_loop([](){
        static uint32_t last_tick = 0;
        uint32_t tick = (uint32_t)millis();
        // Update screen every 66ish ms
        if ( (tick - last_tick) > ((uint32_t)(1000/15)) )
        {
            // 100ms game tick
            if(tick - last_tick > 100)
            {
                last_tick = tick;
                snkc_tick(snake_data);
            }
            // Always force a screen refresh to keep the e-ink updating
            snakec_end_frame(NULL);
        }
    });
    snake_app.set_end([](){
        // fini snake
        snkc_fini(snake_data);

#ifdef ESP32
        // restore display mode
        display.setMode(backup_display_mode);

        // restore cpu freq
        rtc_clk_cpu_freq_set_config(&backup_config);
#endif
    });
    snake_app.set_key_event([](uint8_t key, bool key_down){
        switch (key)
        {
            case BADGE_KEY_LEFT:
                snkc_key_left(snake_data);
                break;
            case BADGE_KEY_UP:
                snkc_key_up(snake_data);
                break;
            case BADGE_KEY_RIGHT:
                snkc_key_right(snake_data);
                break;
            case BADGE_KEY_DOWN:
                snkc_key_down(snake_data);
                break;
            case BADGE_KEY_ENTER:
                if ( key_down )
                {
                    badge_os.pop_app();
                }
                break;
            default:
                break;
        }
    });
}
