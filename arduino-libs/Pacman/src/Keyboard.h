#pragma once

#include <cstdint>

namespace pacman {

#define PM_SCANCODE_NULL       0
#define PM_SCANCODE_ENTER      1
#define PM_SCANCODE_LEFT       2
#define PM_SCANCODE_UP         3
#define PM_SCANCODE_RIGHT      4
#define PM_SCANCODE_DOWN       5
#define PM_NUM_SCANCODES       (PM_SCANCODE_DOWN+1)

class Keyboard
{
public:
	Keyboard();
	~Keyboard();
	void SetKey(uint8_t p_ey, bool p_value);
	bool GetKeyDown(uint8_t p_key);
	bool GetKeyDownOnce(uint8_t p_key);
	void Update();
private:
	bool m_keys[PM_NUM_SCANCODES];
	bool m_keysPrevious[PM_NUM_SCANCODES];
};

}; // namespace pacman
