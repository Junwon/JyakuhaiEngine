#pragma once

#include "Engine/Tools/Python/PythonIncludes.hpp"
#include "Engine/Core/StringUtils.hpp"

// typedef py::detail::accessor<struct py::detail::accessor_policies::str_attr> PyFunction;
typedef py::object PyFunction;

class PyModule
{
public:
	PyModule();
	PyModule(const char* modName);
	~PyModule();

	PyFunction GetFunction(const char* moduleName);
public:
	py::object m_module;
	std::string m_modName;
};