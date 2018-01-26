#pragma once

#include <stack>

#include "Engine/Renderer/RHI/RHITypes.hpp"
#include "Engine/Math/Matrix4.hpp"

class MatrixStack
{
public:
	MatrixStack();

public:
	void Push(const Matrix4& mat);
	void PushDirect(const Matrix4& mat);
	void Pop();
	Matrix4 GetTop() const;

public:
	std::stack<Matrix4> m_stack;
};