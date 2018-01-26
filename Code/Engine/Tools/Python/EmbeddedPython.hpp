#pragma once
#include "Engine/Tools/Python/PythonIncludes.hpp"
#include "Engine/Core/StringUtils.hpp"

void PyStartup();
void PyShutdown();

/**
	Use double back-slashes '\\' instead of forward-slashes '/' for path parameter
*/
void PyIncludeModulePath(const char* path);

std::string GetIncludedPathsString();

py::object ImportPyModule(const char* name);

/**
Notes: 
- no way to check compiler or run-time error for python scripts
*/