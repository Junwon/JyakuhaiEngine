#include "Engine/Math/Vector3.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"

const Vector3 Vector3::ZERO = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::ONE = Vector3(1.f, 1.f, 1.f);

const Vector3 Vector3::X_AXIS = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::Y_AXIS = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::Z_AXIS = Vector3(0.f, 0.f, 1.f);

//CONSTRUCTORS
//============================================================

Vector3::Vector3()
{

}

Vector3::Vector3(const Vector3& copy) :
	x(copy.x),
	y(copy.y),
	z(copy.z)
{
}

Vector3::Vector3(float initialX, float initialY, float initialZ) :
	x(initialX),
	y(initialY),
	z(initialZ)
{
}

Vector3::Vector3(const Vector2& initialXY, float initialZ) :
	x(initialXY.x),
	y(initialXY.y),
	z(initialZ)
{
}

Vector3::Vector3(const Vector3& fromPoint, const Vector3& toPoint) :
	x(toPoint.x - fromPoint.x),
	y(toPoint.y - fromPoint.y),
	z(toPoint.z - fromPoint.z)
{
}

Vector3::Vector3(const Vector4& vec) :
	x(vec.x),
	y(vec.y),
	z(vec.z)
{
}

//ACCESSOR METHODS
//============================================================

float Vector3::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

//MUTATOR METHODS
//============================================================
Vector3 Vector3::Floor()
{
	x = (float)floor(x);
	y = (float)floor(y);
	z = (float)floor(z);

	return Vector3(x, y, z);
}

bool Vector3::IsMostlyEqualTo(const Vector3& comparison, float epsilon)
{
	float differenceBetweenX = fabsf(x - comparison.x);
	float differenceBetweenY = fabsf(y - comparison.y);
	float differenceBetweenZ = fabsf(z - comparison.z);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon && differenceBetweenZ < epsilon)
	{
		return true;
	}

	return false;
}

bool AreMostlyEqual(const Vector3& a, const Vector3& b, float epsilon)
{
	float differenceBetweenX = fabsf(a.x - b.x);
	float differenceBetweenY = fabsf(a.y - b.y);
	float differenceBetweenZ = fabsf(a.z - b.z);

	if (differenceBetweenX < epsilon && differenceBetweenY < epsilon && differenceBetweenZ < epsilon)
	{
		return true;
	}

	return false;
}

//OPERTOR METHODS
//============================================================
const Vector3 Vector3::operator*(const Matrix4& matrix) const
{
	Vector4 thisVector = Vector4(x, y, z, 1.f);
	Vector4 resultVector = Vector4::ZERO;

	resultVector.x = DotProduct(matrix.m_iBasis, thisVector);
	resultVector.y = DotProduct(matrix.m_jBasis, thisVector);
	resultVector.z = DotProduct(matrix.m_kBasis, thisVector);

	return resultVector.XYZ();
}

void DebugPrintVector(const Vector3& vector)
{
	DebuggerPrintlnf("X:%-+5.3f\t Y:%-+5.3f\t Z:%-+5.3f", vector.x, vector.y, vector.z);
}