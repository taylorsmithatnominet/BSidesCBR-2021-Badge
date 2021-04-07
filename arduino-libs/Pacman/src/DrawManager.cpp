#include "DrawManager.h"
#include "Sprite.h"
#include "Color.h"

namespace pacman {

// non - arduino
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*((uint8_t*)addr))
#endif

DrawManager::DrawManager()
{
}

DrawManager::~DrawManager()
{
}

void DrawManager::Initialize()
{
    // initialise m_render here - if needed
}

void DrawManager::Shutdown()
{
    // shutdown renderer here - if needed
}

void DrawManager::DrawPixel(uint16_t x, uint16_t y, uint16_t colour16)
{
    m_draw_pixel(x, y, colour16);
}

void DrawManager::DrawString(uint16_t x, uint16_t y, const char *text, uint16_t colour16)
{
    m_draw_string(x, y, text, colour16);
}

void DrawManager::Draw(Sprite* p_sprite, int p_x, int p_y)
{
    Draw(p_sprite, GetDrawRect(p_sprite, p_x, p_y), nullptr);
}

void DrawManager::Draw(Sprite * p_sprite, float p_x, float p_y)
{
    Draw(p_sprite, GetDrawRect(p_sprite, (int)p_x, (int)p_y), nullptr);
}

void DrawManager::Draw(Sprite * p_sprite, int p_x, int p_y, uint16_t p_color)
{
    Draw(p_sprite, GetDrawRect(p_sprite, p_x, p_y), &p_color);
}

void DrawManager::Draw(Sprite* p_sprite, struct SpriteRect draw_rect, uint16_t *p_color)
{
    SpriteTexture &tex = *(p_sprite->GetTexture());
    SpriteRect src_rect = p_sprite->GetSource();

    size_t values_per_byte = 8 / tex.palette_bits;
    size_t mask = (1 << tex.palette_bits) - 1;

    for ( size_t dx = 0; dx < src_rect.w; dx++ )
    {
        for ( size_t dy = 0; dy < src_rect.h; dy++ )
        {
            uint16_t colour16 = 0x0000;
            uint8_t alpha8 = 0;
            size_t tex_x = src_rect.x + dx;
            size_t tex_y = src_rect.y + dy;
            size_t tex_i = (tex_y * tex.width) + tex_x;
            size_t tex_byte = tex_i / values_per_byte;
            size_t tex_shift = (values_per_byte - 1 - (tex_i % values_per_byte)) * tex.palette_bits;
            if ( tex_byte < tex.size )
            {
                size_t palette_index = 0;
                palette_index = (size_t) pgm_read_byte(&tex.data[tex_byte]);
                palette_index = (palette_index >> tex_shift) & mask;
                if ( palette_index < tex.palette_size )
                {
                    colour16 = 0;
                    colour16 |= ((size_t)pgm_read_byte(&tex.palette[(palette_index * 2) + 0]) << 8) & 0xff00;
                    colour16 |= ((size_t)pgm_read_byte(&tex.palette[(palette_index * 2) + 1]) << 0) & 0x00ff;
                    alpha8 = (uint8_t)pgm_read_byte(&tex.alpha[palette_index]);
                }
            }
            size_t x = draw_rect.x + dx;
            size_t y = draw_rect.y + dy;
            if ( alpha8 >= 0x7f )
            {
                if ( p_color )
                {
                    colour16 = *p_color;
                }
                m_draw_pixel(x, y, colour16);
            }
        }
    }
}

struct SpriteRect DrawManager::GetDrawRect(Sprite * p_sprite, int p_x, int p_y)
{
    struct SpriteRect dstRect = {
        p_x,
        p_y,
        p_sprite->GetSource().w,
        p_sprite->GetSource().h
    };
    return dstRect;
}

}; // namespace pacman
