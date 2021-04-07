#include "Tile.h"

#include "Config.h"

namespace pacman {

Tile::Tile()
{
}

Tile::Tile(TileType p_type) : m_type(p_type), ICollideable()
{
}

Tile::Tile(TileType p_type, int p_x, int p_y)
	: Tile(p_type)
{
	SetPos(p_x, p_y);
}

Tile::~Tile()
{
	
}

TileType Tile::GetType() const
{
	return m_type;
}

bool Tile::collision() const
{
	switch (m_type) {
	case TileType::Empty:
	case TileType::Pellet:
	case TileType::Powerup:
		return false;
	case TileType::Wall:
		return true;
	}
	return true;
}

void Tile::ChangeType(TileType p_type)
{
	m_type = p_type;
}

void Tile::SetPos(int p_x, int p_y)
{
	if (GetType() == Pellet) {
		int size = Config::TILE_SIZE;
		int offset = (Config::TILE_SIZE / 2) - (size / 2);

		m_collider->m_rect.x = p_x * Config::TILE_SIZE + offset;
		m_collider->m_rect.y = (p_y + 3) * Config::TILE_SIZE + offset;
		m_collider->m_rect.w = size;
		m_collider->m_rect.h = size;
	}
	else {
		m_collider->m_rect.x = p_x * Config::TILE_SIZE;
		m_collider->m_rect.y = p_y * Config::TILE_SIZE;
		m_collider->m_rect.w = Config::TILE_SIZE;
		m_collider->m_rect.h = Config::TILE_SIZE;
	}
}

}; // namespace pacman
