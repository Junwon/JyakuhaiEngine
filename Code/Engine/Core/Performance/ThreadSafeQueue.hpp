#pragma once

#include <atomic>
#include "Engine/Core/Performance/CriticalSection.hpp"

#include <queue>

//------------------------------------------------------------------------
template <typename T>
class ThreadSafeQueue
{
public:
	//------------------------------------------------------------------------
	~ThreadSafeQueue()
	{
	}

	//------------------------------------------------------------------------
	void enqueue(const T& v)
	{
		SCOPE_LOCK(m_lock);
		m_queue.push(v);
	}

	//------------------------------------------------------------------------
	bool empty()
	{
		SCOPE_LOCK(m_lock);
		bool result = m_queue.empty();

		return result;
	}

	//------------------------------------------------------------------------
	bool dequeue(T* out)
	{
		SCOPE_LOCK(m_lock);

		if (m_queue.empty()) {
			return false;
		}
		else {
			*out = m_queue.front();
			m_queue.pop();
			return true;
		}
	}

	//------------------------------------------------------------------------
	T front()
	{
		SCOPE_LOCK(m_lock);
		T result = m_queue.front();

		return result;
	}

public:
	std::queue<T> m_queue;
	CriticalSection m_lock;
};