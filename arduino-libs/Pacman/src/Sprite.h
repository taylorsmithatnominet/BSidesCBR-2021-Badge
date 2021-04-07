#pragma once

#include "SpriteManager.h"

namespace pacman {

struct SpriteRect {
    int x;
    int y;
    int w;
    int h;
};

class Sprite // deletion is handled in SpriteManager
{
public:
    Sprite(SpriteTexture* p_texture, int p_x, int p_y, int p_w, int p_h);
    ~Sprite();
    void SetSource(int p_x, int p_y, int p_w, int p_h);
    struct SpriteRect GetSource();
    SpriteTexture* GetTexture();

    bool operator==(const Sprite& p_other);
    bool operator!=(const Sprite& p_other);
private:
    Sprite();
    SpriteTexture* m_texture;
    struct SpriteRect m_source; // Defines what part of the texture that should be drawn
};

}; // namespace pacman
