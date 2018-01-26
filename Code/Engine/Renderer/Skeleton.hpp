#pragma once

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Matrix4.hpp"


static const int INVALID_JOINT_INDEX = -1;

class Vector3;

struct Joint
{
	Joint() :
		m_name(""),
		m_parentJoint(nullptr),
		m_transform(Matrix4::IDENTITY) 
	{};

	Joint(const std::string& name, Joint* parentJoint, const Matrix4& matrix) :
		m_name(name),
		m_parentJoint(parentJoint),
		m_transform(matrix)
	{};

	Vector3 GetJointTranslation() const;

	std::string m_name;
	Joint* m_parentJoint;
	Matrix4 m_transform;
};

class Skeleton
{
public:
	Skeleton();

public:
	// Reset the skeleton - clear out all bones that make up
	// this skeleton
	void Clear();

	// Adds a joint.  Can be parented to another 
	// joint within this skeleton.
	void AddJoint(const std::string& name, const std::string& parentName, const Matrix4& transform);

	// get number of joints/bones in this skeleton.
	unsigned int GetJointCount() const;

	// Get a joint index by name, returns
	// (uint)(-1) if it doesn't exist.
	int GetJointIndex(const std::string& name) const;

	Joint* GetJoint(const std::string& jointName);
	Joint* GetJoint(unsigned int jointIDx);

	// Get the global transform for a joint.
	Matrix4 GetJointTransform(unsigned int jointIDx) const;
	Matrix4 GetJointTransform(const std::string& name) const;
	const std::string GetJointName(unsigned int jointIDx) const;
	int GetJointParentID(unsigned int jointIDx) const;

public:
	std::vector<Joint*> m_joints;
};