#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"


const int BitmapFont::FONT_SHEET_DIMENSIONS = 16;

BitmapFont::BitmapFont() :
	m_spriteSheet(nullptr),
	m_fontFilePath("")
{
}

BitmapFont::BitmapFont(const std::string& bitmapFontName) :
	m_fontFilePath(bitmapFontName)
{
}

BitmapFont::~BitmapFont()
{
	delete m_spriteSheet;
	m_spriteSheet = nullptr;
}

AABB2D BitmapFont::GetTexCoordsForGlyph(char glyphUnicode) const
{
	AABB2D glyphTexCoords = AABB2D::ZERO_TO_ONE;

	glyphTexCoords = m_spriteSheet->GetTextureCoordsForSpriteIndex(glyphUnicode);

	return glyphTexCoords;
}

