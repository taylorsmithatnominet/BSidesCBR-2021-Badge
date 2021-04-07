#ifndef _H_BOOT_ANIMATION_H_
#define _H_BOOT_ANIMATION_H_

#include <Arduino.h>
#include <BadgeOs.h>
#include <BadgeService.h>
#include <BadgeApp.h>
#include <BadgeMenu.h>
#include <BadgeKey.h>

#define BOOT_IMAGES_DELAY_MS        2000

extern BadgeApp         bootanimation_app;
extern BadgeService     bootanimation_service;

void bootanimation_begin();

#endif // _H_BOOT_ANIMATION_H_
