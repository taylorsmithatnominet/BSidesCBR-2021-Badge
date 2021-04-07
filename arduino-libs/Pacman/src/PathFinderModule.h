#pragma once
#include "vect2.h"
#include "Tile.h"
#include <vector>

#include "Config.h"

namespace pacman {

class Level;


struct Ticket {
	int heat;
	Vect2 pos;

	Ticket()
		: heat(0), pos(0, 0)
	{

	}
	Ticket(int p_heat, Vect2 p_pos)
		: heat(p_heat), pos(p_pos)
	{

	}
};

class PathFinderModule
{
private:
	Level* m_levelwPtr;
    int m_heatMap[Config::LEVEL_HEIGHT][Config::LEVEL_WIDTH];

	std::vector<Vect2> m_banned_pos;
public:
	PathFinderModule(Level* p_level);
	~PathFinderModule();

	void UpdatePath(Vect2 p_target);
	void UpdatePath(Vect2 p_target, Vect2 p_banned_position);
	void UpdatePath(Vect2 p_target, std::vector<Vect2> p_banned_positions);
	Vect2 GetNextDir(Vect2 p_pos);
	Vect2 GetNextDir(Vect2 p_pos, Vect2 p_banned_dir);
	Vect2 GetNextDir(Vect2 p_pos, std::vector<Vect2> p_banned_directions);

	bool ReachedGoal(Vect2 p_pos);
};

}; // namespace pacman
