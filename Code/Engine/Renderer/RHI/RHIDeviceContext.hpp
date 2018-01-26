#pragma once

#include "Engine/Renderer/RHI/DX11.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

#include "Engine/Core/Rgba.hpp"

class RHIInstance;		// System level singleton
class RHIDevice;		// Physical GPU
class RHIDeviceContext; // Potential Display/Background worker
class RHIOutput;
class RHITexture2D;
class RasterState;
class BlendState;
class Sampler;
class ConstantBuffer;
class VertexBuffer;
class IndexBuffer;
class DepthStencilState;
class StructuredBuffer;
class ComputeShader;
class ComputeShaderJob;

class RHIDeviceContext
{
public:
	RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx);
	~RHIDeviceContext();

	void ClearState();  // Clears all rendering state
	void Flush();       // Flushes all commands

	void ClearColorTarget(RHITexture2D* output, RgbaFloats const &color);

	void ClearDepthTarget(RHITexture2D* output, float depth = 1.0f, UINT8 stencil = 0);

	void SetRasterState(RasterState* rasterState);

	void SetBlendState(BlendState* blendState);

	void SetRenderTarget(RHITexture2D* targetTexture);

	void SetSingleViewPort(D3D11_VIEWPORT viewport);

	void SetShader(ShaderProgram* shader);

	void SetComputeShader(ComputeShader* computeShader);

	void SetDepthStencilState(DepthStencilState* depthStencilState);

	void SetColorTarget(RHITexture2D* color, RHITexture2D* depthStencilTarget);

	void SetTexture2D(unsigned int textureIndex, const RHITexture2D* texture);

	void SetSampler(unsigned int samplerIndex, Sampler* sampler);

	void SetConstantBuffer(unsigned int constBufferIndex, ConstantBuffer* buffer);

	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY d3d_prim);

	void SetVertexBuffers(VertexBuffer* vbo);

	void SetIndexBuffers(IndexBuffer* ibo);

	void SetStructuredBuffer(unsigned int bindPoint, StructuredBuffer* buffer);

	void DrawNumVertex(unsigned int vertexCount, unsigned int startIndex = 0);

	void DispatchComputeShaderJob(ComputeShaderJob* job);
public:
	RHIDevice *device;
	ID3D11DeviceContext *dxContext;
};