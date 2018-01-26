#pragma once

#include "Engine/Core/Performance/Thread.hpp"

struct LogTest_T {
	LogTest_T() :
		m_threadIndex(0),
		m_lineCount(0)
	{};
	unsigned int m_threadIndex;
	unsigned int m_lineCount;
};

unsigned int FlushMessages(FILE *fh);

void LogFlush();
void LogFlushTest(const char* text);
void LogTaggedPrintf(const char* tag, const char* format, ...);
void LogPrintlnf(const char* msg, ...);
void LogWarningf(const char* msg, ...);
void LogErrorf(const char* msg, ...);
void LoggerThread(void*);
void LogStartup(const char* path);
void LogShutdown();
void LogTest(void* arg);
void LogDisable(const char* tag);
void LogEnable(const char* tag);
void LogDisableAll();
void LogEnableAll();
const char* LogGetGlobalFileDirectory();
void LogReopenGlobalFileHandler();
FILE* LogGetGlobalFileHandler();
void LogCloseGlobalFileHandler();
void ThreadDemo();