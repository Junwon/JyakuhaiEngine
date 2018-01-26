#include "Engine/Renderer/Motion.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Motion::Motion() :
	m_framerate(DEFAULT_FRAMERATE),
	m_localEvaluatedFrame(0.f)
{
}

Motion::~Motion()
{

}

float Motion::SetDuration(float time)
{
	float newDuration = (time * m_framerate + 1);

	m_framePoses.resize(Ceiling(time * m_framerate) + 1);

	for (unsigned int i = 0; i < m_framePoses.size(); ++i)
	{
		m_framePoses[i] = new Pose();
	}

	return newDuration;
}

void Motion::Evaluate(Pose* out, float time) const
{
	float evaluatedFrame = time * m_framerate;

	if (evaluatedFrame < m_framePoses.size())
	{
		int startFrameIndex = (int)(evaluatedFrame);
		Pose startPose = Pose(*m_framePoses[startFrameIndex]);
		Pose endPose = Pose(*m_framePoses[(startFrameIndex + 1) % m_framePoses.size()]);

		InterpolatePose(*out, startPose, endPose, evaluatedFrame - (float)(startFrameIndex));

		//DELETE ME WHEN THIS IS ALL OVER
		if (evaluatedFrame > 20.f && evaluatedFrame <= 20.3f)
		{
			DebuggerPrintlnf("Pew pew");
		}
	}
	else
	{
		if (m_framePoses.size() > 0)
		{
			*out = Pose(*m_framePoses[0]);
		}		
	}
}

void Motion::InterpolatePose(Pose& out, const Pose& startPose, const Pose& endPose, float t) const
{
	for (int i = 0; i < (int)out.m_localTransforms.size(); ++i)
	{
		out.m_localTransforms[i].m_position = Lerp(startPose.m_localTransforms[i].m_position, endPose.m_localTransforms[i].m_position, t);
		out.m_localTransforms[i].m_scale = Lerp(startPose.m_localTransforms[i].m_scale, endPose.m_localTransforms[i].m_scale, t);
		out.m_localTransforms[i].m_quaternion = Slerp(startPose.m_localTransforms[i].m_quaternion, endPose.m_localTransforms[i].m_quaternion, t);
	}
}
