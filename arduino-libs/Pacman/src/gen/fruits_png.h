#ifndef _H_FRUITS_PNG_H_
#define _H_FRUITS_PNG_H_

#include <stdint.h>

#include <Arduino.h>

const size_t fruits_png_id = 7;  // 0x00000007
const size_t fruits_png_width = 48;  // 0x00000030
const size_t fruits_png_height = 48;  // 0x00000030
const size_t fruits_png_palette_size = 16;  // 0x00000010
const size_t fruits_png_palette_bits = 4;  // 0x00000004
const size_t fruits_png_size = 1200;  // 0x000004b0
const char fruits_png_data[1200] PROGMEM  = {
    '\x00','\x00','\xff','\xc6','\xdc','\x8a','\xd4','\xca','\x00','\x00','\xf8','\x00','\xff','\xdf','\xde','\xdf',
    '\x6d','\xdf','\x6d','\x95','\x7f','\x86','\x65','\xdf','\x01','\x9f','\xf5','\xcb','\x12','\x1f','\x00','\x00',
    '\x00','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\xff','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x01','\x10','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x22','\x00','\x00','\x00','\x00','\x00','\x30','\x00','\x00','\x00',
    '\x00','\x00','\x01','\x14','\x41','\x10','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x22','\x22','\x00',
    '\x00','\x05','\x55','\x03','\x05','\x55','\x00','\x00','\x00','\x00','\x11','\x11','\x11','\x11','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x22','\x02','\x00','\x00','\x00','\x55','\x55','\x53','\x55','\x55','\x50','\x00',
    '\x00','\x00','\x11','\x41','\x11','\x11','\x00','\x00','\x00','\x00','\x00','\x02','\x00','\x02','\x00','\x00',
    '\x00','\x55','\x55','\x55','\x55','\x55','\x55','\x00','\x00','\x00','\x14','\x11','\x11','\x11','\x00','\x00',
    '\x00','\x05','\x55','\x20','\x00','\x20','\x00','\x00','\x00','\x55','\x55','\x55','\x55','\x55','\x55','\x00',
    '\x00','\x01','\x14','\x11','\x11','\x11','\x10','\x00','\x00','\x55','\x52','\x55','\x02','\x00','\x00','\x00',
    '\x00','\x55','\x55','\x55','\x55','\x55','\x55','\x00','\x00','\x01','\x14','\x11','\x11','\x11','\x10','\x00',
    '\x00','\x55','\x55','\x50','\x52','\x55','\x00','\x00','\x00','\x55','\x55','\x55','\x55','\x56','\x55','\x00',
    '\x00','\x01','\x11','\x11','\x11','\x11','\x10','\x00','\x00','\x57','\x55','\x05','\x52','\x55','\x50','\x00',
    '\x00','\x55','\x55','\x55','\x55','\x56','\x55','\x00','\x00','\x11','\x41','\x11','\x11','\x11','\x11','\x00',
    '\x00','\x55','\x75','\x05','\x55','\x55','\x50','\x00','\x00','\x05','\x55','\x55','\x55','\x65','\x50','\x00',
    '\x00','\x11','\x41','\x11','\x11','\x11','\x11','\x00','\x00','\x05','\x55','\x05','\x75','\x55','\x50','\x00',
    '\x00','\x05','\x55','\x55','\x55','\x55','\x50','\x00','\x00','\x11','\x11','\x11','\x11','\x11','\x11','\x00',
    '\x00','\x00','\x00','\x05','\x57','\x55','\x50','\x00','\x00','\x00','\x55','\x55','\x55','\x55','\x00','\x00',
    '\x00','\x18','\x88','\x88','\x66','\x88','\x81','\x00','\x00','\x00','\x00','\x00','\x55','\x55','\x00','\x00',
    '\x00','\x00','\x05','\x50','\x55','\x50','\x00','\x00','\x00','\x08','\x88','\x88','\x66','\x88','\x80','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x09','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x99','\x99','\x90','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x60','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x90','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x08','\x88','\x00','\x00','\x00','\x00','\x00','\x0a','\xaa','\x6a','\xaa','\x00','\x00',
    '\x00','\x00','\x00','\x0a','\x6a','\x00','\x00','\x00','\x00','\x00','\x08','\xb4','\x44','\x88','\x00','\x00',
    '\x00','\x00','\x55','\xaa','\xaa','\xa5','\x50','\x00','\x00','\x00','\x0a','\xaa','\x9a','\xaa','\x00','\x00',
    '\x00','\x00','\x08','\xb8','\x88','\x88','\x00','\x00','\x00','\x05','\x55','\x55','\xa5','\x55','\x65','\x00',
    '\x00','\x00','\xa9','\x69','\xaa','\xa9','\xa0','\x00','\x00','\x00','\x08','\x88','\x88','\x88','\x00','\x00',
    '\x00','\x05','\x65','\x55','\x55','\x65','\x55','\x00','\x00','\x00','\xaa','\x9a','\xa6','\x9a','\xa0','\x00',
    '\x00','\x00','\x08','\x88','\xb8','\x88','\x00','\x00','\x00','\x05','\x55','\x65','\x65','\x55','\x55','\x00',
    '\x00','\x0a','\x69','\xaa','\xa9','\xa9','\xaa','\x00','\x00','\x00','\x00','\x06','\x46','\x00','\x00','\x00',
    '\x00','\x05','\x55','\x55','\x55','\x56','\x55','\x00','\x00','\x0a','\x9a','\xa6','\x9a','\xa6','\x9a','\x00',
    '\x00','\x00','\x00','\x06','\x46','\x60','\x00','\x00','\x00','\x00','\x56','\x55','\x65','\x55','\x50','\x00',
    '\x00','\x0a','\x69','\xaa','\xa9','\xaa','\xaa','\x00','\x00','\x00','\x00','\x06','\x46','\x00','\x00','\x00',
    '\x00','\x00','\x55','\x55','\x55','\x55','\x50','\x00','\x00','\x00','\xaa','\x96','\x96','\x9a','\xa0','\x00',
    '\x00','\x00','\x00','\x06','\x44','\x00','\x00','\x00','\x00','\x00','\x05','\x56','\x55','\x60','\x00','\x00',
    '\x00','\x00','\xaa','\xa9','\xaa','\xa9','\xa0','\x00','\x00','\x00','\x00','\x06','\x46','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x55','\x55','\x50','\x00','\x00','\x00','\x00','\x0a','\x9a','\xaa','\x9a','\x00','\x00',
    '\x00','\x00','\x00','\x06','\x46','\x60','\x00','\x00','\x00','\x00','\x00','\x00','\x50','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x0a','\xa9','\x00','\x00','\x00','\x00','\x00','\x00','\x06','\x46','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x60','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x0a','\xa0','\x00','\x00','\x00','\x00','\x00','\x05','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x03','\xaa','\xaa','\xa0','\x00',
    '\x00','\xc0','\x00','\x55','\x50','\x00','\xc0','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x03','\x0a','\xaa','\x00','\x00','\x00','\xc0','\x05','\x55','\x55','\x00','\xc0','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0d','\x33','\x3d','\xdd','\x00','\x00',
    '\x00','\xc1','\x55','\x15','\x15','\x51','\xc0','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\xdd','\xd3','\xdd','\xdd','\xd0','\x00','\x00','\xc1','\x11','\x15','\x11','\x11','\xc0','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0d','\xdd','\xdd','\xdd','\xdd','\xdd','\x00',
    '\x00','\xcc','\x11','\x11','\x11','\x1c','\xc0','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x0d','\xdd','\xdd','\xdd','\xdd','\xdd','\x00','\x00','\x0c','\xc1','\x01','\x01','\xcc','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0d','\xdd','\xdd','\xdd','\xdd','\xdd','\x00',
    '\x00','\x00','\xcc','\x01','\x0c','\xc0','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x0d','\xdd','\xdd','\xdd','\xdd','\xdd','\x00','\x00','\x00','\x0c','\x01','\x0e','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\xdd','\xdd','\xdd','\xdd','\xd0','\x00',
    '\x00','\x00','\x00','\x01','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\xdd','\xdd','\xdd','\xdd','\xd0','\x00','\x00','\x00','\x00','\x01','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0d','\xdd','\xdd','\xd0','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    };

#endif //_H_FRUITS_PNG_H_
