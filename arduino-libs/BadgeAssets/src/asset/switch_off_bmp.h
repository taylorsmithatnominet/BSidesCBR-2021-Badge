#ifndef _H_ASSET_SWITCH_OFF_BMP_H_
#define _H_ASSET_SWITCH_OFF_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_switch_off_bmp_bpp = 2;  // 0x00000002
const size_t asset_switch_off_bmp_width = 32;  // 0x00000020
const size_t asset_switch_off_bmp_height = 16;  // 0x00000010
const size_t asset_switch_off_bmp_size = 128;  // 0x00000080
const char asset_switch_off_bmp_data[128] PROGMEM  = {
    '\x00','\x54','\x55','\x55','\x55','\x55','\x15','\x00','\x40','\x05','\x00','\x00','\x00','\x00','\x50','\x01',
    '\x50','\x00','\x00','\x00','\x00','\x00','\x00','\x05','\x14','\x50','\x05','\x00','\x00','\x00','\x00','\x14',
    '\x04','\x04','\x10','\x00','\x00','\x00','\x00','\x10','\x05','\x01','\x40','\x00','\x00','\x00','\x00','\x50',
    '\x41','\x00','\x00','\x01','\x00','\x00','\x00','\x40','\x41','\x00','\x00','\x01','\x00','\x00','\x00','\x40',
    '\x41','\x00','\x00','\x01','\x00','\x00','\x00','\x40','\x41','\x00','\x00','\x01','\x00','\x00','\x00','\x40',
    '\x05','\x01','\x40','\x00','\x00','\x00','\x00','\x50','\x04','\x04','\x10','\x00','\x00','\x00','\x00','\x10',
    '\x14','\x50','\x05','\x00','\x00','\x00','\x00','\x14','\x50','\x00','\x00','\x00','\x00','\x00','\x00','\x05',
    '\x40','\x05','\x00','\x00','\x00','\x00','\x50','\x01','\x00','\x54','\x55','\x55','\x55','\x55','\x15','\x00',
    };

#endif //_H_ASSET_SWITCH_OFF_BMP_H_
