#pragma once
#include "vect2.h"
#include <vector>

namespace pacman {

class Level;

class MovementModule
{
private:
	Vect2* m_positionwPtr;
	Level* m_levelwPtr;
	Vect2 m_direction;

	bool m_steppedOnTile;

	bool IsDirAvailable(Vect2 p_dir);
	void CheckWarp();

	void OnStep(Vect2 p_dir);

	bool CheckLenient();
	void CorrectOffset();
public:
	MovementModule(Level* p_level, Vect2* p_pos);
	MovementModule(Level* p_level, Vect2* p_pos, Vect2 p_dir);
	~MovementModule();

	void TurnAround();
	bool Update(float p_delta, Vect2 p_bufferedInput, float p_speed, bool p_cornering);

	std::vector<Vect2> AvailableDirections(Vect2 p_pos);
	bool SteppedOnTile();
	Vect2 GetDirection();
};

}; // namespace pacman
