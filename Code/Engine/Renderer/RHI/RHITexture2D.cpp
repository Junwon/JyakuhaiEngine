#include "Engine/Renderer/RHI/RHITexture2D.hpp"

#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "RHIInstance.hpp"

RHITexture2D::RHITexture2D(RHIDevice *owner) : 
	m_device(owner), 
	m_dxTexture2D(nullptr),
	m_dxRenderTargetView(nullptr), 
	m_dxShaderResourceView(nullptr), 
	m_dxDepthStencilView(nullptr),
	m_dxUnorderedAccessView(nullptr),
	m_imageFilePath(""),
	m_width(0), 
	m_height(0), 
	m_dxBindFlags(0U)
{}

RHITexture2D::RHITexture2D(RHIDevice* owner, ID3D11Texture2D* tex, D3D11_USAGE usageType) :
	m_device(owner),
	m_dxTexture2D(tex),
	m_imageFilePath(""),
	m_dxRenderTargetView(nullptr)
{
	if (tex != nullptr) {

		tex->AddRef();
		m_dxTexture2D = tex;

		D3D11_TEXTURE2D_DESC desc;
		desc.Usage = usageType;
		tex->GetDesc(&desc);

		m_width = desc.Width;
		m_height = desc.Height;

		CreateViews();
	}
}

RHITexture2D::RHITexture2D(RHIDevice* owner, RHIOutput* output) :
	RHITexture2D(owner)
{
	if (output != nullptr) {

		ID3D11Texture2D* backBuffer = nullptr;
		output->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

		if (nullptr != backBuffer)  {

			m_dxTexture2D = backBuffer;

			D3D11_TEXTURE2D_DESC desc;
			m_dxTexture2D->GetDesc(&desc);

			m_imageFilePath = "";
			m_width = desc.Width;
			m_height = desc.Height;
			m_dxBindFlags = desc.BindFlags;

			CreateViews();
		}
	}
}



// RHITexture2D::RHITexture2D(RHIDevice* owner, const char* filePath) :
// 	RHITexture2D(owner)
// {
// 	m_imageFilePath = filePath;
// 	LoadImageFromFile(filePath);
// }

RHITexture2D::RHITexture2D(RHIDevice* owner, const std::string filePath, D3D11_USAGE usageType) :
	RHITexture2D(owner)
{
	m_imageFilePath = filePath;
	LoadImageFromFile(filePath.data(), usageType);
}

RHITexture2D::RHITexture2D(RHIDevice* owner, const Rgba& color, D3D11_USAGE usageType) :
	RHITexture2D(owner)
{
	m_imageFilePath = "";
	LoadFromColor(color, usageType);
}

RHITexture2D::RHITexture2D(RHIDevice *owner, UINT w, UINT h, eImageFormat format, D3D11_USAGE usageType) :
	RHITexture2D(owner)
{
	D3D11_USAGE usage = usageType; // D3D11_USAGE_DEFAULT;
	m_imageFilePath = "";
	m_width = w;
	m_height = h;

	DXGI_FORMAT dxFormat;
	UINT dxBinding = 0U;
	switch (format)
	{
	case IMAGEFORMAT_RGBA8:
		dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		dxBinding = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		break;

	case IMAGEFORMAT_D24S8:
		dxFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dxBinding = D3D11_BIND_DEPTH_STENCIL;
		break;

	default:
		ASSERT_OR_DIE(false, "Cannot create RHITexture2D with the provided Image Format.");
		return;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));

	texDesc.Width = m_width;
	texDesc.Height = m_height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Usage = usage;
	texDesc.Format = dxFormat;
	texDesc.BindFlags = dxBinding;
	texDesc.CPUAccessFlags = 0U;
	texDesc.MiscFlags = 0U;

	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	ID3D11Device* dxDevice = m_device->dxDevice;
	HRESULT hr = dxDevice->CreateTexture2D(&texDesc, nullptr, &m_dxTexture2D);

	if (SUCCEEDED(hr))
	{
		m_dxBindFlags = dxBinding;
		CreateViews();
	}
}

RHITexture2D::RHITexture2D(RHIDevice* owner, const Image& image, D3D11_USAGE usageType) :
	RHITexture2D(owner)
{
	m_imageFilePath = "";
	LoadFromImage(image, usageType);
}

RHITexture2D::~RHITexture2D()
{
	if (IsValid())
	{
		DX_SAFE_RELEASE(m_dxRenderTargetView);
		DX_SAFE_RELEASE(m_dxShaderResourceView);
		DX_SAFE_RELEASE(m_dxDepthStencilView);
		DX_SAFE_RELEASE(m_dxUnorderedAccessView);
		DX_SAFE_RELEASE(m_dxTexture2D);
	}
}

bool RHITexture2D::LoadImageFromFile(const char* filePath, D3D11_USAGE usageType)
{
	Image image;
	if (!image.LoadFromFile(filePath)) {
		return false;
	}

	if (LoadFromImage(image, usageType))
	{
		m_width = image.m_width;
		m_height = image.m_height;
		RHIInstance::SetDebugName(filePath, m_dxTexture2D);
		return true;
	}
	else
	{
		return false;
	}
}

bool RHITexture2D::LoadFromImage(const Image &img, D3D11_USAGE usageType)
{
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));

	texDesc.Width = img.GetWidth();
	texDesc.Height = img.GetHeight();
	texDesc.MipLevels = 1;		//No mip mappings
	texDesc.ArraySize = 1;		//No Arrays
	texDesc.Usage = usageType;  //D3D11_USAGE_IMMUTABLE;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0U;
	texDesc.MiscFlags = 0U;

	//multisampling options
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	m_dxBindFlags |= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));

	data.pSysMem = img.m_buffer;
	data.SysMemPitch = img.m_bpp * texDesc.Width;

	ID3D11Device* dxDevice = m_device->dxDevice;
	HRESULT hr = dxDevice->CreateTexture2D(&texDesc, &data, &m_dxTexture2D);

	if (SUCCEEDED(hr))
	{
		CreateViews();
		return true;
	}

	return false;
}

bool RHITexture2D::LoadFromColor(const Rgba& color, D3D11_USAGE usageType)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(textureDesc));

	textureDesc.Width = 1;
	textureDesc.Height = 1;
	textureDesc.MipLevels = 1;																// setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
	textureDesc.ArraySize = 1;
	textureDesc.Usage = usageType;															// data is set at creation time and won't change
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;										// R8G8B8A8 texture
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;		// we're going to be using this texture as a shader resource
	textureDesc.CPUAccessFlags = 0U;														// Determines how we can access this resource CPU side (IMMUTABLE, So none)
	textureDesc.MiscFlags = 0;								

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	m_dxBindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = &color.r;
	data.SysMemPitch = sizeof(color);

	HRESULT hr = m_device->dxDevice->CreateTexture2D(&textureDesc, &data, &m_dxTexture2D);

	m_width = 1;
	m_height = 1;

	if (SUCCEEDED(hr)) 
	{
		CreateViews();
		return true;
	}
	
	return false;
	
}


void RHITexture2D::CreateViews()
{
	ID3D11Device *dd = m_device->dxDevice;

	if (m_dxBindFlags & D3D11_BIND_RENDER_TARGET)
	{
		dd->CreateRenderTargetView(m_dxTexture2D, nullptr, &m_dxRenderTargetView);
	}

	if (m_dxBindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		dd->CreateShaderResourceView(m_dxTexture2D, nullptr, &m_dxShaderResourceView);
	}

	if (m_dxBindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		dd->CreateDepthStencilView(m_dxTexture2D, nullptr, &m_dxDepthStencilView);
	}

	if (m_dxBindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		memset(&uavDesc, 0, sizeof(uavDesc));

		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		HRESULT hr = dd->CreateUnorderedAccessView(m_dxTexture2D, &uavDesc, &m_dxUnorderedAccessView);
		ASSERT_OR_DIE(SUCCEEDED(hr), "CreateUnorderedAccessView Failed.");
	}
}

UINT RHITexture2D::GetWidth()
{
	return m_width;
}

UINT RHITexture2D::GetHeight()
{
	return m_height;
}
