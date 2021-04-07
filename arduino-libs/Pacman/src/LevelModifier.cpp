#include "LevelModifier.h"

#include "Config.h"

namespace pacman {

LevelModifier::LevelModifier()
{
	PacSpeed = Config::MOVEMENT_SPEED;
	GhostSpeed = Config::MOVEMENT_SPEED;
	FleeingTime = Config::FLEEING_TIME;
	effect = FleeEffect;
}

LevelModifier::LevelModifier(float p_pacSpeed, float p_ghostSpeed, float p_fleeingTime, EnergizerEffect p_effect)
	: PacSpeed(p_pacSpeed), GhostSpeed(p_ghostSpeed),
	FleeingTime(p_fleeingTime), effect(p_effect)
{
}

LevelModifier LevelModifier::GetModifier(int p_level)
{
	LevelModifier ret = LevelModifier();

	ret.effect = FleeEffect;
	switch (p_level) {
	case 1:
		ret.PacSpeed = 1.0f;
		ret.GhostSpeed = 0.9f;
		ret.FleeingTime = Config::FLEEING_TIME;
		break;
	case 2:
		ret.PacSpeed = 1.0f;
		ret.GhostSpeed = 1.0f;
		ret.FleeingTime = Config::FLEEING_TIME;
		break;
	case 3:
	case 4:
		ret.PacSpeed = 1.0f;
		ret.PacSpeed = 1.1f;
		ret.GhostSpeed = 1.1f;
		ret.FleeingTime = Config::FLEEING_TIME / 2.0f;
		break;
	case 5:
		ret.PacSpeed = 1.1f;
		ret.GhostSpeed = 1.25f;
		ret.FleeingTime = 0;
		break;
	case 6:
	case 7:
		ret.PacSpeed = 1.2f;
		ret.GhostSpeed = 1.3f;
		ret.FleeingTime = 0;
		ret.effect = ScatterEffect;
		break;
	case 8:
	case 9:
		ret.PacSpeed = 1.2f;
		ret.GhostSpeed = 1.35f;
		ret.FleeingTime = 0;
		ret.effect = ScatterEffect;
		break;
	case 10:
	case 11:
	case 12:
		ret.PacSpeed = 1.3f;
		ret.GhostSpeed = 1.45f;
		ret.FleeingTime = 0;
		ret.effect = ScatterEffect;
		break;
	default:
		ret.PacSpeed = 1.35f;
		ret.GhostSpeed = 1.55f;
		ret.FleeingTime = 0;
		ret.effect = IgnoreEffect;
		break;
	}

	return ret;
}

}; // namespace pacman
