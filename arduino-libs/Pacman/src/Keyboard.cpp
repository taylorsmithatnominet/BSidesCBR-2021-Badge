#include "Keyboard.h"

namespace pacman {

Keyboard::Keyboard() : m_keys{}, m_keysPrevious{}
{
}


Keyboard::~Keyboard()
{
}

void Keyboard::SetKey(uint8_t p_key, bool p_value)
{
	m_keys[p_key] = p_value;
}

bool Keyboard::GetKeyDown(uint8_t p_key)
{
	return m_keys[p_key];
}

bool Keyboard::GetKeyDownOnce(uint8_t p_key)
{
	return (m_keysPrevious[p_key] == false && m_keys[p_key] == true);
}

void Keyboard::Update()
{
	for (int i = 0; i < PM_NUM_SCANCODES; i++)
	{
		m_keysPrevious[i] = m_keys[i];
	}
}

}; // namespace pacman
