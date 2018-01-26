#pragma once

#include "Engine/Renderer/RHI/DX11.hpp"

enum eFilterMode
{
	FILTER_POINT,
	FILTER_LINEAR
};

class RHIDevice;

class Sampler
{
public:
	Sampler(RHIDevice* owner);
	~Sampler();

	bool CreateSampler(eFilterMode minFilter, eFilterMode magFilter);

public:
	RHIDevice* device;
	ID3D11SamplerState* dxSampler;
};