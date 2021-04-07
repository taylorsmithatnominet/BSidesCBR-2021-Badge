#include <cstring>

#include "Font.h"

#include "vect2.h"

#include "ServiceLocator.h"
#include "DrawManager.h"
#include "Color.h"

namespace pacman {

const int Font::SIZE = 8;

Font::Font()
{
	m_drawManagerwPtr = ServiceLocator<DrawManager>::GetService();
}

Font::~Font()
{
}

int Font::GetLength(const char *p_string)
{
	return strlen(p_string) * SIZE; // TODO
}

void Font::Draw(Vect2 p_pos, const char *p_string)
{
	Draw(p_pos, p_string, PM_COLOR16_FROM_RGB(255, 255, 255));
}

void Font::DrawCentered(Vect2 p_pos, const char *p_string)
{
	DrawCentered(p_pos, p_string, PM_COLOR16_FROM_RGB(255, 255, 255));
}

void Font::DrawLeftAnchor(Vect2 p_pos, const char *p_string)
{
	DrawLeftAnchor(p_pos, p_string, PM_COLOR16_FROM_RGB(255, 255, 255));
}

void Font::Draw(Vect2 p_pos, const char *p_string, uint16_t p_color)
{
    m_drawManagerwPtr->DrawString(p_pos.x, p_pos.y, p_string, p_color);
}

void Font::DrawCentered(Vect2 p_pos, const char *p_string, uint16_t p_color)
{
	int length = GetLength(p_string);

	Vect2 pos = p_pos;
	pos.x -= length / 2;

	Draw(pos, p_string, p_color);
}

void Font::DrawLeftAnchor(Vect2 p_pos, const char *p_string, uint16_t p_color)
{
	Vect2 pos = p_pos;

	int offset = GetLength(p_string);
	pos.x -= offset;

	Draw(pos, p_string, p_color);
}

}; // namespace pacman
