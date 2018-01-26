#pragma once
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"

class SpriteSheet
{
public:
	SpriteSheet(const Texture& texture, int tilesWidth, int tilesHeight);
	SpriteSheet(const RHITexture2D& texture, int tilesWidth, int tilesHeight);
	AABB2D GetTextureCoordsForSpriteCoords(int spriteX, int spriteY) const; // mostly for atlases
	AABB2D GetTextureCoordsForSpriteIndex(int spriteIndex) const; // mostly for sprite animations
	int GetFrameIndexWithSpriteIndex(int spriteIndex) const;
	const Texture* GetTexture() const;
	const RHITexture2D* GetRHITexture() const;
private:
	const Texture*			m_spriteSheetTexture;	// Texture with grid-based layout of sub-textures
	const RHITexture2D*		m_spriteSheetRHITexture;
	IntVector2				m_spriteLayout;
};

inline AABB2D SpriteSheet::GetTextureCoordsForSpriteCoords(int spriteX, int spriteY) const
{
	if((spriteX >= 0 && spriteX <= m_spriteLayout.x) && (spriteY >= 0 && spriteY <= m_spriteLayout.y))
	{
		float deltaX = (float) 1.0 / m_spriteLayout.GetX();
		float deltaY = (float) 1.0 / m_spriteLayout.GetY();

		float startCornerX = deltaX * spriteX;
		float startCornerY = deltaY * spriteY;

		Vector2 topLeft = Vector2(startCornerX, startCornerY);
		Vector2 bottomRight = Vector2(startCornerX + deltaX, startCornerY + deltaY);
		return AABB2D(topLeft, bottomRight);
	}
	else
	{
		return AABB2D(Vector2::ZERO, Vector2::ZERO);
	}
}

inline AABB2D SpriteSheet::GetTextureCoordsForSpriteIndex(int spriteIndex) const
{
	int spriteX = spriteIndex % m_spriteLayout.GetX();
	int spriteY = spriteIndex / m_spriteLayout.GetX();
	return GetTextureCoordsForSpriteCoords(spriteX, spriteY);
}


inline int SpriteSheet::GetFrameIndexWithSpriteIndex(int spriteIndex) const
{
	int spriteX = spriteIndex % m_spriteLayout.GetX();
	int spriteY = spriteIndex / m_spriteLayout.GetX();

	return spriteX + spriteY;
}

inline const Texture* SpriteSheet::GetTexture() const
{
	return m_spriteSheetTexture;
}

inline const RHITexture2D* SpriteSheet::GetRHITexture() const
{
	return m_spriteSheetRHITexture;
}