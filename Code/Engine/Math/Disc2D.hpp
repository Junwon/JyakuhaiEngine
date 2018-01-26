#pragma once
#include "Engine/Math/Vector2.hpp"

class Disc2D
{
public:
	//public data members :
	Vector2 m_center;
	float	m_radius;

	//public static const data member :
	static const Disc2D UNIT_CIRCLE; 

	//Constructors / destructor:
	Disc2D();
	Disc2D(const Disc2D& copy);
	~Disc2D();
	explicit Disc2D(float initialX, float initialY, float initialRadius);
	explicit Disc2D(const Vector2& initialCenter, float initialRadius);
	
	//Mutators:
	void StretchToIncludePoint(const Vector2& point);
	void AddPadding(float paddingRadius);
	void Translate(const Vector2& translation);
	
	//Accessors / queries:
	bool IsPointInside(const Vector2& point) const;
	
	//Operators:
	const Disc2D operator + (const Vector2& translation) const;
	const Disc2D operator - (const Vector2& antiTranslation) const;
	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);

	//Helper functions
	friend const Disc2D Interpolate(const Disc2D& start, const Disc2D& end, float fractionToEnd);
};

//Standalone functions
bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b);
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);
