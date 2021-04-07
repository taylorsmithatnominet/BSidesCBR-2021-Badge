#pragma once
#include "Animation.h"
#include "vect2.h"
#include "DirectionCollection.h"

namespace pacman {

class GhostAnimations
{
private:
	DirectionCollection m_colorAnimations[4];
	DirectionCollection m_deathCollection;
public:
	GhostAnimations();
	~GhostAnimations();

	Animation* GetAnimation(Vect2 p_dir, int p_index);
	Animation* GetDeathAnimation(Vect2 p_dir);

	Animation* BlueAnimation;
	Animation* WarningAnimation;
};

}; // namespace pacman
