#include "Engine/Renderer/SpriteSheet.hpp"

SpriteSheet::SpriteSheet(const Texture& texture, int tilesWidth, int tilesHeight) :
	m_spriteSheetTexture(&texture),
	m_spriteSheetRHITexture(nullptr),
	m_spriteLayout(IntVector2(tilesWidth, tilesHeight))
{
}

SpriteSheet::SpriteSheet(const RHITexture2D& texture, int tilesWidth, int tilesHeight) :
	m_spriteSheetTexture(nullptr),
	m_spriteSheetRHITexture(&texture),
	m_spriteLayout(IntVector2(tilesWidth, tilesHeight))
{
}

