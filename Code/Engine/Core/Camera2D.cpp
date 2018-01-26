#include "Engine/Core/Camera2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"

Camera2D::Camera2D(const Vector2& worldPos, float orientationDegree, float numTilesInViewVertically) :
	m_worldPos(worldPos),
	m_orientationDegree(orientationDegree),
	m_numTilesInViewVertically(numTilesInViewVertically)
{
	m_viewLength = VIEW_SCALE;
	m_viewWidth = (m_viewLength * 16) / 9;
}

void Camera2D::Update(const Vector2& worldPos)
{
	float correctedX = worldPos.GetX();
	float correctedY = worldPos.GetY();

	correctedX = ClampFloat(correctedX, m_viewWidth, MAX_MAP_WIDTH - m_viewWidth);
	correctedY = ClampFloat(correctedY, m_viewLength, MAX_MAP_HEIGHT - m_viewLength);

	m_worldPos.SetX(correctedX);
	m_worldPos.SetY(correctedY);
	
	CalcAndSetNewProjection();
}

void Camera2D::Zoom(float zoom)
{
	float newLength = m_viewLength + zoom;
	if (newLength > MAX_ZOOM_IN && newLength < MAX_ZOOM_OUT)
	{
		m_viewLength += zoom;
		m_viewWidth = (m_viewLength * 16) / 9;
		CalcAndSetNewProjection();
	}
}

void Camera2D::CalcAndSetNewProjection()
{
	AABB2D projection = GetAABB2D();

	SetOrthoProjection(projection.m_mins, projection.m_maxs);
}

void Camera2D::SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight)
{
	glLoadIdentity();
	glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f);
}

AABB2D Camera2D::GetAABB2D()
{
	float minX = m_worldPos.x - m_viewWidth;
	float minY = m_worldPos.y - m_viewLength;
	float maxX = m_worldPos.x + m_viewWidth;
	float maxY = m_worldPos.y + m_viewLength;

	Vector2 bottomLeft = Vector2(minX, minY);
	Vector2 topRight = Vector2(maxX, maxY);

	return AABB2D(bottomLeft, topRight);
}