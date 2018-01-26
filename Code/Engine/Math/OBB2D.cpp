#include "Engine/Math/OBB2D.hpp"
#include "MathUtils.hpp"

OBB2D::OBB2D()
{
	m_halfDimensions = Vector2::ZERO;
	m_center = Vector2::ZERO;
	m_orientationDegrees = 0.f;
}

OBB2D::OBB2D(const OBB2D& copy)
{
	m_halfDimensions = copy.m_halfDimensions;
	m_center = copy.m_center;
	m_orientationDegrees = copy.m_orientationDegrees;
}

OBB2D::~OBB2D()
{

}

OBB2D::OBB2D(const Vector2& center, float radiusX, float radiusY, float orientationDegrees)
{
	m_halfDimensions = Vector2(radiusX, radiusY);
	m_center = center;
	m_orientationDegrees = orientationDegrees;
}

OBB2D::OBB2D(const AABB2D& aabb2, float orientationDegrees)
{
	m_halfDimensions = Vector2(aabb2.GetLength() / 2, aabb2.GetHeight() / 2);
	m_center = aabb2.CalcCenter();
	m_orientationDegrees = orientationDegrees;
}

Vector2 OBB2D::GetDimensions() const
{
	return m_halfDimensions;
}

float OBB2D::GetHalfWidth() const
{
	return m_halfDimensions.GetX();
}

float OBB2D::GetHalfHeight() const
{
	return m_halfDimensions.GetY();
}

Vector2 OBB2D::GetOrientedWidth() const
{
	return Vector2(GetHalfWidth() * CosDegrees(m_orientationDegrees), GetHalfHeight() * SinDegrees(m_orientationDegrees));
}

Vector2 OBB2D::GetOrientedHeight() const
{
	return Vector2(GetHalfWidth() * CosDegrees(m_orientationDegrees + 90), GetHalfHeight() * SinDegrees(m_orientationDegrees + 90));
}

Vector2 OBB2D::GetCenter() const
{
	return m_center;
}

float OBB2D::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}

void OBB2D::SetDimensions(const Vector2& halfDimensions)
{
	m_halfDimensions = halfDimensions;
}

void OBB2D::SetCenter(const Vector2& center)
{
	m_center = center;
}

void OBB2D::SetOrientationDegrees(float degrees)
{
	m_orientationDegrees = degrees;
}

