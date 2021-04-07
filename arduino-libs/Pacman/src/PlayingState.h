#pragma once
#include "IGameState.h"
#include "PauseModule.h"
#include <vector>
#include "LevelModifier.h"

namespace pacman {

class Animation;

class PlayingState : public IGameState
{
private:
	enum Food { NothingFood, PelletFood, PowerUpFood };

	PauseModule m_pause;
	PauseModule m_fruitSpawnTime;
	std::vector<Animation*> m_scoreAnimations;
	Animation* m_1UPAnimation;
	int m_combo;
	bool m_hasWon;
	bool m_wakaSwitch;
	LevelModifier m_levelModifier;

	bool CheckGhostCollision();
	Food CheckFoodCollision();
	void UpdateFruits(float p_delta);
	
	void Win();
	void Lose();
	void AtePellet();
	void AtePowerup();
	void CheckIfWin();
public:
	PlayingState(GameStateData* p_data);
	~PlayingState();

	bool Update(float p_delta);
	void Draw();

	void Enter();
	void Exit();
};

}; // namespace pacman
