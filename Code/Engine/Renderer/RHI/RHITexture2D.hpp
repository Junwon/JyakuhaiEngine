#pragma once
#include <string>
#include "Engine/Renderer/Image.hpp"
#include "Engine/Renderer/RHI/DX11.hpp"
#include "Engine/Core/Rgba.hpp"

class RHIDevice;
class RHIOutput;

class RHITexture2D
{
public:
	RHITexture2D(RHIDevice* owner);
	RHITexture2D(RHIDevice* owner, ID3D11Texture2D* tex, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	//RHITexture2D(RHIDevice* owner, const char* filePath);
	RHITexture2D(RHIDevice* owner, const std::string filePath, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	RHITexture2D(RHIDevice* owner, const Rgba& color, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	RHITexture2D(RHIDevice* owner, UINT width, UINT height, eImageFormat format = IMAGEFORMAT_RGBA8, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	RHITexture2D(RHIDevice* owner, RHIOutput* ouput);
	RHITexture2D(RHIDevice* owner, const Image& image, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	~RHITexture2D();

	bool LoadImageFromFile(const char* filePath, D3D11_USAGE usageType = D3D11_USAGE_IMMUTABLE);
	bool LoadFromImage(const Image &img, D3D11_USAGE usageType);
	bool LoadFromColor(const Rgba& color, D3D11_USAGE usageType);
	void CreateViews();
	UINT GetWidth();
	UINT GetHeight();

	inline bool IsValid() const { return (m_dxTexture2D != nullptr); }
	inline bool IsRenderTarget() const { return (m_dxRenderTargetView != nullptr); }

public:
	RHIDevice* m_device;
	ID3D11Texture2D* m_dxTexture2D;
	ID3D11RenderTargetView* m_dxRenderTargetView;
	ID3D11ShaderResourceView* m_dxShaderResourceView;
	ID3D11DepthStencilView* m_dxDepthStencilView;
	ID3D11UnorderedAccessView* m_dxUnorderedAccessView;

	UINT				m_width;
	UINT				m_height;
	UINT				m_dxBindFlags;
	std::string			m_imageFilePath;
};