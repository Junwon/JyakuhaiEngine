#include "Engine/Core/Performance/JobThreadLogger.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Performance/Job.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"
#include "Engine/Core/Performance/ThreadLogger.hpp"

//------------------------------------------------------------------------
void LoggerJobThread(Signal* signal)
{
	JobConsumer logConsumer;
	logConsumer.AddCategory(JOB_LOGGING);

	Signal* loggerSignal = signal;

	// Tell the system to signal the normal logger's signal when logger work
	// comes in so that we wake up this thread.
	JobSystemSetCategorySignal(JOB_LOGGING, loggerSignal);

	/* Initialize - Create file */
	FILE* fileHandler = nullptr;
	errno_t err = fopen_s(&fileHandler, ASYNC_LOGGER_DIR, "w+");

	ASSERT_OR_DIE((err == 0) && (fileHandler != nullptr), "Async Logger Directory is not valid");

	while (JobSystemIsRunning())
	{
		loggerSignal->Wait();

		/* process all logging messages as normal here */
		LogTaggedPrintf("JobLogger", "Processing Job.");

		// NEW
		// check if theres any work that has to happen on the logging thread.
		logConsumer.ConsumeAll();
	}

	fclose(fileHandler);
}