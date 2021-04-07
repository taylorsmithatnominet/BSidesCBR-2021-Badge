#include "PauseModule.h"

namespace pacman {

PauseModule::PauseModule()
{
	m_time = 0;
}

PauseModule::~PauseModule()
{
}

bool PauseModule::Update(float p_delta)
{
	if (m_time > 0) {
		m_time -= p_delta;
	}
	return IsPaused();
}

bool PauseModule::IsPaused()
{
	return m_time > 0;
}

void PauseModule::SetPause(float p_time)
{
	m_time = p_time;
}

float PauseModule::GetTime()
{
	return m_time;
}

}; // namespace pacman
