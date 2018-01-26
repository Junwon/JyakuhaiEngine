#include "Engine/Math/LineSegment2D.hpp"

LineSegment2D::LineSegment2D() :
	m_startPoint(0.f, 0.f),
	m_endPoint(0.f, 0.f)
{
}

LineSegment2D::LineSegment2D(const LineSegment2D& copy) :
	m_startPoint(copy.m_startPoint),
	m_endPoint(copy.m_endPoint)
{
}

LineSegment2D::LineSegment2D(float startingX, float startingY, float endingX, float endingY) :
	m_startPoint(startingX, startingY),
	m_endPoint(endingX, endingY)
{
}

LineSegment2D::LineSegment2D(const Vector2& startingPoint, const Vector2& endingPoint) :
	m_startPoint(startingPoint),
	m_endPoint(endingPoint)
{
}

LineSegment2D::~LineSegment2D()
{

}

void LineSegment2D::Translate(const Vector2& translation)
{
	m_startPoint += translation;
	m_endPoint += translation;
}

const float LineSegment2D::CalcLength() const
{
	float distance = CalcDistance(m_startPoint, m_endPoint);
	return distance;
}

const Vector2 LineSegment2D::CalcCenter() const
{
	float centerX = (m_startPoint.x + m_endPoint.x) * 0.5f;
	float centerY = (m_startPoint.y + m_endPoint.y) * 0.5f;
	return Vector2(centerX, centerY);
}

const Vector2 LineSegment2D::GetVectorLength() const
{
	return Vector2(m_endPoint.x - m_startPoint.x, m_endPoint.y - m_startPoint.y);
}

const LineSegment2D Interpolate(const LineSegment2D& start, const LineSegment2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	LineSegment2D blended;

	blended.m_startPoint = (fractionOfStart * start.m_startPoint) + (fractionToEnd * end.m_startPoint);
	blended.m_endPoint = (fractionOfStart * start.m_endPoint) + (fractionToEnd * end.m_endPoint);
	return blended;
}

// bool DoLineSegment2DsOverlap(const LineSegment2D& a, const LineSegment2D& b)
// {
// 	return false;
// }
