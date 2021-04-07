#pragma once

#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"
#include "PathFinderModule.h"

namespace pacman {

class ScatterState : public IGhostState
{
private:
	PathFinderModule* m_pathfinder;
	Vect2* m_avoidwPtr;
	Vect2 m_target;

	Vect2 m_nextDir;
public:
	ScatterState(GameObjectData* p_data, Vect2 p_target, Vect2* p_avoid);
	~ScatterState();

	bool Update(float p_delta);

	void Enter();
};

}; // namespace pacman
