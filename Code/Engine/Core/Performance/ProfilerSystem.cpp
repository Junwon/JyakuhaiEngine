#include "Engine/Core/Performance/ProfilerSystem.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Core/StringUtils.hpp"

#include "Engine/Core/Performance/BuildConfig.hpp"

//------------------------------------------------------------------------

ProfilerNode_T* m_head = nullptr;
ProfilerNode_T* m_activeNode = nullptr;

DWORD m_mainThreadID = 0;
uint64_t m_startFrameCounter = 0;
double m_lastFrameTimeSeconds = 0;
uint64_t m_activeStartCounter = 0;
bool m_isRunning = true;
bool m_isFrameComplete = false;



ProfilerNode_T::ProfilerNode_T(const char* tag) :
	m_tag(tag),
	m_callCount(0),
	m_startCounter(GetCurrentPerformanceCounter()),
	m_endCounter(0),
	m_selfElapsedTime(0),
	m_lastActiveElapsedTime(0),
	m_parent(nullptr),
	m_firstChild(nullptr),
	m_lastChild(nullptr),
	m_leftSibling(nullptr),
	m_rightSibling(nullptr)
{
};

//------------------------------------------------------------------------
double ProfilerNode_T::GetSelfElapsedTimeSeconds() const
{
	return m_selfElapsedTime;
}

//------------------------------------------------------------------------
bool ProfilerNode_T::HasFirstChild() const
{
	return m_firstChild != nullptr;
}

//------------------------------------------------------------------------
bool ProfilerNode_T::HasLastChild() const
{
	return m_lastChild != nullptr;
}

//------------------------------------------------------------------------
/** Returns -1 if tag cannot be found in children. Otherwise, returns the index starting from the first child
*/
int ProfilerNode_T::IndexOfChildWhereTagExists(const char* tag)
{
	int index = -1;

	ProfilerNode_T* currentChild = m_firstChild;

	while (currentChild != nullptr) {
		++index;

		if (AreEqual(currentChild->m_tag, tag)) {
			break;
		}
		
		currentChild = currentChild->m_rightSibling;
	}

	if (currentChild == nullptr) {
		return -1;
	}

	return index;
}

//------------------------------------------------------------------------
double ProfilerNode_T::GetSelfActiveTime() const
{
	double activeElapsedTime = m_selfElapsedTime;

	ProfilerNode_T* currentChild = m_firstChild;

	while (currentChild != nullptr) {
		activeElapsedTime -= currentChild->m_selfElapsedTime;
		currentChild = currentChild->m_rightSibling;
	}

	return activeElapsedTime;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#if (PROFILED_BUILD == PROFILED_ENABLED) 

/** Profiler System Helper Functions
*/
void PushNewChildOntoActiveNode(const char* tag);
void SetActiveNodeOnPush(ProfilerNode_T* node);
void SetActiveNodeOnPop();

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ProfilerStartup()
{
	m_mainThreadID = ::GetCurrentThreadId();
	LogTaggedPrintf("Profiler", "Profiler Start.");
}

//------------------------------------------------------------------------
void ProfilerShutdown()
{
	ASSERT_OR_DIE(m_mainThreadID == ::GetCurrentThreadId(), "Profiler running on incorrect Thread.");
	LogTaggedPrintf("Profiler", "Profiler End.");
}

//------------------------------------------------------------------------
void ProfilerPush(const char* tag)
{
	ASSERT_OR_DIE(m_mainThreadID == ::GetCurrentThreadId(), "Profiler running on incorrect Thread.");

	if(CanProfileRun()) {
		// If tree already exist
		if (m_activeNode != nullptr) {
			// If tag already exists in active node
			if (m_activeNode->m_tag == tag) {
				// Just update the start counter
				m_activeNode->m_startCounter = GetCurrentPerformanceCounter();
			}
			// If not the in active node's, check the children's tag
			else {
				int indexOfExistingTag = m_activeNode->IndexOfChildWhereTagExists(tag);

				if (indexOfExistingTag != -1) {
					ProfilerNode_T* currentNode = m_activeNode->m_firstChild;

					// Keep traversing the right siblings to reach the node with the tag
					for (int i = 0; i < indexOfExistingTag; ++i) {
						currentNode = currentNode->m_rightSibling;
					}

					// Set the child with the tag as active and update start counter
					SetActiveNodeOnPush(currentNode);
					m_activeNode->m_startCounter = GetCurrentPerformanceCounter();
				}
				// If tag does not exist in the parent's children
				else {
					PushNewChildOntoActiveNode(tag);
				}
			}
		}
		// If tree has not been created yet
		else {
			m_head = new ProfilerNode_T(tag);
			SetActiveNodeOnPush(m_head);
		}
	}
}

//------------------------------------------------------------------------
void ProfilerPop()
{
	ASSERT_OR_DIE(m_mainThreadID == ::GetCurrentThreadId(), "Profiler running on incorrect Thread.");

	if(CanProfileRun()) {
		m_activeNode->m_endCounter = GetCurrentPerformanceCounter();
		m_activeNode->m_selfElapsedTime = CalcPerformanceCounterToSeconds(m_activeNode->m_startCounter, m_activeNode->m_endCounter);
		SetActiveNodeOnPop();
	}
}

//------------------------------------------------------------------------
void PushNewChildOntoActiveNode(const char* tag)
{
	// Create new child node
	ProfilerNode_T* childNode = new ProfilerNode_T(tag);

	// Set the child's parent
	childNode->m_parent = m_activeNode;

	// Check if parent has last child
	if (m_activeNode->HasLastChild()) {
		// Last child is considered the right-most sibling; Set that child to the new's child left sibling
		childNode->m_leftSibling = m_activeNode->m_lastChild;

		// Vice-versa
		childNode->m_leftSibling->m_rightSibling = childNode;

		// Set parent's last child as new child
		m_activeNode->m_lastChild = childNode;

		// Change active node to new child node
		SetActiveNodeOnPush(childNode);
		
	}
	// If parent does not have any children yet
	else {
		// Set this new child as the first and last child
		m_activeNode->m_firstChild = childNode;
		m_activeNode->m_lastChild = childNode;

		// Change active node to new child node
		SetActiveNodeOnPush(childNode);
	}
}

//------------------------------------------------------------------------
void SetActiveNodeOnPush(ProfilerNode_T* node)
{
	m_activeNode = node;
	m_activeNode->m_callCount++;
}

//------------------------------------------------------------------------
void SetActiveNodeOnPop()
{
	if (m_activeNode->m_parent)
	{
		m_activeNode = m_activeNode->m_parent;
	}
	else
	{
		m_isFrameComplete = true;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

ProfilerNode_T* ProfilerGetPreviousFrame()
{
	return m_head;
}

//------------------------------------------------------------------------
ProfilerNode_T* ProfilerGetPreviousFrameWithTag(const char* /*rootTag*/)
{
	return m_head;
}

//------------------------------------------------------------------------
void ProfilerPause()
{
	m_isRunning = false;
}

//------------------------------------------------------------------------
void ProfilerResume()
{
	m_isRunning = true;
}

//------------------------------------------------------------------------
void ProfilerSnapshotSingleFrame()
{
}

//------------------------------------------------------------------------
void ProfilerStartFrame()
{
	m_startFrameCounter = GetCurrentPerformanceCounter();

	if (m_isRunning) {
		m_isFrameComplete = false;
	}
}

//------------------------------------------------------------------------
void ProfilerEndFrame()
{
	m_lastFrameTimeSeconds = CalcPerformanceCounterToSeconds(m_startFrameCounter);
}

//------------------------------------------------------------------------
double ProfilerGetLastFrameTimeSeconds()
{
	return m_lastFrameTimeSeconds;
}

//------------------------------------------------------------------------
bool CanProfileRun()
{
	return m_isRunning && !m_isFrameComplete;
}

#else

void ProfilerStartup() {};
void ProfilerShutdown() {};
void ProfilerPush(const char*) {};
void ProfilerPop() {};

void ProfilerSetRenderer(SimpleRenderer*) {};

ProfilerNode_T* ProfilerGetPreviousFrame() { return nullptr;  };
ProfilerNode_T* ProfilerGetPreviousFrameWithTag(const char*) { return nullptr; };
void ProfilerPause() {};
void ProfilerResume() {};
void ProfilerSnapshotSingleFrame() {};

void ProfilerStartFrame() {};
void ProfilerEndFrame() {};
double ProfilerGetLastFrameTimeSeconds() { return 0; }
bool CanProfileRun() { return false; }
#endif
