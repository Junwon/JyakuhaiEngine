#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/IntVector2.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const int NUM_KEYBOARD_KEYS = 256;
const int MAX_XBOX_CONTROLLERS = 4;
const float HELD_DOWN_DURATION = 0.8f;

enum KeyCode : unsigned char
{
	KEYCODE_BACKSPACE = 0x08,
	KEYCODE_CAPITAL = 0x14,
	KEYCODE_EMPTY = 0x00,
	KEYCODE_ESCAPE = 0x1B,
	KEYCODE_ENTER = 0x0D,
	KEYCODE_PAUSE = 0x13,
	KEYCODE_SHIFT = 0x10,
	KEYCODE_TAB = 0x09,
	KEYCODE_F1 = 0X70,
	KEYCODE_F2 = 0x71,
	KEYCODE_F3 = 0x72,
	KEYCODE_F4 = 0x73,
	KEYCODE_F5 = 0x74,
	KEYCODE_F6 = 0x75,
	KEYCODE_F7 = 0x76,
	KEYCODE_F8 = 0x77,
	KEYCODE_F9 = 0x78,
	KEYCODE_F10 = 0x79,
	KEYCODE_F11 = 0x7A,
	KEYCODE_F12 = 0x7B,
	KEYCODE_SPACE = 0x20,
	KEYCODE_UP = 0x26,
	KEYCODE_RIGHT = 0x27,
	KEYCODE_DOWN = 0x28,
	KEYCODE_LEFT = 0x25,
	KEYCODE_PLUS = 0xBB,
	KEYCODE_MINUS = 0xBD,
	KEYCODE_TILDE = 0xC0,
	MOUSECODE_LEFT = 0x01,	//0x0201,
	MOUSECODE_RIGHT = 0x02, //0x0204,
};

struct KeyState 
{
	bool	m_isDown;
	bool	m_justChanged;
};

IntVector2 GetCursorPositionRelativeToWindowHandler(const HWND& hwnd);
bool IsCursorInClientArea(const HWND& hwnd);
bool IsCursorInClientArea(const HWND& hwnd, IntVector2& outPoint);

class InputSystem {
public:
	InputSystem();
	~InputSystem();

	static InputSystem* GetInstance();
	void Update(float deltaSeconds);
	void RegisterKeyDown(unsigned char keyIndex);
	void RegisterKeyUp( unsigned char keyIndex );
	void RegisterMouseWheelDelta(short delta);

	short GetMouseWheelDelta() const;
	const XboxController& GetControllerState(int controllerID) const;
	bool IsKeyDown(unsigned char keyIndex) const;
	void BeginFrame();
	void EndFrame();
	void SetWindowHandler(HWND* handler);
	bool WasKeyJustPressed(unsigned char keyIndex) const;
	unsigned char GetKeysPressed() const;
	void SetCursorScreenPos(int x, int y);
	void SetCursorScreenPos(const IntVector2& mouseCursorScreenPos);
	void ShowMouseCursor(bool show);
	IntVector2 GetCursorWindowPos() const;
	IntVector2 GetRawCursorScreenPos() const;
	IntVector2 GetScreenCenter() const;

	unsigned char CheckIfCapLock(unsigned char keyIndex);

private:
	KeyState				m_keyStates[NUM_KEYBOARD_KEYS];
	XboxController			m_xboxControllers[MAX_XBOX_CONTROLLERS];
	short					m_mouseWheelDelta;
	unsigned char			m_lastKeyPressed;
	bool					m_isCapLock;
	bool					m_aKeyIsHeldDown;

	float					m_heldDownDuration;
	HWND*					m_windowHandler;
	static InputSystem*		s_instance;
};

inline void InputSystem::RegisterKeyDown(unsigned char keyIndex)
{
	KeyState& keyState = m_keyStates[keyIndex];

	bool wasDown = keyState.m_isDown;
	bool isDown = true;
	bool justChanged = (wasDown != isDown);

	keyState.m_isDown = isDown;
	keyState.m_justChanged = justChanged;

	m_lastKeyPressed = keyIndex;

	if (keyIndex == KEYCODE_SHIFT)
	{
		m_isCapLock = !m_isCapLock;
	}
}

inline void InputSystem::RegisterKeyUp(unsigned char keyIndex)
{
	KeyState& keyState = m_keyStates[keyIndex];

	bool wasDown = keyState.m_isDown;
	bool isDown = false;
	bool justChanged = (wasDown != isDown);

	keyState.m_isDown = isDown;
	keyState.m_justChanged = justChanged;

	m_lastKeyPressed = KEYCODE_EMPTY;

	if (keyIndex == KEYCODE_CAPITAL) {
		m_isCapLock = !m_isCapLock;
	}

	if (keyIndex == KEYCODE_SHIFT) {
		m_isCapLock = !m_isCapLock;
	}
}

inline void InputSystem::RegisterMouseWheelDelta(short delta)
{
	m_mouseWheelDelta = delta;
}

inline short InputSystem::GetMouseWheelDelta() const
{
	return m_mouseWheelDelta;
}

inline const XboxController& InputSystem::GetControllerState(int controllerID) const
{
	return m_xboxControllers[controllerID];
}

inline bool InputSystem::IsKeyDown(unsigned char keyIndex) const
{
	return m_keyStates[keyIndex].m_isDown;
}

inline bool InputSystem::WasKeyJustPressed(unsigned char keyIndex) const
{
	return IsKeyDown(keyIndex) && m_keyStates[keyIndex].m_justChanged;
}

inline unsigned char InputSystem::GetKeysPressed() const
{
	unsigned char key = KEYCODE_EMPTY;
	 
	if (WasKeyJustPressed(m_lastKeyPressed)) {
		key = m_lastKeyPressed;
	}
	
	return key;
}