#pragma once

class IntVector3
{
public:
	int x;
	int y;
	int z;
	static const IntVector3 ZERO;
	static const IntVector3 ONE;

	static const IntVector3 X_AXIS;
	static const IntVector3 Y_AXIS;
	static const IntVector3 Z_AXIS;
public:
	IntVector3(const IntVector3& copy);
	explicit IntVector3(int initialX, int initialY, int initialZ);
	explicit IntVector3(const IntVector3& fromPoint, const IntVector3& toPoint);

	//Accessors
	const int* GetAsIntArray() const;
	int* GetAsIntArray();
	int GetX() const;
	int GetY() const;
	int GetZ() const;
	void GetXYZ(int& out_x, int& out_y, int& out_z) const;
//	float CalcLength() const;
// 	int CalcLengthSquared() const;
// 	float CalcHeadingDegrees() const;
// 	float CalcHeadingRadians() const;

	//Mutators
	void SetX(int newX);
	void SetY(int newY);
	void SetZ(int newZ);
	void SetXYZ(int newX, int newY, int newZ);
// 	void Rotate90Degrees();
// 	void RotateNegative90Degrees();
// 	void RotateDegrees(float degrees);
// 	void RotateRadians(float radians);
	// 	int Normalize();
	// 	int SetLength(int newLength);
	void ScaleUniform(int scale);
	void ScaleNonUniform(const IntVector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const IntVector3& perAxisDivisors);
	/*
	void SetHeadingDegrees(int headingDegrees);
	void SetHeadingRadians(int headingRadians);
	void SetUnitLengthAndHeadingDegrees(int headingDegrees);
	void SetUnitLengthAndHeadingRadians(int headingRadians);
	void SetLengthAndHeadingDegrees(int newLength, int headingDegrees);
	void SetLengthAndHeadingRadians(int newLength, int headingRadians);*/

	//Operators
	bool operator == (const IntVector3& vectorToEqual) const;
	bool operator != (const IntVector3& vectorToNotEqual) const;
	const IntVector3 IntVector3::operator-() const;
	const IntVector3 operator + (const IntVector3& vectorToAdd) const;
	const IntVector3 operator - (const IntVector3& vectorToSubtract) const;
	const IntVector3 operator * (int scale) const;
	const IntVector3 operator * (const IntVector3& perAxisScaleFactors) const;
	const IntVector3 operator / (int inverseScale) const;
	void operator *= (int scale);
	void operator *= (const IntVector3& perAxisScaleFactors);
	void operator += (const IntVector3& vectorToAdd);
	void operator -= (const IntVector3& vectorToSubtract);

	//Friends
	/*
	friend int CalcDistance(const IntVector3& positionA, const IntVector3& positionB);
	friend int CalcDistanceSquared(const IntVector3& posA, const IntVector3& posB);
	friend const IntVector3 operator * (int scale, const IntVector3& vectorToScale);
	friend const IntVector3 GetVectorBetweenTwoPoints(const IntVector3& pointA, const IntVector3 pointB);

	//Helper
	bool IsMostlyEqualTo(const IntVector3& comparison, int epsilon = 0.001f);
	friend bool AreMostlyEqual(const IntVector3& a, const IntVector3& b, int epsilon = 0.001f);
	const IntVector3 GetNormalizedVector() const;*/
	IntVector3 GetXComponentVector() const;
	IntVector3 GetYComponentVector() const;
	IntVector3 GetZComponentVector() const;
	void IntVector3::operator /= (int scale);
	void IntVector3::operator /= (const IntVector3& perAxisScaleFactors);
};