#pragma once

#include <cstdint>

#include "GameStateData.h"

namespace pacman {

class IGameState;
class PlayingState;
class StartAnimationState;
class DeathAnimationState;
class WinAnimationState;
class ScoreScreenState;
class AttractScreenState;

class GameSession
{
private:
	GameStateData* m_data;

	PlayingState* m_playing;
	StartAnimationState* m_startAnimation;
	DeathAnimationState* m_deathAnimation;
	WinAnimationState* m_winAnimation;
	ScoreScreenState* m_scoreScreen;
	AttractScreenState* m_attractScreen;
	IGameState* m_currentStatewPtr;

	void SwitchState(IGameState* p_state);
public:
	GameSession();
	~GameSession();

    void SetGameOver(std::function<void(uint16_t)> func) { m_data->SetGameOver(func); }

	bool Update(float p_delta);
	void Draw();
};

}; // namespace pacman
