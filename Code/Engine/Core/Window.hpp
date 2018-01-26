#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/StringUtils.hpp"

class Vector2;

enum eWindowDisplayMode : UINT
{
	WNDMODE_WINDOWED,    // Default Window 
	WNDMODE_BORDERLESS,  // [OPTIONAL] Window of fixed size with no border.
	WNDMODE_FULLSCREEN,  // [OPTIONAL] Window with no border - clietn size takes up desktop
};

typedef bool(*WindowsMessageCallback)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Window
{
public:
	Window();
	~Window();

	bool Open();
	void SetIsRunning(bool running);
	bool IsRunning() const;
	void Show(bool activate = true);
	void SetTitle(const char* s);
	void SetClientDimension(unsigned int w, unsigned int h);
	void Close();
	bool IsOpen();

	HWND CreateTheWindow(const char* wndTitle = "");
	int ProcessMessages();

	void SetCustomWindowsMessageCallback(WindowsMessageCallback newCallback);
	IntVector2 GetPosition() const;
	void SetClientSize(unsigned int width, unsigned int height);
	Vector2 GetClientSize();

public:
	//#TODO : what the heck is this?
	//bool(*custom_message_handler)(HWND, UINT, WPARAM, LPARAM);
	HWND m_windowHandle;

	int xPos;
	int yPos;
	int m_width;
	int m_height;
	IntVector2 m_resolution;

	const char* title;
	WindowsMessageCallback m_windowsMessageCallback;

private:
	bool m_isRunning;
	bool m_isOpened;
	bool m_isVisible;
};

bool RegisterGameWindowClass();
void UnregisterGameWindowClass();