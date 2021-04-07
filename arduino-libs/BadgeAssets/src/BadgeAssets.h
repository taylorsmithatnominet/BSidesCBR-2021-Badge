#ifndef _BADGE_ASSETS_
#define _BADGE_ASSETS_

#include <Arduino.h>
#include <Adafruit_GFX.h>

#include <stdint.h>
#include <stddef.h>

uint8_t read_pixel_wbr_4bit(size_t x, size_t y, size_t width, const void *buffer, size_t buffer_size);
uint16_t read_pixel_rgb565(size_t x, size_t y, size_t width, const void *buffer, size_t buffer_size);
void draw_asset(size_t x, size_t y, size_t width, size_t height, const void *buffer, size_t buffer_size, Adafruit_GFX &dev);

#define ASSET_WIDTH(name)   ((size_t)asset_##name##_width)
#define ASSET_HEIGHT(name)  ((size_t)asset_##name##_height)
#define ASSET_DATA(name)   ((const void *)asset_##name##_data)
#define ASSET_SIZE(name)   ((size_t)asset_##name##_size)

#define DRAW_ASSET(x,y,name,dev) \
    draw_asset(x,y,ASSET_WIDTH(name),ASSET_HEIGHT(name),ASSET_DATA(name),ASSET_SIZE(name),dev)

#define DRAW_ASSET_OFFSET_TOP_CENTER(y,name,dev) \
    DRAW_ASSET((dev.width()/2)-(ASSET_WIDTH(name)/2),y,name,dev)

#define DRAW_ASSET_TOP_CENTER(name,dev) \
    DRAW_ASSET((dev.width()/2)-(ASSET_WIDTH(name)/2),0,name,dev)

#define DRAW_ASSET_CENTER(name,dev) \
    DRAW_ASSET((dev.width()/2)-(ASSET_WIDTH(name)/2),(dev.height()/2)-(ASSET_HEIGHT(name)/2),name,dev)

#define DRAW_ASSET_BOTTOM_CENTER(name,dev) \
    DRAW_ASSET((dev.width()/2)-(ASSET_WIDTH(name)/2),dev.height()-ASSET_HEIGHT(name),name,dev)

#endif // _BADGE_ASSETS_
