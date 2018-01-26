#pragma once 
#pragma warning(disable : 4201)
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class Quaternion;

class Matrix4
{
public:
	static const Matrix4 IDENTITY;

public:
	Matrix4();
	Matrix4(const Matrix4& copy);
	Matrix4(float element1, float element2, float element3, float element4, 
		float element5, float element6, float element7, float element8, 
		float element9, float element10, float element11, float element12, 
		float element13, float element14, float element15, float element16);
	void Matrix4::operator=(const Matrix4& copy);
	Matrix4 (const float* arrayOfFloats);
	Matrix4 (const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	Matrix4 (const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));
	Matrix4 (const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4(0.f, 0.f, 0.f, 1.f));
	bool IsMostlyEqualTo(const Matrix4& comparison, float epsilon = 0.001f);

public:
	bool operator == (const Matrix4& mat) const;
	bool operator != (const Matrix4& mat) const;
	const Matrix4 operator + (const Matrix4& matrixToAdd) const;

	void MakeIdentity();
	const float* GetAsFloatArray() const;	// Used when the Matrix4 in question in const
	float* GetAsFloatArray();		// Used when the Matrix4 is not const
	Vector3 GetIBasis() const;
	Vector3 GetJBasis() const;
	Vector3 GetKBasis() const;
	Vector3 GetTranslation() const;
	Vector3 GetScale() const;
	Matrix4 GetOrthoNormalizeAboutZ() const;

	void SetIBasis(const Vector3& right);
	void SetJBasis(const Vector3& up);
	void SetKBasis(const Vector3& forward);
	Matrix4 GetInverse() const;
	Matrix4 MakeConcatenatedTransform(const Matrix4& other);
	void ConcatenateTransform(const Matrix4& other);

	Matrix4 GetTransformed(const Matrix4& other) const;
	Matrix4 CreateTransposeMatrix() const;
	Vector2 TransformPosition(const Vector2& position2D) const; // Assumes z=0, w=1
	Vector3 TransformPosition(const Vector3& position3D) const; // Assumes w=1
	
	Vector2 TransformDirection(const Vector2& direction2D) const; // Assumes z=0, w=0
 	Vector3 TransformDirection(const Vector3& direction3D) const; // Assumes w=0
 	Vector4 TransformVector(const Vector4& homogeneousVector) const; // w is provided

	void Translate(const Vector2& translation2D); // z translation assumed to be 0
	void Translate(const Vector3& translation3D);
	void Scale(float uniformScale);
	void Scale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	void Scale(const Vector3& nonUniformScale3D);
 	void RotateDegreesAboutX(float degrees);
	void RotateDegreesAboutY(float degrees);
	void RotateDegreesAboutZ(float degrees); // Possibly also offered as RotateDegrees2D
	void RotateRadiansAboutX(float radians);
	void RotateRadiansAboutY(float radians);
	void RotateRadiansAboutZ(float radians); // Possibly also offered as RotateRadians2D
	void RotateWithQuaternion(const Quaternion& quat);

	static Matrix4 CreateTranslation(const Vector2& translation2D); // z translation assumed to be 0
	static Matrix4 CreateTranslation(const Vector3& translation3D);
	static Matrix4 CreateScale(float uniformScale);
	static Matrix4 CreateScale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	static Matrix4 CreateScale(const Vector3& nonUniformScale3D);
	static Matrix4 CreateRotationDegreesAboutX(float degrees);
	static Matrix4 CreateRotationDegreesAboutY(float degrees);
	static Matrix4 CreateRotationDegreesAboutZ(float degrees); // a.k.a. CreateRotationDegrees2D
	static Matrix4 CreateRotationRadiansAboutX(float radians);
	static Matrix4 CreateRotationRadiansAboutY(float radians);
	static Matrix4 CreateRotationRadiansAboutZ(float radians); // a.k.a. CreateRotationRadians2D
	static Matrix4 MatrixOrthoProjection(float nx, float fx, float ny, float fy, float nz = 0.0f, float fz = 1.0f);
	static Matrix4 MakePerspectiveProjection(float const FOVRadians, float const aspectRatio, float const nearZ, float const farZ);
	float CalcTrace() const;

	Vector4 GetFirstRow() const;
	Vector4 GetSecondRow() const;
	Vector4 GetThirdRow() const;
	Vector4 GetFourthRow() const;
	const Matrix4 operator * (const Matrix4& other) const;
	const Vector3 operator * (const Vector3& other) const;
	const Vector4 operator * (const Vector4& other) const;
	const Matrix4 operator * (float value) const;
	
	union
	{
		struct  
		{
			float m_element[16];
		};
		struct  
		{
			Vector4 m_iBasis;
			Vector4 m_jBasis;
			Vector4 m_kBasis;
			Vector4 m_translation;
		};
	};
};

void PrintMatrixf(const Matrix4& mat);