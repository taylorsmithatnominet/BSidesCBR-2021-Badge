#pragma once
#include "vect2.h"
#include "Animation.h"

namespace pacman {

class DrawManager;

class AnimationController
{
private:
	Animation* m_animation;
	DrawManager* m_drawManagerwPtr;

	int m_length;
	bool m_hasLooped;
	float m_animationPos;
public:
	AnimationController();
	~AnimationController();

	bool Update(float p_delta); // returns true if animation has looped once
	void Draw(Vect2 p_pos);
	void SetAnimation(Animation* p_animation);
	void ResetAnimation();
};

}; // namespace pacman
