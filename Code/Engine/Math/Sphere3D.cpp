#include "Engine/Math/Sphere3D.hpp"

const Sphere3D Sphere3D::UNIT_SPHERE = Sphere3D(0.f, 0.f, 0.f, 1.f);

Sphere3D::Sphere3D() :
	m_center(0.f, 0.f, 0.f),
	m_radius(0.f)
{
}

Sphere3D::Sphere3D(const Sphere3D& copy) :
	m_center(copy.m_center),
	m_radius(copy.m_radius)
{
}

Sphere3D::~Sphere3D()
{

}

Sphere3D::Sphere3D(float initialX, float initialY, float initialZ, float initialRadius) :
	m_center(initialX, initialY, initialZ),
	m_radius(initialRadius)
{
}

Sphere3D::Sphere3D(const Vector3& initialCenter, float initialRadius) :
	m_center(initialCenter),
	m_radius(initialRadius)
{
}

void Sphere3D::StretchToIncludePoint(const Vector3& point)
{
	if (!IsPointInside(point))
	{
		m_radius = CalcDistance(m_center, point);
	}
}

void Sphere3D::AddPadding(float paddingRadius)
{
	m_radius += paddingRadius;
}

void Sphere3D::Translate(const Vector3& translation)
{
	m_center += translation;
}

bool Sphere3D::IsPointInside(const Vector3& point) const
{
	float distanceSquared = CalcDistanceSquared(m_center, point);

	if (distanceSquared > (m_radius * m_radius)) {
		return false;
	}
	return true;
}

void Sphere3D::operator-=(const Vector3& antiTranslation)
{
	Vector3 holderAntiTranslation = Vector3(-antiTranslation.x, -antiTranslation.y, -antiTranslation.z);
	Translate(holderAntiTranslation);
}

void Sphere3D::operator+=(const Vector3& translation)
{
	Translate(translation);
}

const Sphere3D Sphere3D::operator-(const Vector3& antiTranslation) const
{
	Vector3 newCenter = m_center - antiTranslation;
	return Sphere3D(newCenter, m_radius);
}

const Sphere3D Sphere3D::operator+(const Vector3& translation) const
{
	Vector3 newCenter = m_center + translation;
	return Sphere3D(newCenter, m_radius);
}

const Sphere3D Interpolate(const Sphere3D& start, const Sphere3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Sphere3D blended;
	blended.m_center = (fractionOfStart * start.m_center) + (fractionToEnd * end.m_center);
	blended.m_radius = (fractionOfStart * start.m_radius) + (fractionToEnd * end.m_radius);
	return blended;
}

bool DoSpheresOverlap(const Sphere3D& a, const Sphere3D& b)
{
	float distanceBetweenCentersSquared = CalcDistanceSquared(a.m_center, b.m_center);
	float sumsOfRadiusOfBothDiscsSquared = (a.m_radius + b.m_radius) * (a.m_radius + b.m_radius);

	if (distanceBetweenCentersSquared < sumsOfRadiusOfBothDiscsSquared)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoSpheresOverlap(const Vector3& aCenter, float aRadius, const Vector3& bCenter, float bRadius)
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

