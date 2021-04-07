#pragma once

namespace pacman {

struct ColliderRect {
    int x;
    int y;
    int w;
    int h;
};

class Collider
{
public:
    Collider();
    ~Collider();
    struct ColliderRect m_rect;
};

}; // namespace pacman
