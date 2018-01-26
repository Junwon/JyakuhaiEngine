#pragma once

#include <string>

extern unsigned int g_AllocCount;
extern unsigned int g_FrameAllocs;
extern unsigned int g_FrameFrees;
extern size_t g_AllocatedByteCount;
extern unsigned int g_highwater;

void ProfileMemoryFrameTick();
std::string CalculateReadableBytesString(unsigned int bytes);
void PrintCallstack(void*);
void LogRemainingCallstacks(const std::string& directory);