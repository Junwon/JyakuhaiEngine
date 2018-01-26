#include "Engine/Renderer/RHI/RasterState.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

RasterState::RasterState(RHIDevice* owner) :
	device(owner),
	dxRasterizerState(nullptr)
{

	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.FillMode = D3D11_FILL_SOLID;	//D3D11_FILL_WIRE
	desc.CullMode = D3D11_CULL_BACK;	//D3D11_CULL_NONE, D3D11_CULL_BACK
	desc.FrontCounterClockwise = TRUE;
	desc.AntialiasedLineEnable = FALSE;	//TRUE FOR SMOOTHER LINES
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = FALSE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;

	HRESULT hr = device->dxDevice->CreateRasterizerState(&desc, &dxRasterizerState);

	if (!SUCCEEDED(hr))
	{
		ASSERT_OR_DIE(0, "Rasterizer State failed to initalize.");
	}
}

RasterState::~RasterState()
{
	DX_SAFE_RELEASE(dxRasterizerState);
}

