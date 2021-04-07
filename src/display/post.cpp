#include <cstdint>
#include <cstring>

#include <Arduino.h>

#include <BadgeKey.h>
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <DiagApp.h>

#include "display.h"

/* Non-debug POSTs simply attempt to demonstrate that BWR mode works */
#ifndef DEBUG
#  define DISPLAY_POST_FAST
#else //DEBUG
/* Uncomment this to use the manual "click to proceed" mode */
//#  define DISPLAY_POST_MANUAL
#endif//DEBUG
/* We introduce a fixed delay when using fast modes so the user can see the
 * transitions properly and react accordingly.
 */
#define FAST_MODE_DELAY_MS (3000U)

/* Forward declare the delegate functions */
static void _begin(void);
static void _keyh(uint8_t key, bool down);

/* We only define and run a loop in slow POSTs */
#ifdef  DISPLAY_POST_FAST
static const std::nullptr_t _loop = nullptr;
#else //DISPLAY_POST_FAST
static void _loop(void);
#endif//DISPLAY_POST_FAST

/* Construct the diagnostic app with delegates and then expose a shared pointer
 * to it which is what the diagnostics app will link against.
 */
static DiagApp _display_post("Display", _begin, _loop, nullptr, _keyh);
std::shared_ptr<DiagApp> display_post{
    std::shared_ptr<DiagApp>{}, &_display_post
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define FONT_SIZE   (1U)
#define LINE_HEIGHT (8U * FONT_SIZE)
#define INDENT_PX   (8U)

static void _begin(void) {
    size_t y = display.height() / 2;

#ifdef  DISPLAY_POST_FAST

#ifdef ESP32
    display.setMode(Adafruit_SSD1675BX::kModeBlackWhiteRed);
#endif

    size_t w = display.width(), h = display.height() / 3;
    display.fillRect(0, 0, w, h, DISPLAY_COLOR_RED);
    display.fillRect(0, h, w, 2*h, DISPLAY_COLOR_WHITE);
    display.fillRect(0, 2*h, w, display.height(), DISPLAY_COLOR_BLACK);

    display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_WHITE);
    display.setCursor(INDENT_PX, h + LINE_HEIGHT);
    display.print("RIGHT: Pass test");
    display.setCursor(INDENT_PX, h + LINE_HEIGHT * 2);
    display.print("LEFT: Fail test");

#else //DISPLAY_POST_FAST

/* The debug POST is more in depth and tests all modes */

#ifdef ESP32
    display.setMode(Adafruit_SSD1675BX::kModeNormalBlackWhite);
#endif
    display.fillScreen(DISPLAY_COLOR_BLACK);
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(FONT_SIZE, FONT_SIZE);

    display.setCursor(INDENT_PX, y); y+= LINE_HEIGHT * 2;
    display.print("- Display test -");
#ifdef  DISPLAY_POST_MANUAL
    display.setCursor(INDENT_PX, y); y+= LINE_HEIGHT;
    display.print("RIGHT: Begin/proceed");
#endif//DISPLAY_POST_MANUAL
    display.setCursor(INDENT_PX, y); y+= LINE_HEIGHT * 2;
    display.print("LEFT:  Fail test");
    display.setCursor(0, y); y+= LINE_HEIGHT * 4;
    display.print(
        " If screen does not \n"
        " render as expected,\n"
        " hold left to fail  \n"
        " at any time"
    );

#endif//DISPLAY_POST_FAST

    display.display();
}

#ifndef DISPLAY_POST_FAST

static const uint8_t modes[] = {
#ifdef ESP32
    Adafruit_SSD1675BX::kModeBlackWhiteRed,
    Adafruit_SSD1675BX::kModeNormalBlackWhite,
    Adafruit_SSD1675BX::kModeFasterBlackWhite,
    Adafruit_SSD1675BX::kModeFastestBlackWhite,
    /* Do this last so we clean the screen before returning to diag menu */
    Adafruit_SSD1675BX::kModeFullBlackWhite,
#endif
};
typedef enum {
    FRAME_WHITE,
    FRAME_CIRCLE,
    FRAME_RED,
    FRAME_BLACK,
    _FRAME_MAX,
    _FRAME_INVALID = _FRAME_MAX,
} frame_e;

#ifdef  DISPLAY_POST_MANUAL
static bool _proceed = false;
#endif//DISPLAY_POST_MANUAL

static void _loop(void) {
    static size_t iter = 0;
    static const size_t x_midline = display.width() / 2;
    static const size_t w_shape = display.width() / 4;

#ifdef  DISPLAY_POST_MANUAL
    if (!_proceed) return;
    _proceed = false;
#endif//DISPLAY_POST_MANUAL

    size_t frame = iter++ % _FRAME_MAX;
    if (frame == 0) {
        if (iter >= ARRAY_SIZE(modes) * _FRAME_MAX) {
            log(LOG_INFO, "Completed display diagnostic");
            _display_post.test_complete(DiagAppStatus::kPass);
            return;
        }
        size_t mode_idx = iter / _FRAME_MAX;
        log(LOG_INFO, "Set next mode (%zu -> %u)", mode_idx, modes[mode_idx]);
#ifdef ESP32
        display.setMode(modes[mode_idx]);
#endif
    }
    log(LOG_INFO, "Frame %zu of current mode", frame);

    char buf[32] = {0};
    const char *msg;
    /* The subtraction at the end here is to toggle the rendered text up and
     * down on the y axis each frame so it doesn't render over itself.
     */
    size_t y = display.height() / 2 - (LINE_HEIGHT * (iter & 0x1 ? 2 : 0));
    uint16_t bg, fg;
    switch (frame) {
    case FRAME_BLACK:
        bg = DISPLAY_COLOR_BLACK;
        msg = "WHITE ON BLACK";
        break;
    case FRAME_RED:
        bg = DISPLAY_COLOR_RED;
#ifdef ESP32
        if (display.getMode() == Adafruit_SSD1675BX::kModeBlackWhiteRed)
            msg = "WHITE ON RED";
        else
            msg = "WHITE ON BLACK BOX";
#else
        msg = "RED, BLACK & WHITE";
#endif
        break;
    case FRAME_WHITE:
    default:
        bg = DISPLAY_COLOR_WHITE;
        msg = "BLACK ON WHITE";
    }
    fg = (bg == DISPLAY_COLOR_RED) ? DISPLAY_COLOR_WHITE : ~bg;

    display.fillScreen(bg);
    display.setTextColor(fg, bg);
    display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
    display.print(msg);
    display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
    snprintf(
        buf, sizeof(buf), "Step %zu of %zu",
        iter, ARRAY_SIZE(modes) * _FRAME_MAX
    );
    display.print(buf);

    switch (frame) {
    case FRAME_CIRCLE:
        display.drawCircle(x_midline, y, w_shape, fg);
        break;
    }

#if  DISPLAY_POST_MANUAL
    display.setCursor(INDENT_PX, display.height() - LINE_HEIGHT * 2);
    display.print("Press right to proceed");
#endif//DISPLAY_POST_MANUAL

    display.display();
#ifdef ESP32
    if (display.fastMode()) delay(FAST_MODE_DELAY_MS);
#endif
}

#endif//DISPLAY_POST_FAST

static void _keyh(uint8_t key, bool down) {
    log(LOG_INFO, "Received KEY_%s %u", down ? "DOWN" : "UP", key);
    if (!down) return;
    switch (key) {
    case BADGE_KEY_RIGHT:
#if   defined(DISPLAY_POST_FAST)
        _display_post.test_complete(DiagAppStatus::kPass);
#elif defined(DISPLAY_POST_MANUAL)
        _proceed = true;
#endif
        break;
    case BADGE_KEY_LEFT:
        _display_post.test_complete(DiagAppStatus::kFail);
        break;
    }
}
