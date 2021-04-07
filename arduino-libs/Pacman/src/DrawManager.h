#pragma once

#include <cstdint>
#include <functional>

#include "Sprite.h"

namespace pacman {

class DrawManager
{
public:
    DrawManager();
    ~DrawManager();
    void Initialize(); // Create window + renderer
    void Shutdown();   // delete window + renderer
    void Draw(Sprite* p_sprite, int p_x, int p_y); // -||- to rendercopy
    void Draw(Sprite* p_sprite, float p_x, float p_y); // -||- to rendercopy
    void Draw(Sprite* p_sprite, int p_x, int p_y, uint16_t p_color);
    struct SpriteRect GetDrawRect(Sprite * p_sprite, int p_x, int p_y);
    void SetDrawPixel(std::function<void(uint16_t,uint16_t,uint16_t)> func) {m_draw_pixel = func;}
    void SetDrawString8x8(std::function<void(int16_t,int16_t,const char *,uint16_t)> func) { m_draw_string = func;};
    void DrawPixel(uint16_t x, uint16_t y, uint16_t colour16);
    void DrawString(uint16_t x, uint16_t y, const char *text, uint16_t colour16);
private:
    void Draw(Sprite* p_sprite, struct SpriteRect draw_rect, uint16_t *p_color);
    std::function<void(uint16_t,uint16_t,uint16_t)> m_draw_pixel;
    std::function<void(int16_t,int16_t,const char *,uint16_t)> m_draw_string;
};

}; // namespace pacman
