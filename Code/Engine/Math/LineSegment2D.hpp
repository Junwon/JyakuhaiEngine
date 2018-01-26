#pragma once
#include "Engine/Math/Vector2.hpp"

class LineSegment2D
{
public:
	Vector2 m_startPoint;
	Vector2 m_endPoint;

	LineSegment2D();
	LineSegment2D(const LineSegment2D& copy);
	LineSegment2D(float startingX, float startingY, float endingX, float endingY);
	LineSegment2D(const Vector2& startingPoint, const Vector2& endingPoint);
	~LineSegment2D();

	void Translate(const Vector2& translation);
	const float CalcLength() const;
	const Vector2 CalcCenter() const;
	const Vector2 GetVectorLength() const;
	
	friend const LineSegment2D Interpolate(const LineSegment2D& start, const LineSegment2D& end, float fractionToEnd);
};
// bool DoLineSegment2DsOverlap(const LineSegment2D& a, const LineSegment2D& b);
