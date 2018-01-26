#pragma once

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Pose.hpp"

enum MOTION_PLAYBACK
{
	PINGPONG,
	CLAMP,
	PLAYBACK
};

// also referred to as an AnimationClip or Clip.
class Motion
{
private:
	const float DEFAULT_FRAMERATE = 10.f;

public:
	Motion();
	~Motion();

public:

	float SetDuration(float time); 	// Sets the array of poses to be exactly long enough to encompass this time.
	void SetName(const char* name) { m_name = name; };

	// framerate is 10hz, that means the time between two frames takes 1/10th of a second.
	// therefore if we have 3 frames in this motion, the total duration would be (3 - 1) / 10hz,
	// or 0.2 seconds long.  
	float GetDuration() const { return (m_framePoses.size() - 1) / m_framerate; }
	unsigned int GetFrameCount() const { return m_framePoses.size(); }
	Pose* GetPoseAtFrame(unsigned int id) const { return m_framePoses[id]; }

	// This will fill out a pose with the evaluated pose at time
	void Evaluate(Pose* out, float time) const;

	void InterpolatePose(Pose& out, const Pose& lastPose, const Pose& nextPose, float t) const;
public:
	std::string m_name;

	float m_framerate;
	std::vector<Pose*> m_framePoses;
	float m_localEvaluatedFrame;
};
