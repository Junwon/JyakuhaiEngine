#pragma once

#include "Engine/Renderer/RHI/RHI.hpp"

class ConstantBuffer
{
public:
	ConstantBuffer(RHIDevice* owner, const void* buffer, const size_t buffer_size);
	
	~ConstantBuffer();

	inline bool IsValid() const { return (dxBuffer != nullptr); }

	bool Update(RHIDeviceContext* context, void* buffer);

public:
	ID3D11Buffer* dxBuffer;
	size_t size;
};