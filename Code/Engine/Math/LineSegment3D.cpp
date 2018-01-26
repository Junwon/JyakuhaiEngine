#include "Engine/Math/LineSegment3D.hpp"

LineSegment3D::LineSegment3D() :
	m_startPoint(0.f, 0.f, 0.f),
	m_endPoint(0.f, 0.f, 0.f)
{
}

LineSegment3D::LineSegment3D(const LineSegment3D& copy) :
	m_startPoint(copy.m_startPoint),
	m_endPoint(copy.m_endPoint)
{
}

LineSegment3D::~LineSegment3D()
{
}

LineSegment3D::LineSegment3D(float startingX, float startingY, float startingZ, float endingX, float endingY, float endingZ) :
	m_startPoint(startingX, startingY, startingZ),
	m_endPoint(endingX, endingY, endingZ)
{
}

LineSegment3D::LineSegment3D(const Vector3& startingPoint, const Vector3& endingPoint) :
	m_startPoint(startingPoint),
	m_endPoint(endingPoint)
{
}


void LineSegment3D::Translate(const Vector3& translation)
{
	m_startPoint += translation;
	m_endPoint += translation;
}

const float LineSegment3D::CalcLength() const
{
	float distance = CalcDistance(m_startPoint, m_endPoint);
	return distance;
}

const Vector3 LineSegment3D::CalcCenter() const
{
	float centerX = (m_startPoint.x + m_endPoint.x) * 0.5f;
	float centerY = (m_startPoint.y + m_endPoint.y) * 0.5f;
	float centerZ = (m_startPoint.z + m_endPoint.z) * 0.5f;
	return Vector3(centerX, centerY, centerZ);
}

const Vector3 LineSegment3D::GetVectorLength() const
{
	return Vector3(m_endPoint.x - m_startPoint.x, m_endPoint.y - m_startPoint.y, m_endPoint.z - m_startPoint.z);
}

const LineSegment3D Interpolate(const LineSegment3D& start, const LineSegment3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	LineSegment3D blended;

	blended.m_startPoint = (fractionOfStart * start.m_startPoint) + (fractionToEnd * end.m_startPoint);
	blended.m_endPoint = (fractionOfStart * start.m_endPoint) + (fractionToEnd * end.m_endPoint);
	return blended;
}

// bool DoLineSegment3DsOverlap(const LineSegment3D& a, const LineSegment3D& b)
// {
// 	return false;
// }
