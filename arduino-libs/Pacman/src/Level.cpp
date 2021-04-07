#include "Level.h"

#include <vector>

#include "Tile.h"

#include "ServiceLocator.h"
#include "SpriteManager.h"
#include "DrawManager.h"
#include "Sprite.h"

#include "vect2.h"

#include "PowerUp.h"
 

#include "CollisionManager.h"
#include "Player.h"

#include "Config.h"

#include "gen/level_txt.h"

namespace pacman {

Tile* Level::charToTile(char c, int p_x, int p_y)
{
	switch (c) {
	case 'w':
		return new Tile(TileType::Wall, p_x, p_y);
	case ' ':
		return new Tile(TileType::Empty, p_x, p_y);
	case '.':
		return new Tile(TileType::Pellet, p_x, p_y);
	case 'P':
		return new Tile(TileType::Powerup, p_x, p_y);
	}

	return new Tile();
}

void Level::DeletePowerUps()
{
	for (auto p : m_powerUps) {
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}
	}
	m_powerUps.clear();
}

int Level::CorrectTeleportAxis(const int & p_a, const int & p_max) const
{
	int a = p_a;
	if (a < 0)
		a += p_max;
	if (a >= p_max)
		a -= p_max;

	return a;
}

bool Level::IsBanned(const Vect2 & p_pos, const std::vector<Vect2>& p_banned) const
{
	Vect2 pos = p_pos.Round().CorrectTeleport();

	for (auto b : p_banned) {
		if (b == pos)
			return true;
	}

	return false;
}

Level::Level(){
    for ( int y = 0; y < Config::LEVEL_HEIGHT; y++ )
    {
        for ( int x = 0; x < Config::LEVEL_WIDTH; x++ )
        {
            m_tiles[y][x] = nullptr;
        }
    }
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();
}

Level::~Level()
{
    for ( int y = 0; y < Config::LEVEL_HEIGHT; y++ )
    {
        for ( int x = 0; x < Config::LEVEL_WIDTH; x++ )
        {
            delete m_tiles[y][x];
            m_tiles[y][x] = nullptr;
        }
    }
}

void Level::LoadLevel()
{
    for ( int y = 0; y < Config::LEVEL_HEIGHT; y++ )
    {
        for ( int x = 0; x < Config::LEVEL_WIDTH; x++ )
        {
            delete m_tiles[y][x];
            m_tiles[y][x] = nullptr;
        }
    }
	DeletePowerUps();

	m_pelletsCount = 0;

	int y = 0;
    size_t char_index = 0;
	while (char_index < level_txt_size) {
        char c = (char)pgm_read_byte(&level_txt_data[char_index]);
		int x = 0;
		while ( (char_index < level_txt_size) && (c != '\n')) {
			Tile* tile = charToTile(c, x, y);

			if (tile->GetType() == TileType::Pellet)
			{
				m_pelletsCount++;
			}

			if (tile->GetType() == TileType::Powerup) {
				m_powerUps.push_back(
					new PowerUp(Vect2((float)x * Config::TILE_SIZE, (float)y * Config::TILE_SIZE + 24))
				);
			}

            m_tiles[y][x] = tile;

			x++;
            char_index++;
            c = (char)pgm_read_byte(&level_txt_data[char_index]);
		}
		y++;
        char_index++;
	}
}

void Level::DrawPellets(const int &p_x, const int &p_y)
{
    for ( int y = 0; y < Config::LEVEL_HEIGHT; y++ )
    {
        for ( int x = 0; x < Config::LEVEL_WIDTH; x++ )
        {
            auto t = m_tiles[y][x];

			int posx = p_x + (x * 8) + 3;
			int posy = p_y + (y * 8) + 3;

			if (t->GetType() == TileType::Pellet) {
                m_drawManagerwPtr->DrawPixel(posx, posy, 0xfdd5);
                m_drawManagerwPtr->DrawPixel(posx + 1, posy, 0xfdd5);
                m_drawManagerwPtr->DrawPixel(posx, posy + 1, 0xfdd5);
                m_drawManagerwPtr->DrawPixel(posx + 1, posy + 1, 0xfdd5);
			}
		}
	}
}

Tile* Level::GetTile(Vect2 p_vect2)
{
	return GetTile(int(p_vect2.x), int(p_vect2.y));
}

Tile* Level::GetTile(int p_x, int p_y)
{
	int x = p_x;
	int y = p_y;

	if (x >= Config::LEVEL_WIDTH)
		x -= Config::LEVEL_WIDTH;
	if (x < 0)
		x += Config::LEVEL_WIDTH;

	if (y >= Config::LEVEL_HEIGHT)
		y -= Config::LEVEL_HEIGHT;
	if (y < 0)
		y += Config::LEVEL_HEIGHT;
	return m_tiles[y][x];
}

void Level::EatTile(Tile* p_tile)
{
	if (p_tile->GetType() == TileType::Pellet
		//|| oldTile->GetType() == TileType::Powerup
		)
	{
		m_pelletsCount--;

		p_tile->ChangeType(Empty);
	}
}

bool Level::PelletCollision(Player * p_player)
{
	Vect2 pos = p_player->GetPos()->Round().CorrectTeleport();

	std::vector<Tile*> checkTiles;
	checkTiles.push_back(GetTile(pos));
	checkTiles.push_back(GetTile(pos + p_player->GetDirection()));

	for (unsigned int i = 0; i < checkTiles.size(); i++) {
		Tile* tile = checkTiles[i];

		if (tile->GetType() == Pellet && CollisionManager::CheckCollision(p_player, checkTiles[i])) {

			EatTile(tile);
			return true;
		}
	}

	return false;
}

bool Level::PowerUpCollision(ICollideable * p_other)
{
	bool collided = false;

	for (unsigned int i = 0; i < m_powerUps.size(); i++) {
		PowerUp* powerup = m_powerUps[i];

		if (CollisionManager::CheckCollision(powerup, p_other)) {
			collided = true;

			delete powerup;
			m_powerUps[i] = nullptr;

			auto it = m_powerUps.begin();
			m_powerUps.erase(it + i);
			i--;
		}
	}

	return collided;
}

std::vector<Vect2> Level::AvailableDirections(const Vect2& p_pos)
{
	std::vector<Vect2> ret;
	bool up = true;
	bool down = true;
	bool left = true;
	bool right = true;

	Vect2 onTile = p_pos.Round();
	Vect2 offset = p_pos.GetOffset();

	if (GetTile(Vect2(onTile.x, onTile.y - 1))->collision()) {
		if (offset.y > 0)
			up = true;
		else
			up = false;
	}

	if (GetTile(Vect2(onTile.x, onTile.y + 1))->collision()) {
		if (offset.y < 0)
			down = true;
		else
			down = false;
	}

	if (GetTile(Vect2(onTile.x - 1, onTile.y))->collision()) {
		if (offset.x > 0)
			left = true;
		else
			left = false;
	}

	if (GetTile(Vect2(onTile.x + 1, onTile.y))->collision()) {
		if (offset.x < 0)
			right = true;
		else
			right = false;
	}

	if (up) {
		ret.push_back(Vect2::UP);
	}
	if (down) {
		ret.push_back(Vect2::DOWN);
	}
	if (left) {
		ret.push_back(Vect2::LEFT);
	}
	if (right) {
		ret.push_back(Vect2::RIGHT);
	}

	return ret;
}

std::vector<Vect2> Level::AvailableDirections(const int& p_x, const int& p_y)
{
	std::vector<Vect2> ret;

	if (!GetTile(p_x, p_y - 1)->collision()) {
		ret.push_back(Vect2::UP);
	}
	if (!GetTile(p_x, p_y + 1)->collision()) {
		ret.push_back(Vect2::DOWN);
	}
	if (!GetTile(p_x - 1, p_y)->collision()) {
		ret.push_back(Vect2::LEFT);
	}
	if (!GetTile(p_x + 1, p_y)->collision()) {
		ret.push_back(Vect2::RIGHT);
	}

	return ret;
}

int Level::NextIntersection(Vect2 & p_pos, const Vect2 & p_dir)
{
	Vect2 pos = p_pos.Round().CorrectTeleport();

	if (p_dir == Vect2::ZERO) {
		return -1;
	}

	int steps = 0;
	while (!IsIntersection((int)pos.x, (int)pos.y)) {
		pos += p_dir;
		pos = pos.CorrectTeleport();
		steps++;
	}

	p_pos = pos;

	return steps;
}

int Level::NextIntersection(Vect2 & p_pos, const Vect2 & p_dir, const std::vector<Vect2>& p_banned)
{
	Vect2 pos = p_pos.Round().CorrectTeleport();

	if (p_dir == Vect2::ZERO) {
		return -1;
	}

	int steps = 0;
	while (
		!GetTile((int)pos.x, (int)pos.y)->collision() &&
		!IsIntersection((int)pos.x, (int)pos.y) &&
		!IsBanned(p_pos, p_banned))
	{
		pos += p_dir;
		pos = pos.CorrectTeleport();
		steps++;
	}

	p_pos = pos;
	return steps;
}

bool Level::IsIntersection(int p_x, int p_y) const
{
	int x = CorrectTeleportAxis(p_x, Config::LEVEL_WIDTH);
	int y = CorrectTeleportAxis(p_y, Config::LEVEL_HEIGHT);

	bool diry = false;
	bool dirx = false;

	if (!m_tiles[CorrectTeleportAxis(y - 1, Config::LEVEL_HEIGHT)][x]->collision() ||
		!m_tiles[CorrectTeleportAxis(y + 1, Config::LEVEL_HEIGHT)][x]->collision())
	{
		diry = true;
	}
	if (!m_tiles[y][CorrectTeleportAxis(x - 1, Config::LEVEL_WIDTH)]->collision() ||
		!m_tiles[y][CorrectTeleportAxis(x + 1, Config::LEVEL_WIDTH)]->collision())
	{
		dirx = true;
	}

	return dirx && diry;
}

int Level::PelletsCount() const
{
	return m_pelletsCount + m_powerUps.size();
}

void Level::ResetAnimation()
{
	for (auto p : m_powerUps) {
		p->ResetAnimation();
	}
}

void Level::Update(float p_delta)
{
	for (auto p : m_powerUps) {
		p->Update(p_delta);
	}
}

#define WALL_3X3_INDEX(x,y)     ((y*3)+x)
#define IS_WALL(x,y)            (wall3x3[WALL_3X3_INDEX(x,y)])
#define IS_NOT_WALL(x,y)        (!(IS_WALL(x,y)))
void Level::DrawWall(const int &p_x, const int &p_y, bool wall3x3[9], uint16_t color16)
{
    if ( !wall3x3[4] )
    {
        return;
    }

    // ??|??|??
    // --+--+--
    // ??|ww|??
    // --+--+--
    // ??|??|??

    // TOP
    if ( IS_WALL(1, 0) )
    {
        // ??|ww|??
        // --+--+--
        // ??|ww|??
        // --+--+--
        // ??|??|??
        if ( IS_NOT_WALL(0, 0) )
        {
            //   |ww|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|??|??
            m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 0, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 1, color16);
            if ( IS_NOT_WALL(0, 1) )
            {
                //   |ww|??
                // --+--+--
                //   |ww|??
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 2, color16);
            }
        }
        else
        {
            // ww|ww|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|??|??
            if ( IS_NOT_WALL(0, 1) )
            {
                // ww|ww|??
                // --+--+--
                //   |ww|??
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 0, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 1, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 2, color16);
            }
        }
        if ( IS_NOT_WALL(2, 0) )
        {
            // ??|ww|  
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|??|??
            m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 0, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 1, color16);
            if ( IS_NOT_WALL(2, 1) )
            {
                // ??|ww|  
                // --+--+--
                // ??|ww|  
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 2, color16);
            }
        }
        else
        {
            // ??|ww|ww
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|??|??
            if ( IS_NOT_WALL(2, 1) )
            {
                // ??|ww|ww
                // --+--+--
                // ??|ww|  
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 0, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 1, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 2, color16);
            }
        }
    }
    else
    {
        // ??|  |??
        // --+--+--
        // ??|ww|??
        // --+--+--
        // ??|??|??
        m_drawManagerwPtr->DrawPixel(p_x + 3, p_y + 2, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 4, p_y + 2, color16);
    }

    // LEFT
    if ( IS_WALL(0, 1) )
    {
        // ??|??|??
        // --+--+--
        // ww|ww|??
        // --+--+--
        // ??|??|??
        if ( IS_NOT_WALL(0, 0) )
        {
            //   |??|??
            // --+--+--
            // ww|ww|??
            // --+--+--
            // ??|??|??
            m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 2, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 2, color16);
            if ( IS_NOT_WALL(1, 0) )
            {
                //   |  |??
                // --+--+--
                // ww|ww|??
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 2, color16);
            }
        }
        else
        {
            // ww|??|??
            // --+--+--
            // ww|ww|??
            // --+--+--
            // ??|??|??
            if ( IS_NOT_WALL(1, 0) )
            {
                // ww|  |??
                // --+--+--
                // ww|ww|??
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 2, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 2, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 2, color16);
            }
        }
        if ( IS_NOT_WALL(0, 2) )
        {
            // ??|??|??
            // --+--+--
            // ww|ww|??
            // --+--+--
            //   |??|??
            m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 5, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 5, color16);
            if ( IS_NOT_WALL(1, 2) )
            {
                // ??|??|??
                // --+--+--
                // ww|ww|??
                // --+--+--
                //   |  |??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 5, color16);
            }
        }
        else
        {
            // ??|??|??
            // --+--+--
            // ww|ww|??
            // --+--+--
            // ww|??|??
            if ( IS_NOT_WALL(1, 2) )
            {
                // ??|??|??
                // --+--+--
                // ww|ww|??
                // --+--+--
                // ww|  |??
                m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 5, color16);
            }
        }
    }
    else
    {
        // ??|??|??
        // --+--+--
        //   |ww|??
        // --+--+--
        // ??|??|??
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 3, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 4, color16);
    }

    // BOTTOM
    if ( IS_WALL(1, 2) )
    {
        // ??|??|??
        // --+--+--
        // ??|ww|??
        // --+--+--
        // ??|ww|??
        if ( IS_NOT_WALL(0, 2) )
        {
            // ??|??|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            //   |ww|??
            m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 6, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 7, color16);
            if ( IS_NOT_WALL(0, 1) )
            {
                // ??|??|??
                // --+--+--
                //   |ww|??
                // --+--+--
                //   |ww|??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 5, color16);
            }
        }
        else
        {
            // ??|??|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ww|ww|??
            if ( IS_NOT_WALL(0, 1) )
            {
                // ??|??|??
                // --+--+--
                //   |ww|??
                // --+--+--
                // ww|ww|??
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 6, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 7, color16);
            }
        }
        if ( IS_NOT_WALL(2, 2) )
        {
            // ??|??|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|ww|  
            m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 6, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 7, color16);
            if ( IS_NOT_WALL(2, 1) )
            {
                // ??|??|??
                // --+--+--
                // ??|ww|  
                // --+--+--
                // ??|ww|  
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 5, color16);
            }
        }
        else
        {
            // ??|??|??
            // --+--+--
            // ??|ww|??
            // --+--+--
            // ??|ww|ww
            if ( IS_NOT_WALL(2, 1) )
            {
                // ??|??|??
                // --+--+--
                // ??|ww|  
                // --+--+--
                // ??|ww|ww
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 6, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 7, color16);
            }
        }
    }
    else
    {
        // ??|??|??
        // --+--+--
        // ??|ww|??
        // --+--+--
        // ??|  |??
        m_drawManagerwPtr->DrawPixel(p_x + 3, p_y + 5, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 4, p_y + 5, color16);
    }

    // RIGHT
    if ( IS_WALL(2, 1) )
    {
        // ??|??|??
        // --+--+--
        // ??|ww|ww
        // --+--+--
        // ??|??|??
        if ( IS_NOT_WALL(2, 0) )
        {
            // ??|??|  
            // --+--+--
            // ??|ww|ww
            // --+--+--
            // ??|??|??
            m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 2, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 2, color16);
            if ( IS_NOT_WALL(1, 0) )
            {
                // ??|  |  
                // --+--+--
                // ??|ww|ww
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 2, color16);
            }
        }
        else
        {
            // ??|??|ww
            // --+--+--
            // ??|ww|ww
            // --+--+--
            // ??|??|??
            if ( IS_NOT_WALL(1, 0) )
            {
                // ??|  |ww
                // --+--+--
                // ??|ww|ww
                // --+--+--
                // ??|??|??
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 2, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 2, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 2, color16);
            }
        }
        if ( IS_NOT_WALL(2, 2) )
        {
            // ??|??|??
            // --+--+--
            // ??|ww|ww
            // --+--+--
            // ??|??|  
            m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 5, color16);
            m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 5, color16);
            if ( IS_NOT_WALL(1, 2) )
            {
                // ??|??|??
                // --+--+--
                // ??|ww|ww
                // --+--+--
                // ??|  |  
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 5, color16);
            }
        }
        else
        {
            // ??|??|??
            // --+--+--
            // ??|ww|ww
            // --+--+--
            // ??|??|ww
            if ( IS_NOT_WALL(1, 2) )
            {
                // ??|??|??
                // --+--+--
                // ??|ww|ww
                // --+--+--
                // ??|  |ww
                m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 5, color16);
                m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 5, color16);
            }
        }
    }
    else
    {
        // ??|??|??
        // --+--+--
        // ??|ww|  
        // --+--+--
        // ??|??|??
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 3, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 4, color16);
    }

    return;

    // Top row of vertical lines
    if ( IS_WALL(0, 0) != IS_WALL(1, 0) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 0, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 1, color16);
    }
    if ( IS_WALL(1, 0) != IS_WALL(2, 0) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 0, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 1, color16);
    }

    // Middle row of vertical lines
    if ( IS_WALL(0, 1) != IS_WALL(1, 1) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 3, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 4, color16);
    }
    if ( IS_WALL(1, 1) != IS_WALL(2, 1) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 3, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 4, color16);
    }

    // Bottom row of vertical lines
    if ( IS_WALL(0, 2) != IS_WALL(1, 2) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 6, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 2, p_y + 7, color16);
    }
    if ( IS_WALL(1, 2) != IS_WALL(2, 2) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 6, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 5, p_y + 7, color16);
    }

    // First column of horizontal lines
    if ( IS_WALL(0, 0) != IS_WALL(0, 1) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 2, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 2, color16);
    }
    if ( IS_WALL(0, 1) != IS_WALL(0, 2) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 0, p_y + 5, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 1, p_y + 5, color16);
    }

    // Second column of horizontal lines
    if ( IS_WALL(1, 0) != IS_WALL(1, 1) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 3, p_y + 2, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 4, p_y + 2, color16);
    }
    if ( IS_WALL(1, 1) != IS_WALL(1, 2) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 3, p_y + 5, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 4, p_y + 5, color16);
    }

    // Third column of horizontal lines
    if ( IS_WALL(2, 0) != IS_WALL(2, 1) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 2, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 2, color16);
    }
    if ( IS_WALL(2, 1) != IS_WALL(2, 2) )
    {
        m_drawManagerwPtr->DrawPixel(p_x + 6, p_y + 5, color16);
        m_drawManagerwPtr->DrawPixel(p_x + 7, p_y + 5, color16);
    }

    /*
    for ( int dy = 0; dy < 8; dy++ )
    {
        for ( int dx = 0; dx < 8; dx++ )
        {
            m_drawManagerwPtr->DrawPixel(p_x + dx, p_y + dy, color16);
        }
    }
    */
}

void Level::DrawLevel(const int &p_x, const int &p_y, uint16_t color16)
{
    bool wall3x3[9];
    int rows = Config::LEVEL_HEIGHT;
    int columns = Config::LEVEL_WIDTH;
    for ( int y = 0; y < rows; y++ )
    {
        for ( int x = 0; x < columns; x++ )
        {
            wall3x3[4] = false;
            if ( m_tiles[y][x]->GetType() == TileType::Wall )
            {
                for ( int dy = -1; dy <= 1; dy++ )
                {
                    for ( int dx = -1; dx <= 1; dx++ )
                    {
                        int i = ((dy + 1) * 3) + (dx + 1);
                        wall3x3[i] = false;
                        if ( ((y + dy) >= 0 ) && ((y + dy) < rows ) && ((x + dx) >= 0 ) && ((x + dx) < columns ) )
                        {
                            if ( m_tiles[y + dy][x + dx]->GetType() == TileType::Wall )
                            {
                                wall3x3[i] = true;
                            }
                        }
                    }
                }
            }
            DrawWall(p_x + (x * 8), p_y + (y * 8), wall3x3, color16);
        }
    }
}

void Level::Draw()
{
	//m_drawManagerwPtr->Draw(m_levelSpritewPtr, 0, 24);
    DrawLevel(0, 24, 0x001f);

	DrawPellets(0, 24);

	for (auto p : m_powerUps) {
		p->Draw();
	}
}

}; // namespace pacman
