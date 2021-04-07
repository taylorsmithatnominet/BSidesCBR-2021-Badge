#include "sketch.h"

#include <Arduino.h>
#include <BadgeOs.h>
#include <BadgeLog.h>
#include <BadgeMenu.h>
#include <BadgeTextViewer.h>
#include <BSidesBadge2020.h>
#include <display.h>
#include <menu.h>
#ifdef HAVE_IMU
#include <SixAxisMotionDriver.h>
#endif // HAVE_IMU

#define CLICKS_TO_ESCAPE            (3U)
#define IMU_THRESH                  (32000)   /* In 2g mode, gravity is ~16k */
#define IMU_SHAKE_SAMPLES           (2U)
#define IMU_STABLE_SAMPLES          (8U)
#define IMU_CHECK_PERIOD_MILLIS     (500U)

// A sketch line
// Can only be vertical or horizontal
struct SketchLine {
    int x;
    int y;
    int length;
    bool vertical;

    SketchLine(int x, int y, int length, bool vertical)
        : x(x)
        , y(y)
        , length(length)
        , vertical(vertical)
    {}
};

// Cursor location
struct SketchCursor {
    int x;
    int y;
};

// Direction of cursor
enum class SketchDirection {
    kNone = 0,
    kUp,
    kDown,
    kLeft,
    kRight,
};

// Game state
enum class SketchState {
    kSetCursor = 0,
    kDraw,
};

/* Forward declares */
static void _sketch_begin(void);
static void _sketch_loop(void);
static void _sketch_end(void);
static void _sketch_keyh(uint8_t key, bool down);
static BadgeApp sketch_app(_sketch_begin, _sketch_loop, _sketch_end, _sketch_keyh);

// Current state of the game
static SketchState state = SketchState::kSetCursor;
// Most recent direction of the cursor
static SketchDirection last_direction = SketchDirection::kNone;
// Location of the cursor
static SketchCursor cursor = { 0 };
// Sketched lines
static std::vector<std::unique_ptr<SketchLine>> lines;
// Toggle whether the screen should be redrawn
static bool draw_required = true;
// Number of times ESCAPE has been clicked
static uint8_t escape_click_count = 0;
#ifdef HAVE_IMU
// Inertial Measurement Unit (IMU)
static SixAxisMotionDriver imu(ESP32_I2C_IMU_ADDR);
// Number of shaking samples counted
static uint8_t shake_sample_count = 0;
// Number of stable samples counted
static uint8_t stable_sample_count = 0;
#endif // HAVE_IMU

#ifdef HAVE_IMU
// Check IMU to see if board is being shaken
// This will erase the lines
static void _check_for_erase_shake(void) {
    log(LOG_DEBUG, "SKETCH: checking for erase shake");
    int16_t ax, ay, az, gx, gy, gz, temp;
    if ( imu.read() ) {
        ax = imu.ax(); ay = imu.ay(); az = imu.az();
        gx = imu.gx(); gy = imu.gy(); gz = imu.gz();
        temp = imu.temp();

        log(
            LOG_DEBUG,
            "SKETCH: imu ax=%d, ay=%d, az=%d, gx=%d, gy=%d, gz=%d, temp=%d\n",
            ax, ay, az, gx, gy, gz, temp
        );

        // Ensure required duration of shaking occurs
        if (az <= -IMU_THRESH || az >= IMU_THRESH) {
            stable_sample_count = 0;
            if(++shake_sample_count >= IMU_SHAKE_SAMPLES) {
                log(LOG_INFO, "SKETCH: shake detected, will erase");
                shake_sample_count = 0;
                _sketch_begin();
            } else {
                log(LOG_DEBUG, "SKETCH: shake count incremented");
            }
        } else {
            if(++stable_sample_count >= IMU_STABLE_SAMPLES) {
                shake_sample_count = 0;
                stable_sample_count = 0;
                log(LOG_DEBUG, "SKETCH: shake count reset");
            }
        }
    }
}
#endif // HAVE_IMU

// Set the start cursor position
static void _set_cursor(void) {
    state = SketchState::kDraw;
    draw_required = true;
}

// Move the cursor position
static void _cursor(SketchDirection direction) {
    escape_click_count = 0;
    switch (direction)
    {
    case SketchDirection::kUp:
        if (cursor.y > 0) {
            cursor.y--;
        }
        break;
    case SketchDirection::kDown:
        if (cursor.y < display.height()) {
            cursor.y++;
        }
        break;
    case SketchDirection::kLeft:
        if (cursor.x > 0) {
            cursor.x--;
        }
        break;
    case SketchDirection::kRight:
        if (cursor.x < display.width()) {
            cursor.x++;
        }
    default:
        break;
    }
    draw_required = true;
}

// Extend or create a new line
static void _line(SketchDirection direction) {
    // Reset the escape click counter
    escape_click_count = 0;
    // Store the original cursor position
    SketchCursor original_cursor = cursor;
    // Move the cursor
    _cursor(direction);
    // Update the lines
    bool vertical = direction == SketchDirection::kUp || direction == SketchDirection::kDown;
    if(direction == last_direction) {
        // Extend existing line
        if(direction == SketchDirection::kUp) lines.back()->y--;
        else if(direction == SketchDirection::kLeft) lines.back()->x--;
        lines.back()->length++;
    } else {
        // Create a new line
        if(direction == SketchDirection::kUp) lines.push_back(std::make_unique<SketchLine>(cursor.x,
                                                                                           cursor.y,
                                                                                           2,
                                                                                           vertical));
        else if(direction == SketchDirection::kDown) lines.push_back(std::make_unique<SketchLine>(cursor.x,
                                                                                                  cursor.y - 1,
                                                                                                  2,
                                                                                                  vertical));
        else if(direction == SketchDirection::kLeft) lines.push_back(std::make_unique<SketchLine>(cursor.x,
                                                                                                  cursor.y,
                                                                                                  2,
                                                                                                  vertical));
        else if(direction == SketchDirection::kRight) lines.push_back(std::make_unique<SketchLine>(cursor.x - 1,
                                                                                                   cursor.y,
                                                                                                   2,
                                                                                                   vertical));
    }
    last_direction = direction;
    draw_required = true;
}

// Draw the screen while in set cursor state
static void _display_cursor(void) {
    log(LOG_DEBUG, "SKETCH: drawing cursor @ %u, %u", cursor.x, cursor.y);
    // Clear the screen
    display.fillScreen(DISPLAY_COLOR_BLACK);
    // Draw cursor
    display.fillRect(cursor.x - 1, cursor.y - 1, 3, 3, DISPLAY_COLOR_WHITE);
    // Draw instructions
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(1, 1);
    display.setCursor(0, 0);
    display.println(" Position cursor and");
    display.println(" then press start.");
#ifdef HAVE_IMU
    display.setCursor(0, display.height() - 8 * 4);
    display.println(" Erase: Shake");
#else
    display.setCursor(0, display.height() - 8 * 3);
#endif // HAVE_IMU
    display.println(" Move:  D-pad");
    display.println(" Start: Enter");
    display.println(" Exit:  Enter (x3)");
    display.display();
}

// Draw the lines to the screen
static void _display_lines(void) {
    // Clear the screen
    display.fillScreen(DISPLAY_COLOR_BLACK);
    for(auto& line: lines) {
        if(line->vertical) {
            display.drawFastVLine(line->x, line->y, line->length, DISPLAY_COLOR_WHITE);
        } else {
            display.drawFastHLine(line->x, line->y, line->length, DISPLAY_COLOR_WHITE);
        }
    }
    display.display();
}

// Escape the game
static void _escape(void) {
    if(++escape_click_count >= CLICKS_TO_ESCAPE) {
        badge_os.pop_app();
    }
}

static void _sketch_begin(void) {
    log(LOG_INFO, "SKETCH: begin");
    state = SketchState::kSetCursor;
    last_direction = SketchDirection::kNone;
    lines.clear();
    cursor = { display.width() / 2, display.height() / 2 };
    draw_required = true;
    escape_click_count = 0;
#ifdef HAVE_IMU
    if(!imu.begin(ESP32_I2C_IMU_SDA, ESP32_I2C_IMU_SCL)) {
        log(LOG_ERROR, "SKETCH: Failed to start IMU");
    }
#endif // HAVE_IMU
}

static void _sketch_loop(void) {
    if(draw_required) {
        draw_required = false;
        switch (state)
        {
        case SketchState::kSetCursor:
            _display_cursor();
            break;
        case SketchState::kDraw:
            _display_lines();
            break;
        default:
            break;
        }
    }
#ifdef HAVE_IMU
    // Check for erase shake periodically
    if(state == SketchState::kDraw) {
        static uint32_t last = millis();
        double delta = millis() - last;
        if(delta >= IMU_CHECK_PERIOD_MILLIS) {
            _check_for_erase_shake();
            last = millis();
        }
    }
#endif
}

static void _sketch_end(void) {
    log(LOG_INFO, "SKETCH: end");
#ifdef HAVE_IMU
    imu.end();
#endif
}

static void _sketch_keyh(uint8_t key, bool down) {
    switch (state)
    {
    case SketchState::kSetCursor:
        if(down && key == BADGE_KEY_UP) _cursor(SketchDirection::kUp);
        else if(down && key == BADGE_KEY_DOWN) _cursor(SketchDirection::kDown);
        else if(down && key == BADGE_KEY_LEFT) _cursor(SketchDirection::kLeft);
        else if(down && key == BADGE_KEY_RIGHT) _cursor(SketchDirection::kRight);
        else if(down && key == BADGE_KEY_ENTER) _set_cursor();
        break;
    case SketchState::kDraw:
        if(down && key == BADGE_KEY_UP) _line(SketchDirection::kUp);
        else if(down && key == BADGE_KEY_DOWN) _line(SketchDirection::kDown);
        else if(down && key == BADGE_KEY_LEFT) _line(SketchDirection::kLeft);
        else if(down && key == BADGE_KEY_RIGHT) _line(SketchDirection::kRight);
        else if(down && key == BADGE_KEY_ENTER) _escape();
        break;
    default:
        break;
    }
}

void sketch_begin(void) {
    BadgeMenuItem *menu_item = game_menu.create_menu_item();
    menu_item->set_name("Sketch");
    menu_item->set_select_event([](){ badge_os.push_app(sketch_app); });
}
