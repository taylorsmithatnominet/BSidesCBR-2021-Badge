#ifndef _H_ASSET_MAGPIES_EGG_BMP_H_
#define _H_ASSET_MAGPIES_EGG_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_magpies_egg_bmp_bpp = 2;  // 0x00000002
const size_t asset_magpies_egg_bmp_width = 128;  // 0x00000080
const size_t asset_magpies_egg_bmp_height = 83;  // 0x00000053
const size_t asset_magpies_egg_bmp_size = 2656;  // 0x00000a60
const char asset_magpies_egg_bmp_data[2656] PROGMEM  = {
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x54','\x05','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x05','\x14','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x51','\x50','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x51','\x44','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x51','\x44','\x00','\x00','\x50','\x55','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x55',
    '\x01','\x00','\x00','\x00','\x00','\x00','\x00','\x51','\x44','\x00','\x40','\x15','\x40','\x01','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x50','\x05','\x00','\x00','\x00','\x00','\x54','\x01',
    '\x15','\x00','\x00','\x00','\x00','\x00','\x00','\x51','\x44','\x00','\x54','\x00','\x01','\x05','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x15','\x14','\x00','\x00','\x00','\x50','\x05','\x54',
    '\x50','\x00','\x50','\x01','\x00','\x00','\x00','\x51','\x44','\x40','\x05','\x55','\x11','\x14','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x01','\x50','\x01','\x00','\x50','\x15','\x50','\x55',
    '\x40','\x01','\x14','\x01','\x00','\x00','\x00','\x51','\x44','\x50','\x50','\x55','\x51','\x11','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x11','\x01','\x05','\x40','\x15','\x40','\x55','\x55',
    '\x00','\x05','\x05','\x01','\x00','\x00','\x00','\x51','\x44','\x10','\x55','\x55','\x51','\x11','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x40','\x14','\x15','\x14','\x54','\x40','\x55','\x55','\x55',
    '\x00','\x54','\x01','\x01','\x00','\x55','\x01','\x51','\x44','\x10','\x55','\x55','\x51','\x11','\x00','\xfc',
    '\x3f','\x00','\x00','\x00','\x40','\x55','\x01','\x40','\x14','\x55','\x10','\x04','\x55','\x55','\x55','\x55',
    '\x00','\x50','\x00','\x01','\x40','\x01','\x55','\x51','\x44','\x10','\x55','\x55','\x51','\xd1','\xff','\x03',
    '\xc0','\x3f','\x00','\x00','\x50','\x00','\x05','\x40','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x05','\x50','\x44','\x00','\x51','\x44','\x10','\x55','\x55','\x51','\x3d','\x00','\x00',
    '\x00','\xc0','\x3f','\x00','\x15','\x44','\x54','\x40','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x04','\x14','\x45','\x55','\x50','\x44','\x10','\x55','\x55','\x51','\x03','\xfc','\xff',
    '\xff','\x0f','\xc0','\x40','\x41','\x45','\x41','\x40','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x54','\x05','\x45','\x55','\x51','\x44','\x10','\x55','\x55','\xd1','\xc0','\xff','\xff',
    '\xff','\xff','\x03','\x43','\x50','\x45','\x45','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x01','\x45','\x55','\x51','\x44','\x10','\x55','\x55','\x31','\xfc','\xff','\xff',
    '\xff','\xff','\x3f','\x4c','\x54','\x45','\x05','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x41','\x45','\x55','\x51','\x44','\x10','\x55','\x55','\x0f','\xfc','\xff','\xff',
    '\xff','\xff','\xff','\xf0','\x54','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x40','\x45','\x55','\x51','\x44','\x15','\x55','\xd5','\xc0','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x57','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x40','\x45','\x55','\x51','\x04','\x00','\x55','\xd5','\xf0','\xff','\xff','\xff',
    '\x03','\xff','\xff','\x3f','\x5c','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x40','\x45','\x55','\x51','\x44','\x15','\x55','\x35','\xf0','\xff','\xff','\xff',
    '\x3f','\xc0','\xff','\x3f','\x70','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x00','\x45','\x55','\x51','\x44','\x15','\x55','\x0d','\xfc','\xff','\xff','\xff',
    '\xff','\x03','\xff','\xff','\x70','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\x57','\x55','\x51','\x44','\x15','\x55','\x0d','\xff','\xff','\xff','\xff',
    '\xff','\x0f','\xff','\xff','\x73','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\x43','\x55','\x51','\x44','\x15','\x55','\xc3','\xff','\xff','\xff','\xff',
    '\xff','\x0f','\xfc','\xff','\xc3','\x45','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x00','\xfc','\x55','\x51','\x44','\x15','\xd5','\xf0','\xff','\xff','\xff','\xff',
    '\xff','\x3f','\xfc','\xff','\x0f','\x47','\x15','\x41','\x14','\x55','\x11','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x00','\xfc','\x57','\x51','\x44','\x15','\x35','\xf0','\xff','\xff','\xff','\xff',
    '\xff','\x3f','\xfc','\xff','\x0f','\x4c','\x15','\x41','\x14','\x01','\x14','\x44','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xf0','\xff','\x7f','\x51','\x44','\x15','\x35','\xfc','\xff','\xff','\xff','\xff',
    '\xff','\x3f','\xf0','\xfc','\x3f','\x4c','\x15','\x41','\x54','\x00','\x50','\x45','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xfc','\xff','\x3f','\x55','\x44','\x15','\x0d','\xff','\xff','\xff','\xff','\xff',
    '\xff','\x0f','\x00','\xfc','\xff','\x4c','\x15','\x41','\x14','\x3c','\x00','\x45','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xf0','\xff','\x0f','\x54','\x44','\x15','\xcd','\xff','\xff','\xff','\xff','\xff',
    '\xff','\x03','\x03','\xff','\xff','\x70','\x15','\x41','\x14','\x30','\x00','\x54','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xfc','\xff','\x03','\x54','\x44','\x15','\x03','\x00','\xff','\xff','\xff','\xff',
    '\xff','\xc3','\x03','\xff','\xff','\xc3','\x15','\x41','\x05','\x00','\x00','\x40','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xf0','\xff','\x00','\x40','\x45','\x15','\xc3','\x03','\xfc','\xff','\xff','\xff',
    '\xff','\xf0','\x03','\xfc','\xff','\xc3','\x15','\x41','\x35','\x00','\xc0','\x03','\x55','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xf0','\xff','\x00','\x00','\x55','\xd5','\xc0','\xc0','\xfc','\xff','\xff','\xff',
    '\xff','\xf0','\x3f','\xc0','\xff','\x0f','\x17','\x51','\xfd','\x00','\xc0','\x3f','\x50','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\xff','\x03','\x00','\x00','\x00','\x00','\xf0','\xf3','\xff','\xff','\xff',
    '\x3f','\xfc','\xff','\x03','\xff','\x0f','\x17','\x55','\x5f','\x05','\x00','\xff','\x40','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\xff','\x03','\x00','\x00','\x00','\x00','\xfc','\xf3','\xff','\xff','\xff',
    '\x0f','\xff','\xff','\x03','\xff','\x3f','\x17','\x15','\x54','\x05','\x00','\xf0','\x03','\x55','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\xff','\x03','\x00','\x00','\x00','\x00','\xc0','\xcf','\xff','\xff','\xff',
    '\x0f','\xff','\xff','\x3f','\xff','\x3f','\x1c','\xc5','\x54','\x05','\x00','\x00','\x0f','\x54','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\x0f','\xf0',
    '\xcf','\xff','\xff','\x3f','\xff','\xff','\x5c','\x31','\x55','\x05','\xfc','\x00','\x0c','\x50','\x55','\x55',
    '\x00','\x00','\x00','\x00','\xc0','\xff','\x3f','\x00','\x00','\x00','\x00','\xf0','\xff','\xff','\x0f','\xf0',
    '\xc0','\xff','\xff','\x3f','\xf0','\xff','\x5c','\x4c','\x55','\x05','\xfc','\x0f','\x00','\x00','\x55','\x55',
    '\x00','\x00','\x00','\x00','\x00','\xff','\xff','\x00','\x00','\x00','\x00','\xfc','\xf0','\xff','\x03','\x03',
    '\xc0','\xff','\xff','\xff','\xf0','\xff','\x70','\x4c','\x55','\x05','\xfc','\x3f','\x00','\x00','\x54','\x55',
    '\x01','\x00','\x00','\x00','\x00','\xf0','\xff','\x03','\x00','\x00','\x00','\x3f','\xfc','\xff','\xf3','\x03',
    '\xfc','\xff','\xff','\xff','\xff','\xff','\x30','\x53','\x55','\x05','\xfc','\xff','\x00','\x00','\x40','\x55',
    '\x05','\x00','\x00','\x00','\x00','\x3c','\xff','\x0f','\x00','\x00','\x00','\x0f','\xff','\xff','\xf3','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\x33','\x53','\x55','\x15','\xfc','\xff','\x03','\x00','\x00','\x55',
    '\x15','\x00','\x00','\x00','\x00','\x00','\xff','\x03','\x00','\x00','\xc0','\xc3','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xc3','\x54','\x55','\x15','\xfc','\xff','\xc3','\x00','\x00','\x54',
    '\x55','\x01','\x00','\x00','\x00','\x00','\x30','\x00','\x00','\x00','\xf0','\xf0','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xc3','\x55','\x55','\x15','\xf0','\xff','\xcf','\x00','\x00','\x50',
    '\x55','\x05','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xfc','\x00','\x00','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xcf','\x55','\x55','\x55','\xc0','\xff','\x0f','\x00','\x00','\x50',
    '\x55','\x15','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xff','\x03','\x0f','\xf0','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xcf','\x55','\x55','\x55','\x01','\xfc','\x0f','\x00','\x00','\x40',
    '\x54','\x55','\x55','\x00','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xf0','\xff','\x03','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xcf','\x55','\x55','\x55','\x05','\x00','\x00','\x00','\x00','\x00',
    '\x54','\x55','\x55','\x05','\x00','\x00','\x00','\x00','\x00','\xc0','\x0f','\xf0','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xff','\xff','\xff','\xcf','\x55','\x55','\x55','\x05','\x00','\x00','\x00','\xf0','\xff',
    '\x50','\x55','\x55','\x15','\x00','\x00','\xc0','\x00','\x00','\xf0','\xc0','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\x3f','\xf0','\xff','\xff','\xcf','\x55','\x55','\x15','\xf0','\xff','\x3f','\xfc','\x0f','\xc0',
    '\x50','\x55','\x55','\x55','\x01','\x05','\xf0','\x00','\x00','\xf0','\xf0','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xf3','\xcc','\xff','\xff','\xcf','\x55','\x55','\x00','\xff','\xff','\xff','\x00','\x00','\xc0',
    '\x40','\x55','\x55','\x55','\x55','\x05','\xff','\x00','\x00','\xf0','\xf0','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xc3','\x33','\xff','\xff','\x0f','\x57','\x05','\x0c','\xff','\xff','\xff','\xf0','\xff','\xcf',
    '\x01','\x55','\x55','\x50','\x55','\xc0','\xcf','\x03','\x00','\xfc','\xf0','\x0f','\xf0','\xff','\xff','\xff',
    '\xff','\xff','\x33','\xcc','\xfc','\xff','\x3f','\x17','\xf0','\x0f','\xff','\xff','\xff','\xf0','\x00','\xc0',
    '\x01','\x55','\x55','\x00','\x05','\xfc','\xf0','\x0f','\x00','\xfc','\xc0','\xcf','\x03','\xf0','\xff','\xff',
    '\xff','\xff','\xcf','\x03','\xf3','\xff','\x3f','\x03','\xff','\x0f','\xfc','\xff','\xff','\xc0','\xff','\xc0',
    '\x05','\x55','\x55','\x00','\xc0','\x0f','\x3f','\x0f','\xc0','\x3f','\x00','\xc0','\xff','\xfc','\xff','\xff',
    '\xff','\xff','\x3f','\x0c','\x0c','\xff','\x3f','\xff','\xff','\x3f','\xfc','\xff','\xff','\x00','\x03','\xcf',
    '\x05','\x50','\x55','\x01','\xfc','\xf0','\xcf','\x3f','\xc0','\x00','\x0f','\xfc','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xf0','\x30','\xc0','\x3f','\xff','\xff','\x3f','\xfc','\xff','\xff','\x00','\x3c','\xc0',
    '\x14','\x50','\x55','\xc5','\x3f','\xff','\xf0','\xf3','\xc0','\x00','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x0f','\x0f','\x3f','\xff','\xff','\x3f','\xfc','\xff','\xff','\x00','\xc0','\xc0',
    '\x00','\x50','\x55','\x01','\xff','\x3f','\xff','\xfc','\xf3','\xfc','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x0c','\x0f','\x3c','\xff','\xff','\x3f','\xfc','\xff','\xff','\x00','\x00','\xc3',
    '\x00','\x40','\x55','\x00','\xf0','\xcf','\x3f','\xcf','\xcf','\xfc','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x0f','\x00','\x00','\x30','\xff','\xff','\x3f','\xfc','\xff','\xff','\x00','\x00','\xcc',
    '\x00','\x40','\x15','\x00','\xc0','\xf0','\xcf','\xf3','\xcc','\xfc','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x0f','\x00','\x00','\x00','\xff','\xc0','\xff','\xf0','\xff','\xff','\x03','\x00','\xc0',
    '\x00','\x40','\x55','\x00','\x00','\xfc','\xf3','\xf3','\xfc','\xf0','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xc3','\xff','\x3f','\x00','\x00','\xc0','\x03','\xc0','\xff','\xf0','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\xc0','\xfc','\x3c','\x3f','\xf3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\x0f','\xff','\x3f','\x00','\x00','\x0c','\x00','\xc0','\xff','\xf0','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\xfc','\x3c','\x0f','\xf3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\x3f','\x00','\x3c','\x00','\x00','\x3c','\x01','\xc0','\xff','\xf0','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x3c','\x30','\x0f','\xc0','\xf3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x3c','\x00','\x00','\xfc','\x04','\xc0','\xff','\xf0','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x0f','\x00','\xc0','\xff','\xf3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x30','\x00','\xc0','\xff','\x13','\x00','\xff','\xf3','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xff','\xc3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x00','\x00','\xf0','\xfc','\x7f','\x00','\xff','\xc3','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\xf0','\xff','\xff','\xff','\xc3','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x00','\x30','\xfc','\xff','\x01','\xff','\xc3','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\xff','\xff','\xff','\x0f','\x0c','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x00','\x00','\x3f','\xc0','\x07','\xff','\xc3','\xff','\xff','\x03','\x00','\x00',
    '\x00','\x00','\x00','\x00','\xff','\xff','\xff','\x03','\xf0','\x0f','\xff','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x0f','\x00','\xc0','\x0f','\x00','\x1f','\xfc','\xc3','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\x00','\xf0','\xff','\xff','\x0f','\xf0','\xff','\x3f','\xfc','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x3f','\x00','\xc0','\x00','\x00','\x70','\xfc','\xcf','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\x00','\xff','\xff','\xff','\xc3','\xff','\x3f','\x30','\xfc','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\xfc','\x0f','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\xfc','\xff','\xff','\x3f','\xfc','\x3f','\x00','\xc0','\xf0','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x00','\x00','\x00','\x00','\x00','\x00','\xfc','\x0f','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\xff','\xff','\xff','\xc3','\xff','\x00','\x00','\xc0','\xc0','\xff','\xff','\xff','\xff','\xff',
    '\xff','\x0f','\xf0','\x00','\x00','\x00','\x00','\x00','\x00','\xfc','\x0f','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\xfc','\xff','\x3f','\xfc','\x0f','\x00','\x00','\x00','\xc3','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xc3','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xf0','\x0f','\xff','\xff','\x0f','\x00','\x00',
    '\x00','\x00','\xf0','\xff','\x03','\xff','\x00','\x00','\x00','\x00','\xcc','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xf0','\x03','\x0c','\x00','\x00','\x00','\x00','\x00','\xf0','\x3f','\xff','\xff','\x3f','\x00','\x00',
    '\x00','\x3c','\xf0','\xff','\xf0','\x3f','\x00','\x00','\x00','\x00','\x0c','\xff','\xff','\xff','\xff','\xff',
    '\xff','\xf0','\xff','\x0f','\x00','\x00','\x00','\x30','\x00','\xf0','\x3f','\xfc','\xff','\x3f','\x00','\x00',
    '\x00','\x3f','\xc0','\xff','\xf3','\x03','\x00','\x00','\x00','\x00','\x30','\xf0','\xff','\xff','\xff','\xff',
    '\x3f','\xfc','\xff','\x0f','\x00','\x00','\x00','\xf0','\x03','\xf0','\x3f','\xfc','\xff','\x3f','\x00','\x00',
    '\x00','\xff','\xc0','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\xc0','\xf0','\xff','\xff','\xff','\xff',
    '\x3f','\xff','\xff','\x3f','\x00','\x00','\x00','\xc0','\xff','\xf0','\x3f','\xfc','\xff','\x3f','\x00','\x00',
    '\xc0','\xff','\x00','\xff','\x0f','\x00','\x00','\x00','\x00','\x00','\x00','\xc3','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x3f','\x00','\x00','\x00','\x00','\xff','\xf3','\x3f','\xfc','\xff','\xff','\x00','\x00',
    '\xf0','\xff','\x03','\xff','\x0f','\x00','\x00','\x00','\x00','\x00','\x00','\x0c','\xff','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x3f','\x00','\x00','\x00','\x00','\xf0','\xcf','\xff','\xfc','\xff','\xff','\x00','\x00',
    '\xf0','\xff','\x03','\xfc','\x3f','\x00','\x00','\x00','\x00','\x00','\x00','\x30','\xf0','\xff','\xff','\xff',
    '\xff','\xff','\xff','\x0f','\x00','\x00','\x00','\x00','\xc0','\xcf','\xff','\xfc','\xff','\xff','\x00','\x00',
    '\xfc','\xff','\x0f','\xf0','\x3f','\x00','\x00','\x00','\x00','\x00','\x00','\xc0','\x03','\xff','\xff','\xff',
    '\xff','\xff','\xff','\xc0','\x00','\x00','\x00','\x00','\x00','\xcf','\xff','\xf0','\xff','\xff','\x00','\x00',
    '\xff','\xff','\x0f','\xc0','\xff','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0c','\xfc','\xff','\xff',
    '\xff','\xff','\x3f','\xf0','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xf0','\xff','\xff','\x00','\x00',
    '\xff','\xff','\xff','\xc0','\xff','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xf0','\xc0','\xff','\xff',
    '\xff','\xff','\x03','\xcf','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xf0','\xff','\xff','\x00','\x00',
    '\xff','\xff','\xff','\x00','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x3f','\x00','\x00',
    '\x00','\x00','\xfc','\xc0','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xf0','\xff','\xff','\x00','\x00',
    '\xff','\xff','\xff','\x03','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\x00',
    '\x00','\xff','\x03','\x00','\x00','\x00','\x00','\x00','\x00','\xc0','\xff','\xf0','\xff','\xff','\x00','\x00',
    };

#endif //_H_ASSET_MAGPIES_EGG_BMP_H_
