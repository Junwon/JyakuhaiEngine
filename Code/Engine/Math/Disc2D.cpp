#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/MathUtils.hpp"

const Disc2D Disc2D::UNIT_CIRCLE = Disc2D(0.0f, 0.0f, 1.0f);

Disc2D::Disc2D() :
	m_center(0.f, 0.f),
	m_radius(0.f)
{
}

Disc2D::Disc2D(const Disc2D& copy) :
	m_center(copy.m_center),
	m_radius(copy.m_radius)
{
}

Disc2D::~Disc2D()
{

}

Disc2D::Disc2D(float initialX, float initialY, float initialRadius) :
	m_center(initialX, initialY),
	m_radius(initialRadius)
{
}

Disc2D::Disc2D(const Vector2& initialCenter, float initialRadius)
{
	m_center = initialCenter;
	m_radius = initialRadius;
}

void Disc2D::StretchToIncludePoint(const Vector2& point)
{
	if (!IsPointInside(point))
	{
		m_radius = CalcDistance(m_center, point);
	}
}

void Disc2D::AddPadding(float paddingRadius)
{
	m_radius += paddingRadius;
}

void Disc2D::Translate(const Vector2& translation)
{
	m_center += translation;
}

bool Disc2D::IsPointInside(const Vector2& point) const
{
	float distance = CalcDistance(m_center, point);
	
	if (distance > m_radius) {
		return false;
	}
	return true;
}

const Disc2D Disc2D::operator+(const Vector2& translation) const
{
	Vector2 newCenter = m_center + translation;
	return Disc2D(newCenter, m_radius);
}

const Disc2D Disc2D::operator-(const Vector2& antiTranslation) const
{
	Vector2 newCenter = m_center - antiTranslation;
	return Disc2D(newCenter, m_radius);
}

void Disc2D::operator+=(const Vector2& translation)
{
	Translate(translation);
}

void Disc2D::operator-=(const Vector2& antiTranslation)
{
	Vector2 holderAntiTranslation = Vector2(-antiTranslation.x, -antiTranslation.y);
	Translate(holderAntiTranslation);
}

const Disc2D Interpolate(const Disc2D& start, const Disc2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Disc2D blended;
	blended.m_center = (fractionOfStart * start.m_center) + (fractionToEnd * end.m_center);
	blended.m_radius = (fractionOfStart * start.m_radius) + (fractionToEnd * end.m_radius);
	return blended;
}

bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b)
{
	float distanceBetweenCenters = CalcDistance(a.m_center, b.m_center);
	float sumsOfRadiusOfBothDiscs = a.m_radius + b.m_radius;

	if (distanceBetweenCenters < sumsOfRadiusOfBothDiscs)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	float distanceBetweenCenters = CalcDistance(aCenter, bCenter);
	float sumsOfRadiusOfBothDiscs = aRadius + bRadius;

	if (distanceBetweenCenters <= sumsOfRadiusOfBothDiscs)
	{
		return true;
	}
	else
	{
		return false;
	}
}
