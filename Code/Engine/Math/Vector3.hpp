#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector4.hpp"

class Matrix4;

class Vector3
{
public:
	float x;
	float y;
	float z;

	static const Vector3 ZERO;
	static const Vector3 ONE;

	static const Vector3 X_AXIS;
	static const Vector3 Y_AXIS;
	static const Vector3 Z_AXIS;

	//Constructors
	Vector3();
	Vector3(const Vector3& copy);
	explicit Vector3(float initialX, float initialY, float initialZ);
	explicit Vector3(const Vector2& initialXY, float initialZ = 0.f);
	explicit Vector3(const Vector3& fromPoint, const Vector3& toPoint);
	explicit Vector3(const Vector4& vec);

	//Accessors
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	Vector2 GetXY() const;
	void GetXYZ(float& out_x, float& out_y, float& out_z) const;
	float CalcLength() const;
	float CalcLengthSquared() const;

	//Mutators
	void SetX(float newX);
	void SetY(float newY);
	void SetZ(float newZ);
	Vector3 AddToX(float xValue);
	Vector3 AddToY(float yValue);
	Vector3 AddToZ(float zValue);
	void SetXYZ(float newX, float newY, float newZ);
	float Normalize();
	Vector3 CalcNormalizedVector();
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector3& perAxisDivisors);
	Vector3 Floor();

	//Operators
	bool operator == (const Vector3& vectorToEqual) const;
	bool operator != (const Vector3& vectorToNotEqual) const;
	const Vector3 operator + (const Vector3& vectorToAdd) const;
	const Vector3 operator - (const Vector3& vectorToSubtract) const;
	const Vector3 Vector3::operator - () const;
	const Vector3 operator * (float scale) const;
	const Vector3 operator * (const Vector3& perAxisScaleFactors) const;
	const Vector3 operator * (const Matrix4& matrix) const;
	const Vector3 operator / (float inverseScale) const;
	
	void operator *= (float scale);
	void operator *= (const Vector3& perAxisScaleFactors);
	void operator += (const Vector3& vectorToAdd);
	void operator -= (const Vector3& vectorToSubtract);
	void operator -= (float number);
	//Friends
	friend float CalcDistance(const Vector3& positionA, const Vector3& positionB);
	friend float CalcDistanceSquared(const Vector3& posA, const Vector3& posB);
	friend const Vector3 operator * (float scale, const Vector3& vectorToScale);
	friend const Vector3 GetVectorBetweenTwoPoints(const Vector3& pointA, const Vector3 pointB);
	friend Vector3 CrossProduct(const Vector3& u, const Vector3& v);

	//Helper
	bool IsMostlyEqualTo(const Vector3& comparison, float epsilon = 0.001f);
	friend bool AreMostlyEqual(const Vector3& a, const Vector3& b, float epsilon = 0.001f);
	//const Vector3 GetNormalizedVector() const;
	Vector3 GetXComponentVector() const;
	Vector3 GetYComponentVector() const;
	Vector3 GetZComponentVector() const;
	void Vector3::operator /= (float scale);
	void Vector3::operator /= (const Vector3& perAxisScaleFactors);

	Vector3 CrossProduct(const Vector3& other) const;

};

void DebugPrintVector(const Vector3& vector);

struct Vertex3D
{
	Vertex3D();
	Vertex3D(const Vector3& position, const Vector2& textureCoords = Vector2::ZERO, 
		const Rgba& color = Rgba::WHITE, const Vector3& normal = Vector3::ZERO, 
		const Vector3& tangent = Vector3::ZERO, const Vector3& bitangent = Vector3::ZERO,
		const IntVector4& boneIndices = IntVector4::ZERO, const Vector4& boneWeights = Vector4::ZERO);

	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_texCoords;
	Rgba m_color;
	Vector3 m_tangent;
	Vector3 m_bitangent;
	IntVector4 m_boneIndices;
	Vector4 m_boneWeights;
};

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////

inline Vertex3D::Vertex3D() :
	m_position(Vector3::ZERO),
	m_texCoords(Vector2::ZERO),
	m_color(Rgba::WHITE),
	m_normal(Vector3(0.f, 0.f, -1.f)),
	m_tangent(Vector3(1.f, 0.f, 0.f)),
	m_bitangent(Vector3(0.f, 1.f, 0.f)),
	m_boneIndices(IntVector4::ZERO),
	m_boneWeights(Vector4::ZERO)
{
}

inline Vertex3D::Vertex3D(const Vector3& position, const Vector2& textureCoords, 
	const Rgba& color, const Vector3& normal, 
	const Vector3& tangent, const Vector3& bitangent,
	const IntVector4& boneIndices, const Vector4& boneWeights) :
	m_position(position),
	m_texCoords(textureCoords),
	m_color(color),
	m_normal(normal),
	m_tangent(tangent),
	m_bitangent(bitangent),
	m_boneIndices(boneIndices),
	m_boneWeights(boneWeights)
{
}


//////////////////////////////////////////////////////////////////////////

//ACCESSOR METHODS
//============================================================
inline const float* Vector3::GetAsFloatArray() const
{
	return &x;
}

inline float* Vector3::GetAsFloatArray()
{
	return &x;
}

inline float Vector3::GetX() const
{
	return x;
}

inline float Vector3::GetY() const
{
	return y;
}

inline float Vector3::GetZ() const
{
	return z;
}

inline Vector2 Vector3::GetXY() const
{
	return Vector2(x, y);
}

inline void Vector3::GetXYZ(float& out_x, float& out_y, float& out_z) const
{
	out_x = x;
	out_y = y;
	out_z = z;
}

inline float Vector3::CalcLengthSquared() const
{
	return (x*x) + (y*y) + (z*z);
}

//MUTATOR METHODS
//============================================================
inline void Vector3::SetX(float newX)
{
	x = newX;
}

inline void Vector3::SetY(float newY)
{
	y = newY;
}

inline void Vector3::SetZ(float newZ)
{
	z = newZ;
}

inline Vector3 Vector3::AddToX(float xValue)
{
	return Vector3(x + xValue, y, z);
}

inline Vector3 Vector3::AddToY(float yValue)
{
	return Vector3(x, y + yValue, z);
}

inline Vector3 Vector3::AddToZ(float zValue)
{
	return Vector3(x, y, z + zValue);
}

inline void Vector3::SetXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

inline float Vector3::Normalize()
{
	float length = CalcLength();

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

inline float Vector3::SetLength(float newLength)
{
	float length = CalcLength();
	float scale = newLength / length;
	ScaleUniform(scale);
	return length;
}

inline void Vector3::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline void Vector3::ScaleNonUniform(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

inline float CalcDistance(const Vector3& positionA, const Vector3& positionB)
{
	Vector3 displacement = positionA - positionB;
	return displacement.CalcLength();
}

inline float CalcDistanceSquared(const Vector3& posA, const Vector3& posB)
{
	Vector3 displacement = posA - posB;
	return displacement.CalcLengthSquared();
}

inline const Vector3 operator * (float scale, const Vector3& vectorToScale)
{
	return Vector3(vectorToScale.x * scale, vectorToScale.y * scale, vectorToScale.z * scale);
}

inline const Vector3 GetVectorBetweenTwoPoints(const Vector3& pointA, const Vector3 pointB)
{
	return Vector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
}

inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
{
	return Vector3((u.y * v.z - u.z * v.y), (u.z * v.x - u.x * v.z), (u.x * v.y - u.y * v.x));
}

inline void Vector3::InverseScaleNonUniform(const Vector3& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

inline Vector3 Vector3::GetXComponentVector() const
{
	return Vector3(x, 0.f, 0.f);
}

inline Vector3 Vector3::GetYComponentVector() const
{
	return Vector3(0.f, y, 0.f);

}

inline Vector3 Vector3::GetZComponentVector() const
{
	return Vector3(0.f, 0.f, z);

}

inline bool Vector3::operator==(const Vector3& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z) {
		return true;
	}
	else {
		return false;
	}
}

inline bool Vector3::operator!=(const Vector3& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

inline const Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline const Vector3 Vector3::operator+(const Vector3& vectorToAdd) const
{
	return Vector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

inline const Vector3 Vector3::operator-(const Vector3& vectorToSubtract) const
{
	return Vector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

inline const Vector3 Vector3::operator*(float scale) const
{
	return Vector3(x * scale, y * scale, z * scale);
}

inline const Vector3 Vector3::operator*(const Vector3& perAxisScaleFactors) const
{
	return Vector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

inline const Vector3 Vector3::operator/(float inverseScale) const
{
	if (inverseScale != 0) {
		float inverseScaleRecipricol = 1 / inverseScale;
		return Vector3(x * inverseScaleRecipricol, y * inverseScaleRecipricol, z * inverseScaleRecipricol);
	}
	else {
		return Vector3(x, y, z);
	}
}

inline void Vector3::operator -= (float number)
{
	x -= number; 
	y -= number; 
	z -= number;
}

inline void Vector3::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline void Vector3::operator*=(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

inline void Vector3::operator/=(float scale)
{
	float scaleRecipricol = 1 / scale;
	x *= scaleRecipricol;
	y *= scaleRecipricol;
	z *= scaleRecipricol;
}

inline void Vector3::operator/=(const Vector3& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
	z /= perAxisScaleFactors.z;
}

inline void Vector3::operator+=(const Vector3& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

inline void Vector3::operator-=(const Vector3& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}

inline Vector3 Vector3::CrossProduct(const Vector3& other) const
{
	return Vector3(y * other.z - z * other.y, -x * other.z + z * other.x, x * other.y - y * other.x);
}

inline Vector3 Vector3::CalcNormalizedVector()
{
	Vector3 returnVector = Vector3(x, y, z);

	returnVector.Normalize();

	return returnVector;
}

