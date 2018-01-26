#include "Engine/Renderer/RHI/Sampler.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

Sampler::Sampler(RHIDevice* owner) :
	device(owner),
	dxSampler(nullptr)
{
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(dxSampler);
}

static D3D11_FILTER DXGetFilterMode(eFilterMode minFilter, eFilterMode magFilter)
{
	if (minFilter == FILTER_POINT)
	{
		if (magFilter == FILTER_POINT)
		{
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else //LINEAR
		{
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		}
	}
	else
	{
		if (magFilter == FILTER_POINT)
		{
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else //LINEAR
		{
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
	}
}

bool Sampler::CreateSampler(eFilterMode minFilter, eFilterMode magFilter)
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Filter = DXGetFilterMode(minFilter, magFilter);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.BorderColor
	desc.MinLOD = 0;
	desc.MaxLOD = 0;

	ID3D11Device* dxDevice = device->dxDevice;
	HRESULT hr = dxDevice->CreateSamplerState(&desc, &dxSampler);

	return SUCCEEDED(hr);
}