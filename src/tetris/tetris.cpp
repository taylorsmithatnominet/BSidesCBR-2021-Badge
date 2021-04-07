#include "tetris.h"

#include <display.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <menu.h>
#include <tetrisc.h>
#ifdef ESP32
#include <soc/rtc.h>
#endif

#define TETRIS_GRID_WIDTH        10
#define TETRIS_GRID_HEIGHT       20
#define TETRIS_SQUARE_SIZE       7
#define TETRIS_GRID_OFFSET_X     40
#define TETRIS_GRID_OFFSET_Y     68
#define TETRIS_NEXT_OFFSET_X     2
#define TETRIS_NEXT_OFFSET_Y     120
#define TETRIS_BOARDER_INDENT    2
#define TETRIS_BOARDER_WIDTH     ((TETRIS_GRID_WIDTH*TETRIS_SQUARE_SIZE)+(2*TETRIS_BOARDER_INDENT))
#define TETRIS_BOARDER_HEIGTH    ((TETRIS_GRID_HEIGHT*TETRIS_SQUARE_SIZE)+(2*TETRIS_BOARDER_INDENT))

BadgeApp            tetris_app;

uint8_t             tetris_data[TTRS_CALC_DATA_SIZE(TETRIS_GRID_WIDTH, TETRIS_GRID_HEIGHT)];

#ifdef ESP32
static rtc_cpu_freq_config_t backup_config;
static uint8_t backup_display_mode;
#endif

void TTRS_API tetrisc_begin_frame(void *ctx)
{
    display.fillScreen(DISPLAY_COLOR_BLACK);
    size_t y = display_draw_background(BG_DEFAULT);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(8, 0);
    display.setTextSize(2, 2);
    char score_text[20];
    sprintf(score_text, "%8d", ttrs_get_score(tetris_data) * 100);
    display.print(score_text);
    display.drawRect(TETRIS_GRID_OFFSET_X, TETRIS_GRID_OFFSET_Y, TETRIS_BOARDER_WIDTH, TETRIS_BOARDER_HEIGTH, DISPLAY_COLOR_RED);
}

void TTRS_API tetrisc_end_frame(void *ctx)
{
    display.setCursor(TETRIS_NEXT_OFFSET_X + TETRIS_SQUARE_SIZE, TETRIS_NEXT_OFFSET_Y - TETRIS_SQUARE_SIZE);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setTextSize(1, 1);
    display.print("NEXT");
    display.writeFastHLine(0, 16, display.width(), DISPLAY_COLOR_WHITE);
    display.display();
}

void TTRS_API tetrisc_draw_piece(void *ctx, int16_t x,int16_t y, TTRS_PIECE_TYPE type)
{
    x = TETRIS_GRID_OFFSET_X + TETRIS_BOARDER_INDENT + (x * TETRIS_SQUARE_SIZE);
    y = TETRIS_GRID_OFFSET_Y + TETRIS_BOARDER_INDENT + (y * TETRIS_SQUARE_SIZE);
    display.drawRect(x, y, TETRIS_SQUARE_SIZE, TETRIS_SQUARE_SIZE, DISPLAY_COLOR_WHITE);
}

void TTRS_API tetrisc_draw_next_piece(void *ctx, int16_t x,int16_t y, TTRS_PIECE_TYPE type)
{
    x = TETRIS_NEXT_OFFSET_X + (x * TETRIS_SQUARE_SIZE);
    y = TETRIS_NEXT_OFFSET_Y + (y * TETRIS_SQUARE_SIZE);
    display.drawRect(x, y, TETRIS_SQUARE_SIZE, TETRIS_SQUARE_SIZE, DISPLAY_COLOR_RED);
}

void TTRS_API tetrisc_draw_block(void *ctx, int16_t x,int16_t y)
{
    x = TETRIS_GRID_OFFSET_X + TETRIS_BOARDER_INDENT + (x * TETRIS_SQUARE_SIZE);
    y = TETRIS_GRID_OFFSET_Y + TETRIS_BOARDER_INDENT + (y * TETRIS_SQUARE_SIZE);
    display.fillRect(x, y, TETRIS_SQUARE_SIZE, TETRIS_SQUARE_SIZE, DISPLAY_COLOR_RED);
}

int16_t TTRS_API tetrisc_random(void *ctx, int16_t min, int16_t max)
{
#ifdef __unix__
    return min + ((uint16_t)random() % (max  + 1 - min));
#else
    return random(min, max + 1);
#endif
}

void TTRS_API tetrisc_game_over(void *ctx, uint16_t score)
{
    badge_os.pop_app();
}

void tetris_begin(void)
{
    // add tetris to the game menu
    BadgeMenuItem *menu_item = game_menu.create_menu_item();
    menu_item->set_name("Tetris");
    menu_item->set_select_event([](){ badge_os.push_app(tetris_app); });

    // wrap tetris in application
    tetris_app.set_begin([](){
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

        // tetris init
        ttrs_init(tetris_data, sizeof(tetris_data));
        ttrs_set_grid(tetris_data, TETRIS_GRID_WIDTH, TETRIS_GRID_HEIGHT);
        ttrs_set_draw_clear(tetris_data, tetrisc_begin_frame, NULL);
        ttrs_set_draw_piece(tetris_data, tetrisc_draw_piece, NULL);
        ttrs_set_draw_next_piece(tetris_data, tetrisc_draw_next_piece, NULL);
        ttrs_set_draw_block(tetris_data, tetrisc_draw_block, NULL);
        ttrs_set_random(tetris_data, tetrisc_random, NULL);
        ttrs_set_game_over(tetris_data, tetrisc_game_over, NULL);
        ttrs_reset(tetris_data);
    });
    tetris_app.set_loop([](){
        static uint32_t last_tick = 0;
        uint32_t tick = (uint32_t)millis();
        if ( (tick - last_tick) > ((uint32_t)700) )
        {
            last_tick = tick;
            ttrs_tick(tetris_data);
            tetrisc_end_frame(NULL);
        }
    });
    tetris_app.set_end([](){
        // tetris fini
        ttrs_fini(tetris_data);

#ifdef ESP32
        // restore display mode
        display.setMode(backup_display_mode);

        // restore cpu freq
        rtc_clk_cpu_freq_set_config(&backup_config);
#endif
    });
    tetris_app.set_key_event([](uint8_t key, bool key_down){
        if ( !key_down )
        {
            return;
        }
        switch (key)
        {
            case BADGE_KEY_LEFT:
                ttrs_key_left(tetris_data);
                tetrisc_end_frame(NULL);
                break;
            case BADGE_KEY_UP:
                ttrs_key_rotate(tetris_data);
                tetrisc_end_frame(NULL);
                break;
            case BADGE_KEY_RIGHT:
                ttrs_key_right(tetris_data);
                tetrisc_end_frame(NULL);
                break;
            case BADGE_KEY_DOWN:
                ttrs_key_down(tetris_data);
                tetrisc_end_frame(NULL);
                break;
            case BADGE_KEY_ENTER:
                ttrs_key_drop(tetris_data);
                tetrisc_end_frame(NULL);
                break;
            default:
                break;
        }
    });
}
