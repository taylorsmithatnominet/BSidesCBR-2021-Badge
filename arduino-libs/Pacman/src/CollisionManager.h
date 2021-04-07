#pragma once

namespace pacman {

class Collider;
class ICollideable;

class CollisionManager
{
public:
	static bool CheckCollision(Collider* p_first, Collider* p_second);
	static bool CheckCollision(ICollideable* p_first, ICollideable* p_second);
};

}; // namespace pacman
