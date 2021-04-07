#include "GameObjectData.h"

namespace pacman {

GameObjectData::GameObjectData()
{
}

GameObjectData::GameObjectData(Vect2 * p_pos, MovementModule* p_movement, Level* p_level)
{
	m_pos = p_pos;
	m_movement = p_movement;
	m_levelwPtr = p_level;
}

}; // namespace pacman
