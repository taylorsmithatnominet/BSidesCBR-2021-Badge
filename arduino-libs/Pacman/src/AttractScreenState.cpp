#include "AttractScreenState.h"

#include "ServiceLocator.h"
#include "GhostAnimations.h"
#include "AnimationController.h"
#include "InputManager.h"
 
#include "Font.h"
#include "Color.h"
#include "Keyboard.h"

#include "Config.h"

namespace pacman {

const char *CHARACTERS[4] {
	"-SHADOW",
	"-SPEEDY",
	"-BASHFUL",
	"-POKEY",
};

const char *NICKNAMES[4] = {
	"\"BLINKY\"",
	"\"PINKY\"",
	"\"INKY\"",
	"\"CLYDE\""
};

const uint16_t COLORS[4] {
    PM_COLOR16_FROM_RGB(255, 0, 0),     // red
    PM_COLOR16_FROM_RGB(255, 184, 255), // pink
    PM_COLOR16_FROM_RGB(1, 255, 255),   // cyan
    PM_COLOR16_FROM_RGB(255, 184, 81),  // orange
};

AttractScreenState::AttractScreenState(GameStateData * p_data)
	: IGameState(p_data)
{
	m_input = ServiceLocator<InputManager>::GetService();

	GhostAnimations* animations = ServiceLocator<GhostAnimations>::GetService();

	for (int i = 0; i < 4; i++) {
		m_ghosts[i] = new AnimationController();
		m_ghosts[i]->SetAnimation(animations->GetAnimation(Vect2::RIGHT, i));
	}
}

AttractScreenState::~AttractScreenState()
{
	for (auto g : m_ghosts) {
		delete g;
	}
}

bool AttractScreenState::Update(float p_delta)
{
	for (auto g : m_ghosts)
		g->Update(p_delta);

	if (m_input->IsKeyDownOnce(PM_SCANCODE_ENTER)) {
		return false;
	}

	return true;
}

void AttractScreenState::Draw()
{
	float center = Config::WINDOW_WIDTH / 2;

	m_datawPtr->DrawHUD();

	//m_datawPtr->m_fontwPtr->DrawCentered(Vect2(center, 5 * Config::TILE_SIZE), "CHARACTER/NICKNAME");
    m_datawPtr->m_fontwPtr->DrawCentered(Vect2(center, 5 * Config::TILE_SIZE), "GHOST/NICKNAME");

	for (unsigned int i = 0; i < 4; i++) {
		int y = (7 + (i * 3)) * Config::TILE_SIZE;
		m_ghosts[i]->Draw(Vect2(float(1 * Config::TILE_SIZE), float(y - Config::TILE_SIZE / 2)));
		uint16_t color = COLORS[i];
		m_datawPtr->m_fontwPtr->Draw(Vect2(float(3 * Config::TILE_SIZE), (float)y), CHARACTERS[i], color);
		//m_datawPtr->m_fontwPtr->DrawLeftAnchor(Vect2(float(Config::WINDOW_WIDTH - 1 * Config::TILE_SIZE), (float)y), NICKNAMES[i], color);
        m_datawPtr->m_fontwPtr->Draw(Vect2(float(6 * Config::TILE_SIZE), (float)y + float(1 * Config::TILE_SIZE)), NICKNAMES[i], color);
	}

	m_datawPtr->m_fontwPtr->DrawCentered(Vect2(center, Config::WINDOW_HEIGHT - (8 * Config::TILE_SIZE)), "PRESS ENTER!");
}

void AttractScreenState::Enter()
{
}

void AttractScreenState::Exit()
{
}

}; // namespace pacman
