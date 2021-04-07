#include "BadgeOs.h"

#include "BadgeApp.h"
#include "BadgeService.h"

BadgeOs         badge_os;

BadgeOs::BadgeOs()
{
    app_ = nullptr;
    app_enable_ = true;
}

void BadgeOs::set_app(BadgeApp &app)
{
    if ( app_ )
    {
        app_->end();
    }
    app_ = &app;
    if ( app_ )
    {
        app_->begin();
    }
    apps_.clear();
}

void BadgeOs::push_app(BadgeApp &app)
{
    if ( app_ )
    {
        app_->end();
        apps_.push_back(app_);
    }
    app_ = &app;
    if ( app_ )
    {
        app_->begin();
    }
}

bool BadgeOs::pop_app(void)
{
    if ( apps_.size() == 0 )
    {
        return false;
    }
    if ( app_ )
    {
        app_->end();
    }
    app_ = apps_[apps_.size() - 1];
    apps_.erase(apps_.end() - 1);
    app_->begin();
    return true;
}

void BadgeOs::reload_app()
{
    if ( app_)
    {
        app_->end();
        app_->begin();
    }
}

void BadgeOs::begin(void)
{

}

void BadgeOs::key_event(uint8_t key, bool key_down)
{
    if ( app_ )
    {
        app_->key_event(key, key_down);
    }
}

void BadgeOs::loop(void)
{
    if ( app_ && app_enable_ )
    {
        app_->loop();
    }
    for ( BadgeService *service : services_ )
    {
        service->loop();
    }
}

void BadgeOs::end(void)
{
    if ( app_ )
    {
        app_->end();
    }
    app_ = nullptr;
    apps_.clear();
    for ( BadgeService *service : services_ )
    {
        service->end();
    }
}

void BadgeOs::add_service(BadgeService &service)
{
    services_.push_back(&service);
}

BadgeService *BadgeOs::get_service(const std::string &name)
{
    for ( BadgeService *service : services_ )
    {
        if ( service->name() == name )
        {
            return service;
        }
    }
    return nullptr;
}
