#pragma once

namespace pacman {

class IState
{
public:
	IState();
	virtual ~IState();
	virtual void Enter();
	virtual void Exit();
	virtual bool Update(float p_delta) = 0;
	virtual void Draw() = 0;
};

}; // namespace pacman
