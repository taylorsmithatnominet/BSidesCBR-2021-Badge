#include "ICollideable.h"
#include "Collider.h"

namespace pacman {

ICollideable::ICollideable()
{
	m_collider = new Collider();
}

ICollideable::ICollideable(Collider* p_collider)
{
	m_collider = p_collider;
}

ICollideable::~ICollideable()
{
	delete m_collider;
	m_collider = nullptr;
}

void ICollideable::OnCollision(ICollideable * p_other)
{
}

}; // namespace pacman
