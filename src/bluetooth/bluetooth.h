#ifndef _H_BLUETOOTH_H_
#define _H_BLUETOOTH_H_

#include <Arduino.h>

#include <BadgeApp.h>
#include <BadgeService.h>

extern BadgeApp bluetooth_app;
extern BadgeService bluetooth_service;

void bluetooth_begin(void);

#endif // _H_BLUETOOTH_H_
