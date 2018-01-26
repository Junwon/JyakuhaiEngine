#include "Engine/Core/FileBinaryStream.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

FileBinaryStream::FileBinaryStream()
	: m_filePointer(nullptr)
{}

//----------------------------------------------------------------------------
FileBinaryStream::~FileBinaryStream()
{
	Close();
}

//----------------------------------------------------------------------------
bool FileBinaryStream::OpenForRead(char const *filename)
{
	ASSERT_OR_DIE(!IsOpen(), "Cannot open file to read for File Binary Stream.");
	m_filePointer = fopen(filename, "rb"); // open for read & binary
	return IsOpen();
}

//----------------------------------------------------------------------------
bool FileBinaryStream::OpenForWrite(char const *filename)
{
	ASSERT_OR_DIE(!IsOpen(), "Cannot open file to write for File Binary Stream.");
	m_filePointer = fopen(filename, "wb"); // open for read & binary
	return IsOpen();
}

//----------------------------------------------------------------------------
void FileBinaryStream::Close()
{
	if (IsOpen()) 
	{
		fclose(m_filePointer);
		m_filePointer = nullptr;
	}
}

//----------------------------------------------------------------------------
// BinaryStream Impl
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// read/write 'count' bytes.  Returns number of bytes actually read. 
// will return 0 on failure. 
unsigned int FileBinaryStream::ReadBytes(void* outBuffer, const unsigned int count)
{
	unsigned int bytesRead = 0;
	if (IsOpen()) 
	{
		bytesRead = (unsigned int)fread(outBuffer, 1, count, m_filePointer);
	}

	return bytesRead;
}

//----------------------------------------------------------------------------
unsigned int FileBinaryStream::WriteBytes(const void* buffer, const unsigned int count)
{
	unsigned int bytesWritten = 0;
	if (IsOpen()) 
	{
		bytesWritten = (unsigned int)fwrite(buffer, 1, count, m_filePointer);
	}

	return bytesWritten;
}