#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

const void SplitIntoBuffer(std::vector<std::string>& outBuffer, std::string str, std::string delimiter = " ");
const void SplitIntoIntBuffer(std::vector<int>& outBuffer, std::string str, std::string delimiter = " ");
const void SplitIntoFloatBuffer(std::vector<float>& outBuffer, std::string str, std::string delimiter);

bool IsStringNullOrEmpty(const char* string);
bool AreEqual(const char* a, const char* b);
bool IsStringNotOnlyWhiteSpaces(const std::string& string);