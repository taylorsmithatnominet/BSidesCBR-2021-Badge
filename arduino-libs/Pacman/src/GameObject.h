#pragma once
#include "GameObjectData.h"

#include "IAnimationAble.h"
#include "ICollideable.h"

namespace pacman {

class Vect2;

class GameObject : public IAnimationAble, public ICollideable
{
protected:
	GameObjectData* m_data;

	virtual void UpdateCollider();
public:
	GameObject(GameObjectData* p_data, Vect2 p_size);
	virtual ~GameObject();

	virtual bool Update(float p_delta) = 0;
	virtual void Draw();

	virtual void OnCollision(ICollideable* p_other);

	Vect2 GetScreenLocation() const;
	Vect2* GetPos();
};

}; // namespace pacman
