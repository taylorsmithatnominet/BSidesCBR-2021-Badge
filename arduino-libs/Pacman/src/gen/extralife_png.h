#ifndef _H_EXTRALIFE_PNG_H_
#define _H_EXTRALIFE_PNG_H_

#include <stdint.h>

#include <Arduino.h>

const size_t extralife_png_id = 1;  // 0x00000001
const size_t extralife_png_width = 16;  // 0x00000010
const size_t extralife_png_height = 16;  // 0x00000010
const size_t extralife_png_palette_size = 2;  // 0x00000002
const size_t extralife_png_palette_bits = 1;  // 0x00000001
const size_t extralife_png_size = 38;  // 0x00000026
const char extralife_png_data[38] PROGMEM  = {
    '\x00','\x00','\xff','\xc0','\x00','\xff','\x00','\x00','\x00','\x00','\x07','\xc0','\x0f','\xe0','\x1f','\xf0',
    '\x07','\xf8','\x01','\xf8','\x00','\x78','\x01','\xf8','\x07','\xf8','\x1f','\xf0','\x0f','\xe0','\x07','\xc0',
    '\x00','\x00','\x00','\x00','\x00','\x00',};

#endif //_H_EXTRALIFE_PNG_H_
