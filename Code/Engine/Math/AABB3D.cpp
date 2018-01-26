#include "Engine/Math/AABB3D.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB3D AABB3D::ZERO_TO_ONE = AABB3D(1.f, 1.f, 1.f);
const AABB3D AABB3D::NEG_ONE_TO_ONE = AABB3D(-1.f, -1.f, -1.f, 1.f, 1.f, 1.f);

AABB3D::AABB3D() :
	m_mins(0.f, 0.f, 0.f),
	m_maxs(0.f, 0.f, 0.f)
{
}

AABB3D::AABB3D(const AABB3D& copy) :
	m_mins(copy.m_mins),
	m_maxs(copy.m_maxs)
{
}

AABB3D::~AABB3D()
{

}

AABB3D::AABB3D(float initialX, float initialY, float initialZ) :
	m_mins(initialX, initialY, initialZ),
	m_maxs(initialX, initialY, initialZ)
{
}

AABB3D::AABB3D(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) :
	m_mins(minX, minY, minZ),
	m_maxs(maxX, maxY, maxZ)
{
}

AABB3D::AABB3D(const Vector3& mins, const Vector3& maxs) :
	m_mins(mins),
	m_maxs(maxs)
{
}

AABB3D::AABB3D(const Vector3& center, float radiusX, float radiusY, float radiusZ) :
	m_mins(center.x - radiusX, center.y - radiusY, center.z - radiusZ),
	m_maxs(center.x + radiusX, center.y + radiusY, center.z + radiusZ)
{
}

void AABB3D::StretchToIncludePoint(const Vector3& point)
{
	if (!IsPointInside(point))
	{
		if (point.x < m_mins.x)
		{
			m_mins.x = point.x;
		}
		else if (point.x > m_maxs.x)
		{
			m_maxs.x = point.x;
		}

		if (point.y < m_mins.y)
		{
			m_mins.y = point.y;
		}
		else if (point.y > m_maxs.y)
		{
			m_maxs.y = point.y;
		}

		if (point.z < m_mins.z)
		{
			m_mins.z = point.z;
		}
		else if (point.z > m_maxs.z)
		{
			m_maxs.z = point.z;
		}
	}
}

void AABB3D::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius)
{
	m_mins.x -= xPaddingRadius;
	m_maxs.x += xPaddingRadius;

	m_mins.y -= yPaddingRadius;
	m_maxs.y += yPaddingRadius;

	m_mins.z -= zPaddingRadius;
	m_maxs.z += zPaddingRadius;
}

void AABB3D::Translate(const Vector3& translation)
{
	m_mins += translation;
	m_maxs += translation;
}

bool AABB3D::IsPointInside(const Vector3& point) const
{
	if(IsValueBetween(point.x, m_mins.x, m_maxs.x) && 
	   IsValueBetween(point.y, m_mins.y, m_maxs.y) &&
	   IsValueBetween(point.z, m_mins.z, m_maxs.z))
	{
		return true;
	}
	return false;
}

const Vector3 AABB3D::CalcSize() const
{
	float length = m_maxs.x - m_mins.x;
	float height = m_maxs.y - m_mins.y;
	float width = m_maxs.z - m_mins.z;

	return Vector3(length, height, width);
}

const Vector3 AABB3D::CalcCenter() const
{
	float x = (m_mins.x + m_maxs.x) * 0.5f;
	float y = (m_mins.y + m_maxs.y) * 0.5f;
	float z = (m_mins.z + m_maxs.z) * 0.5f;

	return Vector3(x, y, z);
}

float AABB3D::GetLength() const
{
	float length = m_maxs.GetX() - m_mins.GetX();

	if (length < 0)
	{
		length *= -1.f;
	}

	return length;
}

float AABB3D::GetHeight() const
{
	float length = m_maxs.GetY() - m_mins.GetY();

	if (length < 0)
	{
		length *= -1.f;
	}

	return length;
}

float AABB3D::GetWidth() const
{
	float width = m_maxs.GetZ() - m_mins.GetZ();

	if (width < 0)
	{
		width *= -1.f;
	}

	return width;
}

const AABB3D AABB3D::operator+(const Vector3& translation) const
{
	return AABB3D(m_mins.x + translation.x,
		m_mins.y + translation.y,
		m_mins.z + translation.z,
		m_maxs.x + translation.x,
		m_maxs.y + translation.y,
		m_maxs.z + translation.z);
}

const AABB3D AABB3D::operator-(const Vector3& antiTranslation) const
{
	return AABB3D(m_mins.x - antiTranslation.x,
		m_mins.y - antiTranslation.y,
		m_mins.z - antiTranslation.z,
		m_maxs.x - antiTranslation.x,
		m_maxs.y - antiTranslation.y,
		m_maxs.z - antiTranslation.z);
}

void AABB3D::operator+=(const Vector3& translation)
{
	Translate(translation);
}

void AABB3D::operator-=(const Vector3& antiTranslation)
{
	Vector3 holderAntiTranslation = Vector3(-antiTranslation.x, -antiTranslation.y, -antiTranslation.z);
	Translate(holderAntiTranslation);
}

const AABB3D Interpolate(const AABB3D& start, const AABB3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB3D blended;
	blended.m_mins = (fractionOfStart * start.m_mins) + (fractionToEnd * end.m_mins);
	blended.m_maxs = (fractionOfStart * start.m_maxs) + (fractionToEnd * end.m_maxs);
	return blended;
}

bool DoAABB3DsOverlap(const AABB3D& a, const AABB3D& b)
{
	if (a.m_maxs.x < b.m_mins.x ||
		a.m_maxs.y < b.m_mins.y ||
		a.m_maxs.z < b.m_mins.z ||
		a.m_mins.x > b.m_maxs.x ||
		a.m_mins.y > b.m_maxs.y ||
		a.m_mins.z > b.m_maxs.z)
	{
		return false;
	}
	return true;
}



