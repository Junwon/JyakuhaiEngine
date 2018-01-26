#include "Engine/Renderer/Pose.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

Matrix4 PoseTransform_T::GetLocalTransformMatrix() const
{
	Matrix4 returnMatrix = Matrix4::IDENTITY;
	
	returnMatrix.Scale(m_scale);
	returnMatrix.RotateWithQuaternion(m_quaternion);
	returnMatrix.Translate(m_position);
	
	return returnMatrix;
}

void PoseTransform_T::SetLocalTransform(const Matrix4& m)
{
	m_position = m.m_translation.XYZ();
	m_scale = m.GetScale();
	m_quaternion.LoadFromMatrix(m.GetOrthoNormalizeAboutZ());
}

void Pose::SetLocalTransformLength(int size)
{
	m_localTransforms.resize(size);
}

void PrintPoseTransformf(const PoseTransform_T& transform)
{
	DebuggerPrintlnf("Position:\t %.3f, %.3f, %.3f", transform.m_position.x, transform.m_position.y, transform.m_position.z);
	DebuggerPrintlnf("Quaternion:\t %.3f, %.3f, %.3f, %.3f", transform.m_quaternion.m_w, transform.m_quaternion.m_x, transform.m_quaternion.m_y, transform.m_quaternion.m_z);
	DebuggerPrintlnf("Scale:\t\t %.3f, %.3f, %.3f", transform.m_scale.x, transform.m_scale.y, transform.m_scale.z);
}
