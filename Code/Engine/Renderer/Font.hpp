#pragma once

#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include <map>
#include <vector>

unsigned int const DEFAULT_LETTER_SPACING = 1;

struct GlyphInfo_T
{
	int id;
	unsigned int x;
	unsigned int y;
	int width;
	int height;
	int xoffset;
	int yoffset;
	unsigned int xadvance;
};

enum TextAlignment
{
	CENTER_ALIGNED,
	LEFT_ALIGNED,
	RIGHT_ALIGNED
};

class Font
{
public:
	Font(std::string fontName, std::string dir);
	~Font();

public:
	void SetFontTexture(RHIDevice* owner, const char* texturePath);
	GlyphInfo_T* GetGlyph(const char c);
	GlyphInfo_T* GetInvalidGlyph();
	int GetKerning(int firstChar, int secondChar);
	unsigned int CalculateTextWidth(unsigned char character);
	unsigned int CalculateTextWidth(const std::string& text);

public:
	std::string name;
	std::string textureFilepath;

	//load all these textures in
	RHITexture2D* glyphTextures;

	unsigned int size;
	unsigned int lineHeight;
	unsigned int base;
	unsigned int scaleW;
	unsigned int scaleH;
	unsigned int pages;
	unsigned int bold;
	unsigned int italic;

	std::map<int, GlyphInfo_T> glyphInfos;
	std::map<std::pair<int, int>, int> kernings;

	unsigned int m_leterSpacing;

private:
	void LoadFile();
	void LoadIntoVariables(const std::string& buffer);
	std::vector<std::string> SplitByDelim(const std::string& s);

	std::vector<std::string> deliminatedBuffer;
};

/*
When loading, I will usually normalize all sizes (outside of texture coordinates) to the import size.
For example, if my import size is 32.
-	line_height = px_line_height / import_size = 32 / 32 = 1
-	‘A’ size = px_height / import_size = 18 / 32 = 0.5625
-	etc…
*/