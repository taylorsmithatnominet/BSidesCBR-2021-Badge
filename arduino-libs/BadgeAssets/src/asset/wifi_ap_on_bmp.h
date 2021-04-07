#ifndef _H_ASSET_WIFI_AP_ON_BMP_H_
#define _H_ASSET_WIFI_AP_ON_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_wifi_ap_on_bmp_bpp = 2;  // 0x00000002
const size_t asset_wifi_ap_on_bmp_width = 16;  // 0x00000010
const size_t asset_wifi_ap_on_bmp_height = 16;  // 0x00000010
const size_t asset_wifi_ap_on_bmp_size = 64;  // 0x00000040
const char asset_wifi_ap_on_bmp_data[64] PROGMEM  = {
    '\x00','\x00','\x00','\x00','\x40','\x00','\x00','\x01','\x50','\x00','\x00','\x05','\x14','\x04','\x10','\x14',
    '\x04','\x05','\x50','\x10','\x04','\x41','\x41','\x10','\x44','\x51','\x45','\x11','\x44','\x51','\x45','\x11',
    '\x04','\x41','\x41','\x10','\x04','\x45','\x51','\x10','\x14','\x44','\x11','\x14','\x50','\x40','\x01','\x05',
    '\x40','\x40','\x01','\x01','\x00','\x40','\x01','\x00','\x00','\x40','\x01','\x00','\x00','\x40','\x01','\x00',
    };

#endif //_H_ASSET_WIFI_AP_ON_BMP_H_
