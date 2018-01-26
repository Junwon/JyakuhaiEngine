#pragma once

const float INV_255 = 0.0039215686274509803921568627451f;

struct RgbaFloats
{
	RgbaFloats() :
		r(0.f),
		g(0.f),
		b(0.f),
		a(1.f)
	{};

	RgbaFloats(float red, float green, float blue, float alpha = 0.f)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	float r;
	float g;
	float b;
	float a;

	static const RgbaFloats WHITE;
	static const RgbaFloats RED;
	static const RgbaFloats GREEN;
	static const RgbaFloats BLUE;
	static const RgbaFloats ORANGE;
	static const RgbaFloats YELLOW;
	static const RgbaFloats PINK;
	static const RgbaFloats MAGENTA;
};

class Rgba
{
public:
	Rgba();
	Rgba(const Rgba& copy);
	Rgba(const RgbaFloats& floatCopy);
	explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha=0xFF);

	void SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
	void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f);
	void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;
	void ScaleRGB(float rgbScale); // Scales (and clamps) RGB components, but not A
	void ScaleAlpha(float alphaScale); // Scales (and clamps) Alpha, RGB is untouched

	RgbaFloats GetAsRgbaFloats() const;

	const Rgba operator + (const Rgba& other) const;
	const Rgba operator - (const Rgba& other) const;
	const Rgba operator * (float value) const;

public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	static const Rgba CLEAR;
	static const Rgba WHITE;
	static const Rgba BLACK;
	static const Rgba GRAY;
	static const Rgba RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba ORANGE;
	static const Rgba YELLOW;
	static const Rgba PINK;
	static const Rgba BROWN;
	static const Rgba MAGENTA;
	static const Rgba DARK_GREY;
	static const Rgba TRANSPARENT_BLACK;
};

void DebugPrintRgba(const Rgba& color);

Rgba ConvertColorFloatsToBytes(const RgbaFloats& floatsColor);

RgbaFloats ConvertColorBytesToFloats(const Rgba& bytesColor);

inline void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

inline void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = (unsigned char)normalizedRed;
	g = (unsigned char)normalizedGreen;
	b = (unsigned char)normalizedBlue;
	a = (unsigned char)normalizedAlpha;
}

inline void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	float inverse255 = 1 / 255.f;
	out_normalizedRed = r * inverse255;
	out_normalizedGreen = g * inverse255;
	out_normalizedBlue = b * inverse255;
	out_normalizedAlpha = a * inverse255;
}

inline void Rgba::ScaleRGB(float rgbScale)
{
	r *= (unsigned char)rgbScale;
	g *= (unsigned char)rgbScale;
	b *= (unsigned char)rgbScale;
}

inline void Rgba::ScaleAlpha(float alphaScale)
{
	a *= (unsigned char)alphaScale;
}

inline RgbaFloats Rgba::GetAsRgbaFloats() const
{
	return RgbaFloats(r * INV_255, g * INV_255, b * INV_255, a * INV_255);
}

inline const Rgba Rgba::operator + (const Rgba& other) const
{
	return Rgba(r + other.r, g + other.g, b + other.b, a + other.a);
}

inline const Rgba Rgba::operator - (const Rgba& other) const
{
	return Rgba(r - other.r, g - other.g, b - other.b, a - other.a);
}

inline const Rgba Rgba::operator * (float value) const
{
	return Rgba((unsigned char)(r * value), (unsigned char)(g * value), (unsigned char)(b * value), (unsigned char)(a * value));
}