#pragma once

#include <cstdint>

#include "IGame.h"

#include "GameSession.h"

namespace pacman {

class GhostAnimations;
class PlayerAnimations;
class FruitAnimations;
class Font;
class Highscores;

class PacmanGame : public IGame
{
private:
	GameSession* m_GameFSM;

	GhostAnimations* m_ghostAnimations;
	PlayerAnimations* m_playerAnimations;
	FruitAnimations* m_fruitAnimations;
	Font* m_font;
	Highscores* m_scores;
public:
	PacmanGame();
	~PacmanGame();

    void SetGameOver(std::function<void(uint16_t)> func) { m_GameFSM->SetGameOver(func); }

	void Enter();
	void Exit();

	bool Update(float p_delta);
	void Draw();
};

}; // namespace pacman
