#pragma once

#include "Engine/UI/UISprite.hpp"

enum eMouseEvent
{
	MOUSE_NONE,
	MOUSE_CLICKED,
	MOUSE_PRESSED,
	MOUSE_RELEASED,
	MOUSE_HOVER
};

typedef void(*ButtonFunction)();

class UIButton : public UISprite
{
public:
	UIButton();
	~UIButton();

public:
	virtual void Update(float deltaSeconds);
	virtual void Draw() const override;

public:
	void SetButtonFunction(ButtonFunction funcPtr);
	void SetEnabled(bool enable);
	bool IsEnabled() const;
	ButtonFunction m_buttonFuncPtr;
	bool WasButtonReleased() const;
	bool WasButtonClicked() const;
	bool IsCursorWithinBounds() const;

private:
	eMouseEvent m_mouseEvent;
	bool m_justPressed;
	bool m_isEnabled;
};