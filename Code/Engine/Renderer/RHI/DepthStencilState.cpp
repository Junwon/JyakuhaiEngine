#include "Engine/Renderer/RHI/DepthStencilState.hpp"

#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

DepthStencilState::DepthStencilState(RHIDevice* owner, const DepthStencilState_T& desc) :
	device(owner),
	dxState(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC dxDesc;
	memset(&dxDesc, 0, sizeof(dxDesc));

	dxDesc.DepthEnable = desc.depthWritingEnabled || desc.depthTestEnabled;
	dxDesc.DepthWriteMask = desc.depthWritingEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dxDesc.DepthFunc = desc.depthTestEnabled ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;
	dxDesc.StencilEnable = FALSE;

	HRESULT hr = device->dxDevice->CreateDepthStencilState(&dxDesc, &dxState);
	
	if (!SUCCEEDED(hr)) {
		ASSERT_OR_DIE(0, "Depth Stencil State failed to initalize.");
	}
}

DepthStencilState::~DepthStencilState()
{
	DX_SAFE_RELEASE(dxState);
}
