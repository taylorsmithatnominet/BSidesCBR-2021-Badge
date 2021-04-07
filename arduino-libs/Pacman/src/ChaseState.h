#pragma once
#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"

namespace pacman {

class PathFinderModule;

class ChaseState :
	public IGhostState
{
private:
	PathFinderModule* m_pathfinder;
	Vect2* m_targetwPtr;
public:
	ChaseState(GameObjectData* p_data, Vect2* p_target);
	~ChaseState();

	void Enter();

	bool Update(float p_delta);
};

}; // namespace pacman
