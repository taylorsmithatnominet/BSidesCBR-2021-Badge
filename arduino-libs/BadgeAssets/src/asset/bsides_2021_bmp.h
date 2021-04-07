#ifndef _H_ASSET_BSIDES_2021_BMP_H_
#define _H_ASSET_BSIDES_2021_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_bsides_2021_bmp_bpp = 2;  // 0x00000002
const size_t asset_bsides_2021_bmp_width = 128;  // 0x00000080
const size_t asset_bsides_2021_bmp_height = 55;  // 0x00000037
const size_t asset_bsides_2021_bmp_size = 1760;  // 0x000006e0
const char asset_bsides_2021_bmp_data[1760] PROGMEM  = {
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x50','\x55','\x55','\x55','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x05','\x00','\x54','\x00','\x40',
    '\x15','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x00','\x00','\x00','\x00','\x00','\x55','\x01',
    '\x00','\x54','\x55','\x55','\x15','\x00','\x00','\x40','\x55','\x05','\x55','\x05','\x00','\x55','\x00','\x40',
    '\x15','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x00','\x00','\x40','\x55','\x01','\x55','\x01',
    '\x40','\x55','\x51','\x55','\x55','\x05','\x00','\x55','\x55','\x55','\x55','\x05','\x54','\x55','\x00','\x55',
    '\x55','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x01','\x40','\x55','\x55','\x55','\x55','\x01',
    '\x50','\x55','\x01','\x55','\x55','\x15','\x00','\x55','\x55','\x55','\x55','\x05','\x54','\x55','\x40','\x55',
    '\x55','\x15','\x00','\x00','\x00','\x00','\x50','\x55','\x55','\x01','\x40','\x55','\x55','\x55','\x55','\x01',
    '\x50','\x55','\x01','\x40','\x55','\x55','\x00','\x55','\x55','\x55','\x55','\x01','\x50','\x55','\x40','\x55',
    '\x55','\x55','\x05','\x00','\x00','\x55','\x55','\x55','\x55','\x05','\x00','\x55','\x55','\x55','\x55','\x01',
    '\x40','\x55','\x01','\x00','\x55','\x15','\x40','\x55','\x05','\x00','\x50','\x01','\x50','\x55','\x00','\x55',
    '\x15','\x54','\x55','\x15','\x00','\x55','\x55','\x55','\x15','\x00','\x40','\x55','\x05','\x00','\x50','\x00',
    '\x00','\x55','\x01','\x00','\x55','\x05','\x00','\x54','\x00','\x00','\x50','\x00','\x40','\x55','\x00','\x55',
    '\x15','\x40','\x55','\x55','\x01','\x55','\x05','\x00','\x00','\x00','\x00','\x55','\x00','\x00','\x50','\x00',
    '\x00','\x55','\x01','\x00','\x55','\x05','\x00','\x54','\x05','\x00','\x50','\x00','\x40','\x55','\x00','\x55',
    '\x15','\x00','\x55','\x55','\x05','\x54','\x05','\x00','\x00','\x00','\x00','\x54','\x05','\x00','\x50','\x00',
    '\x00','\x55','\x01','\x50','\x15','\x00','\x00','\x50','\x55','\x00','\x50','\x00','\x40','\x55','\x00','\x54',
    '\x15','\x00','\x50','\x55','\x15','\x54','\x05','\x00','\x00','\x00','\x00','\x50','\x15','\x00','\x10','\x00',
    '\x00','\x55','\x01','\x54','\x05','\x00','\x00','\x40','\x55','\x01','\x10','\x00','\x40','\x55','\x00','\x54',
    '\x15','\x00','\x50','\x55','\x15','\x54','\x05','\x00','\x00','\x00','\x00','\x40','\x55','\x00','\x10','\x00',
    '\x00','\x54','\x55','\x55','\x05','\x00','\x00','\x00','\x54','\x55','\x00','\x00','\x40','\x55','\x00','\x54',
    '\x15','\x00','\x00','\x54','\x01','\x50','\x05','\x55','\x55','\x00','\x00','\x00','\x55','\x15','\x00','\x00',
    '\x00','\x54','\x55','\x55','\x55','\x00','\x00','\x00','\x50','\x55','\x01','\x00','\x40','\x15','\x00','\x54',
    '\x15','\x00','\x00','\x50','\x00','\x40','\x55','\x55','\x55','\x00','\x00','\x00','\x54','\x55','\x00','\x00',
    '\x00','\x54','\x01','\x55','\x55','\x05','\x00','\x00','\x40','\x55','\x05','\x00','\x00','\x15','\x00','\x50',
    '\x15','\x00','\x00','\x15','\x00','\x40','\x55','\x55','\x15','\x00','\x00','\x00','\x50','\x55','\x05','\x00',
    '\x00','\x54','\x01','\x54','\x55','\x15','\x00','\x40','\x40','\x55','\x15','\x00','\x00','\x15','\x00','\x50',
    '\x15','\x00','\x00','\x15','\x00','\x40','\x55','\x55','\x15','\x00','\x00','\x00','\x50','\x55','\x05','\x00',
    '\x00','\x54','\x01','\x40','\x55','\x55','\x00','\x50','\x40','\x55','\x15','\x00','\x00','\x15','\x00','\x50',
    '\x15','\x00','\x50','\x01','\x00','\x40','\x05','\x40','\x15','\x00','\x00','\x14','\x40','\x55','\x15','\x00',
    '\x00','\x54','\x00','\x00','\x55','\x55','\x00','\x14','\x00','\x55','\x55','\x01','\x00','\x15','\x00','\x40',
    '\x15','\x40','\x55','\x00','\x00','\x40','\x05','\x00','\x00','\x00','\x00','\x15','\x00','\x55','\x55','\x00',
    '\x00','\x54','\x00','\x00','\x55','\x55','\x00','\x05','\x00','\x54','\x55','\x01','\x00','\x15','\x00','\x40',
    '\x05','\x55','\x15','\x00','\x00','\x00','\x05','\x00','\x00','\x00','\x00','\x05','\x00','\x55','\x55','\x01',
    '\x00','\x50','\x00','\x00','\x55','\x15','\x00','\x05','\x00','\x54','\x55','\x01','\x00','\x05','\x00','\x40',
    '\x55','\x55','\x05','\x00','\x00','\x00','\x05','\x00','\x00','\x00','\x40','\x05','\x00','\x55','\x55','\x00',
    '\x00','\x50','\x00','\x00','\x55','\x05','\x40','\x05','\x00','\x54','\x55','\x00','\x00','\x05','\x00','\x54',
    '\x55','\x55','\x01','\x00','\x00','\x00','\x01','\x55','\x55','\x05','\x40','\x05','\x00','\x54','\x15','\x00',
    '\x00','\x50','\x00','\x40','\x55','\x00','\x40','\x15','\x00','\x55','\x55','\x00','\x00','\x05','\x00','\x54',
    '\x55','\x55','\x00','\x00','\x00','\x00','\x41','\x55','\x55','\x05','\x40','\x15','\x00','\x55','\x15','\x00',
    '\x00','\x10','\x00','\x55','\x05','\x00','\x40','\x55','\x55','\x55','\x55','\x00','\x00','\x01','\x00','\x54',
    '\x55','\x01','\x00','\x00','\x00','\x40','\x55','\x55','\x55','\x05','\x54','\x55','\x55','\x55','\x55','\x00',
    '\x00','\x10','\x40','\x55','\x00','\x00','\x40','\x55','\x55','\x55','\x55','\x01','\x00','\x01','\x00','\x54',
    '\x55','\x00','\x00','\x00','\x00','\x50','\x55','\x55','\x55','\x05','\x54','\x55','\x55','\x55','\x55','\x00',
    '\x00','\x10','\x54','\x05','\x00','\x00','\x40','\x55','\x50','\x55','\x05','\x00','\x00','\x00','\x00','\x54',
    '\x15','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x01','\x54','\x55','\x50','\x55','\x05','\x00',
    '\x00','\x54','\x05','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x50',
    '\x01','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x01','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x00','\x55','\x55','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x55','\x05','\x50','\x55','\x05','\x00','\x00','\x00','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x54','\x55','\x55','\x00','\x00','\x14','\x14','\x00',
    '\x00','\x00','\x55','\x41','\x55','\x55','\x05','\x00','\x50','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x55','\x55','\x55','\x00','\x00','\x15','\x50','\x00',
    '\x00','\x00','\x55','\x55','\x55','\x55','\x05','\x00','\x55','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x55','\x55','\x55','\x00','\x40','\x55','\x05','\x54',
    '\x01','\x00','\x55','\x55','\x55','\x55','\x05','\x54','\x55','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x05','\x00','\x54','\x55','\x01','\x40','\x55','\x15','\x54',
    '\x05','\x00','\x55','\x00','\x40','\x55','\x15','\x50','\x45','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x05','\x00','\x00','\x55','\x00','\x50','\x55','\x05','\x50',
    '\x15','\x00','\x54','\x00','\x00','\x50','\x05','\x40','\x45','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x05','\x00','\x40','\x15','\x00','\x50','\x55','\x05','\x40',
    '\x55','\x00','\x54','\x00','\x00','\x54','\x01','\x00','\x41','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x05','\x00','\x54','\x05','\x00','\x54','\x55','\x00','\x40',
    '\x55','\x00','\x50','\x00','\x40','\x55','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x14','\x40','\x55','\x01','\x00','\x55','\x15','\x00','\x40',
    '\x55','\x01','\x40','\x01','\x54','\x15','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x10','\x50','\x55','\x00','\x00','\x55','\x05','\x00','\x40',
    '\x55','\x01','\x00','\x01','\x55','\x01','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x15','\x00','\x00','\x55','\x01','\x00','\x40',
    '\x55','\x01','\x00','\x40','\x55','\x01','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x15','\x00','\x40','\x55','\x01','\x00','\x40',
    '\x55','\x01','\x00','\x54','\x55','\x00','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x05','\x00','\x40','\x55','\x01','\x00','\x40',
    '\x55','\x01','\x00','\x54','\x55','\x00','\x00','\x00','\x00','\x55','\x01','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x55','\x01','\x00','\x40','\x55','\x00','\x00','\x54',
    '\x55','\x00','\x40','\x55','\x15','\x00','\x00','\x00','\x00','\x54','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x55','\x00','\x00','\x40','\x55','\x00','\x00','\x54',
    '\x55','\x00','\x40','\x55','\x05','\x00','\x00','\x00','\x00','\x54','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x00','\x00','\x40','\x55','\x00','\x00','\x55',
    '\x55','\x00','\x50','\x55','\x05','\x00','\x00','\x00','\x00','\x54','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x55','\x00','\x00','\x00','\x55','\x00','\x00','\x55',
    '\x55','\x00','\x54','\x55','\x05','\x00','\x00','\x00','\x00','\x14','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x55','\x15','\x00','\x00','\x00','\x55','\x00','\x40','\x55',
    '\x15','\x00','\x55','\x55','\x01','\x00','\x00','\x00','\x00','\x14','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x55','\x00','\x00','\x00','\x54','\x01','\x54','\x55',
    '\x15','\x00','\x40','\x55','\x05','\x00','\x00','\x00','\x00','\x14','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x15','\x00','\x00','\x50','\x01','\x55','\x55',
    '\x05','\x00','\x50','\x55','\x55','\x01','\x00','\x00','\x00','\x14','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x55','\x55','\x01','\x40','\x01','\x55','\x55',
    '\x05','\x00','\x50','\x55','\x55','\x55','\x15','\x00','\x00','\x04','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x55','\x15','\x00','\x00','\x15','\x50','\x55',
    '\x01','\x00','\x50','\x55','\x55','\x55','\x01','\x00','\x00','\x04','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x15','\x00','\x00','\x00','\x54','\x40','\x15',
    '\x00','\x00','\x50','\x55','\x55','\x01','\x00','\x00','\x00','\x04','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    };

#endif //_H_ASSET_BSIDES_2021_BMP_H_