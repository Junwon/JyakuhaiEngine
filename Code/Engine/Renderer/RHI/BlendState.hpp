#pragma once
#include "Engine/Renderer/RHI/DX11.hpp"

class RHIDevice;

enum eBlendFactor
{
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_SRCALPHA,
	BLEND_INVSRCALPHA
};

class BlendState
{
public:
	BlendState(RHIDevice* owner, bool isEnabled, eBlendFactor const src = BLEND_ONE, eBlendFactor dest = BLEND_ZERO);
	~BlendState();

	inline bool IsValid() const { return (nullptr != dxState); }

public:

	bool enabled;
	eBlendFactor srcFactor;
	eBlendFactor destFactor;

	RHIDevice* device;
	ID3D11BlendState* dxState;
};