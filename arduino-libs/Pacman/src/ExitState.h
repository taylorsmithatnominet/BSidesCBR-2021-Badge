#pragma once
#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"

namespace pacman {

class ExitState :
	public IGhostState
{
private:
	Vect2 m_currentDirection;
public:
	ExitState(GameObjectData* p_data);
	~ExitState();

	bool Update(float p_delta);

	Vect2 GetDirection();
};

}; // namespace pacman
