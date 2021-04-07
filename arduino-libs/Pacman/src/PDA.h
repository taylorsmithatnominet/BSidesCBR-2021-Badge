#pragma once
#include <vector>

namespace pacman {

class IState_PDA;

class PDA
{
private:
	void PushState(IState_PDA* p_state);
	void PopState();
	void SwitchState(IState_PDA* p_state);
	void ReplaceRootState(IState_PDA* p_state);

	std::vector<IState_PDA*> history;
	IState_PDA* m_currentState;
public:
	PDA(IState_PDA* p_state);
	~PDA();

	enum PDA_ReturnFlag { Keep, Quit, Push, Pop, Switch, Replace_Root };

	bool Update(float p_delta);
	void Draw();
};

}; // namespace pacman
