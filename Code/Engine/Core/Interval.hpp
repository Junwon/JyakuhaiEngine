#pragma once

class Clock;

class Interval
{
public:
	Interval();

public:
	// void SetParentClock(Clock* clock);
	void SetSeconds(float seconds);
	void SetFrequency(float hz);
	bool CheckIfCurrentTimeReachesTargetTime(); // ? what am I checking for?
	bool CheckAndDecrement();
	bool CheckAndReset();
	unsigned int DecrementAll();
	void Reset();

public:
	float m_intervalTime;
	float m_targetTime;
};