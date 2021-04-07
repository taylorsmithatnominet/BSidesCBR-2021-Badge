#ifndef _BADGE_OS_
#define _BADGE_OS_

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

#include "BadgeKey.h"

class BadgeService;
class BadgeApp;

class BadgeOs  {
    public:
        BadgeOs();

        void set_app(BadgeApp &app);
        void push_app(BadgeApp &app);
        bool pop_app(void);
        void disable_app() { app_enable_ = false; }
        void enable_app() { app_enable_ = true; }
        void reload_app();

        void add_service(BadgeService &service);
        std::vector<BadgeService*> services() { return services_; }
        BadgeService *get_service(const std::string &name);

        void update(void) { key_event(BADGE_KEY_UPDATE, true); key_event(BADGE_KEY_UPDATE, false); }

        void begin(void);
        void key_event(uint8_t key, bool key_down);
        void loop(void);
        void end(void);

    private:
        bool app_enable_;
        BadgeApp *app_;
        std::vector<BadgeApp*> apps_;
        std::vector<BadgeService*> services_;
};

extern BadgeOs badge_os;

#endif // _BADGE_OS_
