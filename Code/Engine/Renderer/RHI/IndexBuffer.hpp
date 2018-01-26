#pragma once

#include "Engine/Renderer/RHI/RHI.hpp"

class IndexBuffer
{
public:
	IndexBuffer(RHIDevice* owner, const unsigned int* indices, unsigned int vertexCount, eBufferUsage bufferUsage = BUFFERUSAGE_STATIC);
	~IndexBuffer();

	bool Create(RHIDevice *device, const unsigned int* indices, unsigned int count);
	void Update(RHIDeviceContext* context, const unsigned int* indices, unsigned int count);
	inline bool IsValid() const { return (m_dxBuffer != nullptr); }

public:
	eBufferUsage m_bufferUsage;
	ID3D11Buffer* m_dxBuffer;
	unsigned int m_numIndices;
	unsigned int m_bufferSize;
};