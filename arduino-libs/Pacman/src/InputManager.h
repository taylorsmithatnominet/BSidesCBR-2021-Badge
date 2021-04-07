#pragma once

#include <cstdint>

namespace pacman {

class Keyboard;

class InputManager
{
public:
	InputManager();
	~InputManager();
	void Initialize();
	void Shutdown();
	bool IsKeyDown(uint8_t p_key);
	bool IsKeyDownOnce(uint8_t p_key);
	void KeyEvent(uint8_t p_key, bool p_key_down);
	void Update();
private:
	Keyboard* m_keyboard;
};

}; // namespace pacman
