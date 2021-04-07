#pragma once

namespace pacman {

class PauseModule
{
private:
	float m_time;
public:
	PauseModule();
	~PauseModule();

	bool Update(float p_delta);
	bool IsPaused();

	void SetPause(float p_time);
	float GetTime();
};

}; // namespace pacman
