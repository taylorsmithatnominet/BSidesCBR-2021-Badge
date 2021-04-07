#include "about.h"
#include "about_assets.h"

#include <Arduino.h>
#include <BadgeOs.h>
#include <BadgeLog.h>
#include <BadgeMenu.h>
#include <BadgeTextViewer.h>
#include <BSidesBadge2020.h>
#include <display.h>
#include <menu.h>
#include <device_info.h>

BadgeTextViewer badge_info_viewer;
BadgeTextViewer sponsor_info_viewer;
BadgeTextViewer license_info_viewer;

void begin_frame(BadgeTextViewer &viewer) {
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
}

void calc_string_view(const char *text, size_t *width, size_t *height) {
    *height = 8;
    *width = strlen(text) * 6;
}

void draw_string(size_t x, size_t y, const char *text) {
    display.setCursor(1 + x, 16 + 2 + y);
    log(LOG_INFO, "draw_string: %s (%zd, %zd)", text, x, y);
    display.print(text);
}

void end_frame(void) {
    display.writeFastHLine(0, 16, display.width(), DISPLAY_COLOR_WHITE);
    display.display();
}

void back_button(void) {
    badge_os.pop_app();
}

void about_begin(void)
{
    // Add to the "About" menu
    BadgeMenuItem *menu_item = nullptr;
    menu_item = about_menu.create_menu_item();
    menu_item->set_name(badge_info_name);
    menu_item->set_select_event([](){
        badge_info_viewer.set_name(badge_info_name);
        badge_info_viewer.set_text(badge_info_text, strlen(badge_info_text));
        badge_os.push_app(badge_info_viewer.app());
    });
    menu_item = about_menu.create_menu_item();
    menu_item->set_name(license_info_name);
    menu_item->set_select_event([](){
        license_info_viewer.set_name(license_info_name);
        license_info_viewer.set_text(license_info_text, strlen(license_info_text));
        badge_os.push_app(license_info_viewer.app());
    });
    menu_item = about_menu.create_menu_item();
    menu_item->set_name(sponsor_info_name);
    menu_item->set_select_event([](){
        sponsor_info_viewer.set_name(sponsor_info_name);
        sponsor_info_viewer.set_text(sponsor_info_text, strlen(sponsor_info_text));
        badge_os.push_app(sponsor_info_viewer.app());
    });
    menu_item = about_menu.create_menu_item();
    menu_item->set_name("Status");
    menu_item->set_select_event([](){ badge_os.push_app(device_info); });

    // Set the view dimensions
    size_t width = display.width() - 2 - 3;
    size_t height = display.height() - 16 - 3;
    badge_info_viewer.set_view(width, height);
    sponsor_info_viewer.set_view(width, height);
    license_info_viewer.set_view(width, height);

    // Set the view begin frame
    badge_info_viewer.set_begin_frame(begin_frame);
    sponsor_info_viewer.set_begin_frame(begin_frame);
    license_info_viewer.set_begin_frame(begin_frame);

    // Set the view calc string
    badge_info_viewer.set_calc_string_view(calc_string_view);
    sponsor_info_viewer.set_calc_string_view(calc_string_view);
    license_info_viewer.set_calc_string_view(calc_string_view);

    // Set the view draw string
    badge_info_viewer.set_draw_string(draw_string);
    sponsor_info_viewer.set_draw_string(draw_string);
    license_info_viewer.set_draw_string(draw_string);

    // Set the view end frame
    badge_info_viewer.set_end_frame(end_frame);
    sponsor_info_viewer.set_end_frame(end_frame);
    license_info_viewer.set_end_frame(end_frame);

    // Set the back action
    badge_info_viewer.set_back_action(back_button);
    sponsor_info_viewer.set_back_action(back_button);
    license_info_viewer.set_back_action(back_button);
}
