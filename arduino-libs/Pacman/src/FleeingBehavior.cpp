#include "FleeingBehavior.h"

#include "GameObjectData.h"
#include "RoamAtRandom_State.h"
#include "MovementModule.h"

#include "Config.h"

namespace pacman {

FleeingBehavior::FleeingBehavior(GameObjectData* p_data)
	: m_datawPtr(p_data)
{
	m_roam = new RoamAtRandom_State(p_data);
}

FleeingBehavior::~FleeingBehavior()
{
	delete m_roam;
}

void FleeingBehavior::Enter()
{
	m_datawPtr->m_movement->TurnAround();
}

bool FleeingBehavior::Update(float p_delta)
{
	float subspeed = float(Config::MOVEMENT_SPEED_SLOW) / float(Config::MOVEMENT_SPEED);
	m_roam->Update(p_delta * subspeed);

	return true;
}

}; // namespace pacman
