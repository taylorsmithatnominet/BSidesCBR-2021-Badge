#ifndef _H_ASSET_SWITCH_ON_BMP_H_
#define _H_ASSET_SWITCH_ON_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_switch_on_bmp_bpp = 2;  // 0x00000002
const size_t asset_switch_on_bmp_width = 32;  // 0x00000020
const size_t asset_switch_on_bmp_height = 16;  // 0x00000010
const size_t asset_switch_on_bmp_size = 128;  // 0x00000080
const char asset_switch_on_bmp_data[128] PROGMEM  = {
    '\x00','\x54','\x55','\x55','\x55','\x55','\x15','\x00','\x40','\xf5','\xff','\xff','\xff','\xff','\x5f','\x01',
    '\x50','\xff','\xff','\xff','\xff','\xff','\xff','\x05','\xd4','\xff','\xff','\xff','\xff','\x0f','\xf0','\x17',
    '\xf4','\xff','\xff','\xff','\xff','\x53','\xc5','\x1f','\xf5','\xff','\xff','\xff','\xff','\x54','\x15','\x5f',
    '\xfd','\xff','\xff','\xff','\x3f','\x55','\x55','\x7c','\xfd','\xff','\xff','\xff','\x3f','\x55','\x55','\x7c',
    '\xfd','\xff','\xff','\xff','\x3f','\x55','\x55','\x7c','\xfd','\xff','\xff','\xff','\x3f','\x55','\x55','\x7c',
    '\xf5','\xff','\xff','\xff','\xff','\x54','\x15','\x5f','\xf4','\xff','\xff','\xff','\xff','\x53','\xc5','\x1f',
    '\xd4','\xff','\xff','\xff','\xff','\x0f','\xf0','\x17','\x50','\xff','\xff','\xff','\xff','\xff','\xff','\x05',
    '\x40','\xf5','\xff','\xff','\xff','\xff','\x5f','\x01','\x00','\x54','\x55','\x55','\x55','\x55','\x15','\x00',
    };

#endif //_H_ASSET_SWITCH_ON_BMP_H_
