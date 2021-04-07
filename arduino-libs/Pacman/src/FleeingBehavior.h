#pragma once
#include "vect2.h"

namespace pacman {

class RoamAtRandom_State;
struct GameObjectData;

class FleeingBehavior
{
private:
	RoamAtRandom_State* m_roam;
	GameObjectData* m_datawPtr;
public:
	FleeingBehavior(GameObjectData* p_data);
	~FleeingBehavior();

	void Enter();

	bool Update(float p_delta);
};

}; // namespace pacman
