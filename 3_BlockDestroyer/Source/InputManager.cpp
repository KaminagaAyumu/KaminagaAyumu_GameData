#include "InputManager.h"

InputManager& InputManager::Instance()
{
    static InputManager instance;
    return instance;
}

InputManager::InputManager():
    m_keyNow{},
	m_keyPrev{},
	m_mouseNow(0),
	m_mousePrev(0),
	m_padNow(0),
	m_padPrev(0)
{
}

void InputManager::Update()
{
	m_keyPrev = m_keyNow;
	GetHitKeyStateAll(m_keyNow.data());

	m_mousePrev = m_mouseNow;
	m_mouseNow = GetMouseInput();

	m_padPrev = m_padNow;
	m_padNow = GetJoypadInputState(DX_INPUT_PAD1);
}

bool InputManager::GetKeyDown(int keyCode) const
{
    return m_keyNow[keyCode] && !m_keyPrev[keyCode];
}

bool InputManager::GetKey(int keyCode) const
{
	return m_keyNow[keyCode];
}

bool InputManager::GetKeyUp(int keyCode) const
{
	return !m_keyNow[keyCode] && m_keyPrev[keyCode];
}

bool InputManager::GetMouseDown(int button) const
{
	return (m_mouseNow & button) && !(m_mousePrev & button);
}

bool InputManager::GetMouse(int button) const
{
	return (m_mouseNow & button);
}

bool InputManager::GetMouseUp(int button) const
{
	return !(m_mouseNow & button) && (m_mousePrev & button);
}

bool InputManager::GetPadDown(int button) const
{
	return (m_padNow & button) && !(m_padPrev & button);
}

bool InputManager::GetPad(int button) const
{
	return (m_padNow & button);
}

bool InputManager::GetPadUp(int button) const
{
	return !(m_padNow & button) && (m_padPrev & button);
}
