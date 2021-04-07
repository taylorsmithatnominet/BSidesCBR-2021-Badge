#pragma once
#include "IGhostState.h"
#include "vect2.h"
#include "GameObject.h"

namespace pacman {

class PathFinderModule;
class Level;

class RunToHouseState :
	public IGhostState
{
private:
	static const Vect2 FRONT_OF_HOUSE;

	PathFinderModule* m_pathfinder;

	bool m_done;
	float m_startHouseX;
	Vect2 m_dir;
public:
	RunToHouseState(GameObjectData* p_data);
	~RunToHouseState();

	bool Update(float p_delta);

	void Enter();

	Vect2 GetDirection();
};

}; // namespace pacman
