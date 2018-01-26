#include "Engine/Tools/Python/EmbeddedPython.hpp"

#include "Engine/Tools/Python/PyModule.hpp"

PyModule* pySystem = nullptr;

void PyStartup()
{
	py::initialize_interpreter();

	if (pySystem == nullptr) {
		pySystem = new PyModule("sys");
	}
}

void PyShutdown()
{
	delete pySystem;
	pySystem = nullptr;

	py::finalize_interpreter();
}

void PyIncludeModulePath(const char* path)
{
 	auto pathFunc = pySystem->GetFunction("path");
	pathFunc.attr("append")(path);
}

std::string GetIncludedPathsString()
{
	return py::str(pySystem->GetFunction("path"));
}

/**
 Use this like Python's 'import module'.
*/
py::object ImportPyModule(const char* name)
{
	return py::module::import(name);
}