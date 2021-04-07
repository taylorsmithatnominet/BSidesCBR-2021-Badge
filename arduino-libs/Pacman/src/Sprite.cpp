#include "Sprite.h"

namespace pacman {

Sprite::Sprite() // Private
{
};

Sprite::Sprite(SpriteTexture* p_texture, int p_x, int p_y, int p_w, int p_h)
{
    m_texture = p_texture;
    SetSource(p_x, p_y, p_w, p_h);
};

Sprite::~Sprite()
{
};

void Sprite::SetSource(int p_x, int p_y, int p_w, int p_h)
{
    m_source.x = p_x;
    m_source.y = p_y;
    m_source.w = p_w;
    m_source.h = p_h;
}
struct SpriteRect Sprite::GetSource()
{
    return m_source;
}
SpriteTexture * Sprite::GetTexture()
{
    return m_texture;
}
bool Sprite::operator==(const Sprite & p_other)
{
    bool same_source = 
        m_source.h == p_other.m_source.h &&
        m_source.w == p_other.m_source.w &&
        m_source.x == p_other.m_source.x &&
        m_source.y == p_other.m_source.y;

    bool same_texture = m_texture == p_other.m_texture;

    return same_source && same_texture;
}
bool Sprite::operator!=(const Sprite & p_other)
{
    return !(*this == p_other);
}

}; // namespace pacman
