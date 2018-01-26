#include "Engine/Math/MatrixStack.hpp"

MatrixStack::MatrixStack()
{
}

void MatrixStack::Push(const Matrix4& mat)
{
	m_stack.push(GetTop() * mat);
}

void MatrixStack::PushDirect(const Matrix4& mat)
{
	m_stack.emplace(mat);
}

void MatrixStack::Pop()
{
	m_stack.pop();
}

Matrix4 MatrixStack::GetTop() const
{
	return m_stack.top();
}
