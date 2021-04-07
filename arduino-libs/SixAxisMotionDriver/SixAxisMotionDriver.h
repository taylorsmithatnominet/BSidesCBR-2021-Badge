/*
 * SixAxisMotionDriver.h - Library for controlling the ICM20602 6-axis MEMS motion tracking
 *                         driver on the BSides Badge 2021
 * Reference: https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
 * Created by Peter Rankin, 02 January 2021
 */

#ifndef SIX_AXIS_MOTION_DRIVER_H_
#define SIX_AXIS_MOTION_DRIVER_H_

#include <Arduino.h>

#include "icm20602.h"

class SixAxisMotionDriver
{
    public:
        // Set the slave address of the ICM20602 driver
        SixAxisMotionDriver(char address);
        // Initialise the ICM20602
        bool begin(int sda=-1, int scl=-1);
        // Reset the ICM20602
        bool end(void);
        // Reads current raw values of accelerometer and gyroscope.
        bool read();
        // Getters for values.
        inline int16_t ax() const { return this->ax_; }
        inline int16_t ay() const { return this->ay_; }
        inline int16_t az() const { return this->az_; }
        inline int16_t gx() const { return this->gx_; }
        inline int16_t gy() const { return this->gy_; }
        inline int16_t gz() const { return this->gz_; }
        inline int16_t temp() const { return this->temp_; }

    private:
        // HAL
        struct icm20602_dev dev;
        // Current values
        int16_t ax_;
        int16_t ay_;
        int16_t az_;
        int16_t gx_;
        int16_t gy_;
        int16_t gz_;
        int16_t temp_;
};

#endif // SIX_AXIS_MOTION_DRIVER_H_
