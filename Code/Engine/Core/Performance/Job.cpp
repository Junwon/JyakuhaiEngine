#include "Engine/Core/Performance/Job.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Performance/ThreadSafeQueue.hpp"
#include "Engine/Core/Performance/Signal.hpp"
#include "Engine/Core/Performance/Atomic.hpp"
#include "Engine/Core/Performance/Thread.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"
#include "Engine/Core/Performance/JobThreadLogger.hpp"
#include "Engine/Core/Performance/JobRendering.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"

class JobSystem
{
public:
	JobCategoryQueue* m_queues;
	Signal** m_signals;
	unsigned int m_queueCount;
	JobConsumer* m_genericConsumer;
	bool m_isRunning;
	unsigned int m_liveCount;
	unsigned int m_activeCount;
};

static JobSystem* gJobSystem = nullptr;

//------------------------------------------------------------------------
static void GenericJobThread(Signal* signal)
{
	JobConsumer* genericConsumer = gJobSystem->m_genericConsumer;
	Signal* genericSignal = signal;

	while (gJobSystem->m_isRunning) {
		genericSignal->Wait();
		genericConsumer->ConsumeAll();
	}

	genericConsumer->ConsumeAll();
}

//------------------------------------------------------------------------
void MainJobThread(Signal* signal)
{
	JobConsumer mainConsumer;
	mainConsumer.AddCategory(JOB_MAIN);

	Signal* mainSignal = signal;

	JobSystemSetCategorySignal(JOB_MAIN, mainSignal);

	while (gJobSystem->m_isRunning) {
		mainSignal->Wait();
		mainConsumer.ConsumeAll();
	}

	mainConsumer.ConsumeAll();
}

//------------------------------------------------------------------------
void Job::RunCallback()
{
	m_workCallback(m_userData);
}

//------------------------------------------------------------------------
void Job::OnFinish()
{
	for (unsigned int i = 0; i < m_dependents.size(); ++i) {
		m_dependents[i]->OnDependancyFinished();
	}
}

//------------------------------------------------------------------------
void Job::OnDependancyFinished()
{
	JobDispatchAndRelease(this);
}

//------------------------------------------------------------------------
void Job::DependentOn(Job* parent)
{
	AtomicIncrement(&m_numDependencies);
	parent->m_dependents.push_back(this);
}

//------------------------------------------------------------------------
void JobSystemStartup(unsigned int jobCategoryCount, int genericThreadCount)
{
	int coreCount = (int)std::thread::hardware_concurrency();
	if (genericThreadCount <= 0) {
		coreCount += genericThreadCount;
	}
	coreCount--; // One is always being created - so subtract from total wanted;

	// We need queues! 
	gJobSystem = new JobSystem();
	gJobSystem->m_queues = new ThreadSafeQueue<Job*>[jobCategoryCount];
	gJobSystem->m_signals = new Signal*[jobCategoryCount];
	gJobSystem->m_queueCount = jobCategoryCount;
	gJobSystem->m_isRunning = true;

	JobConsumer* genericConsumer = new JobConsumer();
	genericConsumer->AddCategory(JOB_GENERIC);
	gJobSystem->m_genericConsumer = genericConsumer;

	for (unsigned int i = 0; i < jobCategoryCount; ++i) {
		gJobSystem->m_signals[i] = nullptr;
	}

	// Create the signals
	gJobSystem->m_signals[JOB_GENERIC] = new Signal();
	gJobSystem->m_signals[JOB_LOGGING] = new Signal();
	gJobSystem->m_signals[JOB_MAIN] = new Signal();
	gJobSystem->m_signals[JOB_RENDER] = new Signal();

	ThreadCreate(GenericJobThread, gJobSystem->m_signals[JOB_GENERIC]);
	for (int i = 0; i < coreCount; ++i)  {
		ThreadCreate(GenericJobThread, gJobSystem->m_signals[JOB_GENERIC]);
	}

	ThreadCreate(LoggerJobThread, gJobSystem->m_signals[JOB_LOGGING]);
	ThreadCreate(RenderingJobThread, gJobSystem->m_signals[JOB_RENDER]);
	ThreadCreate(MainJobThread, gJobSystem->m_signals[JOB_MAIN]);
}

//------------------------------------------------------------------------
void JobSystemShutdown()
{
	for (unsigned int i = 0; i < gJobSystem->m_queueCount; ++i) {
		SAFE_DELETE(gJobSystem->m_signals[i]);

		ThreadSafeQueue<Job*> q = gJobSystem->m_queues[i];

		for (unsigned int j = 0; j < q.m_queue.size(); ++j)
		{
			q.m_queue.pop();
		}
	}

	SAFE_DELETE(gJobSystem->m_genericConsumer);

	SAFE_DELETE(gJobSystem);
}

//------------------------------------------------------------------------
void JobSystemSetCategorySignal(unsigned int categoryID, Signal* signal)
{
	gJobSystem->m_signals[categoryID] = signal;
}

//------------------------------------------------------------------------
Job* JobCreate(eJobCategory type, JobWorkCallback workCallback, void* userData)
{
	Job* job = new Job();
	job->m_type = type;
	job->m_workCallback = workCallback;
	job->m_userData = userData;
	job->m_numDependencies = 1;

	return job;
}

//------------------------------------------------------------------------
void JobDispatchAndRelease(Job* job)
{
	// if I'm not ready to run, don't. 
	unsigned int dcount = AtomicDecrement(&job->m_numDependencies);
	if (dcount != 0) {
		return;
	}

	gJobSystem->m_queues[job->m_type].enqueue(job);
	Signal *signal = gJobSystem->m_signals[job->m_type];
	if (nullptr != signal) {
		signal->SignalAll();
	}
}

//------------------------------------------------------------------------
void JobRun(eJobCategory category, JobWorkCallback cb, void* userData)
{
	Job* job = JobCreate(category, cb, userData);
	JobDispatchAndRelease(job);
}

//------------------------------------------------------------------------
void JobDispatch(Job* job)
{
	unsigned int dcount = AtomicDecrement(&job->m_numDependencies);
	if (dcount != 0) {
		return;
	}

	gJobSystem->m_queues[job->m_type].enqueue(job);
}

//------------------------------------------------------------------------
void JobRelease(Job* job)
{
	Signal* signal = gJobSystem->m_signals[job->m_type];
	if (nullptr != signal) {
		signal->SignalAll();
	}
}

//------------------------------------------------------------------------
void JobWait(Job* job)
{
	Signal* signal = gJobSystem->m_signals[job->m_type];

	if (nullptr != signal) {
		signal->Wait();
	}
}

//------------------------------------------------------------------------
void JobWaitAndRelease(Job* job)
{
	Signal* signal = gJobSystem->m_signals[job->m_type];

	if (nullptr != signal) {
		signal->Wait();
		signal->SignalAll();
	}
}

//------------------------------------------------------------------------
unsigned int GetLiveJobCount()
{
	return gJobSystem->m_liveCount;
}

//------------------------------------------------------------------------
unsigned int GetActiveJobCount()
{
	return gJobSystem->m_activeCount;
}

//------------------------------------------------------------------------
bool JobSystemIsRunning()
{
	return gJobSystem->m_isRunning;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------
void JobConsumer::AddCategory(eJobCategory category)
{
	m_categoryQueues.push_back(&(gJobSystem->m_queues)[category]);
}

//------------------------------------------------------------------------
bool JobConsumer::ConsumeJob()
{
	for (JobCategoryQueue* q : m_categoryQueues) {
		Job* job = nullptr;
		if (q->dequeue(&job)) {
			job->RunCallback();

			for (Job* dependencies : job->m_dependents) {
				JobDispatch(dependencies);
			}
			JobRelease(job);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------
unsigned int JobConsumer::ConsumeAll()
{
	Job* job;
	unsigned int processedJobs = 0;

	for (JobCategoryQueue* queue : m_categoryQueues) {
		while (queue->dequeue(&job)) {
			job->RunCallback();
			++processedJobs;

			job->OnFinish();
			delete job;
		}
	}

	return processedJobs;
}

//------------------------------------------------------------------------
unsigned int JobConsumer::ConsumeForMiliseconds(unsigned int amountOfTimeRun)
{
	Job* job;
	unsigned int processedJobs = 0;

	uint64_t startTime = 0; 
	double elapsedTime = 0;

	for (JobCategoryQueue* queue : m_categoryQueues) {
		while (amountOfTimeRun > 0 && queue->dequeue(&job)) {
			startTime = GetCurrentPerformanceCounter();

			job->RunCallback();
			++processedJobs;

			job->OnFinish();
			delete job;

			elapsedTime = CalcPerformanceCounterToSeconds(startTime);
			amountOfTimeRun -= (unsigned int)ConvertSecondsToMilliseconds(elapsedTime);
		}
	}

	return processedJobs;
}