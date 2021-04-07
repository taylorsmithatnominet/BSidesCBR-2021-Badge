#ifndef _BADGE_SERVICE_
#define _BADGE_SERVICE_

#include <stdint.h>
#include <functional>
#include <string>

class BadgeService  {
    public:
        BadgeService(
            std::function<void(void)> begin
            , std::function<void(void)> loop
            , std::function<void(void)> end
        ) { init(); begin_ = begin; loop_ = loop; end_ = end; }
        BadgeService() : BadgeService(nullptr, nullptr, nullptr) {}

        void set_begin(std::function<void(void)> func) { begin_ = func; }
        void set_loop(std::function<void(void)> func) { loop_ = func; }
        void set_end(std::function<void(void)> func) { end_ = func; }
        void set_name(const std::string &name) { name_ = name; }
        void set_draw_icon(std::function<void(uint16_t,uint16_t,uint16_t,uint16_t)> func) {
            draw_icon_ = func;
            visible_ = (draw_icon_ != nullptr);
        }
        void set_period_ms(uint32_t period_ms) { period_ms_ = period_ms; }

        void start(void) { begin(); }
        void stop(void) { end(); }
        bool running(void) { return running_; }
        void show_icon(void) { visible_ = true; }
        void hide_icon(void) { visible_ = false; }
        bool visible(void) { return draw_icon_ && visible_; };
        std::string &name() { return name_; }

        void begin(void);
        void loop(void);
        void end(void) { if ( end_ && running_ ) { end_(); } running_ = false; }
        void draw_icon(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
            if ( draw_icon_ && visible_ )
            {
                draw_icon_(x, y, w, h);
            }
        }

    private:
        std::string name_;
        bool running_;
        bool visible_;
        uint32_t period_ms_;
        uint32_t last_tick_ms_;
        std::function<void(void)> begin_;
        std::function<void(void)> loop_;
        std::function<void(void)> end_;
        std::function<void(uint16_t,uint16_t,uint16_t,uint16_t)> draw_icon_;

        void init(void) {
            running_ = false;
            visible_ = false;
            period_ms_ = 0;
            last_tick_ms_ = 0;
            draw_icon_ = nullptr;
        }
};

#endif // _BADGE_SERVICE_
