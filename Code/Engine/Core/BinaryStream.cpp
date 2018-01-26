#include "Engine/Core/BinaryStream.hpp"
#include "Engine/Core/EngineCommon.hpp"


void CopyReversed(void* /*buffer*/, const void* /*src*/, const unsigned int /*size*/)
{
	/*byte_t* bytes = (byte_t*)buffer;

	unsigned int count = size / 2;
	for (unsigned int i = 0; i < count; ++i) {
		Swap(buffer[i], buffer[size - i - 1]);
	}*/
}

unsigned int BinaryStream::WriteEndianAwareBytes(const void* data, unsigned int dataSize)
{
	/*if (ShouldFlip()) {
		// _alloca allocates memory on the stack
		// src: https://msdn.microsoft.com/en-us/library/wb1s57t5.aspx
		// should be fine for what we're doing
		unsigned char* temp = (unsigned char*)_malloca(dataSize);
		CopyReversed(temp, data, dataSize);
		return WriteBytes(temp, dataSize);
	}
	else {
		return WriteBytes(data, dataSize);
	}*/

	return WriteBytes(data, dataSize);
}

unsigned int BinaryStream::ReadEndianAwareBytes(void* data, unsigned int dataSize)
{
	return ReadBytes(data, dataSize);
}