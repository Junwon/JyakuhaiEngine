#pragma once

#include "Engine/Renderer/RHI/RHI.hpp"

class VertexBuffer
{
public:
	VertexBuffer(RHIDevice* owner, Vertex3D const* vertices, unsigned int vertex_count, eBufferUsage bufferUsage = BUFFERUSAGE_STATIC);
	~VertexBuffer();

	bool Create(RHIDevice *device, const Vertex3D* data, unsigned int count);
	void Update(RHIDeviceContext* context, const Vertex3D* data, unsigned int count);
	inline bool IsValid() const { return (m_dxBuffer != nullptr); }

public:
	eBufferUsage m_bufferUsage;
	ID3D11Buffer* m_dxBuffer;
	unsigned int m_bufferSize;
};