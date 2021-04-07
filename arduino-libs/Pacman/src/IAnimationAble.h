#pragma once
#include "AnimationController.h"

namespace pacman {

class Sprite;
class DrawManager;
class SpriteManager;
class Vect2;

class IAnimationAble
{
public:
	IAnimationAble();
	IAnimationAble(AnimationController* p_animation);
	virtual ~IAnimationAble();
	virtual void Draw() = 0;
protected:
	DrawManager* m_drawManagerwPtr = nullptr;
	AnimationController* m_animation = nullptr;
};

}; // namespace pacman
