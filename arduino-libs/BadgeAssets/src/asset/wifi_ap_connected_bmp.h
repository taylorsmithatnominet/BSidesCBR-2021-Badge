#ifndef _H_ASSET_WIFI_AP_CONNECTED_BMP_H_
#define _H_ASSET_WIFI_AP_CONNECTED_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_wifi_ap_connected_bmp_bpp = 2;  // 0x00000002
const size_t asset_wifi_ap_connected_bmp_width = 16;  // 0x00000010
const size_t asset_wifi_ap_connected_bmp_height = 16;  // 0x00000010
const size_t asset_wifi_ap_connected_bmp_size = 64;  // 0x00000040
const char asset_wifi_ap_connected_bmp_data[64] PROGMEM  = {
    '\x00','\x00','\x00','\x00','\xc0','\x00','\x00','\x03','\xf0','\x00','\x00','\x0f','\x3c','\x0c','\x30','\x3c',
    '\x0c','\x0f','\xf0','\x30','\x00','\xc0','\x03','\x00','\x33','\xf3','\xcf','\xcc','\x33','\xf3','\xcf','\xcc',
    '\x00','\xc0','\x03','\x00','\x0c','\xcf','\xf3','\x30','\x3c','\xcc','\x33','\x3c','\xf0','\xc0','\x03','\x0f',
    '\xc0','\xc0','\x03','\x03','\x00','\xc0','\x03','\x00','\x00','\xc0','\x03','\x00','\x00','\xc0','\x03','\x00',
    };

#endif //_H_ASSET_WIFI_AP_CONNECTED_BMP_H_
