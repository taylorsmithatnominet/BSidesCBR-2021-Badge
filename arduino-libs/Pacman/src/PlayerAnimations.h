#pragma once
#include "Animation.h"
#include "DirectionCollection.h"
#include "vect2.h"

namespace pacman {

class PlayerAnimations
{
private:
	DirectionCollection m_eating;
	DirectionCollection m_stopped;
public:
	PlayerAnimations();
	~PlayerAnimations();

	Animation* GetAnimation(Vect2 p_dir);
	Animation* GetStoppedAnimation(Vect2 p_dir);
	Animation* DeathAnimation;

	Animation* Still;
};

}; // namespace pacman
