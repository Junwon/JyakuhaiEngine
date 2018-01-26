#include "Engine/Renderer/RHI/VertexBuffer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

VertexBuffer::VertexBuffer(RHIDevice* owner, Vertex3D const* vertices, unsigned int count, eBufferUsage bufferUsage)
{
	m_bufferUsage = bufferUsage;
	m_dxBuffer = nullptr;
	m_bufferSize = 0U;

	Create(owner, vertices, count);
}

VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(m_dxBuffer);
}

bool VertexBuffer::Create(RHIDevice* owner, const Vertex3D* vertices, unsigned int count)
{
	DX_SAFE_RELEASE(m_dxBuffer);

	m_bufferSize = count * sizeof(Vertex3D);

	// First, describe the buffer
	D3D11_BUFFER_DESC vbDesc;
	memset(&vbDesc, 0, sizeof(vbDesc));

	vbDesc.ByteWidth = m_bufferSize;					// How much data are we putting into this buffer
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // What can we bind this data as (in this case, only vertex data)
	vbDesc.StructureByteStride = sizeof(Vertex3D);      // How large is each element in this buffer
														// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	if (m_bufferUsage == BUFFERUSAGE_DYNAMIC)
	{
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		vbDesc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
	}

	if (vertices != nullptr)
	{
		D3D11_SUBRESOURCE_DATA initialData;
		memset(&initialData, 0, sizeof(initialData));
		initialData.pSysMem = vertices;

		// Finally create the vertex buffer
		owner->dxDevice->CreateBuffer(&vbDesc, &initialData, &m_dxBuffer);
	}
	else
	{
		owner->dxDevice->CreateBuffer(&vbDesc, nullptr, &m_dxBuffer);
	}

	if (m_dxBuffer == nullptr) 
	{
		m_bufferSize = 0U;
		return false;
	}
	else 
	{
		return true;
	}
}

void VertexBuffer::Update(RHIDeviceContext* context, const Vertex3D* vertices, unsigned int count)
{
	size_t size = sizeof(Vertex3D) * count;

	if (size > m_bufferSize) 
	{
		// too large - can't map it.  Recreate a larger one [note, will require a rebind
		// to the context before it takes affect]
		Create(context->device, vertices, count);
	}
	else 
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		ID3D11DeviceContext *dxContext = context->dxContext;

		if (SUCCEEDED(dxContext->Map(m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) 
		{
			memcpy(resource.pData, vertices, size);
			dxContext->Unmap(m_dxBuffer, 0);
		}
	}
}
