#include "Engine/Math/IntVector2.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

const IntVector2 IntVector2::ZERO = IntVector2(0, 0);
const IntVector2 IntVector2::ONE = IntVector2(1, 1);

IntVector2::IntVector2()
{
}

IntVector2::IntVector2(const IntVector2& copy)
{
	x = copy.x;
	y = copy.y;
}

IntVector2::IntVector2(int initialX, int initialY) :
	x(initialX),
	y(initialY)
{
}

IntVector2::IntVector2(const IntVector2& fromPoint, const IntVector2& toPoint) :
	x(toPoint.GetX() - fromPoint.GetX()),
	y(toPoint.GetY() - fromPoint.GetY())
{
}

float IntVector2::CalcLength() const
{
	return sqrtf((float)CalcLengthSquared());
}

float IntVector2::CalcHeadingRadians() const
{
	return (float)atan2(y, x);
}

float IntVector2::CalcHeadingDegrees() const
{
	float radians = CalcHeadingRadians();
	return ConvertRadiansToDegrees(radians);
}

void IntVector2::Rotate90Degrees()
{
	//#CHECK
}

void IntVector2::RotateNegative90Degrees()
{
	//#CHECK
}

void IntVector2::RotateDegrees(float degrees)
{
	float radians = (float) ConvertDegreesToRadians(degrees);
	RotateRadians(radians);
}

void IntVector2::RotateRadians(float radians)
{
	float lengthR = CalcLength();
	float newTheta = (float)atan2(y, x) + radians;
	x = (int)floor(lengthR * cos(newTheta));
	y = (int) floor(lengthR * sin(newTheta));
}

// int IntVector2::Normalize()
// {
// 	//#CHECK
// }
// 
// int IntVector2::SetLength(int newLength)
// {
// 
// }

/*
void IntVector2::SetHeadingDegrees(int headingDegrees)
{

}

void IntVector2::SetHeadingRadians(int headingRadians)
{

}

void IntVector2::SetUnitLengthAndHeadingDegrees(int headingDegrees)
{

}

void IntVector2::SetUnitLengthAndHeadingRadians(int headingRadians)
{

}

void IntVector2::SetLengthAndHeadingDegrees(int newLength, int headingDegrees)
{

}

void IntVector2::SetLengthAndHeadingRadians(int newLength, int headingRadians)
{

}

bool IntVector2::IsMostlyEqualTo(const Vector2& comparison, int epsilon / *= 0.001f* /)
{

}

const Vector2 IntVector2::GetNormalizedVector() const
{

}*/

IntVector2 IntVector2::GetXComponentVector() const
{
	return IntVector2(x, 0);
}

IntVector2 IntVector2::GetYComponentVector() const
{
	return IntVector2(0, y);
}

Vector2 IntVector2::GetAsFloats()
{
	return Vector2((float)x, (float)y);
}

int CalcDistance(const IntVector2& positionA, const IntVector2& positionB)
{
	IntVector2 displacement = positionA - positionB;
	return (int)displacement.CalcLength();
}

int CalcDistanceSquared(const IntVector2& posA, const IntVector2& posB)
{
	IntVector2 displacement = posA - posB;
	return displacement.CalcLengthSquared();
}

const IntVector2 IntVector2::operator = (const Vector2& floatVector2) const
{
	return IntVector2((int)floatVector2.x, (int)floatVector2.y);
}