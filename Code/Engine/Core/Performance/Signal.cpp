#include "Engine/Core/Performance/Signal.hpp"

//------------------------------------------------------------------------
//
Signal::Signal()
{
	osEvent = ::CreateEvent(nullptr,	// security attributes, not needed
		FALSE,                          // Manual reset - do we manually reset, or auto reset after this is hit.
		TRUE,							// initial state of the signal
		NULL);							// name, used for cross-process communication
}

//------------------------------------------------------------------------
Signal::~Signal()
{
	::CloseHandle(osEvent);
	osEvent = NULL;
}

//------------------------------------------------------------------------
void Signal::SignalAll()
{
	::SetEvent(osEvent);
}

//------------------------------------------------------------------------
void Signal::Wait()
{
	DWORD result = ::WaitForSingleObject(osEvent, INFINITE);
	if (result == WAIT_OBJECT_0) {
		// Success! Can wait for multiple objects,
		// hence the WAIT_OBJECT_0 - meaning the first object was
		// the one who caused the system to activate.

		// All signals will reset. 
		::ResetEvent(osEvent);
	}
}


//------------------------------------------------------------------------
bool Signal::WaitFor(unsigned int ms)
{
	DWORD result = ::WaitForSingleObject(osEvent, ms);
	if (result == WAIT_OBJECT_0) {
		
		::ResetEvent(osEvent);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#include "Engine/Core/Performance/Thread.hpp"

static bool gSignalTestRunning = false;

//------------------------------------------------------------------------
struct SignalTest_T
{
	unsigned int index;
	Signal* signal;
};

//------------------------------------------------------------------------
static void SignalTestThread(void *data)
{
	SignalTest_T* test = (SignalTest_T*)data;

	unsigned int count = 0;
	while (gSignalTestRunning) {
		test->signal->Wait();
		++count;
		printf("Thread[%i] was signaled [%i].\n", test->index, count);
	}

	delete test;
}

//------------------------------------------------------------------------
void SignalTest()
{
	unsigned int num_threads = 8;
	Signal signal;

	gSignalTestRunning = true;

	for (unsigned int i = 0; i < num_threads; ++i) {
		SignalTest_T *test = new SignalTest_T();
		test->index = i;
		test->signal = &signal;

		// Thread will handle deleting the variable.
		ThreadHandle_T handle = ThreadCreate(SignalTestThread, test);
		ThreadDetach(handle);
	}

	// Give all the threads a chance to startup.
	// Not guaranteed, but good enough for a test.
	ThreadSleep(50);

	// Signal them 50 times - do all go at once, or 
	// do they go one at a time?
	for (unsigned int i = 0; i < 50; ++i) {
		signal.SignalAll();
		ThreadSleep(100);
	}

	gSignalTestRunning = false;
	signal.SignalAll();

	// Not the safest - but just wait till everyone has exited.
	ThreadSleep(100);
}