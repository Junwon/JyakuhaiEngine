#pragma once
#if !defined( __JOB__ )
#define __JOB__

#pragma warning(disable: 4239)

#include "Engine/Core/Performance/Atomic.hpp"
#include "Engine/Core/Performance/ThreadSafeQueue.hpp"
#include "Engine/Core/Performance/Signal.hpp"

enum eJobCategory
{
	JOB_GENERIC = 0,
	JOB_MAIN,
	JOB_IO,
	JOB_RENDER,
	JOB_LOGGING,

	JOB_CATEGORY_COUNT,
};

typedef void(*JobWorkCallback)(void*);

class Job;

typedef ThreadSafeQueue<Job*> JobCategoryQueue;

// Used for parameter forwarding
template <typename CB, typename ...ARGS>
struct JobPassData_T
{
	CB cb;
	std::tuple<ARGS...> args;

	JobPassData_T(CB cb, ARGS ...args): 
		cb(cb), 
		args(args...) {}
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class Job
{

public:
	void RunCallback();
	void OnFinish();
	void OnDependancyFinished();

	void DependentOn(Job* parent);

public:
	eJobCategory		m_type;
	JobWorkCallback		m_workCallback;

	void*				m_userData;

	std::vector<Job*>	m_dependents;
	unsigned int		m_numDependencies;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class JobConsumer
{
public:
	void AddCategory(eJobCategory category);

	// Consumes a single job.  Returns true if a job
	// was consume, false if not job was ready.
	bool ConsumeJob();

	// Consumes as many jobs are as available
	// and returns the number consumed.
	unsigned int ConsumeAll();

	unsigned int ConsumeForMiliseconds(unsigned int amountOfTimeRun);
public:
	std::vector<JobCategoryQueue*> m_categoryQueues;
};

// Initializes the system.  Setups up all queues, and spins
// up the generic consumer threads. 
//
// If genericThreadCount is positive, spin up that many threads
// If it is negative, spin up the number of logical cores on the machine added to the supplied count
// You should always spin up at least 1.
void JobSystemStartup(unsigned int jobCategoryCount = JOB_CATEGORY_COUNT, int genericThreadCount = -1);

// Shuts down the system, allowing all generic jobs to finish
// and asserting that all other groups have no enqueued jobs before returning
void JobSystemShutdown();

// If you want to associate a signal with a category
// that will be fired when a job is added to that queue.
void JobSystemSetCategorySignal(unsigned int categoryID, Signal* signal);

Job* JobCreate(eJobCategory type, JobWorkCallback workCallback, void* userData);

// Creates and immediately dispatches and relases.
void JobRun(eJobCategory category, JobWorkCallback cb, void* userData);

// Dispatches without releasing the job.  JobDispatchAndRelease
// could be switched to call this.
void JobDispatch(Job *job);

// Releases a reference to the job. 
void JobRelease(Job *job);

// Similar to JobDispatch and JobRelease
void JobDispatchAndRelease(Job *job);

// Waits on a job to enter the finished state before
// continuing on.
void JobWait(Job *job);

// Same as above, but will release my hold on this job when done
// Functionally similar to ThreadJoin, but for jobs. 
void JobWaitAndRelease(Job *job);

// Returns the current number of "live" jobs - all jobs
// that have at least one reference to them.
unsigned int GetLiveJobCount();

// Returns the number of jobs that are alive, but have not yet
// finished their work.
unsigned int GetActiveJobCount();

// Return isRunning bool
bool JobSystemIsRunning();

//------------------------------------------------------------------------
// Templated Versions;
//------------------------------------------------------------------------

template <typename CB, typename TUPLE, size_t ...INDICES>
void ForwardJobArgumentsWithIndices(CB cb, TUPLE& args, std::integer_sequence<size_t, INDICES...>&)
{
	cb(std::get<INDICES>(args)...);
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
void ForwardArgumentsJob(void *ptr)
{
	PassData_T<CB, ARGS...> *args = (PassData_T<CB, ARGS...>*) ptr;
	ForwardJobArgumentsWithIndices(args->cb, args->args, std::make_index_sequence<sizeof...(ARGS)>());
	delete args;
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
Job* JobCreate(eJobCategory type, CB workCallback, ARGS ...args)
{
	JobPassData_T<CB, ARGS...>* pass = new JobPassData_T<CB, ARGS...>(workCallback, args...);
	return JobCreate(type, ForwardArgumentsJob<CB, ARGS...>, (void*)pass);
}

//------------------------------------------------------------------------
template <typename CB, typename ...ARGS>
void JobRun(eJobCategory category, CB callback, ARGS ...args)
{
	JobPassData_T<CB, ARGS...>* pass = new JobPassData_T<CB, ARGS...>(callback, args...);
	return JobRun(category, ForwardArgumentsJob<CB, ARGS...>, (void*)pass);
}
#endif 