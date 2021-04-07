#include "BadgeAssets.h"

uint8_t read_pixel_wbr_4bit(size_t x, size_t y, size_t width, const void *buffer, size_t buffer_size)
{
    size_t pixel = (y * width) + x;
    size_t byte = pixel / 4;
    size_t bit = pixel % 4;
    uint8_t value = pgm_read_byte(&(((const uint8_t*)buffer)[byte]));
    value = (value >> (bit * 2)) & 0x3;
    return value;
}

uint16_t read_pixel_rgb565(size_t x, size_t y, size_t width, const void *buffer, size_t buffer_size)
{
    uint8_t value = read_pixel_wbr_4bit(x, y, width, buffer, buffer_size);
    switch (value)
    {
        case 0:
            return 0x0000; // BLACK
        case 1:
            return 0xF800; // RED
        case 3:
            return 0xffff; // WHITE

        default:
            break;
    }
    return 0xffff; // WHITE
}

void draw_asset(size_t x, size_t y, size_t width, size_t height, const void *buffer, size_t buffer_size, Adafruit_GFX &dev)
{
    for ( size_t dy = 0; dy < height; dy++ )
    {
        for ( size_t dx = 0; dx < width; dx++ )
        {
            dev.drawPixel(x + dx, y + dy, read_pixel_rgb565(dx, dy, width, buffer, buffer_size));
        }
    }
}
