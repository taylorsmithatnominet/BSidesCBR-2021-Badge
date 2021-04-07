#pragma once

namespace pacman {

class IEntity
{
public:
	IEntity();
	~IEntity();
	virtual void Update(float p_delta) = 0;
protected:
	int m_x;
	int m_y;
};

}; // namespace pacman
