#pragma once

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)

// Creates a scoped critical section object named __lock_<LINE#>( &cs )
#define SCOPE_LOCK( m_criticalSection ) ScopeCriticalSection COMBINE(___lock_,__LINE__)(&m_criticalSection)

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Mutual Exclusive [mutex]: 
// Fair:  Serviced in order of request.
// Recursive: Someone is allowed to lock multiple times.
class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

	// Enteres the lock - only one thread is allowed to be inside this lock at a time, every
	// other caller will block on this call until they're allowed to enter.
	void Lock();

	// Tries to enter the lock - returns TRUE if the lock was entered, false if it failed.
	bool TryLock();

	// Leave the lock - allowing the next person to lock to be able to enter.
	void Unlock();

public:
	CRITICAL_SECTION m_criticalSection;
};

//------------------------------------------------------------------------
class ScopeCriticalSection
{
public:
	ScopeCriticalSection(CriticalSection* ptr);
	~ScopeCriticalSection();

public:
	CriticalSection* m_criticalSectionPtr;
};