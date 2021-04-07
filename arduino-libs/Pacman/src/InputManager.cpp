#include "InputManager.h"
#include "Keyboard.h"

namespace pacman {

InputManager::InputManager()
{
	m_keyboard = nullptr;
}

InputManager::~InputManager()
{
}

void InputManager::Initialize()
{
	m_keyboard = new Keyboard();
}

void InputManager::Shutdown()
{
	delete m_keyboard;
	m_keyboard = nullptr;
}

bool InputManager::IsKeyDown(uint8_t p_key)
{
	return m_keyboard->GetKeyDown(p_key);
}

bool InputManager::IsKeyDownOnce(uint8_t p_key)
{
	return m_keyboard->GetKeyDownOnce(p_key);
}

void InputManager::KeyEvent(uint8_t p_key, bool p_key_down)
{
	m_keyboard->SetKey(p_key, p_key_down);
}

void InputManager::Update()
{
	m_keyboard->Update();
}

}; // namespace pacman
