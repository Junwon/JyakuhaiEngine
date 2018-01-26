#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2D.hpp"

class BitmapFont
{
	friend class Renderer;

private:
	BitmapFont();
	BitmapFont(const std::string& bitmapFontName); // must be constructed by Renderer
	~BitmapFont();

	AABB2D GetTexCoordsForGlyph(char glyphUnicode) const; // pass ‘A’ or 65 for A, etc.
	float GetGlyphAspect(int glyphUnicode) const;

private:
	SpriteSheet* m_spriteSheet; // used internally; assumed to be a 16x16 glyph sheet
	std::string m_fontFilePath;

	static const int FONT_SHEET_DIMENSIONS;
};

inline float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	glyphUnicode = 0;
	return 1.0f;
}
