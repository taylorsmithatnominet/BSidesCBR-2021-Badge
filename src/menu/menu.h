#ifndef _H_MENU_H_
#define _H_MENU_H_

#include <Arduino.h>

#include <BadgeMenu.h>

extern BadgeMenu    main_menu;
extern BadgeMenu    game_menu;
extern BadgeMenu    app_menu;
extern BadgeMenu    about_menu;

#define main_menu_app   main_menu.app()

void menu_begin(void);

#endif // _H_MENU_H_
