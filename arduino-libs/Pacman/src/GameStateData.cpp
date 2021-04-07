#include "GameStateData.h"

#include "MovementModule.h"
#include "Level.h"
#include "AmbushState.h"
#include "ChaseState.h"
#include "RandomBehaviorState.h"
#include "RoamAtRandom_State.h"
 
#include "ServiceLocator.h"
#include "SpriteManager.h"
#include "DrawManager.h"

#include "Font.h"
#include "Highscores.h"
#include "Fruit.h"

#include "Config.h"

#include "gen/extralife_png.h"

namespace pacman {

void GameStateData::DeleteObjects()
{
	DeleteGhosts();
	DeletePlayer();
	DeleteLevel();
	DeleteFruits();
}

void GameStateData::DeleteLevel()
{
	if (m_level != nullptr) {
		delete m_level;
		m_level = nullptr;
	}
}

void GameStateData::DeletePlayer()
{
	if (m_player != nullptr) {
		delete m_player;
		m_player = nullptr;
	}
}

void GameStateData::DeleteGhosts()
{
	for (auto g : m_ghosts) {
		delete g;
		g = nullptr;
	}
	m_ghosts.clear();
}

void GameStateData::DeleteFruits()
{
	for (auto f : m_fruits) {
		delete f;
	}
	m_fruits.clear();
}

void GameStateData::CreateObjects()
{
	CreateLevel(); // do this first, pathfinding is coupled with it :I

	CreatePlayer();

	CreateGhosts();
}

void GameStateData::CreateLevel()
{
	m_level = new Level();
	m_level->LoadLevel();
}

void GameStateData::CreatePlayer()
{
	Vect2* player_pos = new Vect2(Config::LEVEL_CENTER, 23.0f);
	m_player = new Player(
		new GameObjectData(
			player_pos,
			new MovementModule(m_level, player_pos),
			m_level
		));
}

void GameStateData::CreateGhosts()
{
	GameObjectData* data = nullptr;

	//red chaser
	data = CreateGhostData(new Vect2((float)Config::LEVEL_CENTER, (float)Config::HOUSE_EXIT_Y));
	AddGhost(new ChaseState(data, m_player->GetPos()), data, Vect2::LEFT);

	//pink ambusher
	data = CreateGhostData(new Vect2((float)Config::LEVEL_CENTER, (float)Config::HOUSE_Y));
	AddGhost(new AmbushState(data, m_player), data, Vect2::DOWN);

	//oblivious
	data = CreateGhostData(new Vect2((float)Config::LEVEL_CENTER - 2, (float)Config::HOUSE_Y));
	AddGhost(new RandomBehaviorState(
		new AmbushState(data, m_player),
		new ChaseState(data, m_player->GetPos()),
		new RoamAtRandom_State(data)),
		data, Vect2::UP);

	//random boi
	data = CreateGhostData(new Vect2((float)Config::LEVEL_CENTER + 2, (float)Config::HOUSE_Y));
	AddGhost(new RoamAtRandom_State(data), data, Vect2::UP);

	for (unsigned int i = 1; i < m_ghosts.size(); i++) {
		m_ghosts[i]->StartWaiting((i - 1) * Config::SPAWN_TIME);
	}
}

void GameStateData::AddGhost(IState * roaming, GameObjectData * p_data, Vect2 p_dir)
{
	Ghost* ghost = new Ghost(
		p_data,
		roaming,
		m_player->GetPos(),
		m_ghosts.size(),
		p_dir
	);

	m_ghosts.push_back(ghost);
}

GameObjectData * GameStateData::CreateGhostData(Vect2 * p_pos)
{
	return new GameObjectData(
		p_pos,
		new MovementModule(m_level, p_pos),
		m_level
	);
}

GameStateData::GameStateData()
{
    m_level = nullptr;
    m_player = nullptr;
    SpriteTexture tex = CreateSpriteTextureFromHeader(extralife_png);

	SpriteManager* spriteManager = ServiceLocator<SpriteManager>::GetService();
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();
	m_fontwPtr = ServiceLocator<Font>::GetService();
	m_extraLifeSprite = spriteManager->CreateSprite(tex, 0, 0, 16, 16);

	m_highscoreswPtr = ServiceLocator<Highscores>::GetService();
}

GameStateData::~GameStateData()
{
	DeleteObjects();
}

void GameStateData::Update(float p_delta)
{
}

void GameStateData::DrawAll()
{
	DrawLevel();
	DrawFruit();
	DrawGhosts();
	DrawPlayer();
	DrawHUD();
}

void GameStateData::DrawLevel()
{
	m_level->Draw();
}

void GameStateData::DrawGhosts()
{
	for (auto g : m_ghosts)
		g->Draw();
}

void GameStateData::DrawPlayer()
{
	m_player->Draw();
}

void GameStateData::DrawHUD()
{
	float center = Config::WINDOW_WIDTH / 2;

	for (int x = 0; x < m_lives; x++) {
		m_drawManagerwPtr->Draw(m_extraLifeSprite, 16 + (16 * x), Config::WINDOW_HEIGHT - 16);
	}

    char score_string[20] = {0};
    snprintf(score_string, sizeof(score_string), "%d0", m_score);

	m_fontwPtr->DrawLeftAnchor(Vect2(float(7 * Config::TILE_SIZE) - float(1 * Config::TILE_SIZE), float(Config::TILE_SIZE)), score_string);

    /*
	if (m_1UP) {
		m_fontwPtr->Draw(Vect2(float(0.5 * Config::TILE_SIZE), 0.0f), "1UP");
	}
    */

	m_fontwPtr->DrawCentered(Vect2(center, 0), "HIGH SCORE");

    char highscore_string[20] = {0};
	
	if (m_highscoreswPtr->GetHighscore() > m_score) {
        snprintf(highscore_string, sizeof(score_string), "%d0", m_highscoreswPtr->GetHighscore());
	}
	else {
        memcpy(highscore_string, score_string, sizeof(highscore_string));
	}

    m_fontwPtr->DrawCentered(Vect2(center, (float)Config::TILE_SIZE), highscore_string);

    char level[20] = {0};
    snprintf(level, sizeof(score_string), "L%d", m_screen);
    m_fontwPtr->Draw(Vect2(float(Config::WINDOW_WIDTH - (2 * Config::TILE_SIZE)) - float(0.5 * Config::TILE_SIZE), (float)Config::TILE_SIZE), level);
}

void GameStateData::DrawFruit()
{
	for (auto f : m_fruits) {
		f->Draw();
	}
}

void GameStateData::StartGame(int p_lives)
{
	m_spawnedFruit = false;
	m_screen = 0;
	m_score = 0;
	m_1UP = true;
	m_lives = p_lives;
	NextScreen();
}

void GameStateData::NextScreen()
{
	m_screen++;
	m_spawnedFruit = false;
	DeleteObjects();
	CreateObjects();

	//std::cout << "Level : " << m_screen << "\n";
    //std::cout << std::flush;
}

void GameStateData::Retry()
{
	m_lives--;

	DeleteGhosts();
	DeletePlayer();
	DeleteFruits();

	CreatePlayer();
	CreateGhosts();
}

void GameStateData::CheckAgainstHighscores()
{
	if (m_highscoreswPtr->TryScore(m_score)) {
		m_highscoreswPtr->AddScore(m_score);
	}
}

void GameStateData::Exit()
{
	DeleteObjects();
}

bool GameStateData::IsGameOver()
{
	return m_lives == 0;
}

bool GameStateData::GotHighscore()
{
	return m_score == m_highscoreswPtr->GetHighscore();
}

bool GameStateData::AddPoints(int p_points)
{
	m_score += p_points;

	if (m_1UP && m_score > Config::ONE_UP_AT) {
		m_lives++;
		m_1UP = false;
		return true;
	}
	return false;
}

int GameStateData::GetScore()
{
	return m_score;
}

int GameStateData::GetCurrentScreen()
{
	return m_screen;
}

}; // namespace pacman
