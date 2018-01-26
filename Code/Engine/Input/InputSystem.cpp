#include "Engine/Input/InputSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

InputSystem* InputSystem::s_instance = nullptr;

InputSystem::InputSystem()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; ++keyIndex) {
		m_keyStates[keyIndex].m_isDown = false;
		m_keyStates[keyIndex].m_justChanged = false;
	}

	for (int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex) {
		m_xboxControllers[controllerIndex] = XboxController(false, controllerIndex);
	}

	m_mouseWheelDelta = 0;
	m_isCapLock = false;
	m_aKeyIsHeldDown = false;
	m_heldDownDuration = HELD_DOWN_DURATION;
	m_windowHandler = nullptr;

	s_instance = this;
}

InputSystem::~InputSystem()
{
	s_instance = nullptr;
}

InputSystem* InputSystem::GetInstance()
{
	return s_instance;
}

void InputSystem::Update(float deltaSeconds)
{
	if (IsKeyDown(m_lastKeyPressed) && m_heldDownDuration > 0) {
		m_heldDownDuration -= deltaSeconds;
	}

	if (m_heldDownDuration <= 0) {
		m_aKeyIsHeldDown = true;
	}
}

void InputSystem::BeginFrame() 
{
	for (int xboxControllerIndex = 0; xboxControllerIndex < MAX_XBOX_CONTROLLERS; ++xboxControllerIndex) {
		m_xboxControllers[xboxControllerIndex].Update();
	}
}

void InputSystem::EndFrame() 
{
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; ++keyIndex) {
		m_keyStates[keyIndex].m_justChanged = false;
	}

	m_mouseWheelDelta = 0;
}

void InputSystem::SetWindowHandler(HWND* handler)
{
	m_windowHandler = handler;
}

void InputSystem::SetCursorScreenPos(int x, int y)
{
	SetCursorPos(x, y);
}

void InputSystem::SetCursorScreenPos(const IntVector2& mouseCursorScreenPos)
{
	SetCursorPos(mouseCursorScreenPos.x, mouseCursorScreenPos.y);
}

IntVector2 GetCursorPositionRelativeToWindowHandler(const HWND& hwnd)
{
	POINT rawCursorPos;
	GetCursorPos(&rawCursorPos);
	ScreenToClient(hwnd, &rawCursorPos);

	return IntVector2(rawCursorPos.x, rawCursorPos.y);
}

bool IsCursorInClientArea(const HWND& hwnd)
{
	POINT point;
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	GetCursorPos(&point);

	if (PtInRect(&windowRect, point) == 0) {
		return false;
	}

	ScreenToClient(hwnd, &point);

	if (point.x < 0 || point.y < 0) {
		return false;
	}

	return true;
}

bool IsCursorInClientArea(const HWND& hwnd, IntVector2& outPoint)
{
	POINT point;
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	GetCursorPos(&point);

	if (PtInRect(&windowRect, point) == 0) {
		return false;
	}

	ScreenToClient(hwnd, &point);

	if (point.x < 0 || point.y < 0) {
		return false;
	}

	outPoint = IntVector2(point.x, point.y);
	return true;
}

void InputSystem::ShowMouseCursor(bool show)
{
	if (show) {
		int currentShowCount = ShowCursor(TRUE);

		while (currentShowCount < 0) {
			currentShowCount = ShowCursor(TRUE);
		}
	}
	else {
		int currentShowCount = ShowCursor(FALSE);

		while (currentShowCount >= 0) {
			currentShowCount = ShowCursor(FALSE);
		}
	}
}

IntVector2 InputSystem::GetCursorWindowPos() const
{
	ASSERT_OR_DIE(m_windowHandler, "InputSystem screen handler is null.");
	
	IntVector2 returnValue = IntVector2(-1, -1);

	POINT cursorPos;
	if (GetCursorPos(&cursorPos)) {
		ScreenToClient(*m_windowHandler, &cursorPos);
		returnValue = IntVector2(cursorPos.x, cursorPos.y);
	}

	return returnValue;
}

IntVector2 InputSystem::GetRawCursorScreenPos() const
{
	IntVector2 returnValue = IntVector2(-1, -1);

	POINT rawCursorPos;
	if (GetCursorPos(&rawCursorPos)) {
		returnValue = IntVector2(rawCursorPos.x, rawCursorPos.y);
	}

	return returnValue;
}

IntVector2 InputSystem::GetScreenCenter() const
{
	RECT desktopRect;

	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	IntVector2 screenCenter;
	screenCenter.x = (int)((desktopRect.right + desktopRect.left) * 0.5f);
	screenCenter.y = (int)((desktopRect.bottom + desktopRect.top) * 0.5f);
	
	return screenCenter;
}

unsigned char InputSystem::CheckIfCapLock(unsigned char keyIndex)
{
	if (!m_isCapLock) {
		return (unsigned char)tolower(keyIndex);
	}

	return keyIndex;
}

