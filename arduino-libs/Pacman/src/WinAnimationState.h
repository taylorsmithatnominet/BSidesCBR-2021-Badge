#pragma once

#include "IGameState.h"
#include "PauseModule.h"

namespace pacman {

class AnimationController;
class PlayerAnimations;
class Animation;

class WinAnimationState : public IGameState
{
private:
	//AnimationController* m_controllerLevel;
	//AnimationController* m_controllerPlayer;
	//PlayerAnimations* m_animationswPtr;
	//Animation* m_animation;

	PauseModule m_pause;
public:
	WinAnimationState(GameStateData* p_data);
	~WinAnimationState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
