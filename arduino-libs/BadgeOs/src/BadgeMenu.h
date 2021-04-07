#ifndef _BADGE_MENU_
#define _BADGE_MENU_

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

#include "BadgeApp.h"

class BadgeMenuItem  {
    public:
        BadgeMenuItem() : select_(nullptr) {}

        void set_name(const std::string &name) { name_ = name; }
        void set_select_event(std::function<void(void)> func) { select_ = func; }

        std::string &name(void) { return name_; }
        void select(void) { if ( select_ ) { select_(); } }

    private:
        std::string name_;
        std::function<void(void)> select_;
};

typedef struct _BadgeMenuItemState {
    size_t index;
    size_t visible_index;
    size_t visible_count;
    bool selected;
} BadgeMenuItemState;

class BadgeMenu  {
    public:
        BadgeMenu();

        void set_name(const std::string &name) { name_ = name; }
        void set_begin_frame(std::function<void(BadgeMenu&)> func) { begin_frame_ = func; }
        void set_draw_item(std::function<void(BadgeMenu&,BadgeMenuItem&,BadgeMenuItemState&)> func) { draw_item_ = func; }
        void set_end_frame(std::function<void(BadgeMenu&)> func) { end_frame_ = func; }
        void set_back_event(std::function<void(void)> func) { back_ = func; }
        BadgeMenuItem *create_menu_item(void);
        BadgeMenuItem *get_menu_item(const std::string &name);
        void menu_clear();

        void redraw(void) { draw_needed_ = true; }

        BadgeApp &app(void) { return app_; }
        std::string &name(void) { return name_; }
        size_t count(void) { return items_.size(); }

    private:
        bool draw_needed_;
        size_t selected_;
        std::string name_;
        BadgeApp app_;
        std::vector<BadgeMenuItem*> items_;
        std::function<void(BadgeMenu&)> begin_frame_;
        std::function<void(BadgeMenu&,BadgeMenuItem&,BadgeMenuItemState&)> draw_item_;
        std::function<void(BadgeMenu&)> end_frame_;
        std::function<void(void)> back_;
};

#endif // _BADGE_MENU_
