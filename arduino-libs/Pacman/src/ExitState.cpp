#include "ExitState.h"

#include "Level.h"
#include "MovementModule.h"

#include "Config.h"

namespace pacman {

ExitState::ExitState(GameObjectData* p_data)
	: IGhostState(p_data)
{
	m_currentDirection = Vect2::ZERO;
}

ExitState::~ExitState()
{
}

bool ExitState::Update(float p_delta)
{
	float speed = Config::MOVEMENT_SPEED_EXIT_HOUSE;

	Vect2* pos = m_datawPtr->m_pos;

	if (pos->y == Config::HOUSE_EXIT_Y)
		return false; // switch state !!

	if (pos->x != Config::LEVEL_CENTER) {

		if (pos->y != Config::HOUSE_Y) {			// go to center of house Y
			//for animations
			if (pos->y > Config::HOUSE_Y) {
				m_currentDirection = Vect2::UP;
			}
			else {
				m_currentDirection = Vect2::DOWN;
			}

			pos->GotoY((float)Config::HOUSE_Y, speed * p_delta);
		}
		else {										// go to center X
			if (pos->x < Config::LEVEL_CENTER) {
				m_currentDirection = Vect2::RIGHT;
			}
			else {
				m_currentDirection = Vect2::LEFT;
			}

			pos->GotoX(Config::LEVEL_CENTER, speed * p_delta);
		}
	}
	else if (pos->y != Config::HOUSE_EXIT_Y) {
		pos->GotoY((float)Config::HOUSE_EXIT_Y, speed * p_delta);

		m_currentDirection = Vect2::UP;
	}
	else {
		//std::cout << "X: " << m_datawPtr->m_pos->x << "\tY:" << m_datawPtr->m_pos->y;
        //std::cout << std::flush;
	}

	return true;
}

Vect2 ExitState::GetDirection()
{
	return m_currentDirection;
}

}; // namespace pacman
