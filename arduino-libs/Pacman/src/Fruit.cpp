#include "Fruit.h"
 

#include "ServiceLocator.h"
#include "FruitAnimations.h"

#include "Config.h"

namespace pacman {

#define SCORES_COUNT    8
const int m_scores[SCORES_COUNT] = {
	10,
	30,
	50,
	70,
	100,
	200,
	300,
	500
};

Fruit::Fruit(int p_index)
{
	m_isDead = false;
	m_index = p_index;
	if (m_index >= SCORES_COUNT) {
		m_index = SCORES_COUNT - 1;
	}

	m_pos = Vect2(Config::WINDOW_WIDTH / 2 - Config::TILE_SIZE, 20 * Config::TILE_SIZE - Config::TILE_SIZE / 2);

	m_collider->m_rect.x = (int)m_pos.x + Config::TILE_SIZE / 2;
	m_collider->m_rect.y = (int)m_pos.y + Config::TILE_SIZE / 2;
	m_collider->m_rect.w = Config::TILE_SIZE;
	m_collider->m_rect.h = Config::TILE_SIZE;

	m_time = Config::FRUIT_ACTIVE_TIME;
	m_pauseModule.SetPause(100);

	m_animationswPtr = ServiceLocator<FruitAnimations>::GetService();

	m_animation->SetAnimation(m_animationswPtr->GetAnimation(m_index));
}

Fruit::~Fruit()
{
}

void Fruit::Update(float p_delta)
{
	m_animation->Update(p_delta);

	if (m_time > 0) {
		m_time -= p_delta;

		if (m_time <= 0) {
			m_pauseModule.SetPause(Config::FRUIT_DESPAWN_TIME);
			m_animation->SetAnimation(m_animationswPtr->GetFlashAnimation(m_index));
		}
	}
	else {
		if (!m_pauseModule.Update(p_delta)){
			m_isDead = true;
		}
	}
}

void Fruit::Draw()
{
	m_animation->Draw(m_pos);
}

void Fruit::Eat()
{
	m_isDead = true;
	m_animation->SetAnimation(m_animationswPtr->GetScoreAnimation(m_index));
}

bool Fruit::IsDead()
{
	return m_isDead;
}

int Fruit::GetScore()
{
	return m_scores[m_index];
}

}; // namespace pacman
