#ifndef _ADAFRUIT_CACHE_
#define _ADAFRUIT_CACHE_

#include <stdint.h>

#include <Adafruit_GFX.h>

class Adafruit_Cache : public Adafruit_GFX  {
    public:
        Adafruit_Cache(Adafruit_GFX *gfx, int16_t width, int16_t height);

        // Adafruit_GFX provides all other drawing APIs from this
        virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
        virtual void fillScreen(uint16_t color);

        // flush drawing to screen
        void display(void);

        // Pass rotation call to GFX layer
        void setRotation(uint8_t r);

    private:
        size_t get_pixel_byte(int16_t x, int16_t y);
        size_t get_pixel_shift(int16_t x, int16_t y);
        size_t get_pixel_mask(int16_t x, int16_t y);
        uint8_t set_palette_color(uint16_t color);
        uint16_t get_palette_color(uint8_t index);
        uint16_t get_pixel_value(int16_t x, int16_t y, bool front_buffer);
        void set_pixel_value(int16_t x, int16_t y, uint16_t color, bool front_buffer);

        Adafruit_GFX    *gfx_;

        bool            flushed_onced_;

        uint8_t         *pixel_data_back_;
        uint8_t         *pixel_data_front_;
        size_t          pixel_data_size_;

        size_t          row_data_size_;

        uint16_t        *palette_;
        size_t          palette_count_;
        size_t          palette_used_;
};

#endif // _ADAFRUIT_CACHE_
