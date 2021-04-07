#include "RandomBehaviorState.h"

#include <random>
#include "AmbushState.h"
#include "ChaseState.h"
#include "RoamAtRandom_State.h"

namespace pacman {

void RandomBehaviorState::SwitchState()
{
	unsigned int r = rand() % (m_behaviors.size() + 1);

	// double the chances of roam_random
	if (r >= m_behaviors.size())
		r = 0;

	if (m_currentState != nullptr)
		m_currentState->Exit();
	m_currentState = m_behaviors[r];
	m_currentState->Enter();
}

RandomBehaviorState::RandomBehaviorState(AmbushState* p_ambush, ChaseState* p_chase,
	RoamAtRandom_State* p_roamRandom)
{
    m_currentState = nullptr;
	m_behaviors.push_back(p_roamRandom);
	m_behaviors.push_back(p_ambush);
	m_behaviors.push_back(p_chase);
	SwitchState();
}


RandomBehaviorState::~RandomBehaviorState()
{
	for (auto b : m_behaviors) {
		delete b;
		b = nullptr;
	}
	m_behaviors.clear();
}

bool RandomBehaviorState::Update(float p_delta)
{
	if (!m_currentState->Update(p_delta)) {
		SwitchState();
		return false;
	}

	return true;
}

void RandomBehaviorState::Draw()
{
	m_currentState->Draw();
}

}; // namespace pacman
