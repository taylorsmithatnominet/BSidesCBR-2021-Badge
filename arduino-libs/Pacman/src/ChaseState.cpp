#include "ChaseState.h"

#include "PathFinderModule.h"
#include "MovementModule.h"

#include "Config.h"

namespace pacman {

ChaseState::ChaseState(GameObjectData* p_data, Vect2* p_target)
	: IGhostState(p_data), m_targetwPtr(p_target)
{
	m_pathfinder = new PathFinderModule(m_datawPtr->m_levelwPtr);

	m_pathfinder->UpdatePath(*p_target);
}

ChaseState::~ChaseState()
{
	delete m_pathfinder;
	m_pathfinder = nullptr;
}

void ChaseState::Enter()
{
	m_pathfinder->UpdatePath(*m_targetwPtr);
}

bool ChaseState::Update(float p_delta)
{
	Vect2* pos = m_datawPtr->m_pos;

	Vect2 nextDir = Vect2::ZERO;
	if (m_datawPtr->m_movement->SteppedOnTile()) {
		Vect2 currentDir = m_datawPtr->m_movement->GetDirection();
		Vect2 back = Vect2(-currentDir.x, -currentDir.y);

		nextDir = m_pathfinder->GetNextDir(*pos, back);
	}

	m_datawPtr->m_movement->Update(p_delta, nextDir, (float)Config::MOVEMENT_SPEED, false);

	if (m_pathfinder->ReachedGoal(*pos)) {
		m_pathfinder->UpdatePath(*m_targetwPtr);

		return false;
	}

	return true;
}

}; // namespace pacman
