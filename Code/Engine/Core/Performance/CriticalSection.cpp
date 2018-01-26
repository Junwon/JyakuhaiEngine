#include "Engine/Core/Performance/CriticalSection.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

CriticalSection::CriticalSection()
{
	::InitializeCriticalSection(&m_criticalSection);
}

//------------------------------------------------------------------------
CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(&m_criticalSection);
}

//------------------------------------------------------------------------
void CriticalSection::Lock()
{
	::EnterCriticalSection(&m_criticalSection);
}

//------------------------------------------------------------------------
bool CriticalSection::TryLock()
{
	return TRUE == ::TryEnterCriticalSection(&m_criticalSection);
}

//------------------------------------------------------------------------
void CriticalSection::Unlock()
{
	::LeaveCriticalSection(&m_criticalSection);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
ScopeCriticalSection::ScopeCriticalSection(CriticalSection *ptr)
{
	m_criticalSectionPtr = ptr;
	m_criticalSectionPtr->Lock();
}

//------------------------------------------------------------------------
ScopeCriticalSection::~ScopeCriticalSection()
{
	m_criticalSectionPtr->Unlock();
}