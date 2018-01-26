#include "Engine/Core/Performance/ThreadLogger.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdarg.h>
#include <map>
#include <ctime>

#include "Engine/Core/Performance/ThreadSafeQueue.hpp"
#include "Engine/Core/Performance/Signal.hpp"
#include "Engine/Core/Performance/Event.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"

ThreadSafeQueue<std::string> gMessages;
FILE* gFileHandler = nullptr;
const char* gFileDirectory = "";
ThreadHandle_T gLoggerThread = nullptr;
bool gLoggerThreadRunning = true;
bool gUniversalTagFilter = true;

std::map<std::string, bool> gTagFilters;

const char* formattedDateTemplate = "[%s] [%2d/%2d/%4d %2d:%2d:%02d] %s";

const int MESSAGE_MAX_LENGTH = 2048;

//------------------------------------------------------------------------
//------------------------------------------------------------------------

unsigned int FlushMessages(FILE *fh) {
	unsigned int count = 0;
	std::string msg;

	while (gMessages.dequeue(&msg)) {
		fprintf(fh, "%s\n", msg.c_str());
		DebuggerPrintf("Thread: %s\n", msg.c_str());
		++count;
	}

	return count;
}

//------------------------------------------------------------------------
void LogFlush() {
	std::string msg;

	while (gMessages.dequeue(&msg)) {
		fprintf(gFileHandler, "%s\n", msg.c_str());
	}
}

//------------------------------------------------------------------------
void LogFlushTest(const char* text) {

	LogPrintlnf(text);
	LogFlush();
	__debugbreak();
}

//------------------------------------------------------------------------
void LogTaggedPrintf(const char* tag, const char* format, ...) {
	char textLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, format);
	vsnprintf_s(textLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, format, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[MESSAGE_MAX_LENGTH - 1] = '\0';

	auto it = gTagFilters.find(tag);

	if (it != gTagFilters.end()) {
		if (it->second) {
			time_t t = time(NULL);
			tm dateStamp;
			localtime_s(&dateStamp, &t);
			std::string formatString = Stringf(formattedDateTemplate, 
				tag, dateStamp.tm_mon + 1, dateStamp.tm_mday, dateStamp.tm_year + 1900,
				dateStamp.tm_hour, dateStamp.tm_min, dateStamp.tm_sec, textLiteral);
			gMessages.enqueue(formatString);
			DebuggerPrintln(formatString.c_str());
			return;
		}
	}
	else {
		if (gUniversalTagFilter) {
			LogEnable(tag);
			time_t t = time(NULL);
			tm dateStamp;
			localtime_s(&dateStamp, &t);
			std::string formatString = Stringf(formattedDateTemplate,
				tag, dateStamp.tm_mon + 1, dateStamp.tm_mday, dateStamp.tm_year + 1900,
				dateStamp.tm_hour, dateStamp.tm_min, dateStamp.tm_sec, textLiteral);
			gMessages.enqueue(formatString);
			DebuggerPrintln(formatString.c_str());
			return;
		}
		else {
			LogDisable(tag);
		}
	}
}
//------------------------------------------------------------------------
void LogPrintlnf(const char* msg, ...) {
	char textLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[MESSAGE_MAX_LENGTH - 1] = '\0';

	LogTaggedPrintf("log", textLiteral);
}

//------------------------------------------------------------------------
void LogWarningf(const char* msg, ...) {
	char textLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[MESSAGE_MAX_LENGTH - 1] = '\0';

	LogTaggedPrintf("warning", textLiteral);
}

//------------------------------------------------------------------------
void LogErrorf(const char* msg, ...) {
	char textLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[MESSAGE_MAX_LENGTH - 1] = '\0';

	LogTaggedPrintf("error", textLiteral);
	__debugbreak();
}

//------------------------------------------------------------------------
void LoggerThread(void* fileDir) {
	gFileDirectory = (const char*)fileDir; //"Data/Logs/log.log"
	errno_t err = fopen_s(&gFileHandler, gFileDirectory, "w+");
	if ((err != 0) || (gFileHandler == nullptr)) {
		return;
	}

	while (gLoggerThreadRunning) {
		FlushMessages(gFileHandler);
		//ThreadSleep(1000);
	}

	FlushMessages(gFileHandler);

	fclose(gFileHandler);

	std::vector<std::string> buffer;
	SplitIntoBuffer(buffer, gFileDirectory, ".");

	time_t t = time(NULL);
	tm dateStamp;
	localtime_s(&dateStamp, &t);

	std::string timeStampedFilename = buffer[0] + Stringf("_%d%02d%02d", dateStamp.tm_year + 1900, dateStamp.tm_mon + 1, dateStamp.tm_mday) +
		Stringf("_%02d%02d%02d.", dateStamp.tm_hour, dateStamp.tm_min, dateStamp.tm_sec) + buffer[1];
	rename(gFileDirectory, timeStampedFilename.c_str());
}

//------------------------------------------------------------------------
void LogStartup(const char* path) {
	gLoggerThreadRunning = true;
	gLoggerThread = ThreadCreate(LoggerThread, (void*)path);
}

//------------------------------------------------------------------------
void LogShutdown() {
	gLoggerThreadRunning = false;
	ThreadJoin(gLoggerThread);
	gLoggerThread = INVALID_THREAD_HANDLE;
}

//------------------------------------------------------------------------
void LogTest(void* arg) {
	LogTest_T* l_arg = (LogTest_T*)arg;

	unsigned int threadIndex = l_arg->m_threadIndex;
	unsigned int lineCount = l_arg->m_lineCount;

	for (unsigned int i = 0; i < lineCount; ++i) {
		std::string msg = Stringf("Thread %i, writing line %i", threadIndex, (i + 1));
		LogTaggedPrintf("test", msg.c_str());
	}

	delete l_arg;
	l_arg = nullptr;
}

//------------------------------------------------------------------------
void LogDisable(const char* tag) {
	gTagFilters[tag] = false;
}

//------------------------------------------------------------------------
void LogEnable(const char* tag) {
	gTagFilters[tag] = true;
}

//------------------------------------------------------------------------
void LogDisableAll() {
	gUniversalTagFilter = false;
	auto it = gTagFilters.begin();
	while (it != gTagFilters.end()) {
		it->second = false;
		it++;
	}
}

//------------------------------------------------------------------------
void LogEnableAll() {
	gUniversalTagFilter = true;
	auto it = gTagFilters.begin();
	while (it != gTagFilters.end()) {
		it->second = true;
		it++;
	}
}

//------------------------------------------------------------------------
const char* LogGetGlobalFileDirectory()
{
	return gFileDirectory;
}

//------------------------------------------------------------------------
void LogReopenGlobalFileHandler()
{
	errno_t err = fopen_s(&gFileHandler, gFileDirectory, "a+");
	ASSERT_OR_DIE((err == 0) || (gFileHandler != nullptr), "Cannot open file for global log.");
}

//------------------------------------------------------------------------
FILE* LogGetGlobalFileHandler()
{
	return gFileHandler;
}

//------------------------------------------------------------------------
void LogCloseGlobalFileHandler()
{
	fclose(gFileHandler);
}

//------------------------------------------------------------------------
void ThreadDemo() {
	LogStartup("Data/Logs/log.log");

	for (unsigned int i = 0; i < 1000; ++i) {
		LogPrintlnf("Main Thread!");
	}

	LogShutdown();
}