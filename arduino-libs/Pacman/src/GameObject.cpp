#include "GameObject.h"

#include "vect2.h"
#include "AnimationController.h"
#include "Level.h"

#include "Config.h"

namespace pacman {

Vect2 GameObject::GetScreenLocation() const
{
	int size = Config::TILE_SIZE;

	return Vect2(
		m_data->m_pos->x * size - (size / 2),
		(m_data->m_pos->y + 3) * size - (size / 2)
	);
}


void GameObject::UpdateCollider()
{
	Vect2 pos = GetScreenLocation();
	m_collider->m_rect.x = int(pos.x + Config::TILE_SIZE / 2);
	m_collider->m_rect.y = int(pos.y + Config::TILE_SIZE / 2);
}

GameObject::GameObject(GameObjectData * p_data, Vect2 p_size)
	: IAnimationAble(new AnimationController())
{
	m_data = p_data;
	m_collider->m_rect.w = (int)p_size.x;
	m_collider->m_rect.h = (int)p_size.y;
	UpdateCollider();
}

GameObject::~GameObject()
{
	delete m_data;
	m_data = nullptr;
}

bool GameObject::Update(float p_delta)
{
	m_animation->Update(p_delta);

	UpdateCollider();

	return false;
}

void GameObject::Draw()
{
	m_animation->Draw(GetScreenLocation());
}

void GameObject::OnCollision(ICollideable* p_other)
{
}

Vect2 * GameObject::GetPos()
{
	return m_data->m_pos;
}

}; // namespace pacman
