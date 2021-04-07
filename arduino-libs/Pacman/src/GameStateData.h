#pragma once

#include <cstdint>
#include <functional>

#include "Ghost.h"
#include "Player.h"

namespace pacman {

class Level;
class Sprite;
struct GameObjectData;
class Font;
class Highscores;
class Fruit;

class GameStateData
{
private:
	int m_score;
	int m_lives;
	bool m_1UP;
	int m_screen;
	DrawManager* m_drawManagerwPtr;
	Highscores* m_highscoreswPtr;

	void DeleteObjects();
	void DeleteLevel();
	void DeletePlayer();
	void DeleteGhosts();
	void DeleteFruits();

	void CreateObjects();
	void CreateLevel();
	void CreatePlayer();
	void CreateGhosts();
	void AddGhost(IState* roaming, GameObjectData* p_data, Vect2 p_dir);
	GameObjectData* CreateGhostData(Vect2* p_pos);

	Sprite* m_extraLifeSprite;
public:
	GameStateData();
	~GameStateData();

	std::vector<Ghost*> m_ghosts;
	std::vector<Fruit*> m_fruits;
	Player* m_player;
	Level* m_level;
	Font* m_fontwPtr;
	bool m_spawnedFruit;
    std::function<void(uint16_t)> m_game_over;

    void SetGameOver(std::function<void(uint16_t)> func) { m_game_over = func; }

	void Update(float p_delta);
	void DrawAll();
	void DrawLevel();
	void DrawGhosts();
	void DrawPlayer();
	void DrawHUD();
	void DrawFruit();

	void StartGame(int p_lives);
	void NextScreen();
	void Retry();
	void CheckAgainstHighscores();
	void Exit();

	bool IsGameOver();
	bool GotHighscore();
	
	bool AddPoints(int p_points);
	int GetScore();
	int GetCurrentScreen();
};

}; // namespace pacman
