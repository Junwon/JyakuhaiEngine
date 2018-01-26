#include "Engine/UI/UIElement.hpp"

#include <algorithm>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/SimpleRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"

UIElement::UIElement() :
	m_positionMode(UI_POSITION_ABSOLUTE),
	m_position(UIMetric_t()),
	m_size(UIMetric_t()),
	m_pivot(Vector2::ZERO),
	m_bounds(AABB2D::ZERO_TO_ZERO),
	m_parent(nullptr),
	m_color(Rgba::WHITE),
	m_isVisible(true),
	m_renderOutput(SimpleRenderer::GetInstance()),
	m_inputSystem(InputSystem::GetInstance())
{
}

UIElement::~UIElement()
{
	for (UIElement* child : m_childrenElements) {
		SAFE_DELETE(child);
	}
}

Vector2 UIElement::GetSize() const
{
	Vector2 parentSize = Vector2::ZERO;
	if (m_parent) {
		parentSize = m_parent->GetSize();
	}

	return parentSize * m_size.m_ratio + m_size.m_unit;
}

Vector2 UIElement::GetLocalPosition() const
{
	return m_pivot * GetSize();
}

Vector2 UIElement::GetRelativePosition() const
{
	if (m_parent) {
		Vector2 parentSize = m_parent->GetSize();

		Vector2 pivotPosition = parentSize * m_position.m_ratio + m_position.m_unit;

		return pivotPosition;
	} 
	else {
		return GetLocalPosition();
	}
	
}

Vector2 UIElement::GetAbsolutePosition() const
{
	if (m_parent) {
		AABB2D parentBounds = m_parent->CalculateBoundsRelativeToParentPosition();
		Vector2 parentSize = parentBounds.CalcSize();

		Vector2 pivotPosition = parentBounds.m_mins +
			(parentSize * m_position.m_ratio + m_position.m_unit);

		return pivotPosition;
	}
	else {
		return m_position.m_ratio + m_position.m_unit;
	}
}

AABB2D UIElement::GetLocalBounds() const
{
	Vector2 size = GetSize();
	Vector2 pivotPos = size * m_pivot;

	AABB2D bounds = AABB2D(Vector2::ZERO, size);
	bounds.Translate(-pivotPos);
	return bounds;
}

AABB2D UIElement::GetRelativeBounds() const
{
	switch (m_positionMode) {
	case UI_POSITION_ABSOLUTE:
		return CalculateBoundsRelativeToParentPosition();

	case UI_POSITION_ABSOLUTE_BOUNDS:
		return CalculateAbsoluteBounds();

	default:
		// ...
		return AABB2D();
	}
}

AABB2D UIElement::GetElementBounds() const
{
	Vector2 size = GetSize();
	return AABB2D(size.x, size.y, m_pivot);
}

UIElement* UIElement::GetParent() const
{
	return m_parent;
}

Matrix4 UIElement::GetLocalTransform() const
{
	return Matrix4::CreateTranslation(GetLocalPosition());
}

Matrix4 UIElement::GetRelativeTransform() const
{
	return Matrix4::CreateTranslation(GetRelativePosition());
}

Matrix4 UIElement::GetWorldTransform() const
{
// 	if (m_parent) {
// 		Matrix4 parentTransform = m_parent->GetWorldTransform();
// 		return parentTransform * GetLocalTransform();
// 	}
// 	else {
// 		return GetLocalTransform();
// 	}
	return Matrix4::CreateTranslation(GetAbsolutePosition());
}

void UIElement::SetPosition(float ratioX, float ratioY, float unitX, float unitY)
{
	m_position.m_ratio = Vector2(ratioX, ratioY);
	m_position.m_unit = Vector2(unitX, unitY);
}

void UIElement::SetPosition(const Vector2& ratio, const Vector2& unit)
{
	m_position.m_ratio = ratio;
	m_position.m_unit = unit;
}

void UIElement::SetPosition(const Vector2& ratio, float unitX, float unitY)
{
	m_position.m_ratio = ratio;
	m_position.m_unit = Vector2(unitX, unitY);
}

void UIElement::SetPosition(float ratioX, float ratioY, const Vector2& unit)
{
	m_position.m_ratio = Vector2(ratioX, ratioY);
	m_position.m_unit = unit;
}

void UIElement::SetSize(float x, float y)
{
	m_size.m_unit = Vector2(x, y);
}

void UIElement::SetSize(const Vector2& size)
{
	m_size.m_unit = size;
}

void UIElement::SetSizeRatio(const Vector2& sizeRatio)
{
	m_size.m_ratio = sizeRatio;
}

void UIElement::SetPivot(float ratioX, float ratioY)
{
	m_pivot = Vector2(ratioX, ratioY);
}

void UIElement::SetPivot(const Vector2& ratioRespectToMyBounds)
{
	m_pivot = ratioRespectToMyBounds;
}

void UIElement::SetTint(const RgbaFloats& rgbaFloats)
{
	m_color = rgbaFloats;
}

void UIElement::SetRenderOutput(SimpleRenderer* output)
{
	m_renderOutput = output;

	for (UIElement* child : m_childrenElements) {
		child->SetRenderOutput(output);
	}
}

void UIElement::SetInputSystem(InputSystem* input)
{
	m_inputSystem = input;
}

void UIElement::SetColor(const Rgba& color)
{
	m_color = color;
}

void UIElement::SetVisibility(bool visible)
{
	m_isVisible = visible;
}

UIElement* UIElement::AddChild(UIElement* element)
{
	element->m_parent = this;
	m_childrenElements.push_back(element);
	return element;
}

void UIElement::RemoveChild(UIElement* element)
{
	m_childrenElements.erase(std::remove(m_childrenElements.begin(), m_childrenElements.end(), element), m_childrenElements.end());
	element->m_parent = nullptr;
}

void UIElement::RemoveSelfFromParent()
{
	if (m_parent) {
		m_parent->RemoveChild(this);
	}
}

void UIElement::Update(float)
{
}

void UIElement::Draw() const
{
	if (m_renderOutput && m_isVisible) {
		m_renderOutput->DrawQuad2D(GetRelativeBounds(), m_color);
	}
}

AABB2D UIElement::CalculateBounds() const
{
	Vector2 parentSize = Vector2::ZERO;
	if (m_parent) {
		parentSize = m_parent->GetSize();
	}

	// Absolute Positioning
	Vector2 relativeSize = parentSize * m_size.m_ratio;
	Vector2 mySize = relativeSize + m_size.m_unit;

	Vector2 relativePosition = parentSize * m_position.m_ratio;
	Vector2 position = relativePosition + m_position.m_unit;

	return AABB2D(position);
}

AABB2D UIElement::CalculateBoundsRelativeToParentPosition() const
{
	AABB2D myLocalBounds = GetLocalBounds();
	myLocalBounds.Translate(GetAbsolutePosition());

	return myLocalBounds;
}

AABB2D UIElement::CalculateAbsoluteBounds() const
{
	return AABB2D();
}

//////////////////////////////////////////////////////////////////////////

void UpdateUIElement(UIElement* element, float deltaSeconds)
{
	if (element) {
		element->Update(deltaSeconds);

		for (UIElement* child : element->m_childrenElements) {
			UpdateUIElement(child, deltaSeconds);
		}
	}
}

void DrawUIElement(UIElement* element)
{
	if (element) {
		element->Draw();

		for (UIElement* child : element->m_childrenElements) {
			DrawUIElement(child);
		}
	}
}