#include "Engine/Renderer/RHI/IndexBuffer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

IndexBuffer::IndexBuffer(RHIDevice* owner, const unsigned int* indices, unsigned int vertexCount, eBufferUsage bufferUsage) :
	m_bufferUsage(bufferUsage),
	m_dxBuffer(nullptr),
	m_bufferSize(0U)
{
	Create(owner, indices, vertexCount);
}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_dxBuffer);
}

bool IndexBuffer::Create(RHIDevice* device, const unsigned int* indices, unsigned int count)
{
	DX_SAFE_RELEASE(m_dxBuffer);

	m_numIndices = count;
	m_bufferSize = m_numIndices * sizeof(unsigned int);

	D3D11_BUFFER_DESC indexBufferDesc;
	memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));

	indexBufferDesc.ByteWidth = m_bufferSize;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = sizeof(unsigned int);

	if (m_bufferUsage == BUFFERUSAGE_DYNAMIC) {
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	}
	else {
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;               
	}

	indexBufferDesc.MiscFlags = 0;
	
	if (m_bufferUsage == BUFFERUSAGE_DYNAMIC) {
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else {
		indexBufferDesc.CPUAccessFlags = 0;
	}
	
	if (indices != nullptr) {
		D3D11_SUBRESOURCE_DATA initialData;
		memset(&initialData, 0, sizeof(initialData));

		initialData.pSysMem = indices;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;

		device->dxDevice->CreateBuffer(&indexBufferDesc, &initialData, &m_dxBuffer);
	}
	else {
		device->dxDevice->CreateBuffer(&indexBufferDesc, nullptr, &m_dxBuffer);
	}

	if (m_dxBuffer == nullptr) {
		m_bufferSize = 0U;
		return false;
	}
	else {
		return true;
	}
}

void IndexBuffer::Update(RHIDeviceContext* context, const unsigned int* indices, unsigned int count)
{
	size_t bufferSize = count * sizeof(unsigned int);

	if (bufferSize > m_bufferSize && IsValid()) {
		Create(context->device, indices, count);
	}
	else {
		m_numIndices = count;

		D3D11_MAPPED_SUBRESOURCE resource;
		ID3D11DeviceContext* dxContext = context->dxContext;

		if (SUCCEEDED(dxContext->Map(m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
			memcpy(resource.pData, indices, bufferSize);
			dxContext->Unmap(m_dxBuffer, 0);
		}
	}
}
