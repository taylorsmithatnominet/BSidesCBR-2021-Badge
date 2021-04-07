#ifndef _H_ASSET_BATTERY_LOW_BMP_H_
#define _H_ASSET_BATTERY_LOW_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_battery_low_bmp_bpp = 2;  // 0x00000002
const size_t asset_battery_low_bmp_width = 9;  // 0x00000009
const size_t asset_battery_low_bmp_height = 16;  // 0x00000010
const size_t asset_battery_low_bmp_size = 36;  // 0x00000024
const char asset_battery_low_bmp_data[36] PROGMEM  = {
    '\xc0','\x0f','\xfc','\xff','\x3f','\x00','\xf0','\x00','\xc0','\x03','\x00','\x0f','\x00','\x3c','\x00','\xf0',
    '\x00','\xc0','\x03','\x00','\x0f','\x00','\x3c','\x00','\xf0','\x54','\xc5','\x53','\x15','\x4f','\x55','\x3c',
    '\x00','\xf0','\xff','\xff',};

#endif //_H_ASSET_BATTERY_LOW_BMP_H_
