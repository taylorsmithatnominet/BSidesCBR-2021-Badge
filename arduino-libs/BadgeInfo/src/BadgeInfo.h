#ifndef _BADGE_INFO_
#define _BADGE_INFO_

#include <Arduino.h>

#include <stdint.h>
#include <stddef.h>

#include <BadgeService.h>

uint32_t device_id(void);
void wifi_mac_address(uint8_t *mac);
void bt_mac_address(uint8_t *mac);
void device_name(char *out, size_t out_size);
void wifi_passwd(char *out, size_t out_size);
void device_battery(uint16_t *vbat);

size_t device_info_count(void);
void device_info_name(size_t index, char *out, size_t out_size);
void device_info_value(size_t index, char *out, size_t out_size);

extern BadgeService device_info_svc;

#endif // _BADGE_INFO_
