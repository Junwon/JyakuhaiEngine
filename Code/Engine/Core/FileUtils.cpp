#include "Engine/Core/FileUtils.hpp"
#include <ctype.h>

bool WriteBufferToFile(const std::vector<unsigned char>& buffer, const std::string& filePath)
{
	FILE* file = nullptr;

	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb"); 

	if (errorCode != 0)
	{
		return false;
	}

	fwrite(buffer.data(), 1, buffer.size(), file);

	fclose(file);

	return true;
}

bool WriteBufferToFile(std::string& buffer, const std::string& filePath)
{
	FILE* file = nullptr;

	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");

	if (errorCode != 0)
	{
		return false;
	}

	fwrite(buffer.data(), 1, buffer.size(), file);

	fclose(file);

	return true;
}

bool ReadBufferFromFile(std::vector<unsigned char>& outBuffer, const std::string& filePath)
{
	FILE* file = nullptr;

	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");

	if (errorCode != 0) return false;

	fseek(file, 0, SEEK_END);
	size_t  numBytes = ftell(file);

	fseek(file, 0, SEEK_SET);

	outBuffer.resize(numBytes);

	fread_s(outBuffer.data(), numBytes, 1, outBuffer.size(), file);

	fclose(file);

	return true;
}

bool ReadBufferFromFile(std::string& outBuffer, const std::string& filePath)
{
	FILE* file = nullptr;

	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");

	if (errorCode != 0) return false;

	fseek(file, 0, SEEK_END);
	size_t  numBytes = ftell(file);

	fseek(file, 0, SEEK_SET);

	outBuffer.resize(numBytes);

	fread_s(&outBuffer[0], numBytes, 1, outBuffer.size(), file);

	fclose(file);

	return true;
}

bool ReadWhiteSpacesDelimitedBufferFromFile(std::vector<std::string>& outBuffer, const std::string& filePath)
{
	std::string tempBuffer = std::string();

	if (ReadBufferFromFile(tempBuffer, filePath))
	{
		unsigned int index = 0;
		unsigned int startOfString = index;

		while (index < tempBuffer.size())
		{
			if (isspace(tempBuffer[index]))
			{
				std::string tempStr = tempBuffer.substr(startOfString, (index - startOfString));

				if (strcmp(tempStr.c_str(), "") != 0)
				{
					outBuffer.push_back(tempStr);
				}
				
				startOfString = index + 1;
			}

			index++;
		}

		return true;
	}

	return false;
}