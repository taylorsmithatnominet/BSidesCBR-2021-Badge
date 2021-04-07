#include "DeathAnimationState.h"

#include "AnimationController.h"
#include "ServiceLocator.h"
#include "PlayerAnimations.h"

namespace pacman {

DeathAnimationState::DeathAnimationState(GameStateData * p_data)
	: IGameState(p_data)
{
	m_controller = new AnimationController();
	PlayerAnimations* playerAnimations = ServiceLocator<PlayerAnimations>::GetService();
	m_controller->SetAnimation(playerAnimations->DeathAnimation);
}

DeathAnimationState::~DeathAnimationState()
{
	delete m_controller;
	m_controller = nullptr;
}

bool DeathAnimationState::Update(float p_delta)
{
	return !m_controller->Update(p_delta);
}

void DeathAnimationState::Draw()
{
	m_datawPtr->DrawLevel();
	m_datawPtr->DrawHUD();
	m_controller->Draw(m_datawPtr->m_player->GetScreenLocation());
}

void DeathAnimationState::Enter()
{
	m_controller->ResetAnimation();
}

void DeathAnimationState::Exit()
{
}

}; // namespace pacman
