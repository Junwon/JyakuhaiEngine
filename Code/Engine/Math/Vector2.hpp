#pragma once

class IntVector2;

class Vector2 
{
public:
	float x;
	float y;
	static const Vector2 ZERO;
	static const Vector2 CENTER;
	static const Vector2 ONE;
	static const Vector2 NEGATIVE_ONE;

	//Constructors
	Vector2();
	Vector2( const Vector2& copy );
	explicit Vector2(float initialX, float initialY);
	explicit Vector2(const Vector2& fromPoint, const Vector2& toPoint);
	Vector2(const IntVector2& copy);

	//Accessors
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float GetX() const;
	float GetY() const;
	void GetXY(float& out_x, float& out_y) const;
	float CalcLength() const;
	float CalcLengthSquared() const;
	float CalcHeadingDegrees() const;
	float CalcHeadingRadians() const;

	//Mutators
	void SetX(float newX);
	void SetY(float newY);
	void SetXY(float newX, float newY);
	void Rotate90Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
	float Normalize();
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector2& perAxisDivisors);
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
	void SetUnitLengthAndHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
	void SetLengthAndHeadingRadians(float newLength, float headingRadians);
	void AddToX(float xValue);
	void AddToY(float yValue);
	
	//Operators
	bool operator == (const Vector2& vectorToEqual) const;
	bool operator != (const Vector2& vectorToNotEqual) const;
	const Vector2 operator + (const Vector2& vectorToAdd) const;
	const Vector2 operator - (const Vector2& vectorToSubtract) const;
	const Vector2 Vector2::operator - () const;
	const Vector2 operator + (int value) const;
	const Vector2 operator * (float scale) const;
	const Vector2 operator * (const Vector2& perAxisScaleFactors) const;
	const Vector2 operator / (const Vector2& perAxisScaleFactors) const;
	const Vector2 operator / (float inverseScale) const;
	void operator *= (float scale);
	void operator *= (const Vector2& perAxisScaleFactors);
	void operator += (int value);
	void operator += (const Vector2& vectorToAdd);
	void operator -= (const Vector2& vectorToSubtract);

	//Friends
	friend float CalcDistance(const Vector2& positionA, const Vector2& positionB);
	friend float CalcDistanceSquared(const Vector2& posA, const Vector2& posB);
	friend const Vector2 operator * (float scale, const Vector2& vectorToScale);
	friend const Vector2 GetVectorBetweenTwoPoints(const Vector2& pointA, const Vector2 pointB);

	//Helper
	bool IsMostlyEqualTo(const Vector2& comparison, float epsilon = 0.001f);
	friend bool AreMostlyEqual(const Vector2& a, const Vector2& b, float epsilon = 0.001f);
	const Vector2 GetNormalizedVector() const;
	Vector2 GetXComponentVector() const;
	Vector2 GetYComponentVector() const;
	void operator /= (float scale);
	void operator /= (const Vector2& perAxisScaleFactors);
};


//ACCESSOR METHODS
//============================================================

inline float Vector2::GetX() const
{
	return x;
}

inline float Vector2::GetY() const
{
	return y;
}

inline void Vector2::GetXY(float& out_x, float& out_y) const
{
	out_x = x;
	out_y = y;
}

inline float Vector2::CalcLengthSquared() const
{
	return (x*x) + (y*y);
}

//MUTATOR METHODS
//============================================================

inline void Vector2::SetX(float newX)
{
	x = newX;
}

inline void Vector2::SetY(float newY)
{
	y = newY;
}

inline void Vector2::SetXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

inline void Vector2::Rotate90Degrees()
{
	float newX = -y;
	float newY = x;

	x = newX;
	y = newY;
}

inline void Vector2::RotateNegative90Degrees()
{
	float newX = y;
	float newY = -x;

	x = newX;
	y = newY;
}

inline float Vector2::Normalize()
{
	float length = CalcLength();

	if (length > 0.0f) {
		float scale = 1.0f / length;
		x *= scale;
		y *= scale;
		return length;
	}
	else {
		return 0.0f;
	}
}

inline void Vector2::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
}

inline void Vector2::ScaleNonUniform(const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void Vector2::InverseScaleNonUniform(const Vector2& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

inline void Vector2::AddToX(float xValue)
{
	x += xValue;
}

inline void Vector2::AddToY(float yValue)
{
	y += yValue;
}

//OPERATOR METHODS
//============================================================

inline bool Vector2::operator == (const Vector2& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y) {
		return true;
	}
	else {
		return false;
	}
}

inline bool Vector2::operator != (const Vector2& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

inline const Vector2 Vector2::operator - () const
{
	return Vector2(-x, -y);
}

inline const Vector2 Vector2::operator + (const Vector2& vectorToAdd) const
{
	return Vector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

inline const Vector2 Vector2::operator - (const Vector2& vectorToSubtract) const
{
	return Vector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

inline const Vector2 Vector2::operator + (int value) const
{
	return Vector2(x + value, y + value);
}

inline const Vector2 Vector2::operator * (float scale) const
{
	return Vector2(x * scale, y * scale);
}

inline const Vector2 Vector2::operator * (const Vector2& perAxisScaleFactors) const
{
	return Vector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline const Vector2 Vector2::operator / (const Vector2& perAxisScaleFactors) const
{
	return Vector2(x / perAxisScaleFactors.x, y / perAxisScaleFactors.y);
}

inline const Vector2 Vector2::operator / (float inverseScale) const
{
	if (inverseScale != 0) {
		float inverseScaleRecipricol = 1 / inverseScale;
		return Vector2(x * inverseScaleRecipricol, y * inverseScaleRecipricol);
	}
	else {
		return Vector2(x, y);
	}
}

inline void Vector2::operator *= (float scale)
{
	x *= scale;
	y *= scale;
}

inline void Vector2::operator *= (const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void Vector2::operator /= (float scale)
{
	float scaleRecipricol = 1 / scale;
	x *= scaleRecipricol;
	y *= scaleRecipricol;
}

inline void Vector2::operator /= (const Vector2& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
}

inline void Vector2::operator+=(int value)
{
	x += value;
	y += value;
}

inline void Vector2::operator += (const Vector2& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

inline void Vector2::operator -= (const Vector2& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

//FRIENDS METHODS
//============================================================

inline const Vector2 operator * (float scale, const Vector2& vectorToScale)
{
	return Vector2(vectorToScale.x * scale, vectorToScale.y * scale);
}


inline const Vector2 GetVectorBetweenTwoPoints(const Vector2& pointA, const Vector2 pointB)
{
	return Vector2(pointB.x - pointA.x, pointB.y - pointA.y);
}

//HELPER METHODS
//============================================================
inline const float* Vector2::GetAsFloatArray() const
{
	return &x;
}

inline float* Vector2::GetAsFloatArray()
{
	return &x;
}

inline Vector2 Vector2::GetXComponentVector() const
{
	return Vector2(x, 0.f);
}

inline Vector2 Vector2::GetYComponentVector() const
{
	return Vector2(0.f, y);
}