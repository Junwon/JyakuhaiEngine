//-----------------------------------------------------------------------------------------------
// Time.cpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}

//-----------------------------------------------------------------------------------------------
uint64_t GetCurrentPerformanceCounter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

//-----------------------------------------------------------------------------------------------
double CalcPerformanceCounterToSeconds(uint64_t firstCounter)
{
	uint64_t secondCounter = GetCurrentPerformanceCounter();

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (double)(secondCounter - firstCounter) / frequency.QuadPart;
}

//-----------------------------------------------------------------------------------------------
double CalcPerformanceCounterToSeconds(uint64_t firstCounter, uint64_t secondCounter)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return (double)(secondCounter - firstCounter) / frequency.QuadPart;
}

double ConvertSecondsToMilliseconds(double seconds)
{
	return seconds * 1000;
}

double ConvertSecondsToMicroseconds(double seconds)
{
	return seconds * 1000000;
}


