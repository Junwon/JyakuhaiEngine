#include "Engine/Core/Performance/Memory.hpp"

#include <malloc.h>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/Performance/Callstack.hpp"
#include "Engine/Core/Performance/BuildConfig.hpp"
#include "Engine/Core/Performance/CriticalSection.hpp"
#include "Engine/Core/Performance/ThreadLogger.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"

unsigned int g_AllocCount = 0;
unsigned int g_FrameAllocs = 0;
unsigned int g_FrameFrees = 0;
size_t g_AllocatedByteCount = 0;
unsigned int g_highwater = 0;

CriticalSection m_lock;

struct allocation_t {
	allocation_t() :
		callstack(nullptr),
		next(nullptr),
		prev(nullptr)
	{};

	size_t byteSize;

	Callstack* callstack;
	allocation_t* next;
	allocation_t* prev;
};

allocation_t* g_listTail = nullptr;

#if (TRACK_MEMORY == TRACK_MEMORY_BASIC)

void* operator new(size_t const size) {
	++g_AllocCount;

	++g_FrameAllocs;
	g_AllocatedByteCount += size;

	if (g_AllocatedByteCount > g_highwater) {
		g_highwater = g_AllocatedByteCount;
	}

	size_t allocSize = size + sizeof(allocation_t);

	allocation_t* ptr = (allocation_t*)malloc(allocSize);
	ptr->byteSize = size;
	return ptr + 1;
}

void operator delete(void *ptr) {
	--g_AllocCount;
	++g_FrameFrees;

	allocation_t* sizePtr = (allocation_t*)ptr;
	sizePtr--;

	g_AllocatedByteCount -= sizePtr->byteSize;
	free(sizePtr);
}

#elif  (TRACK_MEMORY == TRACK_MEMORY_VERBOSE)

void AddPtrToLinklist(allocation_t* ptr);
void RemovePtrFromLinklist(allocation_t* sizePtr);

void* operator new(size_t const size) {
	++g_AllocCount;

	++g_FrameAllocs;
	g_AllocatedByteCount += size;

	if (g_AllocatedByteCount > g_highwater) {
		g_highwater = g_AllocatedByteCount;
	}
	
	size_t allocSize = size + sizeof(allocation_t);

	allocation_t* ptr = (allocation_t*)malloc(allocSize);
	ptr->byteSize = size;

	AddPtrToLinklist(ptr);

	return ptr + 1;
}

void AddPtrToLinklist(allocation_t* ptr) {


	if (g_hasInitiatedCallstackStartup) {
		m_lock.Lock();
		ptr->callstack = CreateCallstack(0);
	}
	else {
		ptr->callstack = nullptr;
	}

	//if tail doesnt exist yet
	if (g_listTail == nullptr) {
		ptr->prev = nullptr;
		ptr->next = nullptr;
	}
	//if tail exists, add new ptr to end and tail points to it
	else {
		g_listTail->next = ptr;

		ptr->next = nullptr;
		ptr->prev = g_listTail;
	}
	g_listTail = ptr;

	if (g_hasInitiatedCallstackStartup) {
		m_lock.Unlock();
	}
}

void operator delete(void* ptr) {
	--g_AllocCount;
	++g_FrameFrees;

	allocation_t* sizePtr = (allocation_t*)ptr;
	sizePtr--;

	g_AllocatedByteCount -= sizePtr->byteSize;

	RemovePtrFromLinklist(sizePtr);
	
	free(sizePtr);
}

void RemovePtrFromLinklist(allocation_t* sizePtr) {

	if (sizePtr != nullptr) {
		if (g_hasInitiatedCallstackStartup) {
			m_lock.Lock();
			DestroyCallstack(sizePtr->callstack);
		}

		if (sizePtr->prev == nullptr && sizePtr->next == nullptr) {
			g_listTail = nullptr;
		}
		else {
			//end of list
			if (sizePtr->next == nullptr && sizePtr->prev != nullptr) {
				g_listTail = sizePtr->prev;
				g_listTail->next = nullptr;
			}
			//beginning of list
			else if (sizePtr->prev == nullptr && sizePtr->next != nullptr) {
				sizePtr->next->prev = nullptr;
			}
			//somewhere in between
			else {
				sizePtr->prev->next = sizePtr->next;
				sizePtr->next->prev = sizePtr->prev;
			}
		}

		if (g_hasInitiatedCallstackStartup) {
			m_lock.Unlock();
		}
	}
}

#endif


void ProfileMemoryFrameTick() {
	g_FrameAllocs = 0;
	g_FrameFrees = 0;
}

std::string CalculateReadableBytesString(unsigned int bytes) {
	float bytesAsFloat = (float)bytes;
	unsigned char timesConverted = 0;

	while (bytesAsFloat > 1024) {
		bytesAsFloat /= 1024;
		timesConverted++;
	}

	if (timesConverted == 1) {
		return Stringf("%.3f KiB", bytesAsFloat);
	}
	else if (timesConverted == 2) {
		return Stringf("%.3f MiB", bytesAsFloat);
	}
	else if (timesConverted >= 3) {
		return Stringf("%.3f GiB", bytesAsFloat);
	}

	return Stringf("%.0f B", bytesAsFloat);
}

void PrintCallstack(void*) {
	m_lock.Lock();
	allocation_t* tempIndex = g_listTail;
	int times = 0;

	while (tempIndex && tempIndex->callstack) {
		times++;
		LogPrintlnf("\n");
		LogTaggedPrintf("callstack", "Stack: %d", times);
		char lineBuffer[512];
		CallstackLine_T lines[128];
		unsigned int lineCount = CallstackGetLines(lines, 128, tempIndex->callstack);
		for (unsigned int i = 2; i < lineCount; ++i) {
			// this specific format will make it double clickable in an output window 
			// taking you to the offending line.
			sprintf_s(lineBuffer, 512, "%s(%u): %s",
				lines[i].filename, lines[i].line, lines[i].functionName);

			// print to output and console
			// OutputDebugStringA(lineBuffer);
			// printf(lineBuffer);
			LogTaggedPrintf("callstack", lineBuffer);

		}
		tempIndex = tempIndex->prev;
	}
	m_lock.Unlock();
}

void LogRemainingCallstacks(const std::string& directory) {
	std::string content = "";

	m_lock.Lock();
	allocation_t* tempIndex = g_listTail;
	int times = 0;

	while (tempIndex && tempIndex->callstack) {
		times++;
		content += Stringf("\nStack size: %d\n iB", tempIndex->byteSize);
		CallstackLine_T lines[128];
		unsigned int lineCount = CallstackGetLines(lines, 128, tempIndex->callstack);
		for (unsigned int i = 2; i < lineCount; ++i) {

			content += Stringf("%s(%u): %s\n", lines[i].filename, lines[i].line, lines[i].functionName);
		}
		tempIndex = tempIndex->prev;
	}

	WriteBufferToFile(content, directory);
	LogTaggedPrintf("callstack", content.c_str());
	m_lock.Unlock();
}
