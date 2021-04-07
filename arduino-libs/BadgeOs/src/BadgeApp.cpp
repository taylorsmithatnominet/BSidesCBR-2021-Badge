#include "BadgeApp.h"

BadgeApp::BadgeApp(
    std::function<void(void)> begin
    , std::function<void(void)> loop
    , std::function<void(void)> end
    , std::function<void(uint8_t, bool)> keyh
)
{
    begin_ = begin;
    loop_ = loop;
    end_ = end;
    key_event_ = keyh;
}

void BadgeApp::set_begin(std::function<void(void)> func)
{
    begin_  = func;
}

void BadgeApp::set_loop(std::function<void(void)> func)
{
    loop_  = func;
}

void BadgeApp::set_end(std::function<void(void)> func)
{
    end_  = func;
}

void BadgeApp::set_key_event(std::function<void(uint8_t,bool)> func)
{
    key_event_  = func;
}

void BadgeApp::begin(void)
{
    if ( begin_ )
    {
        begin_();
    }
}

void BadgeApp::key_event(uint8_t key, bool key_down)
{
    if ( key_event_ )
    {
        key_event_(key, key_down);
    }
}

void BadgeApp::loop(void)
{
    if ( loop_ )
    {
        loop_();
    }
}

void BadgeApp::end(void)
{
    if ( end_ )
    {
        end_();
    }
}
