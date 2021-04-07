#include <cstdio>

#include "ScoreScreenState.h"

#include "ServiceLocator.h"
#include "Font.h"
 
#include "InputManager.h"
#include "Highscores.h"
#include "Color.h"
#include "Keyboard.h"

#include "Config.h"

namespace pacman {

ScoreScreenState::ScoreScreenState(GameStateData* p_data)
	: IGameState(p_data)
{
	m_highscoreswPtr = ServiceLocator<Highscores>::GetService();
	m_input = ServiceLocator<InputManager>::GetService();

    m_yellow = PM_COLOR16_FROM_RGB(255, 255, 0);
}

ScoreScreenState::~ScoreScreenState()
{
}

bool ScoreScreenState::Update(float p_delta)
{
	if (m_input->IsKeyDownOnce(PM_SCANCODE_ENTER)) {
		return false;
	}

	return true;
}

void ScoreScreenState::Draw()
{
	float center = Config::WINDOW_WIDTH / 2;

	m_datawPtr->m_fontwPtr->DrawCentered(Vect2(center, Config::TILE_SIZE * 8), "HIGH SCORES");

	int y = 12;
	Vect2 pos;
	char str[20] = {0};
	for (auto s : m_highscoreswPtr->m_scores) {
		pos = Vect2(float(center + Config::TILE_SIZE * 2.5f), float(Config::TILE_SIZE * y));
		snprintf(str, sizeof(str), "%d0", s);

		if (s == m_datawPtr->GetScore()) {
            m_datawPtr->m_fontwPtr->DrawLeftAnchor(pos, str, m_yellow);

            if (m_datawPtr->GotHighscore()) {
                pos = Vect2(center, Config::WINDOW_HEIGHT - Config::TILE_SIZE * 12);
                m_datawPtr->m_fontwPtr->DrawCentered(pos, "NEW HIGHSCORE!", m_yellow);
            }
		}
		else {
			m_datawPtr->m_fontwPtr->DrawLeftAnchor(pos, str);
		}

		y += 2;
	}

	pos = Vect2(center, Config::WINDOW_HEIGHT - Config::TILE_SIZE * 8);
	m_datawPtr->m_fontwPtr->DrawCentered(pos, "PRESS ENTER");
}

void ScoreScreenState::Enter()
{
    m_last_score = m_datawPtr->GetScore();
}

void ScoreScreenState::Exit()
{
    if ( m_datawPtr->m_game_over )
    {
        m_datawPtr->m_game_over(m_last_score * 10);
    }
}

}; // namespace pacman
