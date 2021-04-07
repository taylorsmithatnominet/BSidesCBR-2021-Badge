#pragma once
#include "IState.h"
#include "DrawManager.h"
#include "GameStateData.h"

namespace pacman {

class IGameState : public IState
{
protected:
	DrawManager* m_drawManagerwPtr;
	GameStateData* m_datawPtr;
public:
	IGameState(GameStateData* p_data);
	virtual ~IGameState();
};

}; // namespace pacman
