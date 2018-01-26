#pragma once

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//--------------------------------------------------------------------
// Will return the result of the operation
__forceinline
unsigned int AtomicAdd(unsigned int volatile* ptr, unsigned int const value)
{
	return (unsigned int) ::InterlockedAddNoFence((LONG volatile*)ptr, (LONG)value);
}

//--------------------------------------------------------------------
__forceinline
unsigned int AtomicIncrement(unsigned int* ptr)
{
	return (unsigned int) ::InterlockedIncrementNoFence((LONG volatile*)ptr);
}

//--------------------------------------------------------------------
__forceinline
unsigned int AtomicDecrement(unsigned int* ptr)
{
	return (unsigned int) ::InterlockedDecrementNoFence((LONG volatile*)ptr);
}

//--------------------------------------------------------------------
__forceinline
unsigned int CompareAndSet(unsigned int volatile* ptr, unsigned int const comparand, unsigned int const value)
{
	/*
	uint const old_value = *ptr;
	if (old_value == comparand) {
	*ptr = value;
	}
	return old_value;
	*/

	return ::InterlockedCompareExchange(ptr, value, comparand);
}

//--------------------------------------------------------------------
#ifdef _WIN64
// SUPPORTS ONLY IN x64 bit
__forceinline
bool CompareAndSet128(uint64_t volatile data[2], uint64_t comparand[2], uint64_t value[2])
{
	return 1 == ::InterlockedCompareExchange128((long long volatile*)data, value[1], value[0], (long long*)comparand);
}
#endif


//--------------------------------------------------------------------
template <typename T>
__forceinline T* CompareAndSetPointer(T *volatile *ptr, T *comparand, T *value)
{
	return (T*)::InterlockedCompareExchangePointerNoFence((PVOID volatile*)ptr, (PVOID)value, (PVOID)comparand);
}