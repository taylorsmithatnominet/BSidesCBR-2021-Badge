#ifndef _H_ASSET_BT_ON_BMP_H_
#define _H_ASSET_BT_ON_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_bt_on_bmp_bpp = 2;  // 0x00000002
const size_t asset_bt_on_bmp_width = 9;  // 0x00000009
const size_t asset_bt_on_bmp_height = 16;  // 0x00000010
const size_t asset_bt_on_bmp_size = 36;  // 0x00000024
const char asset_bt_on_bmp_data[36] PROGMEM  = {
    '\x00','\x01','\x00','\x14','\x00','\x50','\x01','\x41','\x14','\x15','\x41','\x51','\x45','\x01','\x55','\x01',
    '\x50','\x01','\x40','\x05','\x40','\x55','\x40','\x15','\x45','\x45','\x50','\x04','\x51','\x00','\x54','\x00',
    '\x50','\x00','\x40','\x00',};

#endif //_H_ASSET_BT_ON_BMP_H_
