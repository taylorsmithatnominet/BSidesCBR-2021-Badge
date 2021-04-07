#include "ScatterState.h"

#include "GameObjectData.h"
#include "MovementModule.h"
 
#include "PathFinderModule.h"

#include "Config.h"

namespace pacman {

ScatterState::ScatterState(GameObjectData * p_data, Vect2 p_target, Vect2* p_avoid)
	: IGhostState(p_data)
{
	m_target = p_target;
	m_avoidwPtr = p_avoid;
	m_pathfinder = new PathFinderModule(m_datawPtr->m_levelwPtr);
	m_nextDir = Vect2::ZERO;
}

ScatterState::~ScatterState()
{
	delete m_pathfinder;
}

bool ScatterState::Update(float p_delta)
{
	Vect2* pos = m_datawPtr->m_pos;

	if (m_datawPtr->m_movement->SteppedOnTile())
		m_nextDir = m_pathfinder->GetNextDir(*pos);

	m_datawPtr->m_movement->Update(p_delta, m_nextDir, (float)Config::MOVEMENT_SPEED, false);

	if (m_pathfinder->ReachedGoal(*pos)) {
		return false;
	}

	return true;
}

void ScatterState::Enter()
{
	m_pathfinder->UpdatePath(m_target, *m_avoidwPtr);
	m_nextDir = m_pathfinder->GetNextDir(*m_datawPtr->m_pos);
}

}; // namespace pacman
