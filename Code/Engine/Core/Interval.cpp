#include "Engine/Core/Interval.hpp"

#include "Engine/Core/Time.hpp"

Interval::Interval() :
	m_intervalTime(0.f),
	m_targetTime(0.f)
{
}

// void Interval::SetParentClock(Clock* clock)
// {
// 
// }

void Interval::SetSeconds(float seconds)
{
	m_intervalTime = seconds;
	m_targetTime = (float)(GetCurrentTimeSeconds()) + seconds; // was GetCurrentFrameTime
}

void Interval::SetFrequency(float hz)
{
	SetSeconds(1.0f / hz);
}

bool Interval::CheckIfCurrentTimeReachesTargetTime()
{
	float currentTime = (float)(GetCurrentTimeSeconds());
	return (currentTime >= m_targetTime);
}

bool Interval::CheckAndDecrement()
{
	if (CheckIfCurrentTimeReachesTargetTime()) {
		m_targetTime += m_intervalTime;
		return true;
	}
	else {
		return false;
	}
}

bool Interval::CheckAndReset()
{
	if (CheckIfCurrentTimeReachesTargetTime()) {
		Reset();
		return true;
	}
	else {
		return false;
	}
}

unsigned int Interval::DecrementAll()
{
	unsigned int count = 0;
	while (CheckAndDecrement()) {
		++count;
	}

	return count;
}

void Interval::Reset()
{
	m_targetTime = (float)GetCurrentTimeSeconds() + m_intervalTime;
}
