#pragma once

class Timer
{
public:
	Timer();
	Timer(float maxTime);
public:
	void Update(float deltaSeconds);
	void Start();
	void Reset();
	void Stop();
	bool IfCanRun() const;
	bool IsRunning() const;
	bool IsFinished() const;
	float GetTimePercentage() const;

public:
	float m_maxTime;
	float m_maxTimeRecipricol;
	float m_currentElapsedTime;
	bool  m_hasMaxTimeReached;
	bool  m_canRun;
};
