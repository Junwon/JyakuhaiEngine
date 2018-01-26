#pragma once

#include <vector>

#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/Matrix4.hpp"

class SimpleRenderer;
class InputSystem;

enum ePositionMode 
{
	UI_POSITION_ABSOLUTE,
	UI_POSITION_ABSOLUTE_BOUNDS
};

struct UIMetric_t
{
	UIMetric_t() :
		m_ratio(Vector2::ZERO),
		m_unit(Vector2::ZERO)
	{};

	UIMetric_t(float ratioX, float ratioY = 0.f, float unitX = 0.f, float unitY = 0.f) :
		m_ratio(ratioX, ratioY),
		m_unit(unitX, unitY)
	{};

	explicit UIMetric_t(const Vector2& ratio, const Vector2& unit = Vector2::ZERO) :
		m_ratio(ratio),
		m_unit(unit)
	{};

	explicit UIMetric_t(const Vector2& ratio, float unitX = 0.f, float unitY = 0.f) :
		m_ratio(ratio),
		m_unit(unitX, unitY)
	{};

	explicit UIMetric_t(float ratioX, float ratioY = 0.f, const Vector2& unit = Vector2::ZERO) :
		m_ratio(ratioX, ratioY),
		m_unit(unit)
	{};

	Vector2 m_ratio;
	Vector2 m_unit;
};

class UIElement
{
public:
	UIElement();
	~UIElement();

public:
	Vector2 GetSize() const;
	Vector2 GetLocalPosition() const;
	Vector2 GetRelativePosition() const;
	Vector2 GetAbsolutePosition() const;
	AABB2D GetLocalBounds() const;
	AABB2D GetRelativeBounds() const;
	AABB2D GetElementBounds() const;
	UIElement* GetParent() const;
	Matrix4 GetLocalTransform() const;
	Matrix4 GetRelativeTransform() const;
	Matrix4 GetWorldTransform() const;

	void SetPosition(float ratioX, float ratioY, float unitX, float unitY);
	void SetPosition(const Vector2& ratio, const Vector2& unit = Vector2::ZERO);
	void SetPosition(const Vector2& ratio, float unitX, float unitY);
	void SetPosition(float ratioX, float ratioY, const Vector2& unit = Vector2::ZERO);
	void SetSize(float x, float y);
	void SetSize(const Vector2& size);
	void SetSizeRatio(const Vector2& sizeRatio);
	void SetPivot(float ratioX, float ratioY);
	void SetPivot(const Vector2& ratioRespectToMyBounds);
	void SetTint(const RgbaFloats& rgbaFloats);
	void SetRenderOutput(SimpleRenderer* output);
	void SetInputSystem(InputSystem* input);
	void SetColor(const Rgba& color);
	void SetVisibility(bool visible);

	/*	Need to call this first before calling UISprite::SetSprite()
		because parent needs to set render output for child. */
	UIElement* AddChild(UIElement* element);
	void RemoveChild(UIElement* element);
	void RemoveSelfFromParent();

	template <typename T>
	T* CreateChild() {
		return (T*)AddChild(new T());
	}

public:
	virtual void Update(float deltaSeconds);
	virtual void Draw() const;

public:
	ePositionMode m_positionMode;
	UIMetric_t m_position; // Percent-Offset
	UIMetric_t m_size; // Percent-Offset
	Vector2 m_pivot; // Percent

	AABB2D m_bounds;

	UIElement* m_parent;
	std::vector<UIElement*> m_childrenElements;

	Rgba m_color;
	bool m_isVisible;

	SimpleRenderer* m_renderOutput;
	InputSystem* m_inputSystem;
public:
	AABB2D CalculateBounds() const;
	AABB2D CalculateBoundsRelativeToParentPosition() const;
	AABB2D CalculateAbsoluteBounds() const;
};

void UpdateUIElement(UIElement* element, float deltaSeconds);
void DrawUIElement(UIElement* element);