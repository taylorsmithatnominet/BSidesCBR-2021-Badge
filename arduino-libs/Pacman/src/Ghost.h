#pragma once
#include "GameObject.h"

namespace pacman {

class IState;
class Level;
class GhostAnimations;

class WaitingState;
class ExitState;
class FleeingBehavior;
class RunToHouseState;
class ScatterState;

class Animation;
class PauseModule;

class Ghost : public GameObject
{
private:
	WaitingState* m_waiting;
	ExitState* m_exitHouse;
	IState* m_roaming;
	RunToHouseState* m_runToHouse;
	ScatterState* m_scatterState;
	IState* m_currentStatewPtr;
	PauseModule* m_fleeingTime;

	GhostAnimations* m_animationswPtr;
	FleeingBehavior* m_fleeingBehavior;

	int m_color;
	bool m_justDied;

	void SwitchState(IState* p_state);
	void SetAnimation();
	void EndFlee();
protected:
	void UpdateCollider();
public:
	Ghost(GameObjectData* p_data, IState * p_roaming, Vect2* p_playerPos, int p_color, Vect2 p_startDir);
	virtual ~Ghost();

	virtual bool Update(float p_delta);

	void StartRoaming();
	void StartWaiting(int p_wait);
	void Flee(float p_time);
	void FleeScatter();
	void RunToHouse(Animation* p_animation);

	bool IsFleeing();
	bool IsDead();
	bool HasStartedRunning();
};

}; // namespace pacman
