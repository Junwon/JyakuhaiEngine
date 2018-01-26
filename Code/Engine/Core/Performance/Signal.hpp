#pragma once

#include <atomic>
#include "Engine/Core/Performance/CriticalSection.hpp"

#include <condition_variable>

class Signal
{
public:
	Signal();
	~Signal();

	// Will signal all threads waiting on this object.
	void SignalAll();

	// Suspends calling thread until object is signaled.
	void Wait();

	// Suspends calling thread until object is signaled, or 'ms' MS have
	// passed.  Returns true if object was signaled, and fales if it timed out.
	bool WaitFor(unsigned int ms);

public:
	HANDLE osEvent;
};

void SignalTest();