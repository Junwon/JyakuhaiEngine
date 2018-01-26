#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere3D
{
public:
	//public data members :
	Vector3 m_center;
	float	m_radius;

	//public static const data member :
	static const Sphere3D UNIT_SPHERE;

	//Constructors / destructor:
	Sphere3D();
	Sphere3D(const Sphere3D& copy);
	~Sphere3D();
	explicit Sphere3D(float initialX, float initialY, float initialZ, float initialRadius);
	explicit Sphere3D(const Vector3& initialCenter, float initialRadius);

	//Mutators:
	void StretchToIncludePoint(const Vector3& point);
	void AddPadding(float paddingRadius);
	void Translate(const Vector3& translation);

	//Accessors / queries:
	bool IsPointInside(const Vector3& point) const;

	//Operators:
	const Sphere3D operator + (const Vector3& translation) const;
	const Sphere3D operator - (const Vector3& antiTranslation) const;
	void operator += (const Vector3& translation);
	void operator -= (const Vector3& antiTranslation);

	//Helper functions:
	friend const Sphere3D Interpolate(const Sphere3D& start, const Sphere3D& end, float fractionToEnd);
};
bool DoSpheresOverlap(const Sphere3D& a, const Sphere3D& b);
bool DoSpheresOverlap(const Vector3& aCenter, float aRadius, const Vector3& bCenter, float bRadius);
