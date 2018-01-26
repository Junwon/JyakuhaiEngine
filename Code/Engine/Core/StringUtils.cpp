#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
const void SplitIntoBuffer(std::vector<std::string>& outBuffer, std::string str, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		outBuffer.push_back(token);
		str.erase(0, pos + delimiter.length());
	}

	token = str.substr(0, pos);

	if (!token.empty()) {
		outBuffer.push_back(token);
	}
}

//-----------------------------------------------------------------------------------------------
const void SplitIntoIntBuffer(std::vector<int>& outBuffer, std::string str, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		outBuffer.push_back(stoi(token));
		str.erase(0, pos + delimiter.length());
	}

	token = str.substr(0, pos);
	outBuffer.push_back(stoi(token));
}

//-----------------------------------------------------------------------------------------------
const void SplitIntoFloatBuffer(std::vector<float>& outBuffer, std::string str, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		outBuffer.push_back(stof(token));
		str.erase(0, pos + delimiter.length());
	}

	token = str.substr(0, pos);
	outBuffer.push_back(stof(token));
}

//-----------------------------------------------------------------------------------------------
bool IsStringNullOrEmpty(const char* string)
{
	return string == nullptr || strlen(string) == 0;
}

//-----------------------------------------------------------------------------------------------
bool AreEqual(const char* a, const char* b)
{
	return (strcmp(a, b) == 0);
}

//-----------------------------------------------------------------------------------------------
bool IsStringNotOnlyWhiteSpaces(const std::string& string)
{
	return string.find_first_not_of(' ') != std::string::npos;
}