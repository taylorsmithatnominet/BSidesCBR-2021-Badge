#pragma once

#include <cstdint>

namespace pacman {

#define PM_COLOR16_FROM_RGB(r,g,b) \
    ((((uint16_t)r << (11 - 3)) & 0xf800) | (((uint16_t)g << (5 - 2)) & 0x07c0) | (((uint16_t)b >> (3)) & 0x001f))

}; // namespace pacman
