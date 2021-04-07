#pragma once
#include "PDA.h"

namespace pacman {

class DrawManager;

class IState_PDA
{
protected:
	DrawManager* m_drawManagerwPtr;
public:
	IState_PDA();
	virtual ~IState_PDA();

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual PDA::PDA_ReturnFlag Update(float p_delta) = 0;
	virtual void Draw() = 0;
	virtual IState_PDA* NextState() = 0;
};

}; // namespace pacman
