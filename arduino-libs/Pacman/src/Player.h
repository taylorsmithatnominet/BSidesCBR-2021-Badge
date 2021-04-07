#pragma once
#include "GameObject.h"
#include "vect2.h"
#include <vector>

namespace pacman {

class InputManager;
class PlayerAnimations;

class Player : public GameObject
{
private:
	InputManager* m_inputwPtr;
	PlayerAnimations* m_animationswPtr;

	Vect2 m_currentInput;
	bool m_isDead;

	Vect2 GetAverageInput(const std::vector<Vect2>& p_inputs);
	void UpdateInput();
public:
	Player(GameObjectData* p_data);
	~Player();

	bool Update(float p_delta);

	bool SteppedOnTile();

	void Kill();
	bool IsDead();
	Vect2 GetDirection();
};

}; // namespace pacman
