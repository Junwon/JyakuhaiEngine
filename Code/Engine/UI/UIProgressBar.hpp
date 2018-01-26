#pragma once

#include "Engine/UI/UIElement.hpp"

class UIProgressBar : public UIElement
{
public:
	UIProgressBar();
	UIProgressBar(float max, float min = 0.f);
	~UIProgressBar();

public:
	virtual void Update(float deltaSeconds);
	virtual void Draw() const override;

public:
	float CalculatePercentage() const;
	float GetMinValue() const;
	float GetMaxValue() const;
	float GetCurrentValue() const;
	void SetMinValue(float value);
	void SetMaxValue(float value);
	void SetCurrentValue(float value);
	void DecrementProgress(float decrement);

private:
	float m_min;
	float m_max;
	float m_current;
};