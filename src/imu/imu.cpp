#include "imu.h"

#include <math.h>
#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeMenu.h>
#include <BSidesBadge2020.h>
#include <display.h>
#include <menu.h>
#include <SixAxisMotionDriver.h>

BadgeApp imu_app;

#ifdef ESP32
static SixAxisMotionDriver imu(ESP32_I2C_IMU_ADDR);
static uint8_t backup_display_mode;
#endif

uint32_t g_last_update = 0;

typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t temp;
} imu_data_t;

void imu_begin(void)
{
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("IMU");
    menu_item->set_select_event([](){
        badge_os.push_app(imu_app);
    });

    imu_app.set_begin([](){
#ifdef ESP32
        backup_display_mode = display.getMode();
        display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
        if ( !imu.begin(ESP32_I2C_IMU_SDA, ESP32_I2C_IMU_SCL) ) {
            Serial.println("IMU RESET FAILED");
            return;
        }
#endif
        g_last_update = 0;
    });
    imu_app.set_loop([](){
        static int32_t num_imu_datas = 8;
        static imu_data_t imu_datas[8] = {};
        static int32_t next_imu_data = 0;

        uint32_t tick = (uint32_t)millis();
        if ( (tick - g_last_update) < 40 )
        {
            // too quick
            return;
        }
        g_last_update = tick;

        imu_data_t *imu_data = &imu_datas[next_imu_data];
        next_imu_data = (next_imu_data + 1) % num_imu_datas;
        imu_data->ax = -1;
        imu_data->ay = -1;
        imu_data->az = -1;
        imu_data->gx = -1;
        imu_data->gy = -1;
        imu_data->gz = -1;
        imu_data->temp = 0;
#ifdef ESP32
        if ( !imu.read() ) {
            Serial.println("IMU READ FAILED");
        }
        else {
            // IMU is rotated 90 degrees anticlockwise
            // when compared with the screen orientation
            // screen X is negative IMU Y
            // screen Y is IMU X
            imu_data->ax = -imu.ay();
            imu_data->ay = imu.ax();
            imu_data->az = imu.az();
            imu_data->gx = -imu.gy();
            imu_data->gy = imu.gx();
            imu_data->gz = imu.gz();
            imu_data->temp = imu.temp();
        }
#endif
        char buf[256];
        sprintf(buf, "IMU: ax=%d, ay=%d, az=%d, gx=%d, gy=%d, gz=%d, temp=%d",
            imu_data->ax, imu_data->ay, imu_data->az, imu_data->gx, imu_data->gy, imu_data->gz, imu_data->temp);
        Serial.println(buf);

        display.fillScreen(DISPLAY_COLOR_BLACK);
        size_t y = display_draw_background(BG_DEFAULT);
        display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
        display.setCursor(8, 0);
        display.setTextSize(2, 2);
        display.print("IMU");

        int32_t centre_x = 64;
        int32_t centre_y = 128;
        int32_t max_off = 56;

        for (int32_t i = 0; i < num_imu_datas; i++) {
            imu_data_t *dat = &imu_datas[i];
            int32_t offset_x = (dat->ay * max_off) / INT16_MAX;
            int32_t offset_y = (dat->ax * max_off) / INT16_MAX;
            int32_t arrow_x = centre_x + offset_x;
            int32_t arrow_y = centre_y + offset_y;

            display.drawLine(centre_x, centre_y, arrow_x, arrow_y, DISPLAY_COLOR_WHITE);
            // Make the line thicker
            display.drawLine(centre_x + 1, centre_y, arrow_x + 1, arrow_y, DISPLAY_COLOR_WHITE);
            display.drawLine(centre_x - 1, centre_y, arrow_x - 1, arrow_y, DISPLAY_COLOR_WHITE);
            display.drawLine(centre_x, centre_y + 1, arrow_x, arrow_y + 1, DISPLAY_COLOR_WHITE);
            display.drawLine(centre_x, centre_y - 1, arrow_x, arrow_y - 1, DISPLAY_COLOR_WHITE);

            double mag = sqrt(offset_x * offset_x + offset_y * offset_y);
            if (mag > 16) {
                // Draw an arrow
                double rad_of = M_PI / 12;// 15 degrees
                double arrow_len = 12;

                double tip_x = offset_x;
                double tip_y = offset_y;

                double rad1 = atan(tip_y / tip_x);
                // atan returns within [-pi/2, pi/2], recover the full angle
                if (offset_x < 0) {
                    rad1 = rad1 + M_PI;
                }

                double rad2 = rad1 + M_PI + rad_of;
                double of_x1 = cos(rad2) * arrow_len;
                double of_y1 = sin(rad2) * arrow_len;

                double rad3 = rad1 + M_PI - rad_of;
                double of_x2 = cos(rad3) * arrow_len;
                double of_y2 = sin(rad3) * arrow_len;

                int32_t p0x = centre_x + offset_x;
                int32_t p0y = centre_y + offset_y;
                int32_t p1x = centre_x + offset_x + of_x1;
                int32_t p1y = centre_y + offset_y + of_y1;
                int32_t p2x = centre_x + offset_x + of_x2;
                int32_t p2y = centre_y + offset_y + of_y2;
                display.drawTriangle(p0x, p0y, p1x, p1y, p2x, p2y, DISPLAY_COLOR_WHITE);
                display.fillTriangle(p0x, p0y, p1x, p1y, p2x, p2y, DISPLAY_COLOR_WHITE);
            }
        }

        display.display();
    });
    imu_app.set_end([](){
#ifdef ESP32
        display.setMode(backup_display_mode);
        imu.end();
#endif
    });
    imu_app.set_key_event([](uint8_t key, bool key_down){
        if ( key_down && (key == BADGE_KEY_LEFT) ) {
            badge_os.pop_app();
        }
    });
}
