#include "Engine/Core/Configuration.hpp"

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define UNUSED(x) (void)(x);

std::map< std::string, std::string > m_configKVP;

bool ConfigSystemStartup(char const *config_file)
{
	return ConfigLoadFile(config_file);
}

std::string ConvertBufferToString(std::vector<unsigned char>&  outBuffer)
{
	std::string returnString = "";

	for (auto const& c : outBuffer) {
		returnString += c;
	}

	return returnString;
}

void LoadConfigurationToStrings(const std::string& s)
{
	std::vector<std::string> lineOfString;
	SplitIntoBuffer(lineOfString, s, "\n");

	for (std::string& line : lineOfString) {
		
		if (line.at(0) != '#')
		{
			std::vector<std::string> keyAndValuePair;
			SplitIntoBuffer(keyAndValuePair, line, "=");

			if (keyAndValuePair.size() == 2)
			{
				std::string loadedKey = keyAndValuePair.at(0);
				loadedKey.erase(remove_if(loadedKey.begin(), loadedKey.end(), isspace), loadedKey.end());

				std::string loadedValue = keyAndValuePair.at(1);
				loadedValue.erase(remove_if(loadedValue.begin(), loadedValue.end(), isspace), loadedValue.end());

				m_configKVP.insert(std::pair<std::string, std::string>(loadedKey, loadedValue));
			}
		}
	}
}

void ConfigSet(const char* name, const char* value)
{
	auto index = m_configKVP.find(name);

	if (index != m_configKVP.end()) {
		index->second = value;
	}
}

void ConfigSet(const char* name, const int value)
{
	auto index = m_configKVP.find(name);

	if (index != m_configKVP.end()) {
		index->second = std::to_string(value);
	}
}

void ConfigSet(const char* name, const float value)
{
	auto index = m_configKVP.find(name);

	if (index != m_configKVP.end()) {
		index->second = std::to_string(value);
	}
}

void ConfigSet(const char* name, const bool value)
{
	auto index = m_configKVP.find(name);

	if (index != m_configKVP.end()) {
		if (value) {
			index->second = "true";
		}
		else {
			index->second = "false";
		}
	}
}

bool IsConfigSet(const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		if (!index->second.empty()) {
			return true;
		}
	}

	return false;
}

void ConfigUnset(const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		index->second = "";
	}
}

bool ConfigGetString(const char** outv, const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		*outv = index->second.c_str();
		return true;
	}

	return false;
}

bool ConfigGetString(std::string* outv, const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		*outv = index->second;
		return true;
	}

	return false;
}

bool ConfigGetBool(bool* outv, const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		if (index->second == "true") {
			*outv = true;
		}
		else {
			*outv = false;
		}

		return true;
	}

	return false;
}

bool ConfigGetInt(int* outv, const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		*outv = atoi(index->second.c_str());
		return true;
	}

	return false;
}

bool ConfigGetFloat(float* outv, const char* id)
{
	auto index = m_configKVP.find(id);

	if (index != m_configKVP.end()) {
		*outv = (float)atof(index->second.c_str());
		return true;
	}

	return false;
}

void ConfigList()
{
	DebuggerPrintlnf("=====================");
	DebuggerPrintlnf("Current Loaded Configs:");
	auto iter = m_configKVP.begin();
	while (iter != m_configKVP.end()) {
		DebuggerPrintlnf("%s = %s", iter->first.c_str(), iter->second.c_str());
		iter++;
	}
	DebuggerPrintlnf("=====================");
}

bool ConfigLoadFile(const char* filename)
{
	std::vector<unsigned char> byteBuffer;

	bool readSuccess = false;
	readSuccess = ReadBufferFromFile(byteBuffer, filename);

	if (readSuccess)
	{
		std::string lineOfConfig = ConvertBufferToString(byteBuffer);

		LoadConfigurationToStrings(lineOfConfig);
	}

	return readSuccess;
}

