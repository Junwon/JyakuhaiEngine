#pragma once

#include <vector>

#include "Engine/Renderer/Pose.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "Engine/Renderer/RHI/StructuredBuffer.hpp"

class SkeletonInstance
{
public:
	SkeletonInstance(Skeleton* pSkeleton);

public:
	void ApplyMotion(const Motion* motion, const float time);

	Matrix4 GetJointGlobalTransform(unsigned int jointIDx) const;

	int GetJointParentID(unsigned int jointIDx) const;

public:
	Skeleton* m_skeleton;
	Pose m_currentPose; 
	std::vector<StructuredBuffer> m_skinTransforms;
};
