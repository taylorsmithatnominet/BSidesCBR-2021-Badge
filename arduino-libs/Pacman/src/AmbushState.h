#pragma once
#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"

namespace pacman {

class PathFinderModule;
class Player;

class AmbushState :
	public IGhostState

{
private:
	PathFinderModule* m_pathfinder;
	Player* m_playerwPtr;

	Vect2 m_currentTarget;

	Vect2 GetNextTarget();
	void UpdatePath();
public:
	AmbushState(GameObjectData* p_data, Player* p_player);
	~AmbushState();

	bool Update(float p_delta);

	void Enter();
};

}; // namespace pacman
