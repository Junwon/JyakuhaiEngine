#include "Engine/Math/IntVector4.hpp"
#include <math.h>

const IntVector4 IntVector4::ZERO = IntVector4(0, 0, 0, 0);
const IntVector4 IntVector4::ONE = IntVector4(1, 1, 1, 1);

const IntVector4 IntVector4::X_AXIS = IntVector4(1, 0, 0, 0);
const IntVector4 IntVector4::Y_AXIS = IntVector4(0, 1, 0, 0);
const IntVector4 IntVector4::Z_AXIS = IntVector4(0, 0, 1, 0);
const IntVector4 IntVector4::W_AXIS = IntVector4(0, 0, 0, 1);

//CONSTRUCTORS
//============================================================

IntVector4::IntVector4(const IntVector4& copy) :
	x(copy.x),
	y(copy.y),
	z(copy.z)
{
}

IntVector4::IntVector4(int initialX, int initialY, int initialZ, int initialW) :
	x(initialX),
	y(initialY),
	z(initialZ),
	w(initialW)
{
}

IntVector4::IntVector4(const IntVector4& fromPoint, const IntVector4& toPoint) :
	x(toPoint.x - fromPoint.x),
	y(toPoint.y - fromPoint.y),
	z(toPoint.z - fromPoint.z),
	w(toPoint.w - fromPoint.w)
{
}

//ACCESSOR METHODS
//============================================================
const int* IntVector4::GetAsIntArray() const
{
	return &x;
}

int* IntVector4::GetAsIntArray()
{
	return &x;
}

int IntVector4::GetX() const
{
	return x;
}

int IntVector4::GetY() const
{
	return y;
}

int IntVector4::GetZ() const
{
	return z;
}

int IntVector4::GetW() const
{
	return w;
}

void IntVector4::GetXYZW(int& out_x, int& out_y, int& out_z, int& out_w) const
{
	out_x = x;
	out_y = y;
	out_z = z;
	out_w = w;
}

//MUTATOR METHODS
//============================================================
void IntVector4::SetX(int newX)
{
	x = newX;
}

void IntVector4::SetY(int newY)
{
	y = newY;
}

void IntVector4::SetZ(int newZ)
{
	z = newZ;
}

void IntVector4::SetW(int newW)
{
	w = newW;
}

void IntVector4::SetXYZW(int newX, int newY, int newZ, int newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}


void IntVector4::ScaleUniform(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void IntVector4::ScaleNonUniform(const IntVector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

const IntVector4 operator * (int scale, const IntVector4& vectorToScale)
{
	return IntVector4(vectorToScale.x * scale, vectorToScale.y * scale, vectorToScale.z * scale, vectorToScale.w * scale);
}

const IntVector4 GetVectorBetweenTwoPoints(const IntVector4& pointA, const IntVector4 pointB)
{
	return IntVector4(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z, pointB.w - pointA.w);
}

void IntVector4::InverseScaleNonUniform(const IntVector4& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

bool AreMostlyEqual(const IntVector4& a, const IntVector4& b, float epsilon)
{
	float differenceBetweenX = (float)fabs(a.x - b.x);
	float differenceBetweenY = (float)fabs(a.y - b.y);
	float differenceBetweenZ = (float)fabs(a.z - b.z);
	float differenceBetweenW = (float)fabs(a.w - b.w);

	if (differenceBetweenX <= epsilon && differenceBetweenY <= epsilon && differenceBetweenZ <= epsilon && differenceBetweenW <= epsilon)
	{
		return true;
	}

	return false;
}

IntVector4 IntVector4::GetXComponentVector() const
{
	return IntVector4(x, 0, 0, 0);
}

IntVector4 IntVector4::GetYComponentVector() const
{
	return IntVector4(0, y, 0, 0);

}

IntVector4 IntVector4::GetZComponentVector() const
{
	return IntVector4(0, 0, z, 0);

}

bool IntVector4::operator==(const IntVector4& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z && w == vectorToEqual.w) {
		return true;
	}
	else {
		return false;
	}
}

bool IntVector4::operator!=(const IntVector4& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

const IntVector4 IntVector4::operator-() const
{
	return IntVector4(-x, -y, -z, -w);
}

const IntVector4 IntVector4::operator+(const IntVector4& vectorToAdd) const
{
	return IntVector4(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z, w + vectorToAdd.w);
}

const IntVector4 IntVector4::operator-(const IntVector4& vectorToSubtract) const
{
	return IntVector4(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z, w - vectorToSubtract.w);
}

const IntVector4 IntVector4::operator*(int scale) const
{
	return IntVector4(x * scale, y * scale, z * scale, w * scale);
}

const IntVector4 IntVector4::operator*(const IntVector4& perAxisScaleFactors) const
{
	return IntVector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const IntVector4 IntVector4::operator/(int inverseScale) const
{
	//#CHECK make this faster with multiplication
	if (inverseScale != 0) {
		return IntVector4(x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale);
	}
	else {
		return IntVector4(x, y, z, w);
	}
}

void IntVector4::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void IntVector4::operator*=(const IntVector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void IntVector4::operator/=(int scale)
{
	float scaleRecipricol = (float)(1 / scale);
	x *= (int)scaleRecipricol;
	y *= (int)scaleRecipricol;
	z *= (int)scaleRecipricol;
	w *= (int)scaleRecipricol;
}

void IntVector4::operator/=(const IntVector4& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
	z /= perAxisScaleFactors.z;
	w /= perAxisScaleFactors.w;
}

void IntVector4::operator+=(const IntVector4& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
	w += vectorToAdd.w;
}

void IntVector4::operator-=(const IntVector4& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	w -= vectorToSubtract.w;
}
