#pragma once

#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"

namespace pacman {

class WaitingState :
	public IGhostState
{
private:
	bool m_goingUp;
	int m_wait;
public:
	WaitingState(GameObjectData* p_data);
	~WaitingState();

	void Initialize(int p_wait);

	bool Update(float p_delta);

	Vect2 GetDirection();
};

}; // namespace pacman
