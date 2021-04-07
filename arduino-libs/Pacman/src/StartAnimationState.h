#pragma once

#include <cstdint>

#include "IGameState.h"

namespace pacman {

class Font;
class DrawManager;

class StartAnimationState : public IGameState
{
private:
	float m_time;
	DrawManager* m_drawManagerwPtr;

	uint16_t m_yellow;
public:
	StartAnimationState(GameStateData* p_data);
	~StartAnimationState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
