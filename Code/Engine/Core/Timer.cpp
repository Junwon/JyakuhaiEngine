#include "Engine/Core/Timer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

Timer::Timer() :
	m_maxTime(0.f),
	m_maxTimeRecipricol(0.f),
	m_currentElapsedTime(0.f),
	m_canRun(false),
	m_hasMaxTimeReached(false)
{
}

Timer::Timer(float maxTime) :
	m_maxTime(maxTime),
	m_currentElapsedTime(0.f),
	m_canRun(false),
	m_hasMaxTimeReached(false)
{
	if (maxTime == 0.f)
	{
		m_maxTimeRecipricol = 0.f;
	}
	else
	{
		m_maxTimeRecipricol = 1 / maxTime;
	}
}

void Timer::Update(float deltaSeconds)
{
	if (m_canRun && !m_hasMaxTimeReached)
	{
		m_currentElapsedTime += deltaSeconds;

		if (m_currentElapsedTime >= m_maxTime)
		{
			m_hasMaxTimeReached = true;
		}
	}
}

void Timer::Start()
{
	m_canRun = true;
}

void Timer::Reset()
{
	m_currentElapsedTime = 0.f;
	m_hasMaxTimeReached = false;
}

void Timer::Stop()
{
	m_canRun = false;
}

bool Timer::IfCanRun() const
{
	return m_canRun;
}

bool Timer::IsRunning() const
{
	return !m_hasMaxTimeReached;
}

bool Timer::IsFinished() const
{
	return m_hasMaxTimeReached;
}

float Timer::GetTimePercentage() const
{
	return 1.f - ((m_maxTime - m_currentElapsedTime) * m_maxTimeRecipricol);
}
