#pragma once

namespace pacman {

class Vect2;
class AnimationController;
class MovementModule;
class Level;

struct GameObjectData
{
	Vect2* m_pos;
	MovementModule* m_movement;
	Level* m_levelwPtr;

	GameObjectData();
	GameObjectData(Vect2* p_pos, MovementModule* p_movement, Level* p_level);
};

}; // namespace pacman
