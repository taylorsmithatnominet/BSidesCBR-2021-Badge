#ifndef _H_WIFIAP_H_
#define _H_WIFIAP_H_

#include <BadgeApp.h>
#include <BadgeService.h>

extern BadgeApp wifi_scan_app;
extern BadgeService wifiap_service;

void wifiap_begin(void);

#endif // _H_WIFIAP_H_
