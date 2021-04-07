#pragma once

#include "Collider.h"

namespace pacman {

class ICollideable
{
public:
	ICollideable();
	ICollideable(Collider* p_collider);
	virtual ~ICollideable();
	virtual void OnCollision(ICollideable* p_other);
	Collider* m_collider = nullptr;
};

}; // namespace pacman
