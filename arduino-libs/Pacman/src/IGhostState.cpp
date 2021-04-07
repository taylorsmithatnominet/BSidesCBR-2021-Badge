#include "IGhostState.h"

namespace pacman {

IGhostState::IGhostState()
	: m_datawPtr(nullptr)
{
}

IGhostState::IGhostState(GameObjectData* p_data)
	: m_datawPtr(p_data)
{
}

IGhostState::~IGhostState()
{
}

void IGhostState::Draw()
{
}

}; // namespace pacman
