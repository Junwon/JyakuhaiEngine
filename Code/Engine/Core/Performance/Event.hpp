#pragma once

// Equivalent of a C# Delegate

#include <atomic>
#include "Engine/Core/Performance/CriticalSection.hpp"

#include <vector>

template <typename ...ARGS>
class Event
{
public:
	Event::Event()
	{
	}

	Event::~Event()
	{
		m_subscriptions.clear();
	}

public:
	// TYPES:
	struct SubEvent_T;
	typedef void(*CallBack_T)(SubEvent_T*, ARGS...);
	typedef void(*CallBackWithArgs_T)(void*, ARGS...);
	// Subscription - when subscribing this is the identifying 
	// Information (what to call, and what to call with)

	struct SubEvent_T
	{
		CallBack_T callback;
		void* secondaryCallback;
		void* userArgs;
	};

	// STATIC FUNCTIONS AS GO BETWEENS BETWEEN
	// THE EXPECTED CALLBACK AND UNIVERSAL CALLBACK
	static void FunctionWithArgumentCallback(SubEvent_T* sub, ARGS ...args);

	template <typename T, typename MCB>
	static void MethodCallback(SubEvent_T* sub, ARGS ...args);

	//---------------------------------------------------------------------------
	// Subscribe a single function
	void Subscribe(void* userArg, CallBackWithArgs_T cb)
	{
		// Good safeguard in debug to add is to make sure
		// you're not double subscribing to an event
		// with a similar pair. 
		// TODO - ASSERT AGAINST ABOVE

		// Add subscriptoin
		SubEvent_T sub;
		sub.callback = FunctionWithArgumentCallback;
		sub.secondaryCallback = cb;
		sub.userArgs = userArg;
		m_subscriptions.push_back(sub);
	}

	//---------------------------------------------------------------------------
	// Unsubscribe a function (using user argument as well)
	void Unsubscribe(void* userArg, void* cb)
	{
		for (int i = 0; i < m_subscriptions.size(); ++i) {
			SubEvent_T &sub = m_subscriptions[i];
			if ((sub.secondaryCallback == cb) && (sub.userArgs == userArg)) {
				m_subscriptions.erase(m_subscriptions.begin() + i);
				return; // should be unique, so return immeidately
			}
		}
	}

	//---------------------------------------------------------------------------
	// remove all subscriptions using this user arg.
	void UnsubscribeByArgument(void* userArg)
	{
		for (int i = 0; i < m_subscriptions.size(); ++i) {
			SubEvent_T &sub = m_subscriptions[i];
			if (sub.userArgs == obj) {
				// don't return, just remove this object [could do a fast removal if order doesn't matter
				// by just setting last to this and popping back]
				m_subscriptions.erase(m_subscriptions.begin() + i);
				--i;
			}
		}
	}

	//---------------------------------------------------------------------------
	// Be able to subscribe a method;
	template <typename T>
	void SubscribeMethod(T *obj, void (T::*mcb)(ARGS...))
	{
		SubEvent_T sub;
		sub.callback = MethodCallback<T, decltype(mcb)>;
		sub.secondaryCallback = *(void**)(&mcb);
		sub.userArgs = obj;
		m_subscriptions.push_back(sub);
	}

	//---------------------------------------------------------------------------
	// unsubscribe - just forwards to normal unsubscribe
	template <typename T>
	void UnsubscribeMethod(T *obj, void (T::*mcb)(ARGS...))
	{
		Unsubscribe(obj, *(void**)&mcb);
	}

	//---------------------------------------------------------------------------
	// unsubscribe all allocations for this object;
	template <typename T>
	void UnsubscribeObject(T *obj)
	{
		UnsubscribeByArgument(obj);
	}

	//---------------------------------------------------------------------------
	// Triggers the call - calls all registered callbacks;
	void Trigger(ARGS ...args)
	{
		for (int i = 0; i < m_subscriptions.size(); ++i) {
			SubEvent_T &sub = m_subscriptions[i];
			sub.callback(&sub, args...);
		}
	}

public:
	std::vector<SubEvent_T> m_subscriptions;
};

//--------------------------------------------------------------------
template <typename ...ARGS>
void Event<ARGS...>::FunctionWithArgumentCallback(SubEvent_T* sub, ARGS ...args)
{
	CallBackWithArgs_T cb = (CallBackWithArgs_T)(sub->secondaryCallback);
	cb(sub->userArgs, args...);
}

//--------------------------------------------------------------------
template <typename ...ARGS>
template <typename T, typename MCB>
void Event<ARGS...>::MethodCallback(SubEvent_T* sub, ARGS ...args)
{
	MCB mcb = *(MCB*)&(sub->secondaryCallback);
	T *obj = (T*)(sub->userArgs);
	(obj->*mcb)(args...);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// EVENT VERSION 0
// Basic Idea in place.  
// - Not Thread Safe
// - Only works with void* data
// - Only works with C style functions
//--------------------------------------------------------------------

// An Event/Delegate at its core is just a list of function callbacks
//
// Events can be registered with some identifying information [user_arg].
// And calling the event can take some event specific data [event_arg]
/*
typedef void(*BasicEventCallback)(void* userArg, void* eventArg);

class EventV0 {
// subscription - when subscribing this is the identifying
// information (what to call, and what to call with)
struct EventSub_T {
BasicEventCallback cb;
void* userArg;
};

public:
EventV0() {
}

~EventV0() {
m_subscriptions.clear();
}

void Subscribe(void* userArg, BasicEventCallback cb) {
// Good safeguard in debug to add is to make sure
// you're not double subscribing to an event
// with a similar pair.
// #TODO - ASSERT AGAINST ABOVE

// Add subscriptoin
EventSub_T sub;
sub.cb = cb;
sub.userArg = userArg;
m_subscriptions.push_back(sub);
}

void Unsubscribe(void *user_arg, BasicEventCallback cb) {
for (int i = 0; i < (int)m_subscriptions.size(); ++i) {
EventSub_T &sub = m_subscriptions[i];
if ((sub.cb == cb) && (sub.userArg == user_arg)) {
m_subscriptions.erase(m_subscriptions.begin() + i);
return; // should be unique, so return immeidately
}
}
}

void Trigger(void *event_arg) {
for (int i = 0; i < (int)m_subscriptions.size(); ++i) {
EventSub_T &sub = m_subscriptions[i];
sub.cb(sub.userArg, event_arg);
}
}

public:
std::vector<EventSub_T> m_subscriptions;
};
*/