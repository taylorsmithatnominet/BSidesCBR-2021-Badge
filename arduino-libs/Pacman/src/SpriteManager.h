#pragma once
#include <vector>
#include <IDMap.h>

namespace pacman {

typedef struct _SpriteTexture {
    size_t id;
    size_t width;
    size_t height;
    size_t size;
    size_t palette_size;
    size_t palette_bits;
    const char *data;
    const char *palette;
    const char *alpha;
} SpriteTexture;

#define CreateSpriteTextureFromHeader(name) \
    { \
        name##_id, \
        name##_width, \
        name##_height, \
        name##_size - (name##_palette_size * (2 + 1)), \
        name##_palette_size, \
        name##_palette_bits, \
        name##_data + (name##_palette_size * (2 + 1)), \
        name##_data, \
        name##_data + (name##_palette_size * (2)) \
    };

class DrawManager;
class Sprite;

class SpriteManager
{
public:
    SpriteManager();
    ~SpriteManager();
    Sprite* CreateSprite(const SpriteTexture &tex, unsigned int p_x, unsigned int p_y, unsigned int p_w, unsigned int p_h);
    void DestroySprite(Sprite* p_sprite);

private:
    std::vector<Sprite*> m_sprites;
    ID_MAP(m_textures, SpriteTexture*, 20);
};

}; // namespace pacman
