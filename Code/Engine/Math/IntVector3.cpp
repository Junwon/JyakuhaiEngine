#include "Engine/Math/IntVector3.hpp"
#include <math.h>

const IntVector3 IntVector3::ZERO = IntVector3(0, 0, 0);
const IntVector3 IntVector3::ONE = IntVector3(1, 1, 1);

const IntVector3 IntVector3::X_AXIS = IntVector3(1, 0, 0);
const IntVector3 IntVector3::Y_AXIS = IntVector3(0, 1, 0);
const IntVector3 IntVector3::Z_AXIS = IntVector3(0, 0, 1);

//CONSTRUCTORS
//============================================================

IntVector3::IntVector3(const IntVector3& copy) :
	x(copy.x),
	y(copy.y),
	z(copy.z)
{
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ) :
	x(initialX),
	y(initialY),
	z(initialZ)
{
}

IntVector3::IntVector3(const IntVector3& fromPoint, const IntVector3& toPoint) :
	x(toPoint.x - fromPoint.x),
	y(toPoint.y - fromPoint.y),
	z(toPoint.z - fromPoint.z)
{
}

//ACCESSOR METHODS
//============================================================
const int* IntVector3::GetAsIntArray() const
{
	return &x;
}

int* IntVector3::GetAsIntArray()
{
	return &x;
}

int IntVector3::GetX() const
{
	return x;
}

int IntVector3::GetY() const
{
	return y;
}

int IntVector3::GetZ() const
{
	return z;
}

void IntVector3::GetXYZ(int& out_x, int& out_y, int& out_z) const
{
	out_x = x;
	out_y = y;
	out_z = z;
}

//MUTATOR METHODS
//============================================================
void IntVector3::SetX(int newX)
{
	x = newX;
}

void IntVector3::SetY(int newY)
{
	y = newY;
}

void IntVector3::SetZ(int newZ)
{
	z = newZ;
}

void IntVector3::SetXYZ(int newX, int newY, int newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}


void IntVector3::ScaleUniform(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void IntVector3::ScaleNonUniform(const IntVector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

const IntVector3 operator * (int scale, const IntVector3& vectorToScale)
{
	return IntVector3(vectorToScale.x * scale, vectorToScale.y * scale, vectorToScale.z * scale);
}

const IntVector3 GetVectorBetweenTwoPoints(const IntVector3& pointA, const IntVector3 pointB)
{
	return IntVector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
}

void IntVector3::InverseScaleNonUniform(const IntVector3& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

bool AreMostlyEqual(const IntVector3& a, const IntVector3& b, float epsilon)
{
	float differenceBetweenX = (float)fabs(a.x - b.x);
	float differenceBetweenY = (float)fabs(a.y - b.y);
	float differenceBetweenZ = (float)fabs(a.z - b.z);

	if (differenceBetweenX <= epsilon && differenceBetweenY <= epsilon && differenceBetweenZ <= epsilon)
	{
		return true;
	}

	return false;
}

IntVector3 IntVector3::GetXComponentVector() const
{
	return IntVector3(x, 0, 0);
}

IntVector3 IntVector3::GetYComponentVector() const
{
	return IntVector3(0, y, 0);

}

IntVector3 IntVector3::GetZComponentVector() const
{
	return IntVector3(0, 0, z);

}

bool IntVector3::operator==(const IntVector3& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z) {
		return true;
	}
	else {
		return false;
	}
}

bool IntVector3::operator!=(const IntVector3& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

const IntVector3 IntVector3::operator-() const
{
	return IntVector3(-x, -y, -z);
}

const IntVector3 IntVector3::operator+(const IntVector3& vectorToAdd) const
{
	return IntVector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

const IntVector3 IntVector3::operator-(const IntVector3& vectorToSubtract) const
{
	return IntVector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

const IntVector3 IntVector3::operator*(int scale) const
{
	return IntVector3(x * scale, y * scale, z * scale);
}

const IntVector3 IntVector3::operator*(const IntVector3& perAxisScaleFactors) const
{
	return IntVector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

const IntVector3 IntVector3::operator/(int inverseScale) const
{
	//#CHECK make this faster with multiplication
	if (inverseScale != 0) {
		return IntVector3(x / inverseScale, y / inverseScale, z / inverseScale);
	}
	else {
		return IntVector3(x, y, z);
	}
}

void IntVector3::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void IntVector3::operator*=(const IntVector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void IntVector3::operator/=(int scale)
{
	float scaleRecipricol = (float)(1 / scale);
	x *= (int)scaleRecipricol;
	y *= (int)scaleRecipricol;
	z *= (int)scaleRecipricol;
}

void IntVector3::operator/=(const IntVector3& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
	z /= perAxisScaleFactors.z;
}

void IntVector3::operator+=(const IntVector3& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

void IntVector3::operator-=(const IntVector3& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}
