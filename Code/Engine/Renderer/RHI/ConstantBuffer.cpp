#include "Engine/Renderer/RHI/ConstantBuffer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

ConstantBuffer::ConstantBuffer(RHIDevice* owner, const void* buffer, const size_t buffer_size)
{
	size = buffer_size;
	// First, describe the buffer
	D3D11_BUFFER_DESC cb;
	memset(&cb, 0, sizeof(cb));

	cb.ByteWidth = (unsigned int)buffer_size;			// How much data are we putting into this buffer
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// What can we bind this data as (in this case, only vertex data)
	cb.Usage = D3D11_USAGE_DYNAMIC;						// Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														// for limitations/strenghts of each, see;  
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	cb.StructureByteStride = 0;							// How large is each element in this buffer

														// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initialData;
	memset(&initialData, 0, sizeof(initialData));
	initialData.pSysMem = buffer;

	// Finally create the vertex buffer
	dxBuffer = nullptr;
	owner->dxDevice->CreateBuffer(&cb, &initialData, &dxBuffer);
}

ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(dxBuffer);
}

bool ConstantBuffer::Update(RHIDeviceContext* context, void* buffer)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext* dxContext = context->dxContext;

	if (SUCCEEDED(dxContext->Map(dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, buffer, size);
		dxContext->Unmap(dxBuffer, 0);
		return true;
	}

	return false;
}

