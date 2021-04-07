#ifndef _H_DISPLAY_H_
#define _H_DISPLAY_H_

#include <memory>

#include <BadgeApp.h>
#include <DiagApp.h>
#include <BSidesBadge2020.h>
#ifdef ESP32
#include <Adafruit_SSD1675BX.h>
#endif
#ifdef __unix__
#include <Adafruit_X11.h>
#include <Adafruit_Cache.h>
#endif

#ifdef ESP32
extern Adafruit_SSD1675BX   display;
#endif
#ifdef __unix__
extern Adafruit_X11         x11_window;
extern Adafruit_Cache       display;
#endif

/* This app is used as a POST diagnostic for the display */
extern std::shared_ptr<DiagApp> display_post;
/* This app simply shows an interstitial menu before exiting */
extern BadgeApp display_2020_interstitial;

void display_begin(void);
size_t display_draw_status_bar(void);

/* This enumeration allows us to easily select backgrounds to draw */
typedef enum {
    BG_2020,
    BG_2021,
    BG_DEFAULT = BG_2021,
    BG_SUSPEND,
    BG_WHITE,
    BG_RED,
    BG_BLACK,
} display_bg_e;

size_t display_draw_background(display_bg_e);

#endif // _H_DISPLAY_H_
