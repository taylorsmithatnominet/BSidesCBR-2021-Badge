#include "PowerUp.h"

#include "ServiceLocator.h"
#include "DrawManager.h"

#include "Config.h"

namespace pacman {

PowerUp::PowerUp(Vect2 p_pos)
	: ICollideable(), IAnimationAble()
{
    m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();

	m_pos = p_pos;

	m_collider->m_rect.x = int(p_pos.x);
	m_collider->m_rect.y = int(p_pos.y);
	m_collider->m_rect.w = Config::TILE_SIZE;
	m_collider->m_rect.h = Config::TILE_SIZE;
}

PowerUp::~PowerUp()
{
}

void PowerUp::Update(float p_delta)
{
}

void PowerUp::Draw()
{
    uint8_t line_start[8]   = {2, 1, 0, 0, 0, 0, 1, 2};
    uint8_t line_end[8]     = {6, 7, 8, 8, 8, 8, 7, 6};
    for ( uint8_t y = 0; y < 8; y++ )
    {
        for ( uint8_t x = line_start[y]; x < line_end[y]; x++ )
        {
            m_drawManagerwPtr->DrawPixel(m_collider->m_rect.x + x, m_collider->m_rect.y + y, 0xfdd5);
        }
    }
}

void PowerUp::ResetAnimation()
{
}

}; // namespace pacman
