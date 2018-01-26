#include "Engine/Math/Vector2.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

const Vector2 Vector2::ZERO = Vector2(0.0f, 0.0f);
const Vector2 Vector2::CENTER = Vector2(0.5f, 0.5f);
const Vector2 Vector2::ONE = Vector2(1.0f, 1.0f);
const Vector2 Vector2::NEGATIVE_ONE = Vector2(-1.f, -1.f);

//CONSTRUCTORS
//============================================================
Vector2::Vector2()
{

}

Vector2::Vector2(const Vector2& copy) :
	x(copy.x),
	y(copy.y)
{
}

Vector2::Vector2(float initialX, float initialY) : 
	x(initialX), 
	y(initialY)
{
}

Vector2::Vector2(const Vector2& fromPoint, const Vector2& toPoint) :
	x(toPoint.GetX() - fromPoint.GetX()),
	y(toPoint.GetY() - fromPoint.GetY())
{
}

Vector2::Vector2(const IntVector2& copy) :
	x((float)copy.x),
	y((float)copy.y)
{
}

//ACCESSOR METHODS
//============================================================

float Vector2::CalcHeadingDegrees() const
{
	float radians = CalcHeadingRadians();
	return ConvertRadiansToDegrees(radians);
}

float Vector2::CalcHeadingRadians() const
{
	return (float)atan2(y, x);
}

float Vector2::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

//MUTATOR METHODS
//============================================================

void Vector2::RotateDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	RotateRadians(radians);
}

void Vector2::RotateRadians(float radians)
{
	float lengthR = CalcLength();
	float newTheta = (float)atan2(y, x) + radians;
	x = lengthR * (float)cos(newTheta);
	y = lengthR * (float)sin(newTheta);
}

float Vector2::SetLength(float newLength)
{
	float theta = (float)atan2(y, x);
	x = newLength * (float)cos(theta);
	y = newLength * (float)sin(theta);
	return CalcLength();
}

void Vector2::SetHeadingDegrees(float headingDegrees)
{
	float radians = ConvertDegreesToRadians(headingDegrees);
	SetHeadingRadians(radians);
}

void Vector2::SetHeadingRadians(float headingRadians)
{
	float lengthR = CalcLength();
	x = lengthR * (float)cos(headingRadians);
	y = lengthR * (float)sin(headingRadians);
}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	Normalize();
	SetHeadingDegrees(headingDegrees);
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	Normalize();
	SetHeadingRadians(headingRadians);
}

void Vector2::SetLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	float headingRadians = ConvertDegreesToRadians(headingDegrees);
	x = newLength * (float)cos(headingRadians);
	y = newLength * (float)sin(headingRadians);
}

void Vector2::SetLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * (float)cos(headingRadians);
	y = newLength * (float)sin(headingRadians);
}

//FRIENDS METHODS
//============================================================

float CalcDistance(const Vector2& positionA, const Vector2& positionB)
{
	Vector2 displacement = positionA - positionB;
	return displacement.CalcLength();
}

float CalcDistanceSquared(const Vector2& posA, const Vector2& posB)
{
	float calcDistance = CalcDistance(posA, posB);
	return calcDistance * calcDistance;
}

//HELPER METHODS
//============================================================

bool Vector2::IsMostlyEqualTo(const Vector2& comparison, float epsilon)
{
	float differenceBetweenX = fabsf(x - comparison.x);
	float differenceBetweenY = fabsf(y - comparison.y);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon)
	{
		return true;
	}

	return false;
}

bool AreMostlyEqual(const Vector2& a, const Vector2& b, float epsilon)
{
	float differenceBetweenX = fabsf(a.x - b.x);
	float differenceBetweenY = fabsf(a.y - b.y);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon)
	{
		return true;
	}

	return false;
}

const Vector2 Vector2::GetNormalizedVector() const
{
	float radians = (float)atan2(y, x);
	return Vector2((float)cos(radians), (float)sin(radians));
}