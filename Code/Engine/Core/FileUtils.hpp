#pragma once
#include <vector>

bool WriteBufferToFile(const std::vector<unsigned char>& buffer, const std::string& filePath);
bool WriteBufferToFile(std::string& buffer, const std::string& filePath);

bool ReadBufferFromFile(std::vector<unsigned char>& outBuffer, const std::string& filePath);
bool ReadBufferFromFile(std::string& outBuffer, const std::string& filePath);
bool ReadWhiteSpacesDelimitedBufferFromFile(std::vector<std::string>& outBuffer, const std::string& filePath);