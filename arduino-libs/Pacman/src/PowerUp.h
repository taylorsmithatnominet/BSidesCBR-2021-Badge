#pragma once
#include "ICollideable.h"
#include "IAnimationAble.h"
#include "vect2.h"

namespace pacman {

class DrawManager;

class PowerUp : public ICollideable, public IAnimationAble
{
private:
	DrawManager* m_drawManagerwPtr;
	Vect2 m_pos;
public:
	PowerUp(Vect2 p_pos);
	~PowerUp();

	void Update(float p_delta);
	void Draw();
	void ResetAnimation();
};

}; // namespace pacman
