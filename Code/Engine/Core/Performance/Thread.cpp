#include "Engine/Core/Performance/Thread.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"

// Struct used to pass a name for the attached debugger
#define MS_VC_EXCEPTION      (0x406d1388)

#pragma pack(push, 8)
struct THREADNAME_INFO
{
	DWORD type;			// must be 0x1000
	const char *name;   // name
	DWORD threadID;     // -1 for calling thread
	DWORD flags;        // must be 0, reserved for future use
};
#pragma pack(pop)

//------------------------------------------------------------------------
//------------------------------------------------------------------------

struct ThreadPassData_T
{
	ThreadCB cb;
	void* arg;
};

struct GarbageThreadParameter_T
{
	GarbageThreadParameter_T() :
		fileDir(""),
		byteSize(0)
	{};
	const char* fileDir;
	unsigned int byteSize;
};

//------------------------------------------------------------------------
static DWORD WINAPI ThreadEntryPointCommon(void* arg)
{
	ThreadPassData_T* passPtr = (ThreadPassData_T*)arg;

	passPtr->cb(passPtr->arg);
	delete passPtr;
	return 0;
}

//------------------------------------------------------------------------
// Creates a thread with the entry point of cb, passed data
ThreadHandle_T ThreadCreate(ThreadCB cb, void* data)
{
	// Handle is like pointer, or reference to a thread
	// threadID is unique identifier
	ThreadPassData_T* pass = new ThreadPassData_T();
	pass->cb = cb;
	pass->arg = data;

	DWORD threadID;
	ThreadHandle_T th = (ThreadHandle_T) ::CreateThread(nullptr,   // SECURITY OPTIONS
		0,                         // STACK SIZE, 0 is default
		ThreadEntryPointCommon,    // "main" for this thread
		pass,                      // data to pass to it
		0,                         // initial flags
		&threadID);                // thread_id

	return th;
}

void GarbageThread(void* arg) {
	GarbageThreadParameter_T* l_arg = (GarbageThreadParameter_T*)arg;
	const char* fileDir = l_arg->fileDir;
	unsigned int garbageSize = l_arg->byteSize;

	if (strcmp(fileDir, "")) {
		fileDir = THREAD_TEST_DIR;
	}

	FILE* fh = nullptr;
	errno_t err = fopen_s(&fh, fileDir, "w+");
	if ((err != 0) || (fh == nullptr)) {
		return;
	}

	for (unsigned int i = 0; i < garbageSize; ++i) {
		fwrite("0", 1, 1, fh);
	}

	fclose(fh);

	delete l_arg;
	l_arg = nullptr;

	DebuggerPrintlnf("Thread test complete.");
}

// void* depends on 64(8B) - 32(4B) bit
void ThreadTest(const char* fileDir, unsigned int byteSize) {
	GarbageThreadParameter_T* arg = new GarbageThreadParameter_T();
	arg->fileDir = fileDir;
	arg->byteSize = byteSize;

	ThreadHandle_T threadTest = ThreadCreate(GarbageThread, (void*)arg);

	ThreadDetach(threadTest);

	threadTest = INVALID_THREAD_HANDLE;
}

//------------------------------------------------------------------------
void ThreadSleep(unsigned int ms) {
	::Sleep(ms);
}

//------------------------------------------------------------------------
void ThreadYield() {
	::SwitchToThread();
}

//------------------------------------------------------------------------
// Releases my hold on this thread.
void ThreadDetach(ThreadHandle_T th) {
	::CloseHandle(th);
}

//------------------------------------------------------------------------
void ThreadJoin(ThreadHandle_T th) {
	::WaitForSingleObject(th, INFINITE);
	::CloseHandle(th);
}

//------------------------------------------------------------------------
void ThreadJoin(ThreadHandle_T* th, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i) {
		ThreadJoin(th[i]);
	}
}

//------------------------------------------------------------------------
ThreadID_T ThreadGetCurrentID()
{
	return (ThreadID_T)(uintptr_t) ::GetCurrentThreadId();
}

//------------------------------------------------------------------------
void ThreadSetNameInVisualStudio(const char* name)
{
	if (nullptr == name) {
		return;
	}

	ThreadID_T id = ThreadGetCurrentID();
	if (0 != id) {
		THREADNAME_INFO info;
		info.type = 0x1000;
		info.name = name;
		info.threadID = (DWORD)(uintptr_t)id;
		info.flags = 0;

		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)(&info));
		}
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}
}
