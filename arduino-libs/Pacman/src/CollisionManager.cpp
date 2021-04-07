#include "CollisionManager.h"
#include "Collider.h"
#include "ICollideable.h"

namespace pacman {

bool CollisionManager::CheckCollision(Collider* p_first, Collider* p_second)
{
	int first_left = p_first->m_rect.x;
	int first_right = p_first->m_rect.x + p_first->m_rect.w;
	int first_up = p_first->m_rect.y;
	int first_down = p_first->m_rect.y + p_first->m_rect.h;

	int second_left = p_second->m_rect.x;
	int second_right = p_second->m_rect.x + p_second->m_rect.w;
	int second_up = p_second->m_rect.y;
	int second_down = p_second->m_rect.y + p_second->m_rect.h;

	if (first_right  <  second_left || 
	    second_right <  first_left  || 
		first_down   <  second_up   || 
		second_down  <  first_up) 
	{
		return false;
	}
	return true;
};

bool CollisionManager::CheckCollision(ICollideable* p_first, ICollideable* p_second)
{
	if (CollisionManager::CheckCollision(p_first->m_collider, p_second->m_collider))
	{
		p_first->OnCollision(p_second); 
		p_second->OnCollision(p_first); 
		return true;
	}
	return false;
};

}; // namespace pacman
