#pragma once

#include "Engine/Renderer/RHI/RHI.hpp"

class StructuredBuffer
{
public:
	// Note, it takes an object size, and object_count instead
	// of the arryas total size, as object_size matters in 
	// creation!
	StructuredBuffer(RHIDevice* owner,
		const void* buffer,
		unsigned int objectSize,
		unsigned int objectCount);
	~StructuredBuffer();

	bool CreateViews(RHIDevice* owner);

	bool Update(RHIDeviceContext* context, const void* buffer);

	inline bool isValid() const { return (dxBuffer != nullptr); }

public:
	ID3D11Buffer* dxBuffer;
	ID3D11ShaderResourceView* dxShaderResourceView;

	unsigned int objCount;
	unsigned int objSize;
	size_t bufferSize;
};