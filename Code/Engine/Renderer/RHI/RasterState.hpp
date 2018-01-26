#pragma once

#include "Engine/Renderer/RHI/DX11.hpp"

class RHIDevice;

class RasterState
{
public:
	RasterState(RHIDevice* owner);
	~RasterState();

	inline bool IsValid() const { return (nullptr != dxRasterizerState); }

public:
	RHIDevice* device;
	ID3D11RasterizerState* dxRasterizerState;
};