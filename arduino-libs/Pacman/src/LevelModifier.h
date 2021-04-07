#pragma once

namespace pacman {

struct LevelModifier
{
	enum EnergizerEffect { FleeEffect, ScatterEffect, IgnoreEffect };

	float PacSpeed;
	float GhostSpeed;
	float FleeingTime;
	EnergizerEffect effect;

	LevelModifier();
	LevelModifier(float p_pacSpeed, float p_ghostSpeed, float p_fleeingTime, EnergizerEffect p_effect);

	static LevelModifier GetModifier(int p_level);
};

}; // namespace pacman
