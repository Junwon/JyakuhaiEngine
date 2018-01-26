#pragma once

#include "Engine/UI/UIElement.hpp"

class RHITexture2D;

struct UISpriteImage_t
{
	UISpriteImage_t() :
		m_texture(nullptr),
		m_bounds(AABB2D::ZERO_TO_ONE)
	{};

	RHITexture2D* m_texture;
	AABB2D m_bounds;
};

class UISprite : public UIElement
{
public:
	UISprite();
	~UISprite();

public:
	virtual void Draw() const override;

public:
	void SetSprite(const std::string spriteDir);
	void SetToNativeSize(float sizeRatio = 1.f);
public:
	UISpriteImage_t m_sprite;
};