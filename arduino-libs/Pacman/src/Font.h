#pragma once

#include <cstdint>

namespace pacman {

class Vect2;
class DrawManager;

class Font
{
private:
	DrawManager* m_drawManagerwPtr;
	static const int SIZE;
public:
	Font();
	~Font();
	
	int GetLength(const char *p_string);
	void Draw(Vect2 p_pos, const char *p_string);
	void DrawCentered(Vect2 p_pos, const char *p_string);
	void DrawLeftAnchor(Vect2 p_pos, const char *p_string);

	void Draw(Vect2 p_pos, const char *p_string, uint16_t p_color);
	void DrawCentered(Vect2 p_pos, const char *p_string, uint16_t p_color);
	void DrawLeftAnchor(Vect2 p_pos, const char *p_string, uint16_t p_color);
};

}; // namespace pacman
