#pragma once

#include "Config.h"

namespace pacman {

class Highscores
{
	void CreateEmptyScores();
	void UpdateScores();
public:
	int m_scores[Config::MAX_SCORES];

	Highscores();
	~Highscores();

	int GetHighscore();

	bool TryScore(int p_score);
	bool AddScore(int p_score);
};

}; // namespace pacman
