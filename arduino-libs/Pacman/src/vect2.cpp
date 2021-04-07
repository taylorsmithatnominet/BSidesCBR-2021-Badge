#include <math.h>

#include "vect2.h"

#include "Config.h"

namespace pacman {

const Vect2 Vect2::ZERO = Vect2(0, 0);
const Vect2 Vect2::UP = Vect2(0, -1);
const Vect2 Vect2::DOWN = Vect2(0, 1);
const Vect2 Vect2::LEFT = Vect2(-1, 0);
const Vect2 Vect2::RIGHT = Vect2(1, 0);

Vect2::Vect2()
{
}

Vect2::Vect2(float p_x, float p_y) : x(p_x), y(p_y)
{
}


Vect2::~Vect2()
{
}

Vect2 Vect2::operator+=(const Vect2 & p_other)
{
	x += p_other.x;
	y += p_other.y;
	return *this; // return the result by reference
}

Vect2 Vect2::operator-=(const Vect2 & p_other)
{
	x -= p_other.x;
	y -= p_other.y;
	return *this;
}

Vect2 Vect2::operator+(const Vect2 & p_other)
{
	return Vect2(p_other.x + x, p_other.y + y);
}

Vect2 Vect2::operator-(const Vect2 & p_other)
{
	return Vect2(p_other.x - x, p_other.y - y);
}

Vect2 Vect2::Round() const
{
    return Vect2(round(x), round(y));
}

Vect2 Vect2::Floor() const
{
    return Vect2(float(floor(x)), float(floor(y)));
}

Vect2 Vect2::GetOffset() const
{
    Vect2 floor = Floor();
    return Vect2(x - floor.x, y - floor.y);
}

Vect2 Vect2::CorrectTeleport() const
{
	Vect2 ret = *this;

	if (ret.x < 0)
		ret.x += Config::LEVEL_WIDTH;
	if (ret.x >= Config::LEVEL_WIDTH)
		ret.x -= Config::LEVEL_WIDTH;

	return ret;
}

bool operator==(const Vect2& p_lhs, const Vect2& p_rhs) {
	return p_lhs.x == p_rhs.x && p_lhs.y == p_rhs.y;
}
bool operator!=(const Vect2& p_lhs, const Vect2& p_rhs) {
	return !(p_lhs == p_rhs);
}
bool operator<(const Vect2 & p_l, const Vect2 & p_r)
{
	if (p_l.x == p_r.x) {
		if (p_l.y == p_r.y) {
			return false;
		}

		return p_l.y < p_r.y;
	}

	return p_l.x < p_r.x;
}


void Vect2::GotoX(float p_x, float p_speed)
{
	if (x < p_x) {
		x += p_speed;

		if (x >= p_x)
			x = p_x;
	}

	if (x > p_x) {
		x -= p_speed;

		if (x <= p_x)
			x = p_x;
	}
}

void Vect2::GotoY(float p_y, float p_speed)
{
	if (y < p_y) {
		y += p_speed;

		if (y > p_y)
			y = p_y;
	}

	if (y > p_y) {
		y -= p_speed;

		if (y < p_y)
			y = p_y;
	}
}

}; // namespace pacman
