#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB2D AABB2D::ZERO_TO_ZERO = AABB2D(0.f, 0.f, 0.f, 0.f);
const AABB2D AABB2D::ZERO_TO_ONE = AABB2D(1.0f, 1.0f);
const AABB2D AABB2D::NEG_ONE_TO_ONE = AABB2D(-1.0f, -1.0f, 1.0f, 1.0f);

AABB2D::AABB2D() :
	m_mins(0.f, 0.f),
	m_maxs(0.f, 0.f)
{
}

AABB2D::AABB2D(const AABB2D& copy)
{
	m_mins = copy.m_mins;
	m_maxs = copy.m_maxs;
}

AABB2D::~AABB2D()
{
}

AABB2D::AABB2D(float initialX, float initialY) :
	m_mins(0.f, 0.f),
	m_maxs(initialX, initialY)
{
}

AABB2D::AABB2D(float minX, float minY, float maxX, float maxY) :
	m_mins(minX, minY),
	m_maxs(maxX, maxY)
{
}

AABB2D::AABB2D(const Vector2& initialMaxs) :
	m_mins(0.f, 0.f),
	m_maxs(initialMaxs.x, initialMaxs.y)
{
}

AABB2D::AABB2D(const Vector2& mins, const Vector2& maxs) :
	m_mins(mins),
	m_maxs(maxs)
{
}

AABB2D::AABB2D(const Vector2& center, float radiusX, float radiusY) :
	m_mins(center.x - radiusX, center.y - radiusY),
	m_maxs(center.x + radiusX, center.y + radiusY)
{
}

AABB2D::AABB2D(float width, float height, const Vector2& center) :
	m_mins(center.x - (width * 0.5f), center.y - (height * 0.5f)),
	m_maxs(center.x + (width * 0.5f), center.y + (height * 0.5f))
{
}

Vector2 AABB2D::GetPointFromUV(float u, float v) const
{
	float x = RangeMap(u, 0.f, 1.f, m_mins.x, m_maxs.x);
	float y = RangeMap(v, 0.f, 1.f, m_mins.y, m_maxs.y);

	return Vector2(x, y);
}

Vector2 AABB2D::GetUVFromPoint(float x, float y) const
{
	float u = RangeMap(x, m_mins.x, m_maxs.x, 0.f, 1.f);
	float v = RangeMap(y, m_mins.y, m_maxs.y, 0.f, 1.f);

	return Vector2(u, v);
}

Vector2 AABB2D::GetPointFromUV(const Vector2& uv) const
{
	return GetPointFromUV(uv.x, uv.y);
}

Vector2 AABB2D::GetUVFromPoint(const Vector2& point) const
{
	return GetUVFromPoint(point.x, point.y);
}

Vector2 AABB2D::GetRandomPointWithin() const
{
	return Vector2(GetRandomFloatInRange(m_mins.x, m_maxs.x), GetRandomFloatInRange(m_mins.y, m_maxs.y));
}