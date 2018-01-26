#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/LineSegment3D.hpp"

class AABB3D
{

public:
	Vector3 m_mins;
	Vector3 m_maxs;

	static const AABB3D ZERO_TO_ONE; 
	static const AABB3D NEG_ONE_TO_ONE;

	AABB3D();
	AABB3D(const AABB3D& copy);
	~AABB3D();
	explicit AABB3D(float initialX, float initialY, float initialZ);
	explicit AABB3D(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3D(const Vector3& mins, const Vector3& maxs);
	explicit AABB3D(const Vector3& center, float radiusX, float radiusY, float radiusZ);

	//Mutators:
	void StretchToIncludePoint(const Vector3& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius);
	void Translate(const Vector3& translation);

	//Accessors / queries:
	bool IsPointInside(const Vector3& point) const;
	const Vector3 CalcSize() const;
	const Vector3 CalcCenter() const;
	float GetWidth() const;
	float GetLength() const;
	float GetHeight() const;

	//Operators:
	const AABB3D operator + (const Vector3& translation) const;
	const AABB3D operator - (const Vector3& antiTranslation) const;
	void operator += (const Vector3& translation);
	void operator -= (const Vector3& antiTranslation);

	//Helper Functions:
	friend const AABB3D Interpolate(const AABB3D& start, const AABB3D& end, float fractionToEnd);
};

//Standalone Function
bool DoAABB3DsOverlap(const AABB3D& a, const AABB3D& b);