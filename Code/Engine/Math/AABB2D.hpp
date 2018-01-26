#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/LineSegment2D.hpp"

class AABB2D {

public:
	//public data members :
	Vector2 m_mins;
	Vector2 m_maxs;

	//public static const data members :
	static const AABB2D ZERO_TO_ZERO;
	static const AABB2D ZERO_TO_ONE; // fully decorated name is “AABB2::ZERO_TO_ONE”
	static const AABB2D NEG_ONE_TO_ONE; // center (0,0) and radius 1, from (-1,-1) to (1,1)

	AABB2D();
	AABB2D(const AABB2D& copy);
	~AABB2D();
	explicit AABB2D(float initialX, float initialY);
	explicit AABB2D(float minX, float minY, float maxX, float maxY);
	explicit AABB2D(const Vector2& initialMaxs);
	explicit AABB2D(const Vector2& mins, const Vector2& maxs);
	explicit AABB2D(const Vector2& center, float radiusX, float radiusY);
	explicit AABB2D(float width, float height, const Vector2& center);

	//Mutators:
	void StretchToIncludePoint(const Vector2& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius);
	void Translate(const Vector2& translation);
	void SetCenter(const Vector2& newCenter);
	void FlipAlongY();

	//Accessors / queries:
	bool IsPointInside(const Vector2& point) const;
	LineSegment2D GetEdgeFacingPoint(const Vector2& point) const;
	const Vector2 CalcSize() const;
	const Vector2 CalcCenter() const;
	float GetLength() const;
	float GetHeight() const;
	Vector2 GetPointFromUV(float u, float v) const; // Takes normalized point (UV) and returns position
	Vector2 GetPointFromUV(const Vector2& uv) const;
	Vector2 GetUVFromPoint(float x, float y) const; // Takes position and returns normalized point (UV)
	Vector2 GetUVFromPoint(const Vector2& point) const;
	Vector2 GetTopLeftPoint() const;
	Vector2 GetBottomRightPoint() const;
	Vector2 GetRandomPointWithin() const;

	//Operators:
	const AABB2D operator + (const Vector2& translation) const;
	const AABB2D operator - (const Vector2& antiTranslation) const;
	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);

	//Helper Functions:
	friend const AABB2D Interpolate(const AABB2D& start, const AABB2D& end, float fractionToEnd);
	friend void MoveToBestFit(AABB2D& obj, const AABB2D& container);
};

//Standalone Functions:
bool DoAABB2DsOverlap(const AABB2D& a, const AABB2D& b);

//////////////////////////////////////////////////////////////////////////

inline void AABB2D::StretchToIncludePoint(const Vector2& point)
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
	}
}

inline void AABB2D::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius)
{
	m_mins.x -= xPaddingRadius;
	m_maxs.x += xPaddingRadius;

	m_mins.y -= yPaddingRadius;
	m_maxs.y += yPaddingRadius;
}

inline void AABB2D::Translate(const Vector2& translation)
{
	m_mins += translation;
	m_maxs += translation;
}

inline void AABB2D::SetCenter(const Vector2& newCenter)
{
	Vector2 translation = newCenter - CalcCenter();
	Translate(translation);
}

inline void AABB2D::FlipAlongY()
{
	float tempX = m_maxs.x;
	m_maxs.x = m_mins.x;
	m_mins.x = tempX;
}

inline bool AABB2D::IsPointInside(const Vector2& point) const
{
	if ((point.x >= m_mins.x && point.x <= m_maxs.x) &&
		(point.y >= m_mins.y && point.y <= m_maxs.y))
	{
		return true;
	}
	return false;
}

inline LineSegment2D AABB2D::GetEdgeFacingPoint(const Vector2& point) const
{
	float x = point.GetX();
	float y = point.GetY();

	bool aboveAABB = y > m_maxs.GetY();
	bool belowAABB = y < m_mins.GetY();
	bool rightOfAABB = x > m_maxs.GetX();
	bool leftOfAABB = x < m_mins.GetX();

	LineSegment2D closestLine = LineSegment2D(0.f, 0.f, 0.f, 0.f);

	if (aboveAABB && !rightOfAABB && !leftOfAABB)
	{
		closestLine = LineSegment2D(m_mins.GetX(), m_mins.GetY() + GetHeight(), m_maxs.GetX(), m_maxs.GetY());
	}
	else if (belowAABB && !rightOfAABB && !leftOfAABB)
	{
		closestLine = LineSegment2D(m_maxs.GetX(), m_maxs.GetY() - GetHeight(), m_mins.GetX(), m_mins.GetY());
	}
	else if (rightOfAABB)
	{
		closestLine = LineSegment2D(m_maxs.GetX(), m_maxs.GetY(), m_mins.GetX() + GetLength(), m_mins.GetY());
	}
	else
	{
		closestLine = LineSegment2D(m_mins.GetX(), m_mins.GetY(), m_maxs.GetX() - GetLength(), m_maxs.GetY());
	}

	return closestLine;
}

inline const Vector2 AABB2D::CalcSize() const
{
	float length = m_maxs.x - m_mins.x;
	float height = m_maxs.y - m_mins.y;

	return Vector2(length, height);
}

inline const Vector2 AABB2D::CalcCenter() const
{
	float x = (m_mins.x + m_maxs.x) * 0.5f;
	float y = (m_mins.y + m_maxs.y) * 0.5f;

	return Vector2(x, y);
}

inline const AABB2D AABB2D::operator+(const Vector2& translation) const
{
	return AABB2D(m_mins.x + translation.x,
		m_mins.y + translation.y,
		m_maxs.x + translation.x,
		m_maxs.y + translation.y);
}

inline const AABB2D AABB2D::operator-(const Vector2& antiTranslation) const
{
	return AABB2D(m_mins.x - antiTranslation.x,
		m_mins.y - antiTranslation.y,
		m_maxs.x - antiTranslation.x,
		m_maxs.y - antiTranslation.y);
}

inline void AABB2D::operator+=(const Vector2& translation)
{
	Translate(translation);
}

inline void AABB2D::operator-=(const Vector2& antiTranslation)
{
	Vector2 holderAntiTranslation = Vector2(-antiTranslation.x, -antiTranslation.y);
	Translate(holderAntiTranslation);
}

inline float AABB2D::GetLength() const
{
	float width = m_maxs.GetX() - m_mins.GetX();

	if (width < 0)
	{
		width *= -1.f;
	}

	return width;
}

inline float AABB2D::GetHeight() const
{
	float length = m_maxs.GetY() - m_mins.GetY();

	if (length < 0)
	{
		length *= -1.f;
	}

	return length;
}

inline Vector2 AABB2D::GetTopLeftPoint() const
{
	return Vector2(m_mins.x, m_maxs.y);
}

inline Vector2 AABB2D::GetBottomRightPoint() const
{
	return Vector2(m_maxs.x, m_mins.y);
}

inline const AABB2D Interpolate(const AABB2D& start, const AABB2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB2D blended;
	blended.m_mins = (fractionOfStart * start.m_mins) + (fractionToEnd * end.m_mins);
	blended.m_maxs = (fractionOfStart * start.m_maxs) + (fractionToEnd * end.m_maxs);
	return blended;
}

inline void MoveToBestFit(AABB2D& obj, const AABB2D& container)
{
	Vector2 firstCenter = container.CalcCenter();
	Vector2 secondCenter = obj.CalcCenter();
	Vector2 offset = (container.CalcCenter() - obj.CalcCenter());
	Vector2 normalize = offset.GetNormalizedVector();
	obj.Translate(offset);
}

inline bool DoAABB2DsOverlap(const AABB2D& a, const AABB2D& b)
{
	if (a.m_maxs.x < b.m_mins.x ||
		a.m_maxs.y < b.m_mins.y ||
		a.m_mins.x > b.m_maxs.x ||
		a.m_mins.y > b.m_maxs.y)
	{
		return false;
	}
	return true;
}