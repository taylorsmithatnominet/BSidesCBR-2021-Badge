#pragma once

namespace pacman {

class IGame
{
public:
	IGame();
	virtual ~IGame();

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	virtual bool Update(float p_delta) = 0;
	virtual void Draw() = 0;
};

}; // namespace pacman
