#include "Engine/Renderer/RHI/StructuredBuffer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

StructuredBuffer::StructuredBuffer(RHIDevice* owner, const void* buffer, unsigned int objectSize, unsigned int objectCount)
{
	dxShaderResourceView = nullptr;
	bufferSize = objectSize * objectCount;
	objCount = objectCount;
	objSize = objectSize;

	// First, describe the buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

	vertexBufferDesc.ByteWidth = (unsigned int)bufferSize;                // How much data are we putting into this buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                  // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// CHANGES FROM CONSTANT BUFFER!!!
	vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;       // What can we bind this data as (in this case, only vertex data)
	vertexBufferDesc.StructureByteStride = objectSize;            // How large is each element in this buffer
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initialData;
	memset(&initialData, 0, sizeof(initialData));
	initialData.pSysMem = buffer;

	// Finally create the vertex buffer
	dxBuffer = nullptr;
	owner->dxDevice->CreateBuffer(&vertexBufferDesc, &initialData, &dxBuffer);
	CreateViews(owner);
}

StructuredBuffer::~StructuredBuffer()
{
	DX_SAFE_RELEASE(dxBuffer);
	DX_SAFE_RELEASE(dxShaderResourceView);
}

bool StructuredBuffer::CreateViews(RHIDevice* owner)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0U;
	desc.Buffer.NumElements = objCount;

	HRESULT result = owner->dxDevice->CreateShaderResourceView(dxBuffer, &desc, &dxShaderResourceView);
	
	if (SUCCEEDED(result) == false)
	{
		return false;
	}

	return true;
}

bool StructuredBuffer::Update(RHIDeviceContext* context, const void* buffer)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext *dxContext = context->dxContext;
	if (SUCCEEDED(dxContext->Map(dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) 
	{
		memcpy(resource.pData, buffer, bufferSize);
		dxContext->Unmap(dxBuffer, 0);

		return true;
	}

	return false;
}

