#include "Engine/Tools/XML/XMLFileParser.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <iostream>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"

//-----------------------------------------------------------------------------------------------
XMLNode OpenXMLFile(const char* fileDirectory, const char* attributeName)
{
	return XMLNode::openFileHelper(fileDirectory, attributeName);
}

//-----------------------------------------------------------------------------------------------
int ParseXmlAttribute(const XMLNode& element, const char* attributeName, int defaultValue)
{
	int returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strcmp(attributeValue, "") == 0)
		{
			return defaultValue;
		}

		try
		{
			returnValue = std::stoi(attributeValue);
		}
		catch (std::invalid_argument&)
		{
			ASSERT_OR_DIE(nullptr, Stringf("Expected \'int\' value for attribute \"%s\". Returned \"%s\".", attributeName, attributeValue));
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
char ParseXmlAttribute(const XMLNode& element, const char* attributeName, char defaultValue)
{
	char returnValue = defaultValue;
	
	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strcmp(attributeValue, "") == 0)
		{
			return defaultValue;
		}

		if (strlen(attributeValue) != 0)
		{
			returnValue = attributeValue[0];
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
bool ParseXmlAttribute(const XMLNode& element, const char* attributeName, bool defaultValue)
{
	bool returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strcmp(attributeValue, "") == 0)
		{
			return defaultValue;
		}

		if (strcmp(attributeValue, "true") == 0)
		{
			returnValue = true;
		}
		else if (strcmp(attributeValue, "false") == 0)
		{
			returnValue = false;
		}
		else
		{
			ASSERT_OR_DIE(nullptr, Stringf("Expected boolean value \"true\" or \"false\". Attribute \"%s\" returned \"%s\".", attributeName, attributeValue));
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
float ParseXmlAttribute(const XMLNode& element, const char* attributeName, float defaultValue)
{
	float returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strcmp(attributeValue, "") == 0)
		{
			return defaultValue;
		}

		try
		{
			returnValue = std::stof(attributeValue);
		}
		catch (std::invalid_argument&)
		{
			ASSERT_OR_DIE(nullptr, Stringf("Expected \'float\' value for attribute \"%s\". Returned \"%s\".", attributeName, attributeValue));
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
Rgba ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Rgba& defaultValue)
{
	Rgba returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strlen(attributeValue) != 0)
		{
			std::vector<int> rgbaValues;

			try
			{
				SplitIntoIntBuffer(rgbaValues, attributeValue, ",");
			}
			catch (std::invalid_argument&)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Rgba attribute \"%s\" in node \"%s\" has invalid arguments: \"%s\".", attributeName, element.getName(), attributeValue));
			}	

			if (rgbaValues.size() > 4)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Rgba attribute \"%s\" in node \"%s\" exceeds 4 parameters.", attributeName, element.getName()));
			}

			if (rgbaValues.size() > 0)
			{
				size_t index = 0;

				returnValue.r = (unsigned char)rgbaValues[index];

				index++;

				if (index < rgbaValues.size())
				{
					returnValue.g = (unsigned char)rgbaValues[index];

					index++;

					if (index < rgbaValues.size())
					{
						returnValue.b = (unsigned char)rgbaValues[index];

						index++;

						if (index < rgbaValues.size())
						{
							returnValue.a = (unsigned char)rgbaValues[index];
						}
						else
						{
							returnValue.a = 255;
						}
					}
				}
			}
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
Vector2 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Vector2& defaultValue)
{
	Vector2 returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strlen(attributeValue) != 0)
		{
			std::vector<float> vector2Values;

			try
			{
				SplitIntoFloatBuffer(vector2Values, attributeValue, ",");
			}
			catch (std::invalid_argument&)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Vector2 attribute \"%s\" in node \"%s\" has invalid arguments: \"%s\".", attributeName, element.getName(), attributeValue));
			}

			if (vector2Values.size() != 2)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Vector2 attribute \"%s\" in node \"%s\" requires 2 parameters. Returned \"%s\"", attributeName, element.getName(), attributeValue));
			}

			returnValue.x = vector2Values[0];

			returnValue.y = vector2Values[1];
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
Vector3 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const Vector3& defaultValue)
{
	Vector3 returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strlen(attributeValue) != 0)
		{
			std::vector<float> vector3Values;

			try
			{
				SplitIntoFloatBuffer(vector3Values, attributeValue, ",");
			}
			catch (std::invalid_argument&)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Vector2 attribute \"%s\" in node \"%s\" has invalid arguments: \"%s\".", attributeName, element.getName(), attributeValue));
			}

			if (vector3Values.size() != 3)
			{
				ASSERT_OR_DIE(nullptr, Stringf("Vector2 attribute \"%s\" in node \"%s\" requires 2 parameters. Returned \"%s\"", attributeName, element.getName(), attributeValue));
			}

			returnValue.x = vector3Values[0];

			returnValue.y = vector3Values[1];

			returnValue.z = vector3Values[2];
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
IntVector2 ParseXmlAttribute(const XMLNode& element, const char* attributeName, const IntVector2& defaultValue)
{
	IntVector2 returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strlen(attributeValue) != 0)
		{
			std::vector<int> intVector2Values;

			try
			{
				SplitIntoIntBuffer(intVector2Values, attributeValue, ",");
			}
			catch (std::invalid_argument&)
			{
				ASSERT_OR_DIE(nullptr, Stringf("IntVector2 attribute \"%s\" in node \"%s\" has invalid arguments: \"%s\".", attributeName, element.getName(), attributeValue));
			}

			if (intVector2Values.size() != 2)
			{
				ASSERT_OR_DIE(nullptr, Stringf("IntVector2 attribute \"%s\" in node \"%s\" requires 2 parameters. Returned \"%s\"", attributeName, element.getName(), attributeValue));
			}

			returnValue.x = intVector2Values[0];

			returnValue.y = intVector2Values[1];
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
const char* ParseXmlAttribute(const XMLNode& element, const char* attributeName, const char* defaultValue)
{
	const char* returnValue = defaultValue;

	const char* attributeValue = element.getAttribute(attributeName);

	if (element.getAttribute(attributeName) == nullptr)
	{
		return defaultValue;
	}
	else
	{
		if (strlen(attributeValue) != 0)
		{
			returnValue = attributeValue;
		}
	}

	return returnValue;
}

//-----------------------------------------------------------------------------------------------
void ValidateXmlElement(const XMLNode& element, const char* commaSeparatedListOfValidChildElementNames, const char* commaSeparatedListOfValidAttributeNames)
{
	UNUSED(element);
	UNUSED(commaSeparatedListOfValidChildElementNames);
	UNUSED(commaSeparatedListOfValidAttributeNames);
}

