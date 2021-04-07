#include "Player.h"


#include "Sprite.h"
#include "vect2.h"
#include "DrawManager.h"
#include "MovementModule.h"
#include "ServiceLocator.h"
#include "InputManager.h"
#include "SpriteManager.h"
#include "AnimationController.h"
#include "Animation.h"
#include "PlayerAnimations.h"

#include "Level.h"

#include "CollisionManager.h"
#include "PowerUp.h"

#include "Keyboard.h"

#include "Config.h"

namespace pacman {

Vect2 Player::GetAverageInput(const std::vector<Vect2>& p_inputs)
{
	Vect2 ret = Vect2::ZERO;

	for (auto i : p_inputs) {
		ret.x += i.x;
		ret.y += i.y;
	}

	return ret;
}

void Player::UpdateInput()
{
	std::vector<Vect2> heldinputs;
	std::vector<Vect2> pressedinputs;

	if (m_inputwPtr->IsKeyDown(PM_SCANCODE_UP))
		heldinputs.push_back(Vect2::UP);
	if (m_inputwPtr->IsKeyDown(PM_SCANCODE_DOWN))
		heldinputs.push_back(Vect2::DOWN);
	if (m_inputwPtr->IsKeyDown(PM_SCANCODE_LEFT))
		heldinputs.push_back(Vect2::LEFT);
	if (m_inputwPtr->IsKeyDown(PM_SCANCODE_RIGHT))
		heldinputs.push_back(Vect2::RIGHT);

	if (m_inputwPtr->IsKeyDownOnce(PM_SCANCODE_UP))
		pressedinputs.push_back(Vect2::UP);
	if (m_inputwPtr->IsKeyDownOnce(PM_SCANCODE_DOWN))
		pressedinputs.push_back(Vect2::DOWN);
	if (m_inputwPtr->IsKeyDownOnce(PM_SCANCODE_LEFT))
		pressedinputs.push_back(Vect2::LEFT);
	if (m_inputwPtr->IsKeyDownOnce(PM_SCANCODE_RIGHT))
		pressedinputs.push_back(Vect2::RIGHT);

	Vect2 held = GetAverageInput(heldinputs);
	Vect2 pressed = GetAverageInput(pressedinputs);
	Vect2 newDir = Vect2::ZERO;

	if (held.x != 0 && held.y != 0) {
		if (pressed.x != 0 && pressed.y != 0) {
			newDir = pressed;
			newDir.x = 0; // priority to y axis
		}
		else {
			newDir = pressed;
		}
	}
	else {
		newDir = held;
	}

	if (newDir != Vect2::ZERO) {
		m_currentInput = newDir;
	}
}


Player::Player(GameObjectData * p_data)
	: GameObject(p_data, Vect2(float(Config::TILE_SIZE), float(Config::TILE_SIZE))),
	m_currentInput(Vect2::ZERO), m_isDead(false)
{
	m_data = p_data;

 	m_inputwPtr = ServiceLocator<InputManager>::GetService();

	m_animationswPtr = ServiceLocator<PlayerAnimations>::GetService();
	m_animation->SetAnimation(m_animationswPtr->Still);
}

Player::~Player()
{
    // TODO: - this may break things
	//GameObject::~GameObject();
}

bool Player::Update(float p_delta)
{
	GameObject::Update(p_delta);

	UpdateInput();

	bool bumped = m_data->m_movement->Update(p_delta, m_currentInput, float(Config::MOVEMENT_SPEED), true);

	Vect2 currentdir = m_data->m_movement->GetDirection();
	Animation* next_animation = nullptr;

	if (currentdir == Vect2::ZERO) {
		next_animation = m_animationswPtr->Still;
	}
	else if (bumped) {
		next_animation = m_animationswPtr->GetStoppedAnimation(currentdir);
	}
	else {
		next_animation = m_animationswPtr->GetAnimation(currentdir);
	}
	m_animation->SetAnimation(next_animation);

	return bumped;
}

bool Player::SteppedOnTile()
{
	return m_data->m_movement->SteppedOnTile();
}

void Player::Kill()
{
	m_isDead = true;
}

bool Player::IsDead()
{
	return m_isDead;
}

Vect2 Player::GetDirection()
{
	return m_data->m_movement->GetDirection();
}

}; // namespace pacman
