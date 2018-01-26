#include "Engine/Math/Vector4.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4.hpp"

const Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);

const Vector4 Vector4::ONE = Vector4(1.f, 1.f, 1.f, 1.f);

//CONSTRUCTORS
//============================================================
Vector4::Vector4() :
	x(0.f),
	y(0.f),
	z(0.f),
	w(0.f)
{
}

Vector4::Vector4(const Vector4& copy) :
	x(copy.x),
	y(copy.y),
	z(copy.z),
	w(copy.w)
{
}

Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW) :
	x(initialX),
	y(initialY),
	z(initialZ),
	w(initialW)
{
}

Vector4::Vector4(const Vector4& fromPoint, const Vector4& toPoint) :
	x(toPoint.GetX() - fromPoint.GetX()),
	y(toPoint.GetY() - fromPoint.GetY()),
	z(toPoint.GetZ() - fromPoint.GetZ()),
	w(toPoint.GetW() - fromPoint.GetW())
{
}

Vector4::Vector4(const Vector2& initialXY, float initialZ, float initialW) :
	x(initialXY.x),
	y(initialXY.y),
	z(initialZ),
	w(initialW)
{
}

Vector4::Vector4(const Vector3& initialXYZ, float initialW) :
	x(initialXYZ.x),
	y(initialXYZ.y),
	z(initialXYZ.z),
	w(initialW)
{

}

//ACCESSOR METHODS
//============================================================
const float* Vector4::GetAsFloatArray() const
{
	return &x;
}

float* Vector4::GetAsFloatArray()
{
	return &x;
}

float Vector4::GetX() const
{
	return x;
}

float Vector4::GetY() const
{
	return y;
}

float Vector4::GetZ() const
{
	return z;
}

float Vector4::GetW() const
{
	return w;
}

void Vector4::GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const
{
	out_x = x;
	out_y = y;
	out_z = z;
	out_w = w;
}

Vector2 Vector4::XY() const
{
	return Vector2(x, y);
}

Vector3 Vector4::XYZ() const
{
	return Vector3(x, y, z);
}

float Vector4::CalcLength3D() const
{
	return sqrtf(CalcLengthSquared3D());
}

float Vector4::CalcLength4D() const
{
	return sqrtf(CalcLengthSquared4D());
}

float Vector4::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

float Vector4::CalcLengthSquared3D() const
{
	return (x*x) + (y*y) + (z*z);
}

float Vector4::CalcLengthSquared4D() const
{
	return (x*x) + (y*y) + (z*z) + (w*w);
}

float Vector4::CalcLengthSquared() const
{
	return (x*x) + (y*y) + (z*z) + (w*w);
}

//MUTATOR METHODS
//============================================================
void Vector4::SetX(float newX)
{
	x = newX;
}

void Vector4::SetY(float newY)
{
	y = newY;
}

void Vector4::SetZ(float newZ)
{
	z = newZ;
}

void Vector4::SetW(float newW)
{
	w = newW;
}

void Vector4::SetXYZW(float newX, float newY, float newZ, float newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

void Vector4::SetXYZ(const Vector3& xyz)
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
}

float Vector4::Normalize3D()
{
	float length = CalcLength3D();

	if (length > 0.0f) {
		float scale = 1.0f / length;
		x *= scale;
		y *= scale;
		z *= scale;
		return length;
	}
	else {
		return 0.0f;
	}
}

float Vector4::Normalize4D()
{
	float length = CalcLength4D();

	if (length > 0.0f) {
		float scale = 1.0f / length;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return length;
	}
	else {
		return 0.0f;
	}
}

float Vector4::Normalize()
{
	float length = CalcLength();

	if (length > 0.0f) {
		float scale = 1.0f / length;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return length;
	}
	else {
		return 0.0f;
	}
}

void Vector4::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::ScaleNonUniform(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

float CalcDistance(const Vector4& positionA, const Vector4& positionB)
{
	Vector4 displacement = positionA - positionB;
	return displacement.CalcLength4D();
}

float CalcDistanceSquared(const Vector4& posA, const Vector4& posB)
{
	float calcDistance = CalcDistance(posA, posB);
	return calcDistance * calcDistance;
}

const Vector4 operator * (float scale, const Vector4& vectorToScale)
{
	return Vector4(vectorToScale.x * scale, vectorToScale.y * scale, vectorToScale.z * scale, vectorToScale.w * scale);
}

const Vector4 GetVectorBetweenTwoPoints(const Vector4& pointA, const Vector4 pointB)
{
	return Vector4(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z, pointB.w - pointA.w);
}

//#CHECK
void Vector4::InverseScaleNonUniform(const Vector4& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

bool Vector4::IsMostlyEqualTo(const Vector4& comparison, float epsilon)
{
	float differenceBetweenX = fabsf(x - comparison.x);
	float differenceBetweenY = fabsf(y - comparison.y);
	float differenceBetweenZ = fabsf(z - comparison.z);
	float differenceBetweenW = fabsf(w - comparison.w);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon && differenceBetweenZ < epsilon && differenceBetweenW < epsilon)
	{
		return true;
	}

	return false;
}

bool AreMostlyEqual(const Vector4& a, const Vector4& b, float epsilon)
{
	float differenceBetweenX = fabsf(a.x - b.x);
	float differenceBetweenY = fabsf(a.y - b.y);
	float differenceBetweenZ = fabsf(a.z - b.z);
	float differenceBetweenW = fabsf(a.w - b.w);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon && differenceBetweenZ < epsilon && differenceBetweenW < epsilon)
	{
		return true;
	}

	return false;
}

Vector4 Vector4::GetXComponentVector() const
{
	return Vector4(x, 0.f, 0.f, 0.f);
}

Vector4 Vector4::GetYComponentVector() const
{
	return Vector4(0.f, y, 0.f, 0.f);
}

Vector4 Vector4::GetZComponentVector() const
{
	return Vector4(0.f, 0.f, z, 0.f);
}

Vector4 Vector4::GetWComponentVector() const
{
	return Vector4(0.f, 0.f, 0.f, w);
}

void Vector4::operator-=(const Vector4& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	w -= vectorToSubtract.w;
}

void Vector4::operator+=(const Vector4& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
	w += vectorToAdd.w;
}

void Vector4::operator*=(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void Vector4::operator-=(float difference)
{
	x -= difference;
	y -= difference;
	z -= difference;
	w -= difference;
}

void Vector4::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

const Vector4 Vector4::operator/(float inverseScale) const
{
	if (inverseScale != 0) {
		float inverseScaleRecipricol = 1 / inverseScale;
		return Vector4(x * inverseScaleRecipricol, y * inverseScaleRecipricol, z * inverseScaleRecipricol, w * inverseScaleRecipricol);
	}
	else {
		return Vector4(x, y, z, w);
	}
}

const Vector4 Vector4::operator*(const Vector4& perAxisScaleFactors) const
{
	return Vector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const Vector4 Vector4::operator*(float scale) const
{
	return Vector4(x * scale, y * scale, z * scale, w * scale);
}

const Vector4 Vector4::operator-(const Vector4& vectorToSubtract) const
{
	return Vector4(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z, w - vectorToSubtract.w);
}

const Vector4 Vector4::operator+(const Vector4& vectorToAdd) const
{
	return Vector4(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z, w + vectorToAdd.w);
}

bool Vector4::operator!=(const Vector4& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

bool Vector4::operator==(const Vector4& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z && w == vectorToEqual.w) {
		return true;
	}
	else {
		return false;
	}
}

const Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

void Vector4::operator/=(float scale)
{
	float scaleRecipricol = 1 / scale;

	x *= scaleRecipricol;
	y *= scaleRecipricol;
	z *= scaleRecipricol;
	w *= scaleRecipricol;
}

void Vector4::operator/=(const Vector4& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
	z /= perAxisScaleFactors.z;
	w /= perAxisScaleFactors.w;
}

const Vector4 Vector4::operator*(const Matrix4& matrix) const
{
	Vector4 thisVector = Vector4(x, y, z, w);
	Vector4 resultVector = Vector4::ZERO;

	resultVector.x = DotProduct(matrix.m_iBasis, thisVector);
	resultVector.y = DotProduct(matrix.m_jBasis, thisVector);
	resultVector.z = DotProduct(matrix.m_kBasis, thisVector);
	resultVector.w = DotProduct(matrix.m_translation, thisVector);

	return resultVector;
}
