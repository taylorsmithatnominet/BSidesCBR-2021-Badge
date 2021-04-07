#include "SpriteManager.h"
#include "Sprite.h"
#include "DrawManager.h"

namespace pacman {

SpriteManager::SpriteManager()
{
    ID_MAP_CLEAR(m_textures);
}

SpriteManager::~SpriteManager()
{
    ID_MAP_FOR_EACH(m_textures, SpriteTexture*, tex) {
        delete tex;
    }
    ID_MAP_CLEAR(m_textures);

    for (auto s : m_sprites) {
        delete s;
        s = nullptr;
    }
}

Sprite * SpriteManager::CreateSprite(const SpriteTexture &tex, unsigned int p_x, unsigned int p_y,
                                     unsigned int p_w, unsigned int p_h)
{
    auto it = ID_MAP_FIND(m_textures, tex.id); // Returns an iterator to a pos or end, depending on success
    if (it == ID_MAP_END(m_textures))
    {
        SpriteTexture *tex_copy = new SpriteTexture;
        memcpy(tex_copy, &tex, sizeof(SpriteTexture));
        ID_MAP_INSERT(m_textures, tex.id, tex_copy);
        it = ID_MAP_FIND(m_textures, tex.id);
    }

    Sprite* xSprite = new Sprite(it, p_x, p_y, p_w, p_h);
    // if the sprite was already stored in the vector, return it
    // this way we can reuse the same data
    for (auto s : m_sprites) {
        if (*s == *xSprite) {
            delete xSprite;
            return s;
        }
    }

    m_sprites.push_back(xSprite);
    return xSprite;
}

void SpriteManager::DestroySprite(Sprite * p_sprite)
{
    // FUTURE: iterate over the vector to find the specific sprite, if found, delete
}

}; // namespace pacman
