#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Quaternion.hpp"

const Matrix4 Matrix4::IDENTITY = Matrix4(Vector4(1.0f, 0.f, 0.f, 0.f), Vector4(0.f, 1.0f, 0.f, 0.f), Vector4(0.f, 0.f, 1.0f, 0.f), Vector4(0.f, 0.f, 0.f, 1.f));

/** THIS IS COLUMN MAJOR.
*/

Matrix4::Matrix4() :
	m_iBasis(1, 0, 0, 0),
	m_jBasis(0, 1, 0, 0),
	m_kBasis(0, 0, 1, 0),
	m_translation(0, 0, 0, 1)
{
}

Matrix4::Matrix4(float element1, float element5, float element9, float element13,
				float element2, float element6, float element10, float element14,
				float element3, float element7, float element11, float element15,
				float element4, float element8, float element12, float element16) :
	m_iBasis(element1, element2, element3, element4),
	m_jBasis(element5, element6, element7, element8),
	m_kBasis(element9, element10, element11, element12),
	m_translation(element13, element14, element15, element16)
{
}

Matrix4::Matrix4(const float* arrayOfFloats) :
	m_iBasis(arrayOfFloats[0], arrayOfFloats[4], arrayOfFloats[8], arrayOfFloats[12]),
	m_jBasis(arrayOfFloats[1], arrayOfFloats[5], arrayOfFloats[9], arrayOfFloats[13]),
	m_kBasis(arrayOfFloats[2], arrayOfFloats[6], arrayOfFloats[10], arrayOfFloats[14]),
	m_translation(arrayOfFloats[3], arrayOfFloats[7], arrayOfFloats[11], arrayOfFloats[15])
{
}

Matrix4::Matrix4(const Matrix4& copy) :
	m_iBasis(copy.m_iBasis),
	m_jBasis(copy.m_jBasis),
	m_kBasis(copy.m_kBasis),
	m_translation(copy.m_translation)
{
}

Matrix4::Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation) :
	m_iBasis(iBasis.GetX(), iBasis.GetY(), 0.f, 0.f),
	m_jBasis(jBasis.GetX(), jBasis.GetY(), 0.f, 0.f),
	m_kBasis(Vector4::ZERO),
	m_translation(translation.GetX(), translation.GetY(), 0.f, 1.f)
{
}

Matrix4::Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation) :
	m_iBasis(iBasis.GetX(), iBasis.GetY(), iBasis.GetZ(), 0.f),
	m_jBasis(jBasis.GetX(), jBasis.GetY(), jBasis.GetZ(), 0.f),
	m_kBasis(kBasis.GetX(), kBasis.GetY(), kBasis.GetZ(), 0.f),
	m_translation(translation.GetX(), translation.GetY(), translation.GetZ(), 1.f)
{
}

Matrix4::Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation) :
	m_iBasis(iBasis.GetX(), iBasis.GetY(), iBasis.GetZ(), iBasis.GetW()),
	m_jBasis(jBasis.GetX(), jBasis.GetY(), jBasis.GetZ(), jBasis.GetW()),
	m_kBasis(kBasis.GetX(), kBasis.GetY(), kBasis.GetZ(), kBasis.GetW()),
	m_translation(translation.GetX(), translation.GetY(), translation.GetZ(), translation.GetW())
{
}

bool Matrix4::IsMostlyEqualTo(const Matrix4& comparison, float epsilon)
{
	if (m_iBasis.IsMostlyEqualTo(comparison.m_iBasis, epsilon) &&
		m_jBasis.IsMostlyEqualTo(comparison.m_jBasis, epsilon) &&
		m_kBasis.IsMostlyEqualTo(comparison.m_kBasis, epsilon) &&
		m_translation.IsMostlyEqualTo(comparison.m_translation, epsilon))
	{
		return true;
	}

	return false;
}

bool Matrix4::operator==(const Matrix4& mat) const
{
	if (m_iBasis == mat.m_iBasis &&
		m_jBasis == mat.m_jBasis &&
		m_kBasis == mat.m_kBasis &&
		m_translation == mat.m_translation)
	{
		return true;
	}

	return false;
}

bool Matrix4::operator!=(const Matrix4& mat) const
{
	return !(*this == mat);
}

const Matrix4 Matrix4::operator +(const Matrix4& matrixToAdd) const
{
	return Matrix4(m_iBasis + matrixToAdd.m_iBasis, 
		m_jBasis + matrixToAdd.m_jBasis, 
		m_kBasis + matrixToAdd.m_kBasis, 
		m_translation + matrixToAdd.m_translation);
}

void Matrix4::MakeIdentity()
{
	m_iBasis = Vector4(1.f, 0.f, 0.f, 0.f);
	m_jBasis = Vector4(0.f, 1.f, 0.f, 0.f);
	m_kBasis = Vector4(0.f, 0.f, 1.f, 0.f);
	m_translation = Vector4(0.f, 0.f, 0.f, 1.f);
}

void Matrix4::operator=(const Matrix4& copy)
{
	m_iBasis = copy.m_iBasis;
	m_jBasis = copy.m_jBasis;
	m_kBasis = copy.m_kBasis;
	m_translation = copy.m_translation;
}

const float* Matrix4::GetAsFloatArray() const
{
	return &m_iBasis.x;
}

float* Matrix4::GetAsFloatArray()
{
	return &m_iBasis.x;
}

Vector3 Matrix4::GetIBasis() const
{
	return m_iBasis.XYZ();
}

Vector3 Matrix4::GetJBasis() const
{
	return m_jBasis.XYZ();
}

Vector3 Matrix4::GetKBasis() const
{
	return m_kBasis.XYZ();
}

Vector3 Matrix4::GetTranslation() const
{
	return m_translation.XYZ();
}

Vector3 Matrix4::GetScale() const
{
	return Vector3(m_iBasis.CalcLength(), m_jBasis.CalcLength(), m_kBasis.CalcLength());
}

Matrix4 Matrix4::GetOrthoNormalizeAboutZ() const
{
	Matrix4 orthoNormalize = Matrix4(m_iBasis.XYZ(), m_jBasis.XYZ(), m_kBasis.XYZ());

	orthoNormalize.m_kBasis.Normalize();

	Vector3 projectionK_On_I = DotProduct(orthoNormalize.m_kBasis, orthoNormalize.m_iBasis) * orthoNormalize.m_kBasis.XYZ();
	orthoNormalize.m_iBasis.x -= projectionK_On_I.x;
	orthoNormalize.m_iBasis.y -= projectionK_On_I.y;
	orthoNormalize.m_iBasis.z -= projectionK_On_I.z;
	orthoNormalize.m_iBasis.Normalize();

	orthoNormalize.m_jBasis.SetXYZ(CrossProduct(orthoNormalize.m_kBasis.XYZ(), orthoNormalize.m_iBasis.XYZ()));

	return orthoNormalize;
}

void Matrix4::SetIBasis(const Vector3& right)
{
	m_iBasis.SetXYZ(right);
}

void Matrix4::SetJBasis(const Vector3& up)
{
	m_jBasis.SetXYZ(up);
}

void Matrix4::SetKBasis(const Vector3& forward)
{
	m_kBasis.SetXYZ(forward);
}

Matrix4 Matrix4::GetInverse() const
{
	Matrix4 inverse;

	double determinant;

	inverse.m_element[0] = m_element[5] * m_element[10] * m_element[15] -
		m_element[5] * m_element[11] * m_element[14] -
		m_element[9] * m_element[6] * m_element[15] +
		m_element[9] * m_element[7] * m_element[14] +
		m_element[13] * m_element[6] * m_element[11] -
		m_element[13] * m_element[7] * m_element[10];

	inverse.m_element[4] = -m_element[4] * m_element[10] * m_element[15] +
		m_element[4] * m_element[11] * m_element[14] +
		m_element[8] * m_element[6] * m_element[15] -
		m_element[8] * m_element[7] * m_element[14] -
		m_element[12] * m_element[6] * m_element[11] +
		m_element[12] * m_element[7] * m_element[10];

	inverse.m_element[8] = m_element[4] * m_element[9] * m_element[15] -
		m_element[4] * m_element[11] * m_element[13] -
		m_element[8] * m_element[5] * m_element[15] +
		m_element[8] * m_element[7] * m_element[13] +
		m_element[12] * m_element[5] * m_element[11] -
		m_element[12] * m_element[7] * m_element[9];

	inverse.m_element[12] = -m_element[4] * m_element[9] * m_element[14] +
		m_element[4] * m_element[10] * m_element[13] +
		m_element[8] * m_element[5] * m_element[14] -
		m_element[8] * m_element[6] * m_element[13] -
		m_element[12] * m_element[5] * m_element[10] +
		m_element[12] * m_element[6] * m_element[9];

	inverse.m_element[1] = -m_element[1] * m_element[10] * m_element[15] +
		m_element[1] * m_element[11] * m_element[14] +
		m_element[9] * m_element[2] * m_element[15] -
		m_element[9] * m_element[3] * m_element[14] -
		m_element[13] * m_element[2] * m_element[11] +
		m_element[13] * m_element[3] * m_element[10];

	inverse.m_element[5] = m_element[0] * m_element[10] * m_element[15] -
		m_element[0] * m_element[11] * m_element[14] -
		m_element[8] * m_element[2] * m_element[15] +
		m_element[8] * m_element[3] * m_element[14] +
		m_element[12] * m_element[2] * m_element[11] -
		m_element[12] * m_element[3] * m_element[10];

	inverse.m_element[9] = -m_element[0] * m_element[9] * m_element[15] +
		m_element[0] * m_element[11] * m_element[13] +
		m_element[8] * m_element[1] * m_element[15] -
		m_element[8] * m_element[3] * m_element[13] -
		m_element[12] * m_element[1] * m_element[11] +
		m_element[12] * m_element[3] * m_element[9];

	inverse.m_element[13] = m_element[0] * m_element[9] * m_element[14] -
		m_element[0] * m_element[10] * m_element[13] -
		m_element[8] * m_element[1] * m_element[14] +
		m_element[8] * m_element[2] * m_element[13] +
		m_element[12] * m_element[1] * m_element[10] -
		m_element[12] * m_element[2] * m_element[9];

	inverse.m_element[2] = m_element[1] * m_element[6] * m_element[15] -
		m_element[1] * m_element[7] * m_element[14] -
		m_element[5] * m_element[2] * m_element[15] +
		m_element[5] * m_element[3] * m_element[14] +
		m_element[13] * m_element[2] * m_element[7] -
		m_element[13] * m_element[3] * m_element[6];

	inverse.m_element[6] = -m_element[0] * m_element[6] * m_element[15] +
		m_element[0] * m_element[7] * m_element[14] +
		m_element[4] * m_element[2] * m_element[15] -
		m_element[4] * m_element[3] * m_element[14] -
		m_element[12] * m_element[2] * m_element[7] +
		m_element[12] * m_element[3] * m_element[6];

	inverse.m_element[10] = m_element[0] * m_element[5] * m_element[15] -
		m_element[0] * m_element[7] * m_element[13] -
		m_element[4] * m_element[1] * m_element[15] +
		m_element[4] * m_element[3] * m_element[13] +
		m_element[12] * m_element[1] * m_element[7] -
		m_element[12] * m_element[3] * m_element[5];

	inverse.m_element[14] = -m_element[0] * m_element[5] * m_element[14] +
		m_element[0] * m_element[6] * m_element[13] +
		m_element[4] * m_element[1] * m_element[14] -
		m_element[4] * m_element[2] * m_element[13] -
		m_element[12] * m_element[1] * m_element[6] +
		m_element[12] * m_element[2] * m_element[5];

	inverse.m_element[3] = -m_element[1] * m_element[6] * m_element[11] +
		m_element[1] * m_element[7] * m_element[10] +
		m_element[5] * m_element[2] * m_element[11] -
		m_element[5] * m_element[3] * m_element[10] -
		m_element[9] * m_element[2] * m_element[7] +
		m_element[9] * m_element[3] * m_element[6];

	inverse.m_element[7] = m_element[0] * m_element[6] * m_element[11] -
		m_element[0] * m_element[7] * m_element[10] -
		m_element[4] * m_element[2] * m_element[11] +
		m_element[4] * m_element[3] * m_element[10] +
		m_element[8] * m_element[2] * m_element[7] -
		m_element[8] * m_element[3] * m_element[6];

	inverse.m_element[11] = -m_element[0] * m_element[5] * m_element[11] +
		m_element[0] * m_element[7] * m_element[9] +
		m_element[4] * m_element[1] * m_element[11] -
		m_element[4] * m_element[3] * m_element[9] -
		m_element[8] * m_element[1] * m_element[7] +
		m_element[8] * m_element[3] * m_element[5];

	inverse.m_element[15] = m_element[0] * m_element[5] * m_element[10] -
		m_element[0] * m_element[6] * m_element[9] -
		m_element[4] * m_element[1] * m_element[10] +
		m_element[4] * m_element[2] * m_element[9] +
		m_element[8] * m_element[1] * m_element[6] -
		m_element[8] * m_element[2] * m_element[5];

	determinant = m_element[0] * inverse.m_element[0] + m_element[1] * inverse.m_element[4] + m_element[2] * inverse.m_element[8] + m_element[3] * inverse.m_element[12];

	if (determinant == 0)
		ERROR_AND_DIE("Determinant of matrix is 0.");

	double determinantInverse = 1 / determinant;

	return inverse * (float)determinantInverse;
}

Matrix4 Matrix4::MakeConcatenatedTransform(const Matrix4& other)
{
	Matrix4 concatenatedMatrix(*this);
	concatenatedMatrix.ConcatenateTransform(other);
	return concatenatedMatrix;
}

void Matrix4::ConcatenateTransform(const Matrix4& other)
{
	Matrix4 concatenated = Matrix4();

	Vector4 thisRow_x = Vector4(m_iBasis.x, m_jBasis.x, m_kBasis.x, m_translation.x);
	Vector4 thisRow_y = Vector4(m_iBasis.y, m_jBasis.y, m_kBasis.y, m_translation.y);
	Vector4 thisRow_z = Vector4(m_iBasis.z, m_jBasis.z, m_kBasis.z, m_translation.z);
	Vector4 thisRow_w = Vector4(m_iBasis.w, m_jBasis.w, m_kBasis.w, m_translation.w);

// 	Vector4 otherRow_x = Vector4(other.m_iBasis.x, other.m_jBasis.x, other.m_kBasis.x, other.m_translation.x);
// 	Vector4 otherRow_y = Vector4(other.m_iBasis.y, other.m_jBasis.y, other.m_kBasis.y, other.m_translation.y);
// 	Vector4 otherRow_z = Vector4(other.m_iBasis.z, other.m_jBasis.z, other.m_kBasis.z, other.m_translation.z);
// 	Vector4 otherRow_w = Vector4(other.m_iBasis.w, other.m_jBasis.w, other.m_kBasis.w, other.m_translation.w);

	concatenated.m_iBasis.x = DotProduct(other.m_iBasis, thisRow_x);
	concatenated.m_iBasis.y = DotProduct(other.m_iBasis, thisRow_y);
	concatenated.m_iBasis.z = DotProduct(other.m_iBasis, thisRow_z);
	concatenated.m_iBasis.w = DotProduct(other.m_iBasis, thisRow_w);

	concatenated.m_jBasis.x = DotProduct(other.m_jBasis, thisRow_x);
	concatenated.m_jBasis.y = DotProduct(other.m_jBasis, thisRow_y);
	concatenated.m_jBasis.z = DotProduct(other.m_jBasis, thisRow_z);
	concatenated.m_jBasis.w = DotProduct(other.m_jBasis, thisRow_w);

	concatenated.m_kBasis.x = DotProduct(other.m_kBasis, thisRow_x);
	concatenated.m_kBasis.y = DotProduct(other.m_kBasis, thisRow_y);
	concatenated.m_kBasis.z = DotProduct(other.m_kBasis, thisRow_z);
	concatenated.m_kBasis.w = DotProduct(other.m_kBasis, thisRow_w);

	concatenated.m_translation.x = DotProduct(other.m_translation, thisRow_x);
	concatenated.m_translation.y = DotProduct(other.m_translation, thisRow_y);
	concatenated.m_translation.z = DotProduct(other.m_translation, thisRow_z);
	concatenated.m_translation.w = DotProduct(other.m_translation, thisRow_w);

	m_iBasis = concatenated.m_iBasis;
	m_jBasis = concatenated.m_jBasis;
	m_kBasis = concatenated.m_kBasis;
	m_translation = concatenated.m_translation;
}

Matrix4 Matrix4::CreateTransposeMatrix() const
{
	float transpose[16] = {
		m_iBasis.x, m_iBasis.y, m_iBasis.z, m_iBasis.w,
		m_jBasis.x, m_jBasis.y, m_jBasis.z, m_jBasis.w,
		m_kBasis.x, m_kBasis.y, m_kBasis.z, m_kBasis.w,
		m_translation.x, m_translation.y, m_translation.z, m_translation.w
	};

	Matrix4 transposeMatrix = Matrix4(transpose);

	return transposeMatrix;
}

Matrix4 Matrix4::GetTransformed(const Matrix4& other) const
{
	Matrix4 concatenated = Matrix4();

	Vector4 otherRow_x = Vector4(other.m_iBasis.x, other.m_jBasis.x, other.m_kBasis.x, other.m_translation.x);
	Vector4 otherRow_y = Vector4(other.m_iBasis.y, other.m_jBasis.y, other.m_kBasis.y, other.m_translation.y);
	Vector4 otherRow_z = Vector4(other.m_iBasis.z, other.m_jBasis.z, other.m_kBasis.z, other.m_translation.z);
	Vector4 otherRow_w = Vector4(other.m_iBasis.w, other.m_jBasis.w, other.m_kBasis.w, other.m_translation.w);

	concatenated.m_iBasis.x = DotProduct(m_iBasis, otherRow_x);
	concatenated.m_iBasis.y = DotProduct(m_iBasis, otherRow_y);
	concatenated.m_iBasis.z = DotProduct(m_iBasis, otherRow_z);
	concatenated.m_iBasis.w = DotProduct(m_iBasis, otherRow_w);

	concatenated.m_jBasis.x = DotProduct(m_jBasis, otherRow_x);
	concatenated.m_jBasis.y = DotProduct(m_jBasis, otherRow_y);
	concatenated.m_jBasis.z = DotProduct(m_jBasis, otherRow_z);
	concatenated.m_jBasis.w = DotProduct(m_jBasis, otherRow_w);

	concatenated.m_kBasis.x = DotProduct(m_kBasis, otherRow_x);
	concatenated.m_kBasis.y = DotProduct(m_kBasis, otherRow_y);
	concatenated.m_kBasis.z = DotProduct(m_kBasis, otherRow_z);
	concatenated.m_kBasis.w = DotProduct(m_kBasis, otherRow_w);

	concatenated.m_translation.x = DotProduct(m_translation, otherRow_x);
	concatenated.m_translation.y = DotProduct(m_translation, otherRow_y);
	concatenated.m_translation.z = DotProduct(m_translation, otherRow_z);
	concatenated.m_translation.w = DotProduct(m_translation, otherRow_w);

	return concatenated;
}

Vector2 Matrix4::TransformPosition(const Vector2& position2D) const
{
	float xValue = (m_iBasis.x * position2D.x) + (m_iBasis.y * position2D.y) + m_iBasis.w;
	float yValue = (m_jBasis.x * position2D.x) + (m_jBasis.y * position2D.y) + m_jBasis.w;

	return Vector2(xValue, yValue);
}

Vector3 Matrix4::TransformPosition(const Vector3& position3D) const
{
	float xValue = (m_iBasis.x * position3D.x) + (m_iBasis.y * position3D.y) + (m_iBasis.z * position3D.z) + m_iBasis.w;
	float yValue = (m_jBasis.x * position3D.x) + (m_jBasis.y * position3D.y) + (m_jBasis.z * position3D.z) + m_jBasis.w;
	float zValue = (m_kBasis.x * position3D.x) + (m_kBasis.y * position3D.y) + (m_kBasis.z * position3D.z) + m_kBasis.w;

	return Vector3(xValue, yValue, zValue);
}

Vector2 Matrix4::TransformDirection(const Vector2& direction2D) const
{
	float xValue = (m_iBasis.x * direction2D.x) + (m_iBasis.y * direction2D.y);
	float yValue = (m_jBasis.x * direction2D.x) + (m_jBasis.y * direction2D.y);

	return Vector2(xValue, yValue);
}

Vector3 Matrix4::TransformDirection(const Vector3& direction3D) const
{
	float xValue = (m_iBasis.x * direction3D.x) + (m_iBasis.y * direction3D.y) + (m_iBasis.z * direction3D.z);
	float yValue = (m_jBasis.x * direction3D.x) + (m_jBasis.y * direction3D.y) + (m_jBasis.z * direction3D.z);
	float zValue = (m_kBasis.x * direction3D.x) + (m_kBasis.y * direction3D.y) + (m_kBasis.z * direction3D.z);

	return Vector3(xValue, yValue, zValue);
}

Vector4 Matrix4::TransformVector(const Vector4& homogeneousVector) const
{
	float xValue = DotProduct(m_iBasis, homogeneousVector);
	float yValue = DotProduct(m_jBasis, homogeneousVector);
	float zValue = DotProduct(m_kBasis, homogeneousVector);
	float wValue = DotProduct(m_translation, homogeneousVector);

	return Vector4(xValue, yValue, zValue, wValue);
}

void Matrix4::Translate(const Vector2& translation2D)
{
	m_translation.x += translation2D.x;
	m_translation.y += translation2D.y;
}

void Matrix4::Translate(const Vector3& translation3D)
{
	m_translation.x += translation3D.x;
	m_translation.y += translation3D.y;
	m_translation.z += translation3D.z;
}

void Matrix4::Scale(float uniformScale)
{
	m_iBasis *= uniformScale;
	m_jBasis *= uniformScale;
	m_kBasis *= uniformScale;
}

void Matrix4::Scale(const Vector2& nonUniformScale2D)
{
	m_iBasis.x *= nonUniformScale2D.x;
	m_iBasis.y *= nonUniformScale2D.y;
								 
	m_jBasis.x *= nonUniformScale2D.x;
	m_jBasis.y *= nonUniformScale2D.y;
}

void Matrix4::Scale(const Vector3& nonUniformScale3D)
{
	m_iBasis.x *= nonUniformScale3D.x;
	m_iBasis.y *= nonUniformScale3D.y;
	m_iBasis.z *= nonUniformScale3D.z;
	
	m_jBasis.x *= nonUniformScale3D.x;
	m_jBasis.y *= nonUniformScale3D.y;
	m_jBasis.z *= nonUniformScale3D.z;

	m_kBasis.x *= nonUniformScale3D.x;
	m_kBasis.y *= nonUniformScale3D.y;
	m_kBasis.z *= nonUniformScale3D.z;
}

void Matrix4::RotateDegreesAboutX(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	RotateRadiansAboutX(radians);
}

void Matrix4::RotateDegreesAboutY(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	RotateRadiansAboutY(radians);
}

void Matrix4::RotateDegreesAboutZ(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	RotateRadiansAboutZ(radians);
}

void Matrix4::RotateRadiansAboutX(float radians)
{
	float cosValue = cos(radians);
	float sinValue = sin(radians);

	Matrix4 rotation = Matrix4::IDENTITY;

	rotation.m_jBasis.y = cosValue;
	rotation.m_kBasis.y = -sinValue;

	rotation.m_jBasis.z = sinValue;
	rotation.m_kBasis.z = cosValue;

	ConcatenateTransform(rotation);
}

void Matrix4::RotateRadiansAboutY(float radians)
{
	float cosValue = cos(radians);
	float sinValue = sin(radians);

	Matrix4 rotation = Matrix4::IDENTITY;

	rotation.m_iBasis.x = cosValue;
	rotation.m_kBasis.x = sinValue;

	rotation.m_iBasis.z = -sinValue;
	rotation.m_kBasis.z = cosValue;

	ConcatenateTransform(rotation);
}

void Matrix4::RotateRadiansAboutZ(float radians)
{
	float cosValue = cos(radians);
	float sinValue = sin(radians);

	Matrix4 rotation = Matrix4::IDENTITY;

	rotation.m_iBasis.x = cosValue;
	rotation.m_jBasis.x = -sinValue;

	rotation.m_iBasis.y = sinValue;
	rotation.m_jBasis.y = cosValue;

	ConcatenateTransform(rotation);
}

void Matrix4::RotateWithQuaternion(const Quaternion& quat)
{
	(*this) = quat.ConvertToMatrix();
}

Matrix4 Matrix4::CreateTranslation(const Vector2& translation2D)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.Translate(translation2D);

	return resultMatrix;
}

Matrix4 Matrix4::CreateTranslation(const Vector3& translation3D)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.Translate(translation3D);

	return resultMatrix;
}

Matrix4 Matrix4::CreateScale(float uniformScale)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.Scale(uniformScale);

	return resultMatrix;
}

Matrix4 Matrix4::CreateScale(const Vector2& nonUniformScale2D)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.Scale(nonUniformScale2D);

	return resultMatrix;
}

Matrix4 Matrix4::CreateScale(const Vector3& nonUniformScale3D)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.Scale(nonUniformScale3D);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutX(float degrees)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateDegreesAboutX(degrees);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutY(float degrees)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateDegreesAboutY(degrees);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutZ(float degrees)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateDegreesAboutZ(degrees);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationRadiansAboutX(float radians)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateRadiansAboutX(radians);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationRadiansAboutY(float radians)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateRadiansAboutY(radians);

	return resultMatrix;
}

Matrix4 Matrix4::CreateRotationRadiansAboutZ(float radians)
{
	Matrix4 resultMatrix = Matrix4();

	resultMatrix.RotateRadiansAboutZ(radians);

	return resultMatrix;
}

Matrix4 Matrix4::MatrixOrthoProjection(float nx, float fx, float ny, float fy, float nz, float fz)
{
	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	Matrix4 orthoProjection =
	{
		Vector4(2.0f * sx,			0.0f,				0.0f,			0.0f),
		Vector4(0.0f,				2.0f * sy,			0.0f,			0.0f),
		Vector4(0.0f,				0.0f,				sz,				0.0f),
		Vector4(-(fx + nx) * sx,	-(fy + ny) * sy,	-nz * sz,		1.0f)
	};

	return orthoProjection;
}

Matrix4 Matrix4::MakePerspectiveProjection(float const FOVRadians, float const aspectRatio, float const nearZ, float const farZ)
{
	float size = 1.0f / atan(FOVRadians / 2.0f);

	// scale X or Y depending which dimension is bigger
	float width = size;
	float height = size;
	if (aspectRatio > 1.0f) 
	{
		width /= aspectRatio;
	}
	else 
	{
		height *= aspectRatio;
	}

	float q = 1.0f / (farZ - nearZ);

	Matrix4 perspectiveProjection;
	perspectiveProjection.m_iBasis = Vector4(width, 0.0f, 0.0f, 0.0f);
	perspectiveProjection.m_jBasis = Vector4(0.0f, height, 0.0f, 0.0f);
	perspectiveProjection.m_kBasis = Vector4(0.0f, 0.0f, farZ * q, 1.0f);
	perspectiveProjection.m_translation = Vector4(0.0f, 0.0f, -nearZ * farZ * q, 0.0f);

	return perspectiveProjection;
}

float Matrix4::CalcTrace() const
{
	return m_element[0] + m_element[5] + m_element[10] + m_element[15];
}

Vector4 Matrix4::GetFirstRow() const
{
	return Vector4(m_iBasis.x, m_jBasis.x, m_kBasis.x, m_translation.x);
}

Vector4 Matrix4::GetSecondRow() const
{
	return Vector4(m_iBasis.y, m_jBasis.y, m_kBasis.y, m_translation.y);
}

Vector4 Matrix4::GetThirdRow() const
{
	return Vector4(m_iBasis.z, m_jBasis.z, m_kBasis.z, m_translation.z);
}

Vector4 Matrix4::GetFourthRow() const
{
	return Vector4(m_iBasis.w, m_jBasis.w, m_kBasis.w, m_translation.w);
}

const Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	Matrix4 resultMatrix = *this;
	return resultMatrix.MakeConcatenatedTransform(other);
}

const Vector3 Matrix4::operator*(const Vector3& other) const
{
	Vector3 resultVector = Vector3::ZERO;
	Vector4 asVec4 = Vector4(other, 1.f);

	resultVector.x = DotProduct(GetFirstRow(), asVec4);
	resultVector.y = DotProduct(GetSecondRow(), asVec4);
	resultVector.z = DotProduct(GetThirdRow(), asVec4);

	return resultVector;
}

const Vector4 Matrix4::operator*(const Vector4& other) const
{
	Vector4 resultVector = Vector4::ZERO;
	
	resultVector.x = DotProduct(m_iBasis, other);
	resultVector.y = DotProduct(m_jBasis, other);
	resultVector.z = DotProduct(m_kBasis, other);
	resultVector.w = DotProduct(m_translation, other);

	return resultVector;
}

const Matrix4 Matrix4::operator*(float value) const
{
	float result[16] = 
	{
		m_element[0] * value, m_element[4] * value, m_element[8] * value, m_element[12] * value,
		m_element[1] * value, m_element[5] * value, m_element[9] * value, m_element[13] * value,
		m_element[2] * value, m_element[6] * value, m_element[10] * value, m_element[14] * value,
		m_element[3] * value, m_element[7] * value, m_element[11] * value, m_element[15] * value
	};

	return Matrix4(result);
}

void PrintMatrixf(const Matrix4& mat)
{
	DebuggerPrintlnf("[%10.3f]\t\t[%10.3f]\t\t[%10.3f]\t\t[%10.3f]", mat.m_element[0], mat.m_element[4], mat.m_element[8], mat.m_element[12]);
	DebuggerPrintlnf("[%10.3f]\t\t[%10.3f]\t\t[%10.3f]\t\t[%10.3f]", mat.m_element[1], mat.m_element[5], mat.m_element[9], mat.m_element[13]);
	DebuggerPrintlnf("[%10.3f]\t\t[%10.3f]\t\t[%10.3f]\t\t[%10.3f]", mat.m_element[2], mat.m_element[6], mat.m_element[10], mat.m_element[14]);
	DebuggerPrintlnf("[%10.3f]\t\t[%10.3f]\t\t[%10.3f]\t\t[%10.3f]", mat.m_element[3], mat.m_element[7], mat.m_element[11], mat.m_element[15]);
}
