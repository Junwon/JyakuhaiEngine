#pragma once

#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/Performance/ThreadLogger.hpp"

class SimpleRenderer;

struct ProfilerNode_T
{
public:
	ProfilerNode_T(const char* tag); 

public:
	double GetSelfElapsedTimeSeconds() const;
	bool HasFirstChild() const;
	bool HasLastChild() const;
	int IndexOfChildWhereTagExists(const char* tag);
	double GetSelfActiveTime() const;
public:
	const char* m_tag;
	unsigned int m_callCount;
	uint64_t m_startCounter;
	uint64_t m_endCounter;
	double m_selfElapsedTime;
	double m_lastActiveElapsedTime;
	ProfilerNode_T* m_parent;
	ProfilerNode_T* m_firstChild;
	ProfilerNode_T* m_lastChild;
	ProfilerNode_T* m_leftSibling;
	ProfilerNode_T* m_rightSibling;
};

void ProfilerStartup();
void ProfilerShutdown();
void ProfilerPush(const char* tag);
void ProfilerPop();

ProfilerNode_T* ProfilerGetPreviousFrame();
ProfilerNode_T* ProfilerGetPreviousFrameWithTag(const char* rootTag);
void ProfilerPause();
void ProfilerResume();
void ProfilerSnapshotSingleFrame();
void ProfilerStartFrame();
void ProfilerEndFrame();
double ProfilerGetLastFrameTimeSeconds();
bool CanProfileRun();

///================================================================================


#define PROFILE_LOG_SCOPE(descriptionMsg) ProfilerLogScope __pLogScope(descriptionMsg)

/*
Example: PROFILE_LOG_SCOPE("GenerateNoise");
->LOG: "profiler", "'GenerateNoise' took 32.0024 ms."
*/

class ProfilerLogScope
{
public:
	ProfilerLogScope(const char* scopeName);
	~ProfilerLogScope();

private:
	const char*		m_scopeName;
	uint64_t		m_startingCounter;
};

inline ProfilerLogScope::ProfilerLogScope(const char* scopeName) :
	m_scopeName(scopeName),
	m_startingCounter(GetCurrentPerformanceCounter())
{
}

inline ProfilerLogScope::~ProfilerLogScope()
{
	double elapsedSeconds = CalcPerformanceCounterToSeconds(m_startingCounter);

	if (elapsedSeconds < 0.001)
	{
		double elapsedMicroseconds = ConvertSecondsToMicroseconds(elapsedSeconds);
		LogTaggedPrintf("Profiler", "\"%s\" took %.04f us", m_scopeName, elapsedMicroseconds);
	}
	else if (elapsedSeconds < 1)
	{
		double elapsedMilliseconds = ConvertSecondsToMilliseconds(elapsedSeconds);
		LogTaggedPrintf("Profiler", "\"%s\" took %.04f ms", m_scopeName, elapsedMilliseconds);
	}
	else
	{
		LogTaggedPrintf("Profiler", "\"%s\" took %.04f s", m_scopeName, elapsedSeconds);
	}
}

///================================================================================

#define PROFILE_SCOPE(tagString) ProfilerScope __pScope(tagString)

#define PROFILE_SCOPE_FUNCTION() ProfilerScope __pScope(__FUNCTION__)

class ProfilerScope
{
public:
	ProfilerScope(const char* tagName);
	~ProfilerScope();

private:
	uint64_t		m_startingCounter;
};

inline ProfilerScope::ProfilerScope(const char* tagName) :
	m_startingCounter(GetCurrentPerformanceCounter())
{
	ProfilerPush(tagName);
}

inline ProfilerScope::~ProfilerScope()
{
	ProfilerPop();
}