#pragma once
#include "Math.h"

class Vector2;
class Vector3;
class Vector4;
class LineSegment2D;
class AABB2D;
class Disc2D;
class Quaternion;
class Rgba;

//============================================================
const float M_PI = 3.1415926535897932384626433832795f;
const float fSQRT_3_OVER_3 = 0.57735026918962576450914878050196f;
//============================================================

#define BIT(x) (1 << (x))

float ConvertRadiansToDegrees(float radians);

float ConvertDegreesToRadians(float degrees);

int GetRandomIntLessThan(int maxValueNotInclusive);

unsigned int GetRandomIntLessThan(unsigned int maxValueNotInclusive);

int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive);

float GetRandomFloatZeroToOne();

float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive);

float GetRandomFloatLessThan(float maxValueNotInclusive);

bool IsPercentChance(float probability);

float CosDegrees(float degrees);

float SinDegrees(float degrees);

float Atan2Degrees(float y, float x);

float DotProduct(const Vector2& a, const Vector2& b);

float DotProduct(const Vector3& a, const Vector3& b);

float DotProduct(const Vector4& a, const Vector4& b);

float DotProduct(const Quaternion& a, const Quaternion& b);

/*bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b);*/

/*bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);*/

bool DoDiscAndAABBOverlap(const Disc2D& disc, const AABB2D& aabb);

bool DoDiscAndLineSegmentOverlap(const Disc2D& disc, const LineSegment2D& line);

float GetDiscsOverlappingDistance(const Disc2D& a, const Disc2D& b);

float GetDiscAndPointOverlappingDistance(const Disc2D& disc, const Vector2& point);

float GetAABB2DsOverlappingDistance(const AABB2D& collidingAABB, const AABB2D& collidedAABB);

Vector2 CalcClosestPointOnLine(const Vector2& point, const LineSegment2D& line);

Vector2 CalcNormalBetweenPoints(const Vector2& fromPoint, const Vector2& toPoint);

int ClampInt(int& correctedMagnitude, int minimum, int maximum);

float ClampFloat(float& correctedMagnitude, float minimum, float maximum);

Vector2 ClampVector2(Vector2& correctedVector, const Vector2& minimum, const Vector2& maximum);

Vector3 ClampVector3(Vector3& correctedVector, const Vector3& minimum, const Vector3& maximum);

template <typename T>
T RangeMap(T rawValue, T oldMin, T oldMax, T newMin, T newMax);

float RangeMapFloat(float& rawValue, float oldMin, float oldMax, float newMin, float newMax);

int RangeMapInt(int& rawValue, int oldMin, int oldMax, int newMin, int newMax);

Vector2 RangeMapVector2(const Vector2& rawValue, const Vector2& oldMin, const Vector2& oldMax, const Vector2& newMin, const Vector2& newMax);

float GetAbsoluteValue(float value);

void TurnToward(float& currentDegrees, float goalDegree, float maxTurnDegrees);

float CalcShortestAngularDistance(float startDegrees, float endDegrees);

bool IsValueBetween(int value, int minInclusive, int maxInclusive);

bool IsValueBetween(float value, float minInclusive, float maxInclusive);

float SmoothStep3(float t);

void SwapValues(float& a, float& b);

bool AreMostlyEqual(float a, float b, float epsilon = 0.01f);

float GetFractionPart(float a);

float Max(float a, float b);

float Min(float a, float b);

int Ceiling(float a);

int Floor(float a);

float Lerp(float a, float b, float t);

Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

Rgba Lerp(const Rgba& a, const Rgba& b, float t);

float FastLog(float a);

Vector2 GetRandomPositionWithinRange(const Vector2& startPos, float range);
//////////////////////////////////////////////////////////////////////////

inline float ConvertRadiansToDegrees(float radians)
{
	float newDegrees = radians * (180.0f / M_PI);

	return newDegrees;
}

inline float ConvertDegreesToRadians(float degrees)
{
	return degrees * (M_PI / 180.0f);
}

inline int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive)
{
	int numPossibilities = maxValueInclusive - minValueInclusive + 1;
	return minValueInclusive + GetRandomIntLessThan(numPossibilities);
}

inline float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive)
{
	float numPossibilities = maximumInclusive - minimumInclusive;
	return minimumInclusive + (numPossibilities * GetRandomFloatZeroToOne());
}

inline float GetRandomFloatLessThan(float maxValueNotInclusive)
{
	GetRandomFloatInRange(0.f, maxValueNotInclusive);
}

inline bool IsPercentChance(float probability)
{
	float roll = GetRandomFloatZeroToOne();
	return (roll < probability);
}

inline int ClampInt(int& correctedMagnitude, int minimum, int maximum)
{
	if (correctedMagnitude < minimum)
	{
		correctedMagnitude = minimum;
	}
	else if (correctedMagnitude > maximum)
	{
		correctedMagnitude = maximum;
	}

	return correctedMagnitude;
}

inline float ClampFloat(float& correctedMagnitude, float minimum, float maximum)
{
	if (correctedMagnitude < minimum)
	{
		correctedMagnitude = minimum;
	}
	else if (correctedMagnitude > maximum)
	{
		correctedMagnitude = maximum;
	}

	return correctedMagnitude;
}

template <typename T>
inline T RangeMap(T rawValue, T oldMin, T oldMax, T newMin, T newMax)
{
	float newValue = (rawValue - oldMin) / (oldMax - oldMin);

	newValue = newValue * (newMax - newMin) + newMin;

	return newValue;
}

inline float RangeMapFloat(float& rawValue, float oldMin, float oldMax, float newMin, float newMax)
{
	rawValue = (rawValue - oldMin) / (oldMax - oldMin);

	rawValue = rawValue * (newMax - newMin) + newMin;

	return rawValue;
}

inline int RangeMapInt(int& rawValue, int oldMin, int oldMax, int newMin, int newMax)
{
	rawValue = (rawValue - oldMin) / (oldMax - oldMin);

	rawValue = rawValue * (newMax - newMin) + newMin;

	return rawValue;
}

inline float GetAbsoluteValue(float value)
{
	if (value < 0)
	{
		value *= -1;
	}

	return value;
}

inline void TurnToward(float& currentDegrees, float goalDegree, float maxTurnDegrees)
{
	float signedAngularDist = CalcShortestAngularDistance(currentDegrees, goalDegree);

	if (fabs(signedAngularDist) <= maxTurnDegrees)
	{
		currentDegrees = goalDegree;
	}
	else if (signedAngularDist > 0)
	{
		currentDegrees += maxTurnDegrees;
	}
	else
	{
		currentDegrees -= maxTurnDegrees;
	}
}

inline float CalcShortestAngularDistance(float startDegrees, float endDegrees)
{
	float angularDistance = endDegrees - startDegrees;

	while (angularDistance > 180.f)
	{
		angularDistance -= 360.f;
	}

	while (angularDistance < -180.f)
	{
		angularDistance += 360.f;
	}

	return angularDistance;
}

inline bool IsValueBetween(int value, int minInclusive, int maxInclusive)
{
	if (value >= minInclusive && value <= maxInclusive)
	{
		return true;
	}
	return false;
}

inline bool IsValueBetween(float value, float minInclusive, float maxInclusive)
{
	if (value >= minInclusive && value <= maxInclusive)
	{
		return true;
	}
	return false;
}

inline float SmoothStep3(float t)
{
	float a = t * t;
	float b = 1 - ((1 - t) * (1 - t));
	return ((1 - t) * a) + (t * b);
}

inline float Max(float a, float b)
{
	return a > b ? a : b;
}

inline float Min(float a, float b)
{
	return a < b ? a : b;
}

inline int Ceiling(float a)
{
	int value = (int)a;

	if (a == (float)value)
	{
		return value;
	}
	return value + 1;
}

inline int Floor(float a)
{
	int value = (int)a;

	if (a == (float)value)
	{
		return value;
	}
	return value - 1;
}

inline float Lerp(float a, float b, float t)
{
	return t * (b - a) + a;
}

inline float FastLog(float a)
{
	int* const exponentPtr = reinterpret_cast<int*>(&a);
	int x = *exponentPtr;
	const int log2 = ((x >> 23) & 255) - 128;
	x &= ~(255 << 23);
	x += 127 << 23;
	*exponentPtr = x;

	a = ((-1.0f / 3) * a + 2) * a - 2.0f / 3;

	return (a + log2);
}