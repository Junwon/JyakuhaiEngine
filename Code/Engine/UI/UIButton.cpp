#include "Engine/UI/UIButton.hpp"

#include "Engine/Renderer/RHI/SimpleRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

UIButton::UIButton() :
	m_mouseEvent(MOUSE_NONE),
	m_justPressed(false),
	m_buttonFuncPtr(nullptr),
	m_isEnabled(true)
{
}

UIButton::~UIButton()
{
}

void UIButton::Update(float)
{
	if (m_inputSystem && m_renderOutput) {

		if (m_isEnabled && IsCursorWithinBounds()) {
			if (m_inputSystem->WasKeyJustPressed(MOUSECODE_LEFT)) {
				m_mouseEvent = MOUSE_CLICKED;
				m_justPressed = true;
				if (m_buttonFuncPtr) {
					m_buttonFuncPtr();
				}
			}
			else if (m_inputSystem->IsKeyDown(MOUSECODE_LEFT)) {
				m_mouseEvent = MOUSE_PRESSED;
				m_justPressed = true;
				m_color = RgbaFloats::BLUE;
			}
			else if (!m_inputSystem->IsKeyDown(MOUSECODE_LEFT) && m_justPressed) {
				m_mouseEvent = MOUSE_RELEASED;
				m_justPressed = false;
				m_color = RgbaFloats::WHITE;
			}
			else {
				m_mouseEvent = MOUSE_HOVER;
				m_color = RgbaFloats::YELLOW;
			}
		}
		else {
			m_mouseEvent = MOUSE_NONE;
			m_color = RgbaFloats::WHITE;
		}
	}
}

void UIButton::Draw() const
{
	if (m_renderOutput) {
		if (m_sprite.m_texture) {
			m_renderOutput->DrawTexturedAABB2DSprite(GetRelativeBounds(), AABB2D::ZERO_TO_ONE, m_sprite.m_texture, false, m_color);
		}
		else {
			m_renderOutput->DrawQuad2D(GetRelativeBounds(), m_color);
		}
	}
}

void UIButton::SetButtonFunction(ButtonFunction funcPtr)
{
	m_buttonFuncPtr = funcPtr;
}

void UIButton::SetEnabled(bool enable)
{
	m_isEnabled = enable;
}

bool UIButton::IsEnabled() const
{
	return m_isEnabled;
}

bool UIButton::WasButtonReleased() const
{
	return m_mouseEvent == MOUSE_PRESSED;
}

bool UIButton::WasButtonClicked() const
{
	return m_mouseEvent == MOUSE_CLICKED;
}

bool UIButton::IsCursorWithinBounds() const
{
	// #TODO : handle whether draw starts bottom left or top left
	Vector2 cursorPos = m_inputSystem->GetCursorWindowPos().GetAsFloats();
	cursorPos.y = m_renderOutput->m_height - cursorPos.y;
	AABB2D buttonBounds = GetRelativeBounds();

	return  buttonBounds.IsPointInside(cursorPos);
}
