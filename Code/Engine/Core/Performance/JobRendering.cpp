#include "Engine/Core/Performance/JobRendering.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Performance/Job.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"

//------------------------------------------------------------------------
void RenderingJobThread(Signal* signal)
{
	JobConsumer renderingConsumer;
	renderingConsumer.AddCategory(JOB_RENDER);

	Signal* renderingSignal = signal;

	// Tell the system to signal the normal logger's signal when logger work
	// comes in so that we wake up this thread.
	JobSystemSetCategorySignal(JOB_RENDER, renderingSignal);

	while (JobSystemIsRunning())
	{
		renderingSignal->Wait();
		renderingConsumer.ConsumeAll();
	}
}