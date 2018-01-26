#include "Engine/Renderer/Image.hpp"

#include <stdint.h>
#include "Engine/Math/IntVector2.hpp"
#include <malloc.h>

#define STBI_NO_PSD
#define STBI_NO_PNM
#include "ThirdParty/stb/stb_image.h"

Image::~Image()
{
	Destroy();
}

//------------------------------------------------------------------------
bool Image::LoadFromFile(char const* filename)
{
	Destroy();

	int w;
	int h;
	int comp;

	// Force load in with 4 components
	stbi_uc* data = stbi_load(filename, &w, &h, &comp, 4);
	if (data != nullptr) 
	{
		m_buffer = (unsigned char*)data;
		m_width = w;
		m_height = h;
		m_bpp = 4;
		return true;
	}

	return false;
}

bool Image::CreateClear(unsigned int w, unsigned int h, const RgbaFloats& color)
{
	Destroy();

	m_width = w;
	m_height = h;

	m_format = IMAGEFORMAT_RGBA8;
	m_bpp = 4;
	
	unsigned int size = m_width * m_height * m_bpp;
	m_buffer = (unsigned char*) ::malloc(size);
	Rgba c = color;

	Rgba* colors = (Rgba*)m_buffer;
	unsigned int count = m_width * m_height;

	for (unsigned int i = 0; i < count; ++i)
	{
		colors[i] = c;
	}

	return true;
}

bool Image::CreateBuffer(eImageFormat eformat, unsigned int w, unsigned int h)
{
	Destroy();

	m_width = w;
	m_height = h;

	m_format = eformat;
	m_bpp = 4;

	unsigned int size = m_width * m_height * m_bpp;
	m_buffer = (unsigned char*) ::malloc(size);

	if (m_buffer)
		return true;

	return false;
}

void Image::SetPixel(unsigned int posX, unsigned int posY, const Rgba& color)
{
	Rgba c = color;

	Rgba* colors = (Rgba*)m_buffer;

	unsigned int index = (posY * m_width) + posX;

	colors[index] = c;
}

//------------------------------------------------------------------------
void Image::Destroy()
{
	if (nullptr != m_buffer)
	{
		::free(m_buffer);
		m_buffer = nullptr;
	}
}