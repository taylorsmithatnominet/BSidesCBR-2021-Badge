#include "PlayingState.h"

#include "Level.h"

#include "CollisionManager.h"

#include "Animation.h"
#include "SpriteManager.h"
#include "ServiceLocator.h"
#include "Fruit.h"

#include "Config.h"

#include "gen/ghost_score_png.h"

namespace pacman {

bool PlayingState::CheckGhostCollision()
{
	for (auto g : m_datawPtr->m_ghosts) {
		if (CollisionManager::CheckCollision(g, m_datawPtr->m_player)) {
			if (!g->IsFleeing() && !g->IsDead()) {
				Lose();
			}

			if (g->IsFleeing()) {
				m_pause.SetPause(Config::PAUSE_TIME);

				m_combo++;
				int multiplier = m_combo * m_combo;
				Animation* score = nullptr;
				if (m_datawPtr->AddPoints(multiplier * Config::POINT_GHOST)) {
					score = m_1UPAnimation;
				}
				else {
					score = m_scoreAnimations[m_combo - 1];
				}
				g->RunToHouse(score);

				return true; // only one ghost may flee per frame
			}
		}
	}

	return false;
}

PlayingState::Food PlayingState::CheckFoodCollision()
{
	bool ate_pellet = false;
	bool ate_power_up = false;

	if (m_datawPtr->m_level->PelletCollision(m_datawPtr->m_player)) {
		ate_pellet = true;
	}
	if (m_datawPtr->m_level->PowerUpCollision(m_datawPtr->m_player)) {
		ate_power_up = true;
	}

	if (ate_pellet) {
		AtePellet();
	}
	if (ate_power_up) {
		AtePowerup();
	}

	if (ate_power_up)
		return PowerUpFood;
	if (ate_pellet)
		return PelletFood;
	return NothingFood;
}

void PlayingState::UpdateFruits(float p_delta)
{
	if (!m_datawPtr->m_spawnedFruit) {
		if (m_fruitSpawnTime.IsPaused()) {
			m_fruitSpawnTime.Update(p_delta);
			if (!m_fruitSpawnTime.IsPaused()) {
				m_datawPtr->m_spawnedFruit = true;
				m_datawPtr->m_fruits.push_back(new Fruit(m_datawPtr->GetCurrentScreen() - 1));
			}
		}
	}

	for (auto f : m_datawPtr->m_fruits) {
		f->Update(p_delta);
	}

	for (unsigned int i = 0; i < m_datawPtr->m_fruits.size(); i++) {
		Fruit* f = m_datawPtr->m_fruits[i];
		if (f->IsDead()) {

			m_datawPtr->m_fruits.erase(m_datawPtr->m_fruits.begin() + i);
			i--;
		}
		else if (CollisionManager::CheckCollision(f, m_datawPtr->m_player)) {
			f->Eat();
			m_datawPtr->AddPoints(f->GetScore());
			m_pause.SetPause(Config::PAUSE_TIME);
		}
	}
}

void PlayingState::Win()
{
	m_hasWon = true;
	m_pause.SetPause(Config::WIN_PAUSE_TIME);
}

void PlayingState::Lose()
{
	if (!m_datawPtr->m_player->IsDead()) {
		m_datawPtr->m_player->Kill();
		m_pause.SetPause(Config::PAUSE_TIME);
	}
}

void PlayingState::AtePellet()
{
	m_datawPtr->AddPoints(Config::POINT_PELLET);

	CheckIfWin();
}

void PlayingState::AtePowerup()
{
	m_datawPtr->AddPoints(Config::POINT_POWER_UP);

	if (m_levelModifier.effect != LevelModifier::EnergizerEffect::IgnoreEffect) {
		if (m_levelModifier.effect == LevelModifier::EnergizerEffect::FleeEffect) {
			m_combo = 0;
			for (auto g : m_datawPtr->m_ghosts) {
				g->Flee(m_levelModifier.FleeingTime);

			}
		}
		else {
			for (auto g : m_datawPtr->m_ghosts) {
				g->FleeScatter();
			}
		}
	}

	CheckIfWin();
}

void PlayingState::CheckIfWin()
{
	if (m_datawPtr->m_level->PelletsCount() == 0) {
		Win();
	}
}

PlayingState::PlayingState(GameStateData * p_data)
	: IGameState(p_data)
{
	m_wakaSwitch = false;

	SpriteManager* spriteManager = ServiceLocator<SpriteManager>::GetService();
    SpriteTexture tex = CreateSpriteTextureFromHeader(ghost_score_png);

	m_scoreAnimations.clear();
	for (int x = 0; x < 4; x++) {
		AnimationFrame next_frame = AnimationFrame(
			spriteManager->CreateSprite(tex, x * 16, 0, 16, 16),
			1000
		);
		Animation* next_animation = new Animation();
		next_animation->AddFrame(next_frame);
		m_scoreAnimations.push_back(next_animation);
	}


	m_1UPAnimation = new Animation();
	AnimationFrame next_frame = AnimationFrame(
		spriteManager->CreateSprite(tex, 64, 0, 22, 16),
		1000
	);
	m_1UPAnimation->AddFrame(next_frame);
}

PlayingState::~PlayingState()
{
	for (auto s : m_scoreAnimations) {
		delete s;
	}

	delete m_1UPAnimation;
}

bool PlayingState::Update(float p_delta)
{
	if (!m_pause.Update(p_delta)) {
		UpdateFruits(p_delta);

		for (auto g : m_datawPtr->m_ghosts)
			g->Update(p_delta * m_levelModifier.GhostSpeed);
		bool bumped = m_datawPtr->m_player->Update(p_delta * m_levelModifier.PacSpeed);

		CheckGhostCollision();

		Food food = CheckFoodCollision();
		if (food == PowerUpFood || food == PelletFood) {
			m_wakaSwitch = !m_wakaSwitch;
		}
	}
	else if (!m_hasWon){
		for (auto g : m_datawPtr->m_ghosts) { // update the dead ghosts
			if (g->HasStartedRunning())
				g->Update(p_delta);
		}
	}

	m_datawPtr->m_level->Update(p_delta); // update the pellet animation regardless

	if (!m_pause.IsPaused()) {
		if (m_datawPtr->m_player->IsDead()) {
			return false;
		}
		if (m_hasWon) {
			return false;
		}
	}

	return true;
}

void PlayingState::Draw()
{
	m_datawPtr->DrawLevel();
	m_datawPtr->DrawHUD();
	m_datawPtr->DrawFruit();
	m_datawPtr->DrawGhosts();
	if (m_pause.IsPaused()
		&& !m_datawPtr->m_player->IsDead()
		&& !m_hasWon) {
		
	}
	else {
		m_datawPtr->DrawPlayer();
	}
}

void PlayingState::Enter()
{
	m_hasWon = false;
	m_levelModifier = LevelModifier::GetModifier(m_datawPtr->GetCurrentScreen());
	m_fruitSpawnTime.SetPause(Config::FRUIT_SPAWN_TIME);
}

void PlayingState::Exit()
{
}

}; // namespace pacman
