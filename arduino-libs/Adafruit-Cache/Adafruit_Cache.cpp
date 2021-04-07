#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>

#include "Adafruit_Cache.h"

#define PALETTE_SIZE        4
#define PALETTE_MASK        0x3
#define PIXELS_PER_BYTE     4
#define BITS_PER_PIXEL      2

Adafruit_Cache::Adafruit_Cache(Adafruit_GFX *gfx, int16_t width, int16_t height) : Adafruit_GFX(width, height)
{
    gfx_ = gfx;
    size_t width_ = (size_t)width;
    size_t height_ = (size_t)height;

    // display has never been flushed before
    flushed_onced_ = false;

    // no pixel data
    pixel_data_front_ = NULL;
    pixel_data_back_ = NULL;
    pixel_data_size_ = 0;

    // no palette
    palette_ = NULL;
    palette_count_ = 0;
    palette_used_ = 0;

    // width must be a multiple of pixels per byte
    if ( (width_ % PIXELS_PER_BYTE) != 0 )
    {
        return;
    }
    row_data_size_ = width_ / PIXELS_PER_BYTE;

    // allocate pixels
    pixel_data_back_ = new uint8_t[((width_ * height_) / PIXELS_PER_BYTE) + 1];
    if ( pixel_data_back_ )
    {
        pixel_data_size_ = ((width_ * height_) / PIXELS_PER_BYTE) + 1;
        memset(pixel_data_back_, 0 , pixel_data_size_);
    }
    pixel_data_front_ = new uint8_t[((width_ * height_) / PIXELS_PER_BYTE) + 1];
    if ( pixel_data_front_ )
    {
        pixel_data_size_ = ((width_ * height_) / PIXELS_PER_BYTE) + 1;
        memset(pixel_data_front_, 0 , pixel_data_size_);
    }

    // allocate palette
    palette_ = new uint16_t[PALETTE_SIZE];
    if ( palette_ )
    {
        palette_count_ = PALETTE_SIZE;
        memset(palette_, 0 , palette_count_ * sizeof(uint16_t));
    }
}

void Adafruit_Cache::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    set_pixel_value(x, y, color, false);
}

void Adafruit_Cache::fillScreen(uint16_t color)
{
    if ( !pixel_data_back_ )
    {
        return;
    }
    uint8_t palette_color = set_palette_color(color);
    uint8_t palette_byte = 0;
    for ( size_t i = 0; i < PIXELS_PER_BYTE; i++ )
    {
        palette_byte |= (palette_color << (i * BITS_PER_PIXEL));
    }
    memset(pixel_data_back_, palette_byte, pixel_data_size_);
}

void Adafruit_Cache::setRotation(uint8_t r)
{
    Adafruit_GFX::setRotation(r);
    row_data_size_ = ((size_t)width()) / PIXELS_PER_BYTE;
    if ( gfx_ )
    {
        gfx_->setRotation(r);
    }
}

void Adafruit_Cache::display(void)
{
    if ( !gfx_ )
    {
        return;
    }
    if ( !flushed_onced_ )
    {
        this->fillScreen(0x0000);
        gfx_->fillScreen(0x0000);
        flushed_onced_ = true;
    }
    for ( size_t y = 0; y < (size_t)height(); y++ )
    {
        uint8_t *row_data_front = &pixel_data_front_[y * row_data_size_];
        uint8_t *row_data_back = &pixel_data_back_[y * row_data_size_];
        if ( 0 != memcmp(row_data_front, row_data_back, row_data_size_))
        {
            for ( size_t x = 0; x < (size_t)width(); x++ )
            {
                uint16_t front_color = get_pixel_value(x, y, true);
                uint16_t back_color = get_pixel_value(x, y, false);
                if ( front_color != back_color )
                {
                    // draw back color
                    gfx_->drawPixel(x, y, back_color);

                    // update front buffer pixel value
                    set_pixel_value(x, y, back_color, true);
                }
            }
        }
    }
}

size_t Adafruit_Cache::get_pixel_byte(int16_t x, int16_t y)
{
    return ((y * (size_t)width()) + x) / PIXELS_PER_BYTE;
}

size_t Adafruit_Cache::get_pixel_shift(int16_t x, int16_t y)
{
    return (((y * (size_t)width()) + x) % PIXELS_PER_BYTE) * BITS_PER_PIXEL;
}

size_t Adafruit_Cache::get_pixel_mask(int16_t x, int16_t y)
{
    return PALETTE_MASK;
}

uint8_t Adafruit_Cache::set_palette_color(uint16_t color)
{
    if ( !palette_ )
    {
        return 0;  // no palette, return first index
    }
    for (uint8_t i = 0; i < (uint8_t)palette_used_; i++ )
    {
        if ( color == palette_[i] )
        {
            return i;
        }
    }
    if ( palette_used_ < palette_count_ )
    {
        uint8_t i = palette_used_;
        palette_[i] = color;
        palette_used_++;
        return i;
    }
    return 0;  // no space, return first color
}

uint16_t Adafruit_Cache::get_palette_color(uint8_t index)
{
    if ( !palette_ )
    {
        return 0x0000;  // no palette, return black
    }
    if ( index < palette_used_ )
    {
        return palette_[index];
    }
    return 0x0000;  // invalid index, return black
}

uint16_t Adafruit_Cache::get_pixel_value(int16_t x, int16_t y, bool front_buffer)
{
    uint8_t *pixel_data = nullptr;
    if ( !pixel_data_front_ || !pixel_data_back_ )
    {
        return 0x0000;
    }
    if ( front_buffer )
    {
        pixel_data = pixel_data_front_;
    }
    else
    {
        pixel_data = pixel_data_back_;
    }
    uint8_t pixel_byte = pixel_data[get_pixel_byte(x, y)];
    uint8_t pixel_mask = get_pixel_mask(x, y);
    uint8_t pixel_shift = get_pixel_shift(x, y);
    uint8_t palette_color = (pixel_byte >> pixel_shift) & pixel_mask;
    return get_palette_color(palette_color);
}

void Adafruit_Cache::set_pixel_value(int16_t x, int16_t y, uint16_t color, bool front_buffer)
{
    uint8_t *pixel_data = nullptr;
    if ( !pixel_data_front_ || !pixel_data_back_ )
    {
        return;
    }
    if ( front_buffer )
    {
        pixel_data = pixel_data_front_;
    }
    else
    {
        pixel_data = pixel_data_back_;
    }
    uint8_t palette_color = set_palette_color(color);
    uint8_t pixel_byte = pixel_data[get_pixel_byte(x, y)];
    uint8_t pixel_mask = get_pixel_mask(x, y);
    uint8_t pixel_shift = get_pixel_shift(x, y);
    pixel_byte &=~ (pixel_mask << pixel_shift);
    pixel_byte |= (palette_color << pixel_shift) & (pixel_mask << pixel_shift);
    pixel_data[get_pixel_byte(x, y)] = pixel_byte;
}
