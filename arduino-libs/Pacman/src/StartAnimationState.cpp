#include "StartAnimationState.h"

#include "ServiceLocator.h"
#include "DrawManager.h"
#include "Font.h"
#include "Color.h"
 
#include "Level.h"

#include "Config.h"

namespace pacman {

StartAnimationState::StartAnimationState(GameStateData * p_data)
	: IGameState(p_data)
{
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();

    m_yellow = PM_COLOR16_FROM_RGB(255, 255, 0);
}

StartAnimationState::~StartAnimationState()
{
}

bool StartAnimationState::Update(float p_delta)
{
    return false;
}

void StartAnimationState::Draw()
{
	m_datawPtr->DrawAll();

	Vect2 pos = Vect2(Config::WINDOW_WIDTH / 2 + Config::TILE_SIZE / 2, 20 * Config::TILE_SIZE);

	m_datawPtr->m_fontwPtr->DrawCentered(pos, "READY!", m_yellow);
}

void StartAnimationState::Enter()
{
	m_datawPtr->m_level->ResetAnimation();
}

void StartAnimationState::Exit()
{
}

}; // namespace pacman
