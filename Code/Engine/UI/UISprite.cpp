#include "Engine/UI/UISprite.hpp"

#include "Engine/Renderer/RHI/SimpleRenderer.hpp"

UISprite::UISprite()
{

}

UISprite::~UISprite()
{

}

void UISprite::Draw() const
{
	if (m_renderOutput && m_isVisible) {
		m_renderOutput->DrawTexturedAABB2DSprite(GetRelativeBounds(), AABB2D::ZERO_TO_ONE, m_sprite.m_texture, false, m_color);
	}
}

void UISprite::SetSprite(const std::string spriteDir)
{
	RHITexture2D* sprite = SimpleRenderer::GetInstance()->CreateOrGetTexture(spriteDir);

	if (sprite) {
		m_sprite.m_texture = sprite;
		m_sprite.m_bounds = AABB2D(Vector2((float)sprite->GetWidth(), (float)sprite->GetHeight()));
	}
	else {
		m_sprite.m_texture = nullptr;
		m_sprite.m_bounds = AABB2D::ZERO_TO_ZERO;
	}
}

void UISprite::SetToNativeSize(float sizeRatio)
{
	SetSize(m_sprite.m_bounds.CalcSize() * sizeRatio);
}
