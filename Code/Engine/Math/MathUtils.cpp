#include "Engine/Math/MathUtils.hpp"

#include <stdlib.h>

#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

int GetRandomIntLessThan(int maxValueNotInclusive)
{
	if (maxValueNotInclusive == 0) {
		return 0;
	}

	int randomInt = rand();
	return randomInt % maxValueNotInclusive;
}

unsigned int GetRandomIntLessThan(unsigned int maxValueNotInclusive)
{
	return (unsigned int)GetRandomIntLessThan((int)maxValueNotInclusive);
}

float CosDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return (float)cos(radians);
}

float SinDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return (float)sin(radians);
}

float Atan2Degrees(float y, float x)
{
	float radians = (float)atan2(y, x);
	return ConvertRadiansToDegrees(radians);
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float DotProduct(const Quaternion& a, const Quaternion& b)
{
	return (a.m_x * b.m_x) + (a.m_y * b.m_y) + (a.m_z * b.m_z) + (a.m_w * b.m_w);
}

float GetRandomFloatZeroToOne()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b)
// {
// 	float distanceBetweenCenters = CalcDistance(a.m_center, b.m_center);
// 	float sumsOfRadiusOfBothDiscs = a.m_radius + b.m_radius;
// 
// 	if (distanceBetweenCenters < sumsOfRadiusOfBothDiscs)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

// bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
// {
// 	float distanceBetweenCenters = CalcDistance(aCenter, bCenter);
// 	float sumsOfRadiusOfBothDiscs = aRadius + bRadius;
// 
// 	if (distanceBetweenCenters <= sumsOfRadiusOfBothDiscs)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

bool DoDiscAndAABBOverlap(const Disc2D& disc, const AABB2D& aabb)
{
	LineSegment2D closestLine = aabb.GetEdgeFacingPoint(disc.m_center);
	
	return DoDiscAndLineSegmentOverlap(disc, closestLine);
}

bool DoDiscAndLineSegmentOverlap(const Disc2D& disc, const LineSegment2D& line)
{
	Vector2 pointOnLine = CalcClosestPointOnLine(disc.m_center, line);
	float distanceBetweenLinePointAndDiscCenter = CalcDistance(pointOnLine, disc.m_center);

	if (distanceBetweenLinePointAndDiscCenter <= disc.m_radius)
	{
		return true;
	}

	return false;
}

float GetDiscsOverlappingDistance(const Disc2D& a, const Disc2D& b)
{
	float distance = 0.f;
	float totalRadii = a.m_radius + b.m_radius;
	if (DoDiscsOverlap(a, b))
	{
		distance = CalcDistance(a.m_center, b.m_center);
		
		distance = totalRadii - distance;
	}

	return distance;
}

float GetDiscAndPointOverlappingDistance(const Disc2D& disc, const Vector2& point)
{
	//#CHECK better way of passing return value if point isn't in disc.
	float distance = 0.f;

	if (disc.IsPointInside(point))
	{
		distance = disc.m_radius - CalcDistance(disc.m_center, point);
	}
	return distance;
}

// #CHECK slightly broken if AABBs are overlapped completely
float GetAABB2DsOverlappingDistance(const AABB2D& collidingAABB, const AABB2D& collidedAABB)
{
	if (DoAABB2DsOverlap(collidedAABB, collidingAABB))
	{
		Vector2 collidingAABBCenter = collidingAABB.CalcCenter();
		Vector2 collidedAABBCenter = collidedAABB.CalcCenter();

		// Get closest point on the collided AABB from the center of the colliding AABB
		LineSegment2D collidedAABBLine = collidedAABB.GetEdgeFacingPoint(collidingAABBCenter);
		Vector2 closestPointOnEdgeOfCollidedAABB = CalcClosestPointOnLine(collidingAABBCenter, collidedAABBLine);

		// Same step, but get closests point on the colliding AABB from the center of collided AABB
		LineSegment2D collidingAABBLine = collidingAABB.GetEdgeFacingPoint(collidedAABB.CalcCenter());
		Vector2 closestPointOnEdgeOfCollidingAABB = CalcClosestPointOnLine(closestPointOnEdgeOfCollidedAABB, collidingAABBLine);

		Vector2 overlappingVector = closestPointOnEdgeOfCollidingAABB - closestPointOnEdgeOfCollidedAABB;
		float  overlappingDistance = abs(overlappingVector.CalcLength());

		return overlappingDistance;
	}
	else
	{
		return 0.f;
	}
}

Vector2 CalcClosestPointOnLine(const Vector2& point, const LineSegment2D& line)
{
	Vector2 pointToStartPoint = GetVectorBetweenTwoPoints(line.m_startPoint, point);
	float lineMagnitude = CalcDistanceSquared(line.m_startPoint, line.m_endPoint);

	float dotProductOfPointAndLine = DotProduct(pointToStartPoint, line.GetVectorLength());

	float distance = 0;

	if (lineMagnitude != 0)
	{
		distance = dotProductOfPointAndLine / lineMagnitude;
	}

	if (distance < 0)      
	{
		return line.m_startPoint;
	}
	else if (distance > 1) 
	{
		return line.m_endPoint;
	}
	else
	{
		return line.m_startPoint + line.GetVectorLength() * distance;
	}
}

Vector2 CalcNormalBetweenPoints(const Vector2& fromPoint, const Vector2& toPoint)
{
	return (toPoint - fromPoint).GetNormalizedVector();
}

Vector2 ClampVector2(Vector2& correctedVector, const Vector2& minimum, const Vector2& maximum)
{
	ClampFloat(correctedVector.x, minimum.x, maximum.x);
	ClampFloat(correctedVector.y, minimum.y, maximum.y);

	return correctedVector;
}

Vector3 ClampVector3(Vector3& correctedVector, const Vector3& minimum, const Vector3& maximum)
{
	ClampFloat(correctedVector.x, minimum.x, maximum.x);
	ClampFloat(correctedVector.y, minimum.y, maximum.y);
	ClampFloat(correctedVector.z, minimum.z, maximum.z);

	return correctedVector;
}

Vector2 RangeMapVector2(const Vector2& rawValue, const Vector2& oldMin, const Vector2& oldMax, const Vector2& newMin, const Vector2& newMax)
{
	Vector2 newValue = (rawValue - oldMin) / (oldMax - oldMin);

	newValue = newValue * (newMax - newMin) + newMin;

	return newValue;
}

inline void SwapValues(float& a, float& b)
{
	float temp = a;
	a = b;
	b = temp;
}

bool AreMostlyEqual(float a, float b, float epsilon)
{
	return fabsf(a - b) < epsilon;
}

float GetFractionPart(float a)
{
	int integerPart = (int)a;

	return (float)(a - integerPart);
}

Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	Quaternion normalizedA = a.GetNormalized();
	Quaternion normalizedB = b.GetNormalized();

	// Compute the cosine of the angle between the two vectors.
	float dot = DotProduct(normalizedA, normalizedB);

	const double DOT_THRESHOLD = 0.9995;

	// If the dot product is negative, the quaternions
	// have opposite handed-ness and slerp won't take
	// the shorter path. Fix by reversing one quaternion.
	if (dot < 0.0f)
	{
		normalizedB = -normalizedB;
		dot = -dot;
	}

	if (dot > DOT_THRESHOLD) 
	{
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.

		Quaternion result = normalizedA + t * (normalizedB - normalizedA);
		result.Normalize();
		return result;
	}	

	dot = ClampFloat(dot, -1.f, 1.f);         // Robustness: Stay within domain of acos()
	double thetaRadians = acos(dot);		  // theta = angle between input vectors
	double theta = thetaRadians * t;		  // theta = angle between v0 and result 

	Quaternion c = normalizedB - normalizedA * dot;
	c.Normalize();						// { v0, v2 } is now an orthonormal basis

	return normalizedA * cos(theta) + c * sin(theta);
}

Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
{
	return a + t * (b - a);
}

Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
{
	return a + t * (b - a);
}

Rgba Lerp(const Rgba& a, const Rgba& b, float t)
{
	float diffR = (float)(b.r - a.r);
	float diffG = (float)(b.g - a.g);
	float diffB = (float)(b.b - a.b);
	float diffA = (float)(b.a - a.a);

	float fractionR = diffR * t;
	float fractionG = diffG * t;
	float fractionB = diffB * t;
	float fractionA = diffA * t;
	
	float resultR = a.r + fractionR;
	float resultG = a.g + fractionG;
	float resultB = a.b + fractionB;
	float resultA = a.a + fractionA;

	Rgba result = Rgba((unsigned char)ClampFloat(resultR, 0, 255), (unsigned char)ClampFloat(resultG, 0, 255),
		(unsigned char)ClampFloat(resultB, 0, 255), (unsigned char)ClampFloat(resultA, 0, 255));

	return result;
}

Vector2 GetRandomPositionWithinRange(const Vector2& startPos, float range)
{
	float randomX = GetRandomFloatInRange(-range, range);
	float randomY = GetRandomFloatInRange(-range, range);

	return startPos + Vector2(randomX, randomY);
}
