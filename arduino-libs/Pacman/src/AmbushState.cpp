#include "AmbushState.h"

#include "Level.h"
#include "MovementModule.h"
#include "Player.h"
#include "PathFinderModule.h"

#include "Config.h"

namespace pacman {

Vect2 AmbushState::GetNextTarget()
{
	Vect2 next = *m_playerwPtr->GetPos();
	m_datawPtr->m_levelwPtr->NextIntersection(next, m_playerwPtr->GetDirection());

	// if ghost is on target it will stop, so it sets the target to the player
	if (next == m_datawPtr->m_pos->Round()) {
		return m_playerwPtr->GetPos()->Round();
	}

	return next;
}

void AmbushState::UpdatePath()
{
	m_currentTarget = GetNextTarget();
	m_pathfinder->UpdatePath(m_currentTarget, *m_playerwPtr->GetPos());
}


AmbushState::AmbushState(GameObjectData* p_data, Player* p_player)
	: IGhostState(p_data), m_playerwPtr(p_player)
{
	m_pathfinder = new PathFinderModule(m_datawPtr->m_levelwPtr);

	UpdatePath();
}

AmbushState::~AmbushState()
{
	delete m_pathfinder;
	m_pathfinder = nullptr;
}

bool AmbushState::Update(float p_delta)
{
	Vect2* pos = m_datawPtr->m_pos;
	
	Vect2 nextDir = Vect2::ZERO;
	if (m_datawPtr->m_movement->SteppedOnTile()) {
		Vect2 currentDir = m_datawPtr->m_movement->GetDirection();
		Vect2 back = Vect2(-currentDir.x, -currentDir.y);

		nextDir = m_pathfinder->GetNextDir(*pos, back);
	}

	m_datawPtr->m_movement->Update(p_delta, nextDir, float(Config::MOVEMENT_SPEED), false);

	if (m_pathfinder->ReachedGoal(*m_datawPtr->m_pos)) {
		UpdatePath();
		return false;
	}

	return true;
}

void AmbushState::Enter()
{
	UpdatePath();
}

}; // namespace pacman
