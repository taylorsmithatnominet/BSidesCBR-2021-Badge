#ifndef _BADGE_TEXT_VIEWER_
#define _BADGE_TEXT_VIEWER_

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

#include <viewerc.h>

#include "BadgeApp.h"

typedef void(VWRC_API *VwrcCalcStringViewFn)(void *, const char *, size_t *, size_t *);
typedef void(VWRC_API *VwrcDrawStringFn)(void *, size_t, size_t, const char *);

class BadgeTextViewer  {
    public:
        BadgeTextViewer();
        void set_name(const std::string &name) { name_ = name; }
        void set_text(const char *text, size_t text_size);
        void set_view(size_t width, size_t height) { width_ = width; height_ = height; }
        void set_calc_string_view(std::function<void(const char *, size_t *, size_t *)> func) { calc_string_view_ = func; };
        void set_begin_frame(std::function<void(BadgeTextViewer&)> func) { begin_frame_ = func; }
        void set_draw_string(std::function<void(size_t, size_t, const char *)> func) { draw_string_ = func; }
        void set_end_frame(std::function<void(void)> func) { end_frame_ = func; }
        void set_back_action(std::function<void(void)> func) { back_ = func; }

        // getters
        std::string &name(void) { return name_; }
        size_t row(void) { size_t row = 0; vwrc_get_row(this->viewerc_, &row); return row; }
        size_t row_count(void) { size_t row_count = 0; vwrc_get_row_count(this->viewerc_, &row_count); return row_count; }
        size_t rows_per_view(void) { size_t rows_per_view = 0; vwrc_get_rows_per_view(this->viewerc_, &rows_per_view); return rows_per_view; }
        BadgeApp &app(void) { return app_; }

        // internal use
        ssize_t read(size_t offset, char *text, size_t text_size);
        void calc_string_view(const char *text, size_t *width, size_t *height);
        void draw_string(size_t x, size_t y, const char *text);

    private:
        std::string name_;
        bool draw_needed_;
        char *text_;
        size_t text_size_;
        size_t width_;
        size_t height_;
        BadgeApp app_;
        std::function<void(void)> back_;
        std::function<void(const char *, size_t *, size_t *)> calc_string_view_;
        std::function<void(BadgeTextViewer&)> begin_frame_;
        std::function<void(size_t, size_t, const char *)> draw_string_;
        std::function<void(void)> end_frame_;
        uint8_t viewerc_[VWRC_CALC_DATA_SIZE(100)];
};

#endif // _BADGE_TEXT_VIEWER_
