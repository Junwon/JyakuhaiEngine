#include "Engine/Core/Performance/PerformanceCommon.hpp"

bool g_hasInitiatedCallstackStartup = false;

const std::string LOG_DIR = "Data/Logs/";
const char* THREAD_TEST_DIR = "Data/Logs/ThreadTest.log";
const char* LARGE_ASYNC_DIR = "Data/Logs/LargeAsync.log";
const char* THREAD_LOG_TEST_DIR = "Data/Logs/LogTest.log";
const char* GLOBAL_LOGGER_DIR = "Data/Logs/GlobalLogs/log.log";
const char* COPY_LOG_DIR = "Data/Logs/Copy.log";
const char* ASYNC_LOGGER_DIR = "Data/Logs/GlobalLogs/AsyncLog.log";

const unsigned char MAX_MEMORY_USAGE_HISTORY_SIZE = 200;