#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "Engine/Core/BinaryStream.hpp"

#include <stdio.h>

class FileBinaryStream : public BinaryStream
{
public:
	FileBinaryStream();
	~FileBinaryStream();

public:
	bool OpenForRead(const char* filename);

	bool OpenForWrite(const char* filename);

	void Close();

	virtual unsigned int ReadBytes(void* outBuffer, const unsigned int count) override;

	virtual unsigned int WriteBytes(const void* outBuffer, const unsigned int count) override;

	inline bool IsOpen() const { return nullptr != m_filePointer; }

public:
	FILE* m_filePointer;

};
