#include "PacmanGame.h"
#include "IGame.h"

#include "GameSession.h"

#include "ServiceLocator.h"
#include "GhostAnimations.h"
#include "PlayerAnimations.h"
#include "FruitAnimations.h"
#include "Font.h"
#include "Highscores.h"

namespace pacman {

PacmanGame::PacmanGame()
{
	m_ghostAnimations = new GhostAnimations();
	m_playerAnimations = new PlayerAnimations();
	m_fruitAnimations = new FruitAnimations();
	m_font = new Font();
	m_scores = new Highscores();
	ServiceLocator<GhostAnimations>::SetService(m_ghostAnimations);
	ServiceLocator<PlayerAnimations>::SetService(m_playerAnimations);
	ServiceLocator<FruitAnimations>::SetService(m_fruitAnimations);
	ServiceLocator<Font>::SetService(m_font);
	ServiceLocator<Highscores>::SetService(m_scores);


	m_GameFSM = new GameSession();
}


PacmanGame::~PacmanGame()
{
	delete m_GameFSM;
	m_GameFSM = nullptr;

	delete m_ghostAnimations;
	m_ghostAnimations = nullptr;

	delete m_font;
	m_font = nullptr;

	delete m_scores;
	m_scores = nullptr;

	delete m_fruitAnimations;
	m_fruitAnimations = nullptr;

	delete m_playerAnimations;
	m_playerAnimations = nullptr;
}

void PacmanGame::Enter()
{
}

void PacmanGame::Exit()
{
}

bool PacmanGame::Update(float p_delta)
{
	return m_GameFSM->Update(p_delta);
}

void PacmanGame::Draw()
{
	m_GameFSM->Draw();
}

}; // namespace pacman
