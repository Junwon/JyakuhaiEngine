#pragma once
#pragma warning(disable : 4201)

/*
* Quaternion: 
* 1. A complex number of the form w + xi + yj + zk, where w, x, y, z are real numbers and i, j, k are imaginary units that satisfy certain conditions.
* 2. A four-dimensional hypercomplex number that consists of a real dimension and 3 imaginary ones (i, j, k) that are each a square root of -1. 
*/

class Matrix4;

#include "Engine/Math/Vector3.hpp"

class Quaternion
{
public:
	Quaternion();
	Quaternion(float angleDegrees, float normalizedXComponent, float normalizedYComponent, float normalizedZComponent);
	Quaternion(float angleDegrees, const Vector3& normalizedVectorr);
	Quaternion(const Quaternion& q);

public:
	const Quaternion Quaternion::operator = (const Quaternion& quat);
	const Quaternion Quaternion::operator - () const;
	const Quaternion operator + (const Quaternion& quat) const;
	const Quaternion operator - (const Quaternion& quat) const;
	const Quaternion operator * (const Quaternion& other);
	const Quaternion operator * (float scale) const;
	const Quaternion operator * (double scale) const;
	const Vector3 operator * (const Vector3& point);

	friend const Quaternion operator * (float scale, const Quaternion& quat);

public:
	void SetComponents(const Quaternion& quat);
	void SetComponents(float w, float x, float y, float z);
	void LoadFromMatrix(const Matrix4& matrix);
	Matrix4 ConvertToMatrix() const;
	Quaternion GetConjugate();
	float CalcMagnitude();
	void Normalize();
	Quaternion GetNormalized() const;

public:
	float m_w;
	union
	{
		struct
		{
			float m_x;
			float m_y;
			float m_z;
		};
		struct 
		{
			Vector3 m_axis;
		};
	};
};

void PrintQuaternion(const Quaternion& q);

inline const Quaternion Quaternion::operator=(const Quaternion& quat)
{
	SetComponents(quat);
	return *this;
}

inline const Quaternion Quaternion::operator-() const
{
	Quaternion q = Quaternion();
	q.SetComponents(-m_w, -m_x, -m_y, -m_z);
	return q;
}

inline const Quaternion Quaternion::operator+(const Quaternion& quat) const
{
	Quaternion q = Quaternion();
	q.SetComponents(m_w + quat.m_w, m_x + quat.m_x, m_y + quat.m_y, m_z + quat.m_z);
	return q;
}

inline const Quaternion Quaternion::operator-(const Quaternion& quat) const
{
	Quaternion q = Quaternion();
	q.SetComponents(m_w - quat.m_w, m_x - quat.m_x, m_y - quat.m_y, m_z - quat.m_z);
	return q;
}

inline const Vector3 Quaternion::operator*(const Vector3& point)
{
	Vector3 crossProductResult = CrossProduct(m_axis, point);

	return point + ((2 * m_w) * crossProductResult) + (2 * (CrossProduct(m_axis, crossProductResult)));
}

inline const Quaternion Quaternion::operator*(float scale) const
{
	Quaternion q = Quaternion();
	q.SetComponents(m_w * scale, m_x * scale, m_y * scale, m_z * scale);
	return q;
}

inline const Quaternion Quaternion::operator*(double scale) const
{
	Quaternion q = Quaternion();
	q.SetComponents((float)(m_w * scale), (float)(m_x * scale), (float)(m_y * scale), (float)(m_z * scale));
	return q;
}

inline const Quaternion operator * (float scale, const Quaternion& quat)
{
	Quaternion q = Quaternion();
	q.SetComponents(quat.m_w * scale, quat.m_x * scale, quat.m_y * scale, quat.m_z * scale);
	return q;
}

inline void Quaternion::SetComponents(const Quaternion& quat)
{
	m_w = quat.m_w;
	m_x = quat.m_x;
	m_y = quat.m_y;
	m_z = quat.m_z;
}

inline void Quaternion::SetComponents(float w, float x, float y, float z)
{
	m_w = w;
	m_x = x;
	m_y = y;
	m_z = z;
}