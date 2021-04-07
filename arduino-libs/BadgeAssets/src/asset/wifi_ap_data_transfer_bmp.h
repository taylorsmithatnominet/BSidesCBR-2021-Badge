#ifndef _H_ASSET_WIFI_AP_DATA_TRANSFER_BMP_H_
#define _H_ASSET_WIFI_AP_DATA_TRANSFER_BMP_H_

#include <stdint.h>

#include <Arduino.h>
const size_t asset_wifi_ap_data_transfer_bmp_bpp = 2;  // 0x00000002
const size_t asset_wifi_ap_data_transfer_bmp_width = 16;  // 0x00000010
const size_t asset_wifi_ap_data_transfer_bmp_height = 16;  // 0x00000010
const size_t asset_wifi_ap_data_transfer_bmp_size = 64;  // 0x00000040
const char asset_wifi_ap_data_transfer_bmp_data[64] PROGMEM  = {
    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x0f','\xf0','\x00',
    '\xc0','\x3f','\xf0','\x00','\xf0','\xff','\xf0','\x00','\xfc','\xff','\xf3','\x00','\x00','\x0f','\xf0','\x00',
    '\x00','\x0f','\xf0','\x00','\x00','\xcf','\xff','\x3f','\x00','\x0f','\xff','\x0f','\x00','\x0f','\xfc','\x03',
    '\x00','\x0f','\xf0','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
    };

#endif //_H_ASSET_WIFI_AP_DATA_TRANSFER_BMP_H_
