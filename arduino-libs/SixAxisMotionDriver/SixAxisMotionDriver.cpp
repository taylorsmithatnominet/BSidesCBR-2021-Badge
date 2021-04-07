/*
 * SixAxisMotionDriver.h - Library for controlling the ICM20602 6-axis MEMS motion tracking
 *                         driver on the BSides Badge 2021
 * Reference: https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
 * Created by Peter Rankin, 02 January 2021
 */

#include <Arduino.h>
#include <Wire.h>
#include "icm20602.h"
#include "SixAxisMotionDriver.h"

int8_t icm20602_hal_wr_impl(uint8_t id, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission((uint16_t)id);
    if ( 1 != Wire.write(reg) )
    {
        return -1;
    }
    for ( uint16_t i = 0; i < len; i++ ) {
        if ( 1 != Wire.write(data[i]) )
        {
            return -1;
        }
    }
    Wire.endTransmission(true);
    return 0;
}

int8_t icm20602_hal_rd_impl(uint8_t id, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission((uint16_t)id);
    Wire.write(reg);
    if ( 0 != Wire.endTransmission(false) )
    {
        return -1;
    }
#ifdef ESP32
    if ( Wire.requestFrom((uint16_t)id, (uint8_t)len, true) != (uint8_t)len ) {
#else
    if ( Wire.requestFrom((uint8_t)id, (uint8_t)len) != (uint8_t)len ) {
#endif
        return -1;
    }
    for ( uint16_t i = 0; i < len; i++ ) {
        data[i] = Wire.read();
    }
#ifndef ESP32
    Wire.endTransmission(true);
#endif
    return 0;
}

void icm20602_hal_sleep_impl(uint32_t ms)
{
    delay(ms);
}

// Set the slave address of the ICM20602 driver
SixAxisMotionDriver::SixAxisMotionDriver(char address)
{
    this->dev = ICM20602_DEFAULT_INIT();
    this->dev.id = (uint8_t)address;
    this->dev.hal_wr = icm20602_hal_wr_impl;
    this->dev.hal_rd = icm20602_hal_rd_impl;
    this->dev.hal_sleep = icm20602_hal_sleep_impl;
    this->ax_ = 0;
    this->ay_ = 0;
    this->az_ = 0;
    this->gx_ = 0;
    this->gy_ = 0;
    this->gz_ = 0;
    this->temp_ = 0;
}

bool SixAxisMotionDriver::begin(int sda, int scl)
{
#ifdef ESP32
    Wire.begin(sda, scl);
#else
    Wire.begin();
#endif
    this->ax_ = 0;
    this->ay_ = 0;
    this->az_ = 0;
    this->gx_ = 0;
    this->gy_ = 0;
    this->gz_ = 0;
    this->temp_ = 0;
    return 0 == icm20602_init(&this->dev);
}

bool SixAxisMotionDriver::end(void)
{
    return 0 == icm20602_reset(&this->dev);
}

bool SixAxisMotionDriver::read()
{
    return 0 == icm20602_read_data_raw(&this->dev,
        &this->ax_,
        &this->ay_,
        &this->az_,
        &this->gx_,
        &this->gy_,
        &this->gz_,
        &this->temp_);
}
