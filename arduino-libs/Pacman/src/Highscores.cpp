#include "Highscores.h"

#include "Config.h"

namespace pacman {

void Highscores::CreateEmptyScores()
{
	for (int i = 0; i < Config::MAX_SCORES; i++) {
		m_scores[i] = Config::START_SCORE - i;
	}

	UpdateScores();
}

void Highscores::UpdateScores()
{
    // FUTURE: - save highscores
}

Highscores::Highscores()
{
    // FUTURE: - load highscores
    CreateEmptyScores();
}

Highscores::~Highscores()
{
}

int Highscores::GetHighscore()
{
	return m_scores[0];
}

bool Highscores::TryScore(int p_score)
{
	bool add = false;
	for (auto s : m_scores) {
		if (p_score > s) {
			add = true;
		}
	}
	return add;
}

bool Highscores::AddScore(int p_score)
{
	// this shouldn't happen! use "TryScore" first
	// this function will delete the pointer if it can't be added
	if (!TryScore(p_score)) {
		return false;
	}

    bool swap = false;
    int s_prev = 0;
    int s_tmp = 0;
    for (unsigned int i = 0; i < Config::MAX_SCORES; i++) {
        if ( swap )
        {
            s_tmp = m_scores[i];
            m_scores[i] = s_prev;
            s_prev = s_tmp;
        }
        else if (p_score > m_scores[i]) { // add here
            s_prev = m_scores[i];
            m_scores[i] = p_score;
            swap = true;
        }
    }

    if ( swap )
    {
        UpdateScores();
    }

    return false;
}

}; // namespace pacman
