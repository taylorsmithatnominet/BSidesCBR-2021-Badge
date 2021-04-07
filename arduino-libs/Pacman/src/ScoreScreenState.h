#pragma once

#include <cstdint>

#include "IGameState.h"

namespace pacman {

class ScoreScreenState : public IGameState
{
private:
	Highscores* m_highscoreswPtr;
	InputManager* m_input;
	uint16_t m_yellow;
    uint16_t m_last_score;
public:
	ScoreScreenState(GameStateData* p_data);
	~ScoreScreenState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
