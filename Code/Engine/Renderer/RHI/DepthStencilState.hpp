#pragma once
#include "Engine/Renderer/RHI/DX11.hpp"
#include "Engine/Renderer/RHI/RHITypes.hpp"

class RHIDevice;

struct DepthStencilState_T
{
	bool depthWritingEnabled;
	bool depthTestEnabled;
};

class DepthStencilState
{
public:
	DepthStencilState(RHIDevice* owner, const DepthStencilState_T& desc);
	~DepthStencilState();

	inline bool IsValid() const { return (nullptr != dxState); }

public:

	bool enabled;

	RHIDevice* device;
	ID3D11DepthStencilState* dxState;
};