#include "Engine/Renderer/SkeletonInstance.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

SkeletonInstance::SkeletonInstance(Skeleton* pSkeleton) :
	m_skeleton(pSkeleton)
{
	m_currentPose.SetLocalTransformLength(m_skeleton->m_joints.size());
}


void SkeletonInstance::ApplyMotion(const Motion* motion, const float time)
{
	motion->Evaluate(&m_currentPose, time);
}


Matrix4 SkeletonInstance::GetJointGlobalTransform(unsigned int jointIDx) const
{
	int parentID = GetJointParentID(jointIDx);

	Matrix4 globalTransform = m_currentPose.m_localTransforms[jointIDx].GetLocalTransformMatrix();

	while (parentID != -1)
	{
		Matrix4 parentMat = m_currentPose.m_localTransforms[parentID].GetLocalTransformMatrix();
		globalTransform = parentMat * globalTransform;
		parentID = GetJointParentID(parentID);
	} 	

	return globalTransform;
}


int SkeletonInstance::GetJointParentID(unsigned int jointIDx) const
{
	return m_skeleton->GetJointParentID(jointIDx);
}
