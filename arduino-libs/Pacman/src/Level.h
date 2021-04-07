#pragma once
#include <cstdint>
#include <vector>
#include "Tile.h"
#include "Config.h"

namespace pacman {

class DrawManager;
class Sprite;
class Vect2;
class PowerUp;
class ICollideable;
class Player;

class Level
{
private:
	DrawManager* m_drawManagerwPtr;

	int m_pelletsCount;
	void DeletePowerUps();

    Tile* m_tiles[Config::LEVEL_HEIGHT][Config::LEVEL_WIDTH];
	std::vector<PowerUp*> m_powerUps;
	Tile* charToTile(char c, int p_x, int p_y);

	void DrawPellets(const int &p_x, const int &p_y);
	void DrawWall(const int &p_x, const int &p_y, bool wall3x3[9], uint16_t color16);
	void EatTile(Tile* p_tile);

	int CorrectTeleportAxis(const int & p_a, const int & p_max) const;
	bool IsBanned(const Vect2& p_pos, const std::vector<Vect2>& p_banned) const;
public:
	Level();
	~Level();

	void LoadLevel();

	void DrawLevel(const int &p_x, const int &p_y, uint16_t color16);

	Tile* GetTile(Vect2 p_vect2);
	Tile* GetTile(int p_x, int p_y);

	bool PelletCollision(Player* p_player);
	bool PowerUpCollision(ICollideable* p_other);

	std::vector<Vect2> AvailableDirections(const Vect2& p_pos);
	std::vector<Vect2> AvailableDirections(const int& p_x, const int& p_y);
	
	//const bool& IsIntersection(Vect2 p_pos) const;
	int NextIntersection(Vect2& p_pos, const Vect2& p_dir);
	int NextIntersection(Vect2& p_pos, const Vect2& p_dir, const std::vector<Vect2>& p_banned);
	bool IsIntersection(int p_x, int p_y) const;
	int PelletsCount() const;

	void ResetAnimation();
	void Update(float p_delta);
	void Draw();
};

}; // namespace pacman
