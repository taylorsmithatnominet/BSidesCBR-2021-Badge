#include "BadgeTextViewer.h"

#include <Arduino.h>
#include <viewerc.h>

#include "BadgeApp.h"
#include "BadgeKey.h"

ssize_t VWRC_API viewerc_read(void *ctx, size_t offset, char *text, size_t text_size)
{
    return reinterpret_cast<BadgeTextViewer*>(ctx)->read(offset, text, text_size);
}

void VWRC_API viewerc_calc_string_view(void *ctx, const char *text, size_t *width, size_t *height)
{
    return reinterpret_cast<BadgeTextViewer*>(ctx)->calc_string_view(text, width, height);
}

void VWRC_API viewerc_draw_string(void *ctx, size_t x, size_t y, const char *text)
{
    return reinterpret_cast<BadgeTextViewer*>(ctx)->draw_string(x, y, text);
}

BadgeTextViewer::BadgeTextViewer()
{
    back_ = nullptr;
    calc_string_view_ = nullptr;
    begin_frame_ = nullptr;
    draw_string_ = nullptr;
    end_frame_ = nullptr;
    text_ = nullptr;
    text_size_ = 0;
    draw_needed_ = true;
    width_ = 0;
    height_ = 0;

    app_.set_begin([this](){
        vwrc_init(this->viewerc_, sizeof(this->viewerc_));
        vwrc_set_view(this->viewerc_, this->width_, this->height_);
        vwrc_set_calc_string_view(this->viewerc_, viewerc_calc_string_view, this);
        vwrc_set_draw_string(this->viewerc_, viewerc_draw_string, this);
        vwrc_set_text(this->viewerc_, this->text_size_, viewerc_read, this);
        draw_needed_ = true;
    });
    app_.set_loop([this](){
        if ( !this->draw_needed_ )
        {
            return;
        }
        this->begin_frame_(*this);
        vwrc_draw_view(this->viewerc_);
        this->end_frame_();
        draw_needed_ = false;
    });
    app_.set_end([this](){
        if ( this->text_ )
        {
            delete this->text_;
        }
        this->text_ = nullptr;
        this->text_size_ = 0;
        vwrc_fini(this->viewerc_);
    });
    app_.set_key_event([this](uint8_t key, bool key_down){
        if ( !key_down )
        {
            return;
        }
        switch(key)
        {
            case BADGE_KEY_UP:
                vwrc_scroll_up_page(this->viewerc_);
                this->draw_needed_ = true;
                break;
            case BADGE_KEY_DOWN:
                vwrc_scroll_down_page(this->viewerc_);
                this->draw_needed_ = true;
                break;
            case BADGE_KEY_ENTER:
                if ( this->back_ )
                {
                    this->back_();
                }
                break;
            case BADGE_KEY_LEFT:
                if ( this->back_ )
                {
                    this->back_();
                }
                break;
            case BADGE_KEY_UPDATE:
                this->draw_needed_ = true;
                break;
            default:
                break;
        }
    });
}

void BadgeTextViewer::set_text(const char *text, size_t text_size)
{
    if ( this->text_ )
    {
        delete this->text_;
    }
    this->text_ = new char[text_size];
    memcpy(this->text_, text, text_size);
    this->text_size_ = text_size;
}

ssize_t BadgeTextViewer::read(size_t offset, char *text, size_t text_size)
{
    if ( !this->text_ )
    {
        return 0;
    }
    if ( offset >= this->text_size_ )
    {
        return 0;
    }
    if ( (offset + text_size ) > this->text_size_ )
    {
        text_size = this->text_size_ - offset;
    }
    memcpy(text, this->text_ + offset, text_size);
    return text_size;
}

void BadgeTextViewer::calc_string_view(const char *text, size_t *width, size_t *height)
{
    if ( this->calc_string_view_ )
    {
        this->calc_string_view_(text, width, height);
    }
    else
    {
        *width = 0;
        *height = 0;
    }
}

void BadgeTextViewer::draw_string(size_t x, size_t y, const char *text)
{
    if ( this->draw_string_ )
    {
        this->draw_string_(x, y, text);
    }
}
