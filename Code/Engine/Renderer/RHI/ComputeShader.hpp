#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/RHI.hpp"

class ComputeShader
{
public:
	// Similar to a pipeline shader - needs the device used for creation
	// and the .hlsl file to load.
	ComputeShader(RHIDevice* device);

	// Cleans up used resources
	~ComputeShader();
	void Destroy();

	bool LoadFile(const char* filename, const char* entryPoint = nullptr);

	inline bool IsValid() const { return (dxComputeShader != nullptr); }

public:
	ID3D11ComputeShader* dxComputeShader;
	RHIDevice* m_rhiDevice;
};