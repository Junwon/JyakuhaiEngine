#pragma once

class Vector2;

class IntVector2
{
public:
	int x;
	int y;
	static const IntVector2 ZERO;
	static const IntVector2 ONE;

public:
	IntVector2();
	IntVector2(const IntVector2& copy);
	explicit IntVector2(int initialX, int initialY);
	explicit IntVector2(const IntVector2& fromPoint, const IntVector2& toPoint);

	//Accessors
	const int* GetAsIntArray() const;
	int* GetAsIntArray();
	int GetX() const;
	int GetY() const;
	void GetXY(int& out_x, int& out_y) const;
	float CalcLength() const;
	int CalcLengthSquared() const;
	float CalcHeadingDegrees() const;
	float CalcHeadingRadians() const;

	//Mutators
	void SetX(int newX);
	void SetY(int newY);
	void SetXY(int newX, int newY);
	void Rotate90Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
// 	int Normalize();
// 	int SetLength(int newLength);
	void ScaleUniform(int scale);
	void ScaleNonUniform(const IntVector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const IntVector2& perAxisDivisors);
/*
	void SetHeadingDegrees(int headingDegrees);
	void SetHeadingRadians(int headingRadians);
	void SetUnitLengthAndHeadingDegrees(int headingDegrees);
	void SetUnitLengthAndHeadingRadians(int headingRadians);
	void SetLengthAndHeadingDegrees(int newLength, int headingDegrees);
	void SetLengthAndHeadingRadians(int newLength, int headingRadians);*/

	//Operators
	bool operator == (const IntVector2& vectorToEqual) const;
	bool operator != (const IntVector2& vectorToNotEqual) const;
	const IntVector2 operator + (const IntVector2& vectorToAdd) const;
	const IntVector2 operator - (const IntVector2& vectorToSubtract) const;
	const IntVector2 operator * (int scale) const;
	const IntVector2 operator * (const IntVector2& perAxisScaleFactors) const;
	const IntVector2 operator / (int inverseScale) const;
	const IntVector2 operator = (const Vector2& floatVector2) const;
	void operator *= (int scale);
	void operator *= (const IntVector2& perAxisScaleFactors);
	void operator += (const IntVector2& vectorToAdd);
	void operator -= (const IntVector2& vectorToSubtract);

	//Friends

	friend int CalcDistance(const IntVector2& positionA, const IntVector2& positionB);
	friend int CalcDistanceSquared(const IntVector2& posA, const IntVector2& posB);
/*	friend const IntVector2 operator * (int scale, const IntVector2& vectorToScale);
	friend const IntVector2 GetVectorBetweenTwoPoints(const IntVector2& pointA, const IntVector2 pointB);

	//Helper
	bool IsMostlyEqualTo(const IntVector2& comparison, int epsilon = 0.001f);
	friend bool AreMostlyEqual(const IntVector2& a, const IntVector2& b, int epsilon = 0.001f);
	const IntVector2 GetNormalizedVector() const;*/
	IntVector2 GetXComponentVector() const;
	IntVector2 GetYComponentVector() const;
	void IntVector2::operator /= (int scale);
	void IntVector2::operator /= (const IntVector2& perAxisScaleFactors);

	Vector2 GetAsFloats();
};

//////////////////////////////////////////////////////////////////////////

inline const int* IntVector2::GetAsIntArray() const
{
	return &x;
}

inline int* IntVector2::GetAsIntArray()
{
	return &x;
}

inline int IntVector2::GetX() const
{
	return x;
}

inline int IntVector2::GetY() const
{
	return y;
}

inline void IntVector2::GetXY(int& out_x, int& out_y) const
{
	out_x = x;
	out_y = y;
}

inline int IntVector2::CalcLengthSquared() const
{
	return (x*x) + (y*y);
}

inline void IntVector2::SetX(int newX)
{
	x = newX;
}

inline void IntVector2::SetY(int newY)
{
	y = newY;
}

inline void IntVector2::SetXY(int newX, int newY)
{
	x = newX;
	y = newY;
}

inline void IntVector2::ScaleUniform(int scale)
{
	x *= scale;
	y *= scale;
}

inline void IntVector2::ScaleNonUniform(const IntVector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void IntVector2::InverseScaleNonUniform(const IntVector2& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

inline void IntVector2::operator -= (const IntVector2& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

inline void IntVector2::operator += (const IntVector2& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

inline void IntVector2::operator *= (const IntVector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void IntVector2::operator *= (int scale)
{
	x *= scale;
	y *= scale;
}

inline const IntVector2 IntVector2::operator / (int inverseScale) const
{
	if (inverseScale != 0) {
		return IntVector2(x / inverseScale, y / inverseScale);
	}
	else {
		return IntVector2(x, y);
	}
}

inline const IntVector2 IntVector2::operator * (const IntVector2& perAxisScaleFactors) const
{
	return IntVector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline const IntVector2 IntVector2::operator * (int scale) const
{
	return IntVector2(x * scale, y * scale);
}

inline const IntVector2 IntVector2::operator - (const IntVector2& vectorToSubtract) const
{
	return IntVector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

inline const IntVector2 IntVector2::operator + (const IntVector2& vectorToAdd) const
{
	return IntVector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

inline bool IntVector2::operator != (const IntVector2& vectorToNotEqual) const
{
	if (*this == vectorToNotEqual) {
		return false;
	}
	else {
		return true;
	}
}

inline bool IntVector2::operator == (const IntVector2& vectorToEqual) const
{
	if (x == vectorToEqual.x && y == vectorToEqual.y) {
		return true;
	}
	else {
		return false;
	}
}

inline void IntVector2::operator /= (int scale)
{
	x /= scale;
	y /= scale;
}

inline void IntVector2::operator /= (const IntVector2& perAxisScaleFactors)
{
	x /= perAxisScaleFactors.x;
	y /= perAxisScaleFactors.y;
}