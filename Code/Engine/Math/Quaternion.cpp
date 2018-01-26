#include "Engine/Math/Quaternion.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Quaternion::Quaternion() :
	m_w(1.f),
	m_x(0.f),
	m_y(0.f),
	m_z(0.f)
{
}

Quaternion::Quaternion(float angleDegrees, float normalizedXComponent, float normalizedYComponent, float normalizedZComponent)
{
	float sinTheta = SinDegrees(angleDegrees * 0.5f);

	m_w = CosDegrees(angleDegrees * 0.5f);
	m_x = normalizedXComponent * sinTheta;
	m_y = normalizedYComponent * sinTheta;
	m_z = normalizedZComponent * sinTheta;
}

Quaternion::Quaternion(float angleDegrees, const Vector3& normalizedVector)
{
	float sinTheta = SinDegrees(angleDegrees * 0.5f);

	m_w = CosDegrees(angleDegrees * 0.5f);
	m_x = normalizedVector.x * sinTheta;
	m_y = normalizedVector.y * sinTheta;
	m_z = normalizedVector.z * sinTheta;
}

Quaternion::Quaternion(const Quaternion& q) :
	m_w(q.m_w),
	m_x(q.m_x),
	m_y(q.m_y),
	m_z(q.m_z)
{
}

const Quaternion Quaternion::operator*(const Quaternion& other)
{
	Quaternion q;

	q.m_w = m_w * other.m_w - DotProduct(m_axis, other.m_axis);
	q.m_axis = m_axis * other.m_w + other.m_axis * m_w + CrossProduct(m_axis, other.m_axis);

	return q;
}

void Quaternion::LoadFromMatrix(const Matrix4& matrix)
{
	/* Relationship of rotation matrix and quaternion component
	* 1. M_12 + M_21 = 4xy
	* 2. M_32 + M_23 = 4yz
	* 3. M_13 + M_31 = 4zx
	* 4. M_23 - M_32 = 4wx
	* 5. M_31 - M_13 = 4wy 
	* 6. M_12 - M_21 = 4wz
	*/

	float firstDiagonalElement = matrix.m_element[0];
	float secondDiagonalElement = matrix.m_element[5];
	float thirdDiagonalElement = matrix.m_element[10];

	float trace = matrix.CalcTrace();

	if (trace >= 1.f)
	{
		float traceSqrt = sqrtf(trace);
		float fourW = 2.f * traceSqrt;
		float fourWInverse = 1.f / fourW;

		m_w = traceSqrt * 0.5f;
		m_x = (matrix.m_element[6] - matrix.m_element[9]) * fourWInverse;
		m_y = (matrix.m_element[8] - matrix.m_element[2]) * fourWInverse;
		m_z = (matrix.m_element[1] - matrix.m_element[4]) * fourWInverse;
	}
	else if (firstDiagonalElement > secondDiagonalElement && firstDiagonalElement > thirdDiagonalElement)
	{
		float calculatedX = firstDiagonalElement - (secondDiagonalElement + thirdDiagonalElement) + 1.f;
		m_x = sqrtf(calculatedX) * 0.5f;

		float divisor = 1.f / (4.f * m_x);

		m_y = (matrix.m_element[1] + matrix.m_element[4]) * divisor;
		m_z = (matrix.m_element[2] + matrix.m_element[8]) * divisor;
		m_w = (matrix.m_element[6] - matrix.m_element[9]) * divisor;
	}
	else if (secondDiagonalElement > thirdDiagonalElement)
	{
		float calculatedY = secondDiagonalElement - (firstDiagonalElement + thirdDiagonalElement) + 1.f;
		m_y = sqrtf(calculatedY) * 0.5f;

		float divisor = 1.f / (4.f * m_y);

		m_x = (matrix.m_element[4] + matrix.m_element[1]) * divisor;
		m_z = (matrix.m_element[6] + matrix.m_element[9]) * divisor;
		m_w = (matrix.m_element[8] - matrix.m_element[2]) * divisor;
	}
	else
	{
		float calculatedZ = thirdDiagonalElement - (firstDiagonalElement + secondDiagonalElement) + 1.f;
		m_z = sqrtf(calculatedZ) * 0.5f;

		float divisor = 1.f / (4.f * m_z);

		m_x = (matrix.m_element[8] + matrix.m_element[2]) * divisor;
		m_y = (matrix.m_element[9] + matrix.m_element[6]) * divisor;
		m_w = (matrix.m_element[1] - matrix.m_element[4]) * divisor;
	}
}

Matrix4 Quaternion::ConvertToMatrix() const
{
	Matrix4 matrix;

	matrix.m_iBasis.x = 1.f - (2.f * (m_y * m_y + m_z * m_z));
	matrix.m_iBasis.y = 2.f * (m_x * m_y + m_w * m_z);
	matrix.m_iBasis.z = 2.f * (m_x * m_z - m_w * m_y);

	matrix.m_jBasis.x = 2.f * (m_y * m_x - m_w * m_z);
	matrix.m_jBasis.y = 1.f - (2.f * (m_x * m_x + m_z * m_z));
	matrix.m_jBasis.z = 2.f * (m_y * m_z + m_w * m_x);

	matrix.m_kBasis.x = 2.f * (m_z * m_x + m_w * m_y);
	matrix.m_kBasis.y = 2.f * (m_z * m_y - m_w * m_x);
	matrix.m_kBasis.z = 1.f - (2.f * (m_y * m_y + m_x * m_x));

	return matrix;
}

Quaternion Quaternion::GetConjugate()
{
	return Quaternion(m_w, -m_x, -m_y, -m_z);
}

float Quaternion::CalcMagnitude()
{
	return sqrtf(m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z);
}

void Quaternion::Normalize()
{
	float magnitude = CalcMagnitude();

	if (magnitude > 0.0f) 
	{
		float magnitudeInverse = 1.0f / magnitude;
		m_w *= magnitudeInverse;
		m_x *= magnitudeInverse;
		m_y *= magnitudeInverse;
		m_z *= magnitudeInverse;
	}
}

Quaternion Quaternion::GetNormalized() const
{
	Quaternion q = (*this);
	q.Normalize();
	return q;
}

void PrintQuaternion(const Quaternion& q)
{
	DebuggerPrintlnf("w:%4.4f\tx:%4.4f\ty:%4.4f\tz:%4.4f", q.m_w, q.m_x, q.m_y, q.m_z);
}