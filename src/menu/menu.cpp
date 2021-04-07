#include "menu.h"

#include <display.h>
#include <BadgeSchedule.h>
#include <schedule.h>
#include <BadgeOs.h>
#include <BadgeService.h>
#include <BadgeMenu.h>
#include <BadgeTextViewer.h>
#include <BadgeAssets.h>
#include <asset/switch_on_bmp.h>
#include <asset/switch_off_bmp.h>
#ifdef HAVE_SUSPEND
#include <suspend.h>
#endif // HAVE_SUSPEND

BadgeMenu               main_menu;
BadgeMenu               services_menu;
BadgeMenu               game_menu;
BadgeMenu               app_menu;
BadgeMenu               schedule_menu;
BadgeMenu               day_menu;
BadgeTextViewer         talk_viewer;
BadgeMenu               about_menu;

void draw_menu_begin(BadgeMenu &menu)
{
#ifdef ESP32
    display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
#endif
    display.fillScreen(DISPLAY_COLOR_BLACK);
    size_t y = display_draw_background(BG_DEFAULT);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(8, 0);
    display.setTextSize(2, 2);
    display.print(menu.name().c_str());
    display.setCursor(6, display.height() - 102);
    display.setTextSize(1, 1);
    display.print("Press LEFT to go     back in any menu");
    display.setCursor(8, y);
}

void draw_menu_item(BadgeMenu &menu, BadgeMenuItem &item, BadgeMenuItemState &state)
{
    int16_t y = display.getCursorY();
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    if ( state.selected )
    {
        display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_RED);
    }
    display.setCursor(8, y);
    display.setTextSize(2, 2);
    display.print(item.name().c_str());
    display.print("\n");
}

void draw_menu_item_small(BadgeMenu &menu, BadgeMenuItem &item, BadgeMenuItemState &state)
{
    int16_t y = display.getCursorY();
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    if ( state.selected )
    {
        display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_RED);
    }
    display.setCursor(8, y);
    display.setTextSize(1, 1);
    display.print(item.name().c_str());
    display.print("\n");
}

void draw_menu_item_service(BadgeMenu &menu, BadgeMenuItem &item, BadgeMenuItemState &state)
{
    int16_t y = display.getCursorY();
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    if ( state.selected )
    {
        display.setTextColor(DISPLAY_COLOR_BLACK, DISPLAY_COLOR_RED);
    }
    BadgeService *service = badge_os.get_service(item.name());
    size_t x = 8;
    if ( service )
    {
        if ( service->running() )
        {
            DRAW_ASSET(3, y, switch_on_bmp, display);
        }
        else
        {
            DRAW_ASSET(3, y, switch_off_bmp, display);
        }
        x = 3 + 32 + 3;
    }
    display.setCursor(x, y);
    display.setTextSize(2, 2);
    display.print(item.name().c_str());
    display.setCursor(0, y + 16 + 3);
}

void draw_menu_end(BadgeMenu &menu)
{
    display.writeFastHLine(0, 16, display.width(), DISPLAY_COLOR_WHITE);
    display.display();
}

void menu_begin(void)
{
    // setup menus
    BadgeMenuItem *menu_item = nullptr;

    // main menu
    main_menu.set_name("Menu");
    main_menu.set_begin_frame(draw_menu_begin);
    main_menu.set_draw_item(draw_menu_item);
    main_menu.set_end_frame(draw_menu_end);
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("Apps");
    menu_item->set_select_event([](){ badge_os.push_app(app_menu.app()); });
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("Games");
    menu_item->set_select_event([](){ badge_os.push_app(game_menu.app()); });
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("Schedule");
    // TODO: add selectable schedule rather than QRCode
    menu_item->set_select_event([](){ badge_os.push_app(schedule_app); });
    //menu_item->set_select_event([](){ badge_os.push_app(schedule_menu.app()); });
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("Services");
    menu_item->set_select_event([](){ badge_os.push_app(services_menu.app()); });
    // Add less important menu items
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("About");
    menu_item->set_select_event([](){ badge_os.push_app(about_menu.app()); });
#ifdef HAVE_SUSPEND
    menu_item = main_menu.create_menu_item();
    menu_item->set_name("Suspend");
    menu_item->set_select_event([](){ badge_os.push_app(suspend_app); });
#endif // HAVE_SUSPEND

    // services menu
    services_menu.set_name("Services");
    services_menu.set_begin_frame(draw_menu_begin);
    services_menu.set_draw_item(draw_menu_item_service);
    services_menu.set_end_frame(draw_menu_end);
    services_menu.set_back_event([](){ badge_os.pop_app(); });
    for ( BadgeService *service : badge_os.services() )
    {
        if ( service->name() == "" )
        {
            continue;
        }
        menu_item = services_menu.create_menu_item();
        menu_item->set_name(service->name());
        menu_item->set_select_event([service](){
            if ( service->running() )
            {
                service->stop();
            }
            else
            {
                service->start();
            }
            badge_os.update();
        });
    }

    // game menu
    game_menu.set_name("Games");
    game_menu.set_begin_frame(draw_menu_begin);
    game_menu.set_draw_item(draw_menu_item);
    game_menu.set_end_frame(draw_menu_end);
    game_menu.set_back_event([](){ badge_os.pop_app(); });

    // app menu
    app_menu.set_name("Apps");
    app_menu.set_begin_frame(draw_menu_begin);
    app_menu.set_draw_item(draw_menu_item);
    app_menu.set_end_frame(draw_menu_end);
    app_menu.set_back_event([](){ badge_os.pop_app(); });

    // schedule menu
    schedule_menu.set_name("Schedule");
    schedule_menu.set_begin_frame(draw_menu_begin);
    schedule_menu.set_draw_item(draw_menu_item);
    schedule_menu.set_end_frame(draw_menu_end);
    schedule_menu.set_back_event([](){ badge_os.pop_app(); });
    // add all the days as menu items in the schedule menu
    for (size_t i = 0; i < schedule_get_talk_count(); i++)
    {
        char text[20];
        size_t recd = 0;
        recd = schedule_get_talk_value(i, SCHEDULE_DAY_VALUE_ID, 0, text, sizeof(text)-1);
        text[recd] = '\0';
        if ( recd == 0 )
        {
            continue;
        }
        std::string day(text);
        menu_item = schedule_menu.get_menu_item(day);
        // update the day entries when the given day is selected
        menu_item->set_select_event([day](){
            day_menu.set_name(day);
            day_menu.menu_clear();
            day_menu.set_back_event([](){ badge_os.pop_app(); });
            BadgeMenuItem *menu_item = nullptr;
            // add all day/time entries for this (selected) day
            for (size_t i = 0; i < schedule_get_talk_count(); i++)
            {
                char text[20];
                size_t recd = 0;
                recd = schedule_get_talk_value(i, SCHEDULE_DAY_VALUE_ID, 0, text, sizeof(text)-1);
                text[recd] = '\0';
                if ( day != text )
                {
                    continue;
                }
                recd = schedule_get_talk_value(i, SCHEDULE_TIME_VALUE_ID, 0, text, sizeof(text)-1);
                text[recd] = '\0';
                if ( recd == 0 )
                {
                    continue;
                }
                std::string time(text);
                size_t talk_index = i;
                menu_item = day_menu.create_menu_item();
                menu_item->set_name(day + " - " + time);
                // update the talk viewer when the day/time is selected
                menu_item->set_select_event([talk_index, time](){
                    char *text = NULL;
                    size_t text_size = schedule_get_talk_text_size(talk_index);
                    text = new char[text_size];
                    size_t recd = schedule_get_talk_text(talk_index, 0, text, text_size);
                    talk_viewer.set_name(time);
                    talk_viewer.set_text(text, recd);
                    delete text;
                    badge_os.push_app(talk_viewer.app());
                });
            }
            badge_os.push_app(day_menu.app());
        });
    }

    // day menu
    day_menu.set_name("Day");
    day_menu.set_begin_frame(draw_menu_begin);
    day_menu.set_draw_item(draw_menu_item_small);
    day_menu.set_end_frame(draw_menu_end);

    // talk viewer
    talk_viewer.set_view(display.width() - 2 - 3, display.height() - 16 - 3);
    talk_viewer.set_begin_frame([](BadgeTextViewer &viewer){
        display.fillScreen(DISPLAY_COLOR_BLACK);
        display_draw_status_bar();
        if ( viewer.rows_per_view() < viewer.row_count() )
        {
            size_t view_offset = 16 + 2;
            size_t view_height = display.height() - 16 - 3;
            size_t progress_bar_y = view_offset + ((viewer.row() * view_height) / viewer.row_count());
            size_t progress_bar_height = ((viewer.rows_per_view() * view_height) / viewer.row_count());
            display.writeFastVLine(display.width() - 3, progress_bar_y, progress_bar_height, DISPLAY_COLOR_RED);
            display.writeFastVLine(display.width() - 2, progress_bar_y, progress_bar_height, DISPLAY_COLOR_RED);
        }
        display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
        display.setCursor(8, 0);
        display.setTextSize(2, 2);
        display.print(viewer.name().c_str());
        display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
        display.setTextSize(1, 1);
    });
    talk_viewer.set_calc_string_view([](const char *text, size_t *width, size_t *height){
        *height = 8;
        *width = strlen(text) * 6;
    });
    talk_viewer.set_draw_string([](size_t x, size_t y, const char *text){
        display.setCursor(1 + x, 16 + 2 + y);
        display.print(text);
    });
    talk_viewer.set_end_frame([](){
        display.writeFastHLine(0, 16, display.width(), DISPLAY_COLOR_WHITE);
        display.display();
    });
    talk_viewer.set_back_action([](){ badge_os.pop_app(); });

    // about menu
    about_menu.set_name("About");
    about_menu.set_begin_frame(draw_menu_begin);
    about_menu.set_draw_item(draw_menu_item);
    about_menu.set_end_frame(draw_menu_end);
    about_menu.set_back_event([](){ badge_os.pop_app(); });
}
