#pragma once
#include "vect2.h"

namespace pacman {

class Animation;

struct DirectionCollection
{
	Animation* Up;
	Animation* Down;
	Animation* Left;
	Animation* Right;

	Animation* GetDirFromInt(int p_index);
	Animation* GetDirFromVect(Vect2 p_dir);

	DirectionCollection();
	~DirectionCollection();

	void DeletePointers();
};

}; // namespace pacman
