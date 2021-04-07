#include <BadgeKey.h>
#include <BadgeLog.h>
#include <DiagApp.h>

#include <BSidesBadge2020.h>
#include <SixAxisMotionDriver.h>

#include <display.h>

#include <climits>

#include "imu_post.h"

/* IMU POST definitions */
#define IMU_THRESH          (10000)   /* In 2g mode, gravity is ~16k */
#define IMU_BEGIN_DELAY     (3000U)
#define IMU_REDRAW_PERIOD   (1000U)

/* Display definitions */
#define FONT_SIZE       (1U)
#define FONT_SIZE_HDR   (2U * FONT_SIZE)
#define LINE_HEIGHT     (8U * FONT_SIZE)
#define INDENT_PX       (8U)

/* Forward decls and the DiagApp */
static void _begin(void);
static void _loop(void);
static void _end(void);
static void _kh(uint8_t key, bool down);
DiagApp imu_post("IMU", _begin, _loop, _end, _kh);

/* Implementation */
static SixAxisMotionDriver imu(ESP32_I2C_IMU_ADDR);
static uint32_t g_loops = 0;

static size_t _draw_frame(void) {
    size_t y = display.height() / 2;

    display.fillScreen(DISPLAY_COLOR_BLACK);
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(FONT_SIZE, FONT_SIZE);
    display.setCursor(INDENT_PX, y); y += LINE_HEIGHT * 2;
    display.print("- IMU test -");

    return y;
}

static unsigned state;

static void _begin(void) {
    size_t y;

#ifdef ESP32
    display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
#endif
    y = _draw_frame();
    if ( imu.begin(ESP32_I2C_IMU_SDA, ESP32_I2C_IMU_SCL) ) {
        state = 0;
        log(LOG_INFO, "Ready to start IMU POST");
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT * 2;
        display.print("LEFT:  Fail test");
        display.setCursor(0, y); y += LINE_HEIGHT * 4;
        display.print(
            " You will need to   \n"
            " pick up the board  \n"
            " to test gravity on \n"
            " each axis.\n"
            " Hold left to fail  \n"
            " test at any time"
        );
    } else {
        log(LOG_ERROR, "IMU RESET FAILED");
        display.print(
            " TEST FAILED:       \n"
            " Unable to init IMU"
        );
        imu_post.test_complete(DiagAppStatus::kFail);
    }
    display.display();
    /* We (ab)use a short delay to ensure IMU will get good readings and the
     * screen will actually refresh.
     */
    delay(IMU_BEGIN_DELAY);
}

static void _end(void) {
    imu.end();
}

static void _loop(void) {
    static uint32_t last = 0;
    size_t y;
    int16_t ax, ay, az, gx, gy, gz, temp;
    uint32_t delta, now = millis();

    if ( imu.read() ) {
        ax = imu.ax(); ay = imu.ay(); az = imu.az();
        gx = imu.gx(); gy = imu.gy(); gz = imu.gz();
        temp = imu.temp();

        log(LOG_DEBUG, "IMU POST state %u", state);
        log(
            LOG_DEBUG,
            "IMU: ax=%d, ay=%d, az=%d, gx=%d, gy=%d, gz=%d, temp=%d\n",
            ax, ay, az, gx, gy, gz, temp
        );

        switch (state) {
        case 0:
            if (ay <= -IMU_THRESH || ay >= IMU_THRESH) {
                log(LOG_INFO, "Passed Y axis test - %hi", ax);
                state++;
            }
            break;
        case 1:
            if (ax <= -IMU_THRESH || ax >= IMU_THRESH) {
                log(LOG_INFO, "Passed X axis test - %hi", ay);
                state++;
            }
            break;
        case 2:
            if (az <= -IMU_THRESH || az >= IMU_THRESH) {
                log(LOG_INFO, "Passed Z axis test - %hi", az);
                state++;
            }
            break;
        case 3:
            log(LOG_INFO, "IMU POST passed");
            imu_post.test_complete(DiagAppStatus::kPass);
            break;
        default:
            log(LOG_ERROR, "Invalid IMU POST state");
            imu_post.test_complete(DiagAppStatus::kFail);
        }
    } else {
        log(LOG_ERROR, "IMU READ FAILED");
        imu_post.test_complete(DiagAppStatus::kFail);
        state = UINT_MAX;
    }

    if (now < last) {
        /* assume single wrap */
        delta = UINT32_MAX - (last - now);
    } else {
        delta = now - last;
    }
    if (imu_post.status == DiagAppStatus::kPending && delta >= IMU_REDRAW_PERIOD) {
        last = now;
        y = _draw_frame();
        display.setCursor(0, y);
        switch (state) {
        case 0: display.println("Y axis - hold up facing you"); break;
        case 1: display.println("X axis - turn 90 degrees"); break;
        case 2: display.println("Z axis - put board down"); break;
        case 3: display.println("Test complete!"); break;
        default: display.println("Test error");
        }
        if (state <= 2) display.println("LEFT:  Fail test");
        display.display();
    }
}
static void _kh(uint8_t key, bool down) {
    if (down && key == BADGE_KEY_LEFT) {
        log(LOG_ERROR, "Failed IMU POST due to user input");
        imu_post.test_complete(DiagAppStatus::kFail);
    }
}
