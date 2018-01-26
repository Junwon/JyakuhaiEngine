#pragma once

#include "Engine/Core/Rgba.hpp"

class IntVector2;

enum eImageFormat : unsigned int
{
	IMAGEFORMAT_RGBA8, 
	IMAGEFORMAT_D24S8,
};

class Image
{
public:
	Image() : 
		m_buffer(nullptr), 
		m_width(0),
		m_height(0),
		m_bpp(0)
	{}
	Image(const Image& i) = delete;
	~Image();

	bool LoadFromFile(char const* filename);
	bool CreateClear(unsigned int w, unsigned int h, const RgbaFloats& color);
	void Destroy();

	inline bool IsValid() const { return nullptr != m_buffer; }

	inline unsigned int GetPixelSize() const { return m_bpp; }
	inline unsigned int GetWidth() const { return m_width; }
	inline unsigned int GetHeight() const { return m_height; }
	inline unsigned int GetStride() const { return m_width * m_bpp; }
	inline unsigned int GetSlice() const { return GetStride() * m_height; }

	bool CreateBuffer(eImageFormat format, unsigned int w, unsigned int h);
	void SetPixel(unsigned int posX, unsigned int posY, const Rgba& color = Rgba::WHITE);

public:
	unsigned char* m_buffer;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_bpp;

	eImageFormat m_format;
};