#ifndef _H_ASSET_BATTERY_EMPTY_BMP_H_
#define _H_ASSET_BATTERY_EMPTY_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_battery_empty_bmp_bpp = 2;  // 0x00000002
const size_t asset_battery_empty_bmp_width = 9;  // 0x00000009
const size_t asset_battery_empty_bmp_height = 16;  // 0x00000010
const size_t asset_battery_empty_bmp_size = 36;  // 0x00000024
const char asset_battery_empty_bmp_data[36] PROGMEM  = {
    '\x40','\x05','\x54','\x55','\x15','\x00','\x50','\x00','\x40','\x01','\x00','\x05','\x00','\x14','\x00','\x50',
    '\x00','\x40','\x01','\x00','\x05','\x00','\x14','\x00','\x50','\x00','\x40','\x01','\x00','\x45','\x55','\x14',
    '\x00','\x50','\x55','\x55',};

#endif //_H_ASSET_BATTERY_EMPTY_BMP_H_
