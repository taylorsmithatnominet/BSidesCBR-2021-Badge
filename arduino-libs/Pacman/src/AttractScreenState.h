#pragma once
#include "IGameState.h"

namespace pacman {

class AnimationController;

class AttractScreenState : public IGameState
{
private:
	InputManager* m_input;
	AnimationController* m_ghosts[4];
public:
	AttractScreenState(GameStateData* p_data);
	~AttractScreenState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
