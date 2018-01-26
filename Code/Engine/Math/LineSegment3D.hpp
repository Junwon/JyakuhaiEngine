#pragma once
#include "Engine/Math/Vector3.hpp"

class LineSegment3D
{
public:
	Vector3 m_startPoint;
	Vector3 m_endPoint;

	LineSegment3D();
	LineSegment3D(const LineSegment3D& copy);
	LineSegment3D(float startingX, float startingY, float startingZ, float endingX, float endingY, float endingZ);
	LineSegment3D(const Vector3& startingPoint, const Vector3& endingPoint);
	~LineSegment3D();

	void Translate(const Vector3& translation);
	const float CalcLength() const;
	const Vector3 CalcCenter() const;
	const Vector3 GetVectorLength() const;

	friend const LineSegment3D Interpolate(const LineSegment3D& start, const LineSegment3D& end, float fractionToEnd);
};
// bool DoLineSegment3DsOverlap(const LineSegment3D& a, const LineSegment3D& b);
