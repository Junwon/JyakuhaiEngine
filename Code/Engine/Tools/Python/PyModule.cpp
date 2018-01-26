#include "Engine/Tools/Python/PyModule.hpp"

#include "Engine/Tools/Python/EmbeddedPython.hpp"

PyModule::PyModule()
{

}

PyModule::PyModule(const char* modName) :
	m_module(ImportPyModule(modName)),
	m_modName(modName)
{

}

PyModule::~PyModule()
{

}

PyFunction PyModule::GetFunction(const char* moduleName)
{
	return m_module.attr(moduleName);
}
