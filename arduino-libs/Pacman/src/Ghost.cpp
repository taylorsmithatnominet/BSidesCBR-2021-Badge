#include "Ghost.h"

#include "IState.h"
#include "vect2.h"

#include "ExitState.h"
#include "RunToHouse.h"
#include "WaitingState.h"

#include "ServiceLocator.h"
#include "GhostAnimations.h"
#include "AnimationController.h"
#include "FleeingBehavior.h"
#include "ScatterState.h"
#include "MovementModule.h"
#include "PauseModule.h"

#include "Config.h"

namespace pacman {

void Ghost::SwitchState(IState * p_state)
{
	if (m_currentStatewPtr != nullptr)
		m_currentStatewPtr->Exit();
	m_currentStatewPtr = p_state;
	m_currentStatewPtr->Enter();
}

void Ghost::SetAnimation()
{
	// get dir
	Vect2 dir = m_data->m_movement->GetDirection();
	
	if (m_currentStatewPtr == m_waiting) {
		dir = m_waiting->GetDirection();
	}
	if (m_currentStatewPtr == m_exitHouse) {
		dir = m_exitHouse->GetDirection();
	}
	if (m_currentStatewPtr == m_runToHouse) {
		dir = m_runToHouse->GetDirection();
	}

	if (dir == Vect2::ZERO) {
		dir = Vect2::UP;
	}

	// get animation
	Animation* animation = nullptr;

	if (IsFleeing()) {
		if (m_fleeingTime->GetTime() < Config::WARNING_TIME) {
			animation = m_animationswPtr->WarningAnimation;
		}
		else {
			animation = m_animationswPtr->BlueAnimation;
		}
	}
	else if (m_currentStatewPtr == m_runToHouse) {
		animation = m_animationswPtr->GetDeathAnimation(dir);
	}
	else if (
		m_currentStatewPtr == m_roaming ||
		m_currentStatewPtr == m_exitHouse ||
		m_currentStatewPtr == m_waiting ||
		m_currentStatewPtr == m_scatterState
		)
	{
		animation = m_animationswPtr->GetAnimation(dir, m_color);
	}

	if (animation != nullptr) {
		m_animation->SetAnimation(animation);
	}
}

void Ghost::EndFlee()
{
	if (m_currentStatewPtr == m_roaming) {
		SwitchState(m_scatterState);
	}
}

void Ghost::UpdateCollider()
{
	GameObject::UpdateCollider();

	if (IsFleeing()) {
		m_collider->m_rect.w = Config::TILE_SIZE;
		m_collider->m_rect.h = Config::TILE_SIZE;
	}
	else {
		m_collider->m_rect.x += Config::TILE_SIZE / 2;
		m_collider->m_rect.y += Config::TILE_SIZE / 2;
		m_collider->m_rect.w = 0;
		m_collider->m_rect.h = 0;
	}
}

Ghost::Ghost(GameObjectData * p_data, IState * p_roaming, Vect2 * p_playerPos, int p_color, Vect2 p_startDir)
	: GameObject(p_data, Vect2::ZERO)
{
    m_currentStatewPtr = nullptr;

	m_color = p_color;
	m_justDied = false;
	m_fleeingTime = new PauseModule();

	m_roaming = p_roaming;
	m_waiting = new WaitingState(p_data);
	m_runToHouse = new RunToHouseState(p_data);
	m_exitHouse = new ExitState(p_data);

	m_fleeingBehavior = new FleeingBehavior(p_data);

	Vect2 scatter_pos = Vect2::ZERO;
	switch (m_color)
	{
	case 0:
		scatter_pos = Vect2(1,1);
		break;
	case 1:
		scatter_pos = Vect2(Config::LEVEL_WIDTH - 2, 1);
		break;
	case 2:
		scatter_pos = Vect2(Config::LEVEL_WIDTH - 2, Config::LEVEL_HEIGHT - 2);
		break;
	case 3:
		scatter_pos = Vect2(1, Config::LEVEL_HEIGHT - 2);
		break;
	default:
		break;
	}

	m_scatterState = new ScatterState(p_data, scatter_pos, p_playerPos);

	SwitchState(m_scatterState);
	//m_roaming->OnSpawn();

	m_animationswPtr = ServiceLocator<GhostAnimations>::GetService();
	m_animation->SetAnimation(m_animationswPtr->GetAnimation(p_startDir, m_color));
}

Ghost::~Ghost()
{
    // FUTURE: this may break things
	//GameObject::~GameObject();

	delete m_waiting;
	delete m_exitHouse;
	delete m_roaming;
	delete m_fleeingBehavior;
	delete m_runToHouse;
	delete m_scatterState;
}

bool Ghost::Update(float p_delta)
{
	GameObject::Update(p_delta);

	if (m_justDied)
		m_justDied = false;

	if (m_fleeingTime->IsPaused()) {
		if (!m_fleeingTime->Update(p_delta)) {
			EndFlee();
		}
	}

	bool ret = true;
	if (IsFleeing()) {
		if (m_currentStatewPtr == m_waiting
			|| m_currentStatewPtr == m_exitHouse)
		{
			ret = m_currentStatewPtr->Update(p_delta);
		}
		else
			m_fleeingBehavior->Update(p_delta);
	}
	else
		ret = m_currentStatewPtr->Update(p_delta);

	SetAnimation();

	if (!ret) {
		if (m_currentStatewPtr == m_runToHouse ||
			m_currentStatewPtr == m_waiting)
		{
			SwitchState(m_exitHouse);
		}
		else if (m_currentStatewPtr == m_exitHouse) {
			SwitchState(m_scatterState);
		}
		else if (m_currentStatewPtr == m_scatterState) {
			SwitchState(m_roaming);
		}
	}

	return false;
}

void Ghost::StartRoaming()
{
}

void Ghost::StartWaiting(int p_wait)
{
	SwitchState(m_waiting);
	m_waiting->Initialize(p_wait);
}

void Ghost::Flee(float p_time)
{
	m_fleeingTime->SetPause(p_time + Config::WARNING_TIME);
	m_fleeingBehavior->Enter();
}

void Ghost::FleeScatter()
{
	if (m_currentStatewPtr == m_roaming) {
		SwitchState(m_scatterState);
	}
}

void Ghost::RunToHouse(Animation* p_animation)
{
	m_animation->SetAnimation(p_animation);
	m_fleeingTime->SetPause(0);
	m_justDied = true;
	SwitchState(m_runToHouse);
}

bool Ghost::IsFleeing()
{
	return m_fleeingTime->IsPaused() && m_currentStatewPtr != m_runToHouse;
}

bool Ghost::IsDead()
{
	return (m_currentStatewPtr == m_runToHouse);
}

bool Ghost::HasStartedRunning()
{
	return !m_justDied && IsDead();
}

}; // namespace pacman
