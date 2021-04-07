#include "BadgeMenu.h"

#include "BadgeApp.h"
#include "BadgeKey.h"
#include "BadgeLog.h"

#define __STR(x) #x
const char* BADGE_KEY_NAMES[BADGE_KEY_MAX] = {
    [BADGE_KEY_ENTER]   = __STR(BADGE_KEY_ENTER),
    [BADGE_KEY_LEFT]    = __STR(BADGE_KEY_LEFT),
    [BADGE_KEY_UP]      = __STR(BADGE_KEY_UP),
    [BADGE_KEY_RIGHT]   = __STR(BADGE_KEY_RIGHT),
    [BADGE_KEY_DOWN]    = __STR(BADGE_KEY_DOWN),
    [BADGE_CAPSENSE_1]  = __STR(BADGE_CAPSENSE_1),
    [BADGE_CAPSENSE_2]  = __STR(BADGE_CAPSENSE_2),
    [BADGE_CAPSENSE_3]  = __STR(BADGE_CAPSENSE_3),
    [BADGE_CAPSENSE_4]  = __STR(BADGE_CAPSENSE_4),
    [BADGE_CAPSENSE_5]  = __STR(BADGE_CAPSENSE_5),
    [BADGE_CAPSENSE_6]  = __STR(BADGE_CAPSENSE_6),
    [BADGE_CAPSENSE_7]  = __STR(BADGE_CAPSENSE_7),
    [BADGE_CAPSENSE_8]  = __STR(BADGE_CAPSENSE_8),
    [BADGE_KEY_UPDATE]  = __STR(BADGE_KEY_UPDATE),
};

BadgeMenu::BadgeMenu()
{
    selected_ = 0;
    draw_needed_ = true;
    begin_frame_ = nullptr;
    draw_item_ = nullptr;
    end_frame_ = nullptr;
    back_ = nullptr;

    app_.set_begin([this](){
        draw_needed_ = true;
    });
    app_.set_loop([this](){
        if ( !draw_needed_ )
        {
            return;
        }
        draw_needed_ = false;
        if (this->begin_frame_) this->begin_frame_(*this);
        for ( size_t i = 0; i < this->items_.size(); i++ )
        {
            BadgeMenuItem       &menu_item = *this->items_[i];
            BadgeMenuItemState  state;
            state.index = i;
            state.visible_index = i;
            state.visible_count = this->items_.size();
            state.selected = (i == this->selected_);
            if (this->draw_item_) this->draw_item_(*this, menu_item, state);
        }
        if (this->end_frame_) this->end_frame_(*this);
    });
    app_.set_end([this](){
    });
    app_.set_key_event([this](uint8_t key, bool key_down){
        /* Ignore KEY_UP events and anything non-DPAD buttons */
        if ( !key_down || !BADGE_KEY_IS_DPAD(key) )
        {
            return;
        }
        switch(key)
        {
            case BADGE_KEY_UP:
                if ( this->selected_ > 0 )
                {
                    this->selected_--;
                    draw_needed_ = true;
                }
                break;
            case BADGE_KEY_DOWN:
                if ( (this->selected_ + 1) < this->items_.size() )
                {
                    this->selected_++;
                    draw_needed_ = true;
                }
                break;
            case BADGE_KEY_RIGHT:
            case BADGE_KEY_ENTER:
                if ( this->selected_ < this->items_.size() )
                {
                    items_[this->selected_]->select();
                    draw_needed_ = true;
                }
                break;
            case BADGE_KEY_LEFT:
                if ( back_ )
                {
                    back_();
                }
                break;
            case BADGE_KEY_UPDATE:
                draw_needed_ = true;
                break;
            default:
                break;
        }
        if (!this->items_.empty()) {
            log(
                LOG_INFO, "Current menu selection: %s (%zu)"
                , this->items_[this->selected_]->name().c_str()
                , this->selected_
            );
            log_dump();
        }
    });
}

BadgeMenuItem *BadgeMenu::create_menu_item(void)
{
    BadgeMenuItem *menu_item = nullptr;

    menu_item = new BadgeMenuItem();
    items_.push_back(menu_item);

    return menu_item;
}

BadgeMenuItem *BadgeMenu::get_menu_item(const std::string &name)
{
    BadgeMenuItem *menu_item = nullptr;
    for (size_t i = 0; i < items_.size(); i++ )
    {
        menu_item = items_[i];
        if ( menu_item->name() == name )
        {
            return menu_item;
        }
    }
    menu_item = create_menu_item();
    menu_item->set_name(name);
    return menu_item;
}

void BadgeMenu::menu_clear()
{
    BadgeMenuItem *menu_item = nullptr;
    for (size_t i = 0; i < items_.size(); i++ )
    {
        menu_item = items_[i];
        delete menu_item;
    }
    items_.clear();
}
