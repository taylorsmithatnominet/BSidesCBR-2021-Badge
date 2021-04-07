#pragma once
#include "IState.h"
#include "GameObjectData.h"

namespace pacman {

class IGhostState : public IState
{
protected:
	GameObjectData* m_datawPtr;
public:
	IGhostState();
	IGhostState(GameObjectData* p_data);
	~IGhostState();
	void Draw();
};

}; // namespace pacman
