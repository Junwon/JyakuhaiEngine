#pragma once

#include <vector>

#include "Engine/Math/Quaternion.hpp"
#include "Engine/Renderer/Skeleton.hpp"

struct PoseTransform_T
{
	PoseTransform_T() :
		m_position(Vector3::ZERO),
		m_scale(Vector3::ONE),
		m_quaternion(Quaternion())
	{};

	Vector3 m_position;
	Vector3 m_scale;
	Quaternion m_quaternion;

	Matrix4 GetLocalTransformMatrix() const;
	void SetLocalTransform(const Matrix4& m);
};

void PrintPoseTransformf(const PoseTransform_T& transform);

class Pose
{
public:
	void SetLocalTransformLength(int size);

public:
	std::vector<PoseTransform_T> m_localTransforms;
};