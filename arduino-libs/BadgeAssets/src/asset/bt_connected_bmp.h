#ifndef _H_ASSET_BT_CONNECTED_BMP_H_
#define _H_ASSET_BT_CONNECTED_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_bt_connected_bmp_bpp = 2;  // 0x00000002
const size_t asset_bt_connected_bmp_width = 16;  // 0x00000010
const size_t asset_bt_connected_bmp_height = 16;  // 0x00000010
const size_t asset_bt_connected_bmp_size = 64;  // 0x00000040
const char asset_bt_connected_bmp_data[64] PROGMEM  = {
    '\x00','\xc0','\x00','\x00','\x00','\xc0','\x03','\x00','\x00','\xc0','\x0f','\x00','\x00','\xc3','\x3c','\x00',
    '\xc0','\xcf','\xf0','\x00','\x00','\xff','\x3c','\x00','\x00','\xfc','\x0f','\x00','\xcf','\xf3','\xc3','\xf3',
    '\xcf','\xf3','\xc3','\xf3','\x00','\xfc','\x0f','\x00','\x00','\xff','\x3c','\x00','\xc0','\xcf','\xf0','\x00',
    '\x00','\xc3','\x3c','\x00','\x00','\xc0','\x0f','\x00','\x00','\xc0','\x03','\x00','\x00','\xc0','\x00','\x00',
    };

#endif //_H_ASSET_BT_CONNECTED_BMP_H_
