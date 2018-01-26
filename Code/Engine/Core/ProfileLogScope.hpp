#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

/*
Example: PROFILE_LOG_SCOPE("GenerateNoise");
->LOG: "profiler", "'GenerateNoise' took 32.0024 ms."
*/

class ProfileLogScope
{
public:
	ProfileLogScope(const char* scopeName);
	~ProfileLogScope();

private:
	const char*		m_scopeName;
	double			m_timeAtStart;
};

inline ProfileLogScope::ProfileLogScope(const char* scopeName) :
	m_scopeName(scopeName),
	m_timeAtStart(GetCurrentTimeSeconds())
{
}

inline ProfileLogScope::~ProfileLogScope()
{
	double currentTime = GetCurrentTimeSeconds();
	double elapseSeconds = currentTime - m_timeAtStart;
	DebuggerPrintf("ProfileLogScope \"%s\" took %.05f ms\n", m_scopeName, 1000.0 * elapseSeconds);
}