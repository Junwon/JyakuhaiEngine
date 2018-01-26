#include "Engine/Renderer/Font.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Font::Font(std::string fontName, std::string dir) :
	name(fontName),
	textureFilepath(dir),
	glyphTextures(nullptr),
	m_leterSpacing(DEFAULT_LETTER_SPACING)
{
	LoadFile();
}

Font::~Font()
{
	if (glyphTextures != nullptr)
	{
		delete glyphTextures;
		glyphTextures = nullptr;
	}
}

void Font::SetFontTexture(RHIDevice* owner, const char* texturePath)
{
	if (glyphTextures == nullptr)
	{
		glyphTextures = new RHITexture2D(owner, texturePath, D3D11_USAGE_DEFAULT);
	}
}

GlyphInfo_T* Font::GetGlyph(const char c)
{
	return &glyphInfos[c];
}

GlyphInfo_T* Font::GetInvalidGlyph()
{
	return &glyphInfos[-1];
}

int Font::GetKerning(int firstChar, int secondChar)
{
	return kernings[std::pair<int, int>(firstChar, secondChar)];
}


unsigned int Font::CalculateTextWidth(unsigned char character)
{
	unsigned int textWidth = 0;

	textWidth += GetGlyph(character)->width;

	return textWidth;
}

unsigned int Font::CalculateTextWidth(const std::string& text)
{
	unsigned int textWidth = 0;

	for (char c : text)
	{
		textWidth += GetGlyph(c)->xadvance;
	}
	return textWidth;
}

void Font::LoadFile()
{
	if (textureFilepath == "")
	{
		ASSERT_OR_DIE(false, "Font directory is invalid.");
	}
	else
	{
		std::string outBuffer;
		bool readSuccess = ReadBufferFromFile(outBuffer, textureFilepath);

		if (readSuccess)
		{	
			LoadIntoVariables(outBuffer);
		}
	}
}

void Font::LoadIntoVariables(const std::string& buffer)
{
	deliminatedBuffer = SplitByDelim(buffer);

	for (unsigned int i = 0; i < deliminatedBuffer.size(); ++i)
	{
		if (deliminatedBuffer[i] == "size")
		{
			size = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "lineHeight")
		{
			lineHeight = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "base")
		{
			base = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "scaleW")
		{
			scaleW = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "scaleH")
		{
			scaleH = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "pages")
		{
			pages = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "bold")
		{
			bold = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "italic")
		{
			italic = std::stoi(deliminatedBuffer[++i]);
		}
		else if (deliminatedBuffer[i] == "char")
		{
			GlyphInfo_T glyph;
			i += 2;
			glyph.id = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.x = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.y = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.width = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.height = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.xoffset = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.yoffset = std::stoi(deliminatedBuffer[i]);
			i += 2;
			glyph.xadvance = std::stoi(deliminatedBuffer[i]);

			glyphInfos[glyph.id] = glyph;
		}
		else if (deliminatedBuffer[i] == "kerning")
		{
			i += 2;
			int first = std::stoi(deliminatedBuffer[i]);
			i += 2;
			int second = std::stoi(deliminatedBuffer[i]);
			std::pair<int, int> kerningPair = std::pair<int, int>(first, second);

			i += 2;
			kernings[kerningPair] = std::stoi(deliminatedBuffer[i]) + m_leterSpacing;
		}
	}

	//std::vector<std::string>().swap(deliminatedBuffer);
}

std::vector<std::string> Font::SplitByDelim(const std::string& s) 
{
	std::vector<std::string> result;

	bool record = true;
	unsigned int pos = 0;
	unsigned int startPos = pos;
	unsigned int subSize = 0;

	bool quotations = false;
	
	for (auto c : s)
	{
		if (c == ' ' || c == '\n' || c == '=' || c == '\r')
		{
			if (!quotations)
			{
				if (record)
				{
					result.push_back(s.substr(startPos, subSize));
					subSize = 0;
					record = false;
				}
			}
		}
		else
		{
			if (!record)
			{
				record = true;
				startPos = pos;
			}

			if (c == '\"')
			{
				quotations = !quotations;
			}

			subSize++;
		}

		pos++;
	}

	return result;
}
