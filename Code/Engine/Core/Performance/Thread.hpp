#pragma once

#pragma warning(disable: 4239)

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tuple>
#include <utility>

#define INVALID_THREAD_HANDLE 0

// Empty structs to make C++ tell me if I'm casting wrong.
struct ThreadHandleName_T {};

typedef ThreadHandleName_T* ThreadHandle_T;
typedef unsigned int ThreadID_T;

typedef void(*ThreadCB)(void*);

//////////////////////////////////////////////////////////////////////////

// Used for parameter forwarding
template <typename CB, typename ...ARGS>
struct PassData_T
{
	CB cb;
	std::tuple<ARGS...> args;

	PassData_T(CB cb, ARGS ...args)
		: cb(cb)
		, args(args...) {}
};

//////////////////////////////////////////////////////////////////////////

// Creates a thread with the entry point of cb, passed data
ThreadHandle_T ThreadCreate(ThreadCB cb, void *data);

void ThreadTest(const char* fileDir, unsigned int byteSize);

void ThreadSleep(unsigned int ms);

// Releases my hold on this thread [one of these MUST be called per create]
void ThreadDetach(ThreadHandle_T th);
void ThreadJoin(ThreadHandle_T th);
void ThreadJoin(ThreadHandle_T* th, unsigned int count);

ThreadID_T ThreadGetCurrentID();

void ThreadSetNameInVisualStudio(const char* name);

//------------------------------------------------------------------------
// Templated Versions;
//------------------------------------------------------------------------

template <typename CB, typename TUPLE, size_t ...INDICES>
void ForwardArgumentsWithIndices(CB cb, TUPLE& args, std::integer_sequence<size_t, INDICES...>&)
{
	cb(std::get<INDICES>(args)...);
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
void ForwardArgumentsThread(void *ptr)
{
	PassData_T<CB, ARGS...> *args = (PassData_T<CB, ARGS...>*) ptr;
	ForwardArgumentsWithIndices(args->cb, args->args, std::make_index_sequence<sizeof...(ARGS)>());
	delete args;
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
ThreadHandle_T ThreadCreate(CB entry_point, ARGS ...args)
{
	PassData_T<CB, ARGS...> *pass = new PassData_T<CB, ARGS...>(entry_point, args...);
	return ThreadCreate(ForwardArgumentsThread<CB, ARGS...>, (void*)pass);
}
