#pragma once
#include "Engine/Math/AABB2D.hpp"

class OBB2D
{
public:
	Vector2 m_halfDimensions;
	Vector2 m_center;
	float	m_orientationDegrees;

	OBB2D();
	OBB2D(const OBB2D& copy);
	~OBB2D();
	explicit OBB2D(const Vector2& center, float radiusX, float radiusY, float orientationDegrees = 0.f);
	explicit OBB2D(const AABB2D& aabb2, float orientationDegrees = 0.f);

	Vector2 GetDimensions() const;
	float GetHalfWidth() const;
	float GetHalfHeight() const;
	Vector2 GetOrientedWidth() const;
	Vector2 GetOrientedHeight() const;
	Vector2 GetCenter() const;
	float GetOrientationDegrees() const;

	void SetDimensions(const Vector2& halfDimensions);
	void SetCenter(const Vector2& center);
	void SetOrientationDegrees(float degrees);
};