#include "Engine/Core/Window.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Input/InputSystem.hpp"

LPCWSTR WNDCLASS_GAME_NAME = L"GameWindowClass";

static unsigned int gWindowSystemCount = 0U;

Window* GetWindowFromHandle(HWND windowHandle)
{
	return (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
}

static LRESULT CALLBACK GameWindowMessageHandlingProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Custom Handling
	Window* window = GetWindowFromHandle(hwnd);
	if (window && window->m_windowsMessageCallback) 
	{
		bool allowDefaultProcessing = window->m_windowsMessageCallback(hwnd, msg, wParam, lParam);
		if (!allowDefaultProcessing) 
		{
			return 0;
		}
	}

	// Engine Handling
	switch (msg)
	{
		// Store the Window pointer itself within the userdata for future use
		case WM_CREATE:
		{
			CREATESTRUCT* cp = (CREATESTRUCT*)lParam;
			window = (Window*)cp->lpCreateParams;
			window->m_windowHandle = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
			if (window->m_windowsMessageCallback) {
				bool allowDefaultProcessing = window->m_windowsMessageCallback(hwnd, msg, wParam, lParam);
				if (!allowDefaultProcessing) {
					return 0;
				}
			}
		} 
		break;

	}

	// Default Handling
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window() :
	m_isOpened(false),
	m_isVisible(false),
	m_isRunning(true),
	title("")
{
	m_windowsMessageCallback = nullptr;
	RegisterGameWindowClass();
}

Window::~Window()
{
	UnregisterGameWindowClass();
}

static HINSTANCE GetCurrentHINSTANCE()
{
	return GetModuleHandle(NULL);
}

static RECT WindowGetRect(const IntVector2& res, DWORD style)
{
	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = res.x;
	windowRect.bottom = res.y;

	AdjustWindowRect(&windowRect, style, FALSE);

	return windowRect;
}

bool Window::Open()
{
	if (IsOpen())
	{
		return false;
	}

	m_windowHandle = CreateTheWindow();

	if (NULL == m_windowHandle) 
	{
		return false;
	}

	return true;
}

void Window::SetIsRunning(bool running)
{
	m_isRunning = running;
}

bool Window::IsRunning() const
{
	return m_isRunning;
}

void Window::Show(bool activate)
{
	if (m_isOpened && !m_isVisible) 
	{
		int const command = activate ? SW_SHOW : SW_SHOWNOACTIVATE;
		::ShowWindow(m_windowHandle, command);
	}
	else if (m_isOpened)
	{
		m_isOpened = false;
	}
}

void Window::SetTitle(const char* s)
{
	SetWindowTextA(m_windowHandle, s);
	title = s;
}

void Window::SetClientDimension(unsigned int w, unsigned int h)
{
	int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYSCREEN);

	// Allows the screen to be centered
	xPos = (monitorWidth - w) / 2;
	yPos = (monitorHeight - h) / 2;
	m_width = (int)w;
	m_height = (int)h;
	m_resolution = IntVector2(m_width, m_height);
}

void Window::Close()
{
	if (IsOpen()) 
	{
		DestroyWindow(m_windowHandle);
	}
}

bool Window::IsOpen()
{
	return IsWindow(m_windowHandle) != 0;
}

bool RegisterGameWindowClass()
{
	DebuggerPrintlnf("Registering Window Class");

	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));

	// Setup the definition for this window class
	wc.cbSize = sizeof(WNDCLASSEX);

	// This sets that it will redraw for vertical or horizontal changes
	// and it also owns its own device context handle (more effecient if we're
	// drawing to this window a lot - which we will be.
	wc.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC);

	// The Windows Messaeg Pump - handles messages for this window type.
	wc.lpfnWndProc = GameWindowMessageHandlingProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetCurrentHINSTANCE();

	// Default icon for this window (can be set later)
	wc.hIcon = NULL;
	wc.hIconSm = NULL;

	// Cursor to use (can be set later if needed)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)0;
	wc.lpszMenuName = NULL;

	// Name to use when creating windows so it knows to use this class
	wc.lpszClassName = WNDCLASS_GAME_NAME;

	ATOM result = RegisterClassEx(&wc);
	if (NULL == result) {
		DebuggerPrintf("Failed to register window class [ErrNo: %u]", GetLastError());
		return false;
	}

	return true;
}

void UnregisterGameWindowClass()
{
	UnregisterClass(WNDCLASS_GAME_NAME, GetCurrentHINSTANCE());
	//m_isOpened = false;
}

HWND Window::CreateTheWindow(const char* wndTitle)
{
	DWORD extendedStyle = 0;
	DWORD style = 0U;

	// If you want a "window" - this gives it the caption bar, a sysmenu, and a border.
	// Default: WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER
	style |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER);

	// this style will instead make it borderless - good for fullscreen windows.
	// MUTUALLY EXCLUSIVE WITH ABOVE
	// style |= WS_POPUP;

	// Window starts invisible by default - this will allow it to be visible
	style |= WS_VISIBLE;

	RECT winRect = WindowGetRect(m_resolution, style);

	xPos += winRect.left;
	yPos += winRect.top;
	m_width = winRect.right - winRect.left;
	m_height = winRect.bottom - winRect.top;

	if (strcmp(title, wndTitle) != 0)
	{
		title = wndTitle;
	}
	
	HWND handler = CreateWindowEx(
		extendedStyle,
		WNDCLASS_GAME_NAME,
		NULL,
		style,
		xPos, yPos,
		m_width, m_height,
		NULL,
		NULL,
		GetCurrentHINSTANCE(),
		this);

	if (handler == NULL) 
	{
		DWORD error = GetLastError();
		DebuggerPrintf("Failed to create window:  Error[%u]", error);
	}
	else
	{
		m_windowHandle = handler;
	}

	return m_windowHandle;
}

int Window::ProcessMessages()
{
	int messagesProcessed = 0;

	if (NULL != m_windowHandle) 
	{
		// See if this window has a message to process - if so, remove it.
		MSG msg;

		while (::PeekMessage(&msg, m_windowHandle, 0, 0, PM_REMOVE)) 
		{
			++messagesProcessed;

			// Processes virtual key messages into character key messages (and posts them to the queue)
			::TranslateMessage(&msg);

			// Processes this message (sends it to the windows procedue)
			::DispatchMessage(&msg);
		}
	}

	// Not needed, but metrics
	return messagesProcessed;
}

void Window::SetCustomWindowsMessageCallback(WindowsMessageCallback newCallback)
{
	m_windowsMessageCallback = newCallback;
}

IntVector2 Window::GetPosition() const
{
	RECT windowRect;
	GetWindowRect(m_windowHandle, &windowRect);
	return IntVector2(windowRect.left, windowRect.top);
}

void Window::SetClientSize(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	m_resolution = IntVector2(m_width, m_height);
	IntVector2 position = GetPosition();

	SetWindowPos(m_windowHandle, NULL, position.x, position.y, width, height, SWP_FRAMECHANGED);
}

Vector2 Window::GetClientSize()
{
	return Vector2((float)m_width, (float)m_height);
}

