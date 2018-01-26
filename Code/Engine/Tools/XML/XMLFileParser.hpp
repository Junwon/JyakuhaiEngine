#pragma once

#include "ThirdParty/XMLParser/XMLParser.hpp"

class Rgba;
class Vector2;
class Vector3;
class IntVector2;

XMLNode OpenXMLFile(const char* fileDirectory, const char* attributeName);

int ParseXmlAttribute(const XMLNode& element, const char* attributeName, int defaultValue);

char ParseXmlAttribute(const XMLNode& element, const char* attributeName, char defaultValue);

bool ParseXmlAttribute(const XMLNode& element, const char* attributeName, bool defaultValue);

float ParseXmlAttribute(const XMLNode& element, const char* attributeName, float defaultValue);

Rgba ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Rgba& defaultValue);

Vector2 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Vector2& defaultValue);

Vector3 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Vector3& defaultValue);

IntVector2 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const IntVector2& defaultValue);

const char* ParseXmlAttribute(const XMLNode& element, const char* attributeName, const char* defaultValue);

void ValidateXmlElement(const XMLNode& element, const char* commaSeparatedListOfValidChildElementNames, const char* commaSeparatedListOfValidAttributeNames);