#ifndef _H_SERIALBRIDGE_H_
#define _H_SERIALBRIDGE_H_

#include <Arduino.h>
#include <memory>

#include <BadgeApp.h>
#include <DiagApp.h>
#include <BSidesBadge2020.h>

void serialbridge_begin(void);

/* This app is used as a POST diagnostic for the display */
extern std::shared_ptr<DiagApp> serialbridge_post;

#endif // _H_SERIALBRIDGE_H_
