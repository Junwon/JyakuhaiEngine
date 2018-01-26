//-----------------------------------------------------------------------------------------------
// Time.hpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh
#pragma once

typedef unsigned long long uint64_t;

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

//-----------------------------------------------------------------------------------------------
uint64_t GetCurrentPerformanceCounter();
double CalcPerformanceCounterToSeconds(uint64_t counter);
double CalcPerformanceCounterToSeconds(uint64_t firstCounter, uint64_t secondCounter);
double ConvertSecondsToMilliseconds(double seconds);
double ConvertSecondsToMicroseconds(double seconds);

