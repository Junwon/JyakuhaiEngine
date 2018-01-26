#pragma once
#pragma warning(disable : 4201)

class Vector2;
class Vector3;
class Matrix4;

class Vector4
{
public:
	static const Vector4 ZERO;
	static const Vector4 ONE;

	//Constructors
	Vector4();
	Vector4(const Vector4& copy);
	explicit Vector4(float initialX, float initialY, float initialZ, float initialW);
	explicit Vector4(const Vector4& fromPoint, const Vector4& toPoint);
	explicit Vector4(const Vector2& initialXY, float initialZ, float initialW);
	explicit Vector4(const Vector3& initialXYZ, float initialW);

	//Accessors
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetW() const;
	void GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const;
	Vector2 XY() const;
	Vector3 XYZ() const;
	float CalcLength3D() const;
	float CalcLength4D() const;
	float CalcLength() const;
	float CalcLengthSquared3D() const;
	float CalcLengthSquared4D() const;
	float CalcLengthSquared() const;

	//Mutators
	void SetX(float newX);
	void SetY(float newY);
	void SetZ(float newZ);
	void SetW(float newW);
	void SetXYZW(float newX, float newY, float newZ, float newW);
	void SetXYZ(const Vector3& xyz);
	float Normalize3D();
	float Normalize4D();
	float Normalize();
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector4& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector4& perAxisDivisors);

	//Operators
	bool operator == (const Vector4& vectorToEqual) const;
	bool operator != (const Vector4& vectorToNotEqual) const;
	const Vector4 operator + (const Vector4& vectorToAdd) const;
	const Vector4 operator - (const Vector4& vectorToSubtract) const;
	const Vector4 Vector4::operator - () const;
	const Vector4 operator * (float scale) const;
	const Vector4 operator * (const Vector4& perAxisScaleFactors) const;
	const Vector4 operator / (float inverseScale) const;
	void operator -= (float difference);
	void operator *= (float scale);
	void operator *= (const Vector4& perAxisScaleFactors);
	void operator += (const Vector4& vectorToAdd);
	void operator -= (const Vector4& vectorToSubtract);

	const Vector4 operator * (const Matrix4& matrix) const;

	//Friends
	friend float CalcDistance(const Vector4& positionA, const Vector4& positionB);
	friend float CalcDistanceSquared(const Vector4& posA, const Vector4& posB);
	friend const Vector4 operator * (float scale, const Vector4& vectorToScale);
	friend const Vector4 GetVectorBetweenTwoPoints(const Vector4& pointA, const Vector4 pointB);

	//Helper
	bool IsMostlyEqualTo(const Vector4& comparison, float epsilon = 0.001f);
	friend bool AreMostlyEqual(const Vector4& a, const Vector4& b, float epsilon = 0.001f);
	//const Vector4 GetNormalizedVector() const;
	Vector4 GetXComponentVector() const;
	Vector4 GetYComponentVector() const;
	Vector4 GetZComponentVector() const;
	Vector4 GetWComponentVector() const;
	void Vector4::operator /= (float scale);
	void Vector4::operator /= (const Vector4& perAxisScaleFactors);

public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float m_components[4];
		};
	};
};