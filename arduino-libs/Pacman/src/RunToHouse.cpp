#include "RunToHouse.h"

#include "MovementModule.h"
#include "PathFinderModule.h"
#include "Level.h"

#include "Config.h"

namespace pacman {

const Vect2 RunToHouseState::FRONT_OF_HOUSE = Vect2(14, 11);

RunToHouseState::RunToHouseState(GameObjectData* p_data)
	: IGhostState(p_data)
{
	m_startHouseX = m_datawPtr->m_pos->x;

	m_pathfinder = new PathFinderModule(m_datawPtr->m_levelwPtr);
	m_pathfinder->UpdatePath(FRONT_OF_HOUSE);
}

RunToHouseState::~RunToHouseState()
{
	delete m_pathfinder;
	m_pathfinder = nullptr;
}

bool RunToHouseState::Update(float p_delta)
{
	float speed = Config::MOVEMENT_SPEED_FAST;
	float center = Config::LEVEL_CENTER;

	Vect2* pos = m_datawPtr->m_pos;

	if (m_pathfinder->ReachedGoal(*pos) ||
		FRONT_OF_HOUSE.Floor() == pos->Floor())
	{
		m_done = true;
	}

	if (m_done) {
		if (pos->y == Config::HOUSE_Y) {
			if (pos->x != m_startHouseX) {
				pos->GotoX(m_startHouseX, speed * p_delta);

				if (m_startHouseX > center) {
					m_dir = Vect2::RIGHT;
				}
				else {
					m_dir = Vect2::LEFT;
				}
			}
			else {
				return false;
			}
		}
		else if (pos->x == center) {
			pos->GotoY((float)Config::HOUSE_Y, speed * p_delta);

			m_dir == Vect2::DOWN;
		}
		else {
			pos->GotoX(center, speed * p_delta);

			if (pos->x > center) {
				m_dir = Vect2::LEFT;
			}
			else if (pos->x < center) {
				m_dir = Vect2::RIGHT;
			}
			else {
				m_dir = Vect2::DOWN;
			}
		}
	}
	else
	{
		m_datawPtr->m_movement->Update(p_delta, m_pathfinder->GetNextDir(*pos), speed, false);
		m_dir = m_datawPtr->m_movement->GetDirection();
	}

	return true;
}

void RunToHouseState::Enter()
{
	m_done = false;
}

Vect2 RunToHouseState::GetDirection()
{
	return m_dir;
}

}; // namespace pacman
