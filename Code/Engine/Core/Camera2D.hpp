#pragma once
#pragma comment( lib, "opengl32" )
#include <Windows.h>
#include <gl/GL.h>
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2D.hpp"

const float VIEW_SCALE = 400.f;
const float MAX_ZOOM_IN = 100.f;
const float MAX_ZOOM_OUT = 550.f;

class Camera2D
{
public:
	Camera2D(const Vector2& worldPos, float orientationDegree = 90.f, float numTilesInViewVertically = 0.f);
	void Update(const Vector2& worldPos);
	void Zoom(float zoom);
	AABB2D GetAABB2D();
private:
	void CalcAndSetNewProjection();
	void SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight);

	
	Vector2 m_worldPos;
	float m_orientationDegree;
	float m_numTilesInViewVertically;
	float m_viewLength;
	float m_viewWidth;
};
