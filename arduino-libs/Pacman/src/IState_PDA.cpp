#include "IState_PDA.h"

#include "ServiceLocator.h"
#include "DrawManager.h"

namespace pacman {

IState_PDA::IState_PDA()
{
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();
}


IState_PDA::~IState_PDA()
{
}

}; // namespace pacman
