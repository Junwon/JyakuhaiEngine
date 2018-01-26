#include "Engine/Renderer/RHI/BlendState.hpp"

#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

static D3D11_BLEND ToDXFactor(const eBlendFactor factor)
{
	switch (factor) {
	case BLEND_ONE:
		return D3D11_BLEND_ONE;

	case BLEND_ZERO:
		return D3D11_BLEND_ZERO;

	case BLEND_SRCALPHA:
		return D3D11_BLEND_SRC_ALPHA;

	case BLEND_INVSRCALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;

	default:
		ASSERT_OR_DIE(false, "Undefined type in Blend State");
		return D3D11_BLEND_ONE;
	}
}

BlendState::BlendState(RHIDevice* owner, bool enabled, eBlendFactor const src, eBlendFactor dest) :
	device(owner),
	dxState(nullptr)
{
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.AlphaToCoverageEnable = false;		//multisample
	desc.IndependentBlendEnable = false;	//multiple render target

	desc.RenderTarget[0].BlendEnable = enabled;

	//Color Blending
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = ToDXFactor(src);
	desc.RenderTarget[0].DestBlend = ToDXFactor(dest);

	//expose Alpha Blending
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device* dxDevice = device->dxDevice;
	HRESULT hr = dxDevice->CreateBlendState(&desc, &dxState);

	if (!SUCCEEDED(hr)) {
		ASSERT_OR_DIE(0, "Blend State failed to initalize.");
	}
}

BlendState::~BlendState()
{
	DX_SAFE_RELEASE(dxState);
}

