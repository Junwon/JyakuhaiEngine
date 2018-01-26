#include "Engine/Core/Rgba.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

const Rgba Rgba::CLEAR = Rgba(255, 255, 255, 0);
const Rgba Rgba::WHITE = Rgba(255, 255, 255, 255);
const Rgba Rgba::BLACK = Rgba(0, 0, 0, 255);
const Rgba Rgba::GRAY = Rgba(128, 128, 128, 255);
const Rgba Rgba::RED = Rgba(255, 0, 0, 255);
const Rgba Rgba::GREEN = Rgba(0, 255, 0, 255);
const Rgba Rgba::BLUE = Rgba(0, 0, 255, 255);
const Rgba Rgba::ORANGE = Rgba(255, 165, 0, 255);
const Rgba Rgba::YELLOW = Rgba(255, 255, 0, 255);
const Rgba Rgba::PINK = Rgba(255, 192, 203, 255);
const Rgba Rgba::BROWN = Rgba(102, 51, 0, 255);
const Rgba Rgba::MAGENTA = Rgba(255, 0, 255, 255);
const Rgba Rgba::DARK_GREY = Rgba(20, 20, 20, 255);
const Rgba Rgba::TRANSPARENT_BLACK = Rgba(0, 0, 0, 100);

const RgbaFloats RgbaFloats::WHITE = RgbaFloats(1.f, 1.f, 1.f, 1.f);
const RgbaFloats RgbaFloats::RED = RgbaFloats(1.f, 0.f, 0.f, 1.f);
const RgbaFloats RgbaFloats::GREEN = RgbaFloats(0.f, 1.f, 0.f, 1.f);
const RgbaFloats RgbaFloats::BLUE = RgbaFloats(0.f, 0.f, 1.f, 1.f);
const RgbaFloats RgbaFloats::ORANGE = RgbaFloats(1.f, .65f, 0.f, 1.f);
const RgbaFloats RgbaFloats::YELLOW = RgbaFloats(1.f, 1.f, 0.f, 1.f);;
const RgbaFloats RgbaFloats::PINK = RgbaFloats(1.f, 0.75f, 0.8f, 1.f);;
const RgbaFloats RgbaFloats::MAGENTA = RgbaFloats(1.f, 0.f, 1.f, 1.f);;

Rgba::Rgba() :
	r(0),
	g(0),
	b(0),
	a(0)
{
}

Rgba::Rgba(const Rgba& copy) :
	r(copy.r),
	g(copy.g),
	b(copy.b),
	a(copy.a)
{
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) :
	r(red),
	g(green),
	b(blue),
	a(alpha)
{
}

Rgba::Rgba(const RgbaFloats& floatCopy) :
	r((unsigned char)(floatCopy.r * 255)),
	g((unsigned char)(floatCopy.g * 255)),
	b((unsigned char)(floatCopy.b * 255)),
	a((unsigned char)(floatCopy.a * 255))
{
}

Rgba ConvertColorFloatsToBytes(const RgbaFloats& floatsColor)
{
	return Rgba((unsigned char)floatsColor.r * 255, (unsigned char)floatsColor.g * 255, (unsigned char)floatsColor.b * 255, (unsigned char)floatsColor.a * 255);
}

RgbaFloats ConvertColorBytesToFloats(const Rgba& bytesColor)
{
	float divisor = 1 / 255.f;
	return RgbaFloats(bytesColor.r * divisor, bytesColor.g * divisor, bytesColor.b * divisor, bytesColor.a * divisor);
}

void DebugPrintRgba(const Rgba& color)
{
	DebuggerPrintlnf("R: %u, G: %u, B: %u, A: %u", color.r, color.g, color.b, color.a);
}


