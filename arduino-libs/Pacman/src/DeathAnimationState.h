#pragma once
#include "IGameState.h"

namespace pacman {

class AnimationController;
class PlayerAnimations;

class DeathAnimationState : public IGameState
{
private:
	AnimationController* m_controller;
public:
	DeathAnimationState(GameStateData* p_data);
	~DeathAnimationState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
