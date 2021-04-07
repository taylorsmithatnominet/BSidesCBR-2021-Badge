#pragma once
#include "IGhostState.h"
#include <vector>

namespace pacman {

class AmbushState;
class ChaseState;
class RoamAtRandom_State;

class RandomBehaviorState :
	public IGhostState
{
private:
	std::vector<IGhostState*> m_behaviors;
	IGhostState* m_currentState;

	void SwitchState();
public:
	RandomBehaviorState(AmbushState* p_ambush, ChaseState* p_chase,
		RoamAtRandom_State* p_roamRandom);
	~RandomBehaviorState();

	bool Update(float p_delta);
	void Draw();
};

}; // namespace pacman
