#ifndef _BADGE_APP_
#define _BADGE_APP_

#include <stdint.h>
#include <functional>

class BadgeApp  {
    public:
        BadgeApp(
            std::function<void(void)> begin
            , std::function<void(void)> loop
            , std::function<void(void)> end
            , std::function<void(uint8_t, bool)> keyh
        );
        BadgeApp() { BadgeApp(nullptr, nullptr, nullptr, nullptr); }

        void set_begin(std::function<void(void)> func);
        void set_loop(std::function<void(void)> func);
        void set_end(std::function<void(void)> func);
        void set_key_event(std::function<void(uint8_t,bool)> func);

        virtual void begin(void);
        virtual void key_event(uint8_t key, bool key_down);
        virtual void loop(void);
        virtual void end(void);

    protected:
        std::function<void(void)> begin_;
        std::function<void(void)> loop_;
        std::function<void(void)> end_;
        std::function<void(uint8_t,bool)> key_event_;
};

#endif // _BADGE_APP_
