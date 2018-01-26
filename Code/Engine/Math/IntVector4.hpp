#pragma once
#pragma warning(disable : 4201)

class IntVector4
{
public:
	static const IntVector4 ZERO;
	static const IntVector4 ONE;

	static const IntVector4 X_AXIS;
	static const IntVector4 Y_AXIS;
	static const IntVector4 Z_AXIS;
	static const IntVector4 W_AXIS;
public:
	IntVector4(const IntVector4& copy);
	explicit IntVector4(int initialX, int initialY, int initialZ, int initialW);
	explicit IntVector4(const IntVector4& fromPoint, const IntVector4& toPoint);

	//Accessors
	const int* GetAsIntArray() const;
	int* GetAsIntArray();
	int GetX() const;
	int GetY() const;
	int GetZ() const;
	int GetW() const;
	void GetXYZW(int& out_x, int& out_y, int& out_z, int& out_w) const;
	//	float CalcLength() const;
	// 	int CalcLengthSquared() const;
	// 	float CalcHeadingDegrees() const;
	// 	float CalcHeadingRadians() const;

	//Mutators
	void SetX(int newX);
	void SetY(int newY);
	void SetZ(int newZ);
	void SetW(int newW);
	void SetXYZW(int newX, int newY, int newZ, int newW);
	// 	void Rotate90Degrees();
	// 	void RotateNegative90Degrees();
	// 	void RotateDegrees(float degrees);
	// 	void RotateRadians(float radians);
	// 	int Normalize();
	// 	int SetLength(int newLength);
	void ScaleUniform(int scale);
	void ScaleNonUniform(const IntVector4& perAxisScaleFactors);
	void InverseScaleNonUniform(const IntVector4& perAxisDivisors);
	/*
	void SetHeadingDegrees(int headingDegrees);
	void SetHeadingRadians(int headingRadians);
	void SetUnitLengthAndHeadingDegrees(int headingDegrees);
	void SetUnitLengthAndHeadingRadians(int headingRadians);
	void SetLengthAndHeadingDegrees(int newLength, int headingDegrees);
	void SetLengthAndHeadingRadians(int newLength, int headingRadians);*/

	//Operators
	bool operator == (const IntVector4& vectorToEqual) const;
	bool operator != (const IntVector4& vectorToNotEqual) const;
	const IntVector4 IntVector4::operator-() const;
	const IntVector4 operator + (const IntVector4& vectorToAdd) const;
	const IntVector4 operator - (const IntVector4& vectorToSubtract) const;
	const IntVector4 operator * (int scale) const;
	const IntVector4 operator * (const IntVector4& perAxisScaleFactors) const;
	const IntVector4 operator / (int inverseScale) const;
	void operator *= (int scale);
	void operator *= (const IntVector4& perAxisScaleFactors);
	void operator += (const IntVector4& vectorToAdd);
	void operator -= (const IntVector4& vectorToSubtract);

	//Friends
	/*
	friend int CalcDistance(const IntVector4& positionA, const IntVector4& positionB);
	friend int CalcDistanceSquared(const IntVector4& posA, const IntVector4& posB);
	friend const IntVector4 operator * (int scale, const IntVector4& vectorToScale);
	friend const IntVector4 GetVectorBetweenTwoPoints(const IntVector4& pointA, const IntVector4 pointB);

	//Helper
	bool IsMostlyEqualTo(const IntVector4& comparison, int epsilon = 0.001f);
	friend bool AreMostlyEqual(const IntVector4& a, const IntVector4& b, int epsilon = 0.001f);
	const IntVector4 GetNormalizedVector() const;*/
	IntVector4 GetXComponentVector() const;
	IntVector4 GetYComponentVector() const;
	IntVector4 GetZComponentVector() const;
	void IntVector4::operator /= (int scale);
	void IntVector4::operator /= (const IntVector4& perAxisScaleFactors);

public:

	union
	{
		struct
		{
			int x;
			int y;
			int z;
			int w;
		};
		struct  
		{
			int m_components[4];
		};
	};
};