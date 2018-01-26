#include "Engine/Renderer/Skeleton.hpp"

#include "Engine/Math/Vector3.hpp"

Skeleton::Skeleton()
{
}

void Skeleton::Clear()
{
	m_joints.clear();
}

void Skeleton::AddJoint(const std::string& name, const std::string& parentName, const Matrix4& transform)
{
	m_joints.push_back(new Joint(name, GetJoint(parentName), transform));
}

unsigned int Skeleton::GetJointCount() const
{
	return m_joints.size();
}

int Skeleton::GetJointIndex(const std::string& name) const
{
	int index = -1;

	if (name == "")
	{
		return -1;
	}

	for each(Joint* j in m_joints)
	{
		index++;
		if (j->m_name == name)
		{
			return index;
		}

	}

	return index;
}

Joint* Skeleton::GetJoint(const std::string& jointName)
{
	for each(Joint* j in m_joints)
	{
		if (j->m_name == jointName)
		{
			return j;
		}
	}

	return nullptr;
}

Joint* Skeleton::GetJoint(unsigned int jointIDx)
{
	return m_joints[jointIDx];
}

Matrix4 Skeleton::GetJointTransform(unsigned int jointIDx) const
{	
	Matrix4 jointTransform = Matrix4::IDENTITY;

	if (jointIDx == INVALID_JOINT_INDEX)
	{
		return jointTransform;
	}

	return m_joints[jointIDx]->m_transform;
}

Matrix4 Skeleton::GetJointTransform(const std::string& name) const
{
	Matrix4 jointTransform = Matrix4::IDENTITY;

	for each(Joint* j in m_joints)
	{
		if (j->m_name == name)
		{
			jointTransform = j->m_transform;
			break;
		}
	}

	return jointTransform;
}

const std::string Skeleton::GetJointName(unsigned int jointIDx) const
{
	return m_joints[jointIDx]->m_name;
}

int Skeleton::GetJointParentID(unsigned int jointIDx) const
{
	Joint* parentJoint = m_joints[jointIDx]->m_parentJoint;

	if (parentJoint == nullptr)
	{
		return INVALID_JOINT_INDEX;
	}

	return std::find(m_joints.begin(), m_joints.end(), parentJoint) - m_joints.begin();
}

Vector3 Joint::GetJointTranslation() const
{
	return m_transform.m_translation.XYZ();
}
