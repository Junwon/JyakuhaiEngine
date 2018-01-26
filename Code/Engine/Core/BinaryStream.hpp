#pragma once
#include <malloc.h>
#include <string>

typedef uint8_t byte_t;

static constexpr unsigned int ENDIAN_CHECK = 0x01020304;

bool constexpr IsBigEndian()
{
	return ((unsigned char*)(&ENDIAN_CHECK))[0] == 0x01;
}

enum eEndianness
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

eEndianness constexpr GetHostOrder() 
{
	return IsBigEndian() ? BIG_ENDIAN : LITTLE_ENDIAN;
}

class BinaryStream
{
public:
	BinaryStream() {};

public:
	virtual unsigned int ReadBytes(void* outBuffer, const unsigned int count) = 0;
	virtual unsigned int WriteBytes(const void* outBuffer, const unsigned int count) = 0;

public:
	template <typename T>
	T Read();

public:
	template <typename T>
	void Write(const T& data);

public:
	inline bool ShouldFlip() const { return m_streamOrder != GetHostOrder(); }

	unsigned int WriteEndianAwareBytes(const void* data, unsigned int dataSize);
	unsigned int ReadEndianAwareBytes(void* data, unsigned int dataSize);

public:
	eEndianness m_streamOrder = LITTLE_ENDIAN;
};

template <typename T>
T BinaryStream::Read()
{
	T data;
	ReadBytes(&data, sizeof(T));
	return data;
}

template <typename T>
void BinaryStream::Write(const T& data)
{
	WriteBytes(&data, sizeof(T));
}
