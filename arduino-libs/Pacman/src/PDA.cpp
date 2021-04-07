#include "PDA.h"
#include "IState_PDA.h"

namespace pacman {

PDA::PDA(IState_PDA * p_state)
{
	m_currentState = p_state;
	m_currentState->Enter();
}

PDA::~PDA()
{
	if (m_currentState != nullptr) {
		m_currentState->Exit();
		delete m_currentState;
		m_currentState = nullptr;
	}
	for (auto s : history) {
		s->Exit();
		delete s;
	}
	history.clear();
}

bool PDA::Update(float p_delta)
{
	switch (m_currentState->Update(p_delta)) {
	case PDA::PDA_ReturnFlag::Quit:
		return false;
	case PDA::PDA_ReturnFlag::Keep:
		return true;
	case PDA::PDA_ReturnFlag::Pop:
		PopState();
		break;
	case PDA::PDA_ReturnFlag::Push:
		PushState(m_currentState->NextState());
		break;
	case PDA::PDA_ReturnFlag::Replace_Root:
		ReplaceRootState(m_currentState->NextState());
		break;
	case PDA::PDA_ReturnFlag::Switch:
		SwitchState(m_currentState->NextState());
		break;
	}
	return true;
}

void PDA::Draw()
{
	m_currentState->Draw();
}

void PDA::PushState(IState_PDA * p_state)
{
	m_currentState->Exit();
	history.push_back(m_currentState);

	m_currentState = p_state;
	m_currentState->Enter();
}

void PDA::PopState()
{
	m_currentState->Exit();
	delete m_currentState;

	m_currentState = history.back();
	history.pop_back();
	m_currentState->Enter();
}

void PDA::SwitchState(IState_PDA * p_state)
{
	m_currentState->Exit();
	delete m_currentState;

	m_currentState = p_state;
	m_currentState->Enter();
}

void PDA::ReplaceRootState(IState_PDA * p_state)
{
	m_currentState->Exit();
	delete m_currentState;

	for (auto s : history) {
		delete s;
	}

	history.clear();

	m_currentState = p_state;
	m_currentState->Enter();
}

}; // namespace pacman
