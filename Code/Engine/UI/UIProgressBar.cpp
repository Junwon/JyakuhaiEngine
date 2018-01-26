#include "Engine/UI/UIProgressBar.hpp"

#include "Engine/Renderer/RHI/SimpleRenderer.hpp"

UIProgressBar::UIProgressBar() :
	m_min(0.f),
	m_max(0.f),
	m_current(0.f)
{
}

UIProgressBar::UIProgressBar(float max, float min) :
	m_min(min),
	m_max(max),
	m_current(max)
{
}

UIProgressBar::~UIProgressBar()
{

}

void UIProgressBar::Update(float)
{

}

void UIProgressBar::Draw() const
{
	AABB2D containingBounds = GetRelativeBounds();
	AABB2D progressBounds = AABB2D(containingBounds.m_mins, containingBounds.m_maxs * CalculatePercentage());
	m_renderOutput->DrawQuad2D(progressBounds, m_color);
}

float UIProgressBar::CalculatePercentage() const
{
	return m_current / (m_max - m_min);
}

float UIProgressBar::GetMinValue() const
{
	return m_min;
}

float UIProgressBar::GetMaxValue() const
{
	return m_max;
}

float UIProgressBar::GetCurrentValue() const
{
	return m_current;
}

void UIProgressBar::SetMinValue(float value)
{
	m_min = value;
}

void UIProgressBar::SetMaxValue(float value)
{
	m_max = value;
}

void UIProgressBar::SetCurrentValue(float value)
{
	m_current = value;
}

void UIProgressBar::DecrementProgress(float decrement)
{
	m_current -= decrement;
}
