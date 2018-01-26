#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"

#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"

#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/RasterState.hpp"
#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/ComputeShader.hpp"
#include "Engine/Renderer/RHI/BlendState.hpp"
#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/ConstantBuffer.hpp"
#include "Engine/Renderer/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/RHI/IndexBuffer.hpp"
#include "Engine/Renderer/RHI/DepthStencilState.hpp"
#include "Engine/Renderer/RHI/StructuredBuffer.hpp"

#include "Engine/Renderer/RHI/ComputeShaderJob.hpp"

RHIDeviceContext::RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx) :
	device(owner),
	dxContext(ctx)
{
}

RHIDeviceContext::~RHIDeviceContext()
{
	DX_SAFE_RELEASE(dxContext);
}

void RHIDeviceContext::ClearState()
{
	dxContext->ClearState();
}

void RHIDeviceContext::Flush()
{
	dxContext->Flush();
}

void RHIDeviceContext::ClearColorTarget(RHITexture2D* output, RgbaFloats const &color)
{
	const float color_floats[4] = { color.r, color.g, color.b, color.a };
	dxContext->ClearRenderTargetView(output->m_dxRenderTargetView, color_floats);
}

void RHIDeviceContext::ClearDepthTarget(RHITexture2D* output, float depth, UINT8 stencil)
{
	if (output != nullptr) {
		dxContext->ClearDepthStencilView(output->m_dxDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}
}

void RHIDeviceContext::SetRasterState(RasterState* rasterState)
{
	dxContext->RSSetState(rasterState->dxRasterizerState);
}

void RHIDeviceContext::SetBlendState(BlendState* blendState)
{
	float constant[] = { 1.f, 0.1f, 0.1f, 1.f };
	dxContext->OMSetBlendState(blendState->dxState, constant, 0xffffffff);
}

void RHIDeviceContext::SetRenderTarget(RHITexture2D* targetTexture)
{
	dxContext->OMSetRenderTargets(1, &targetTexture->m_dxRenderTargetView, nullptr);
}

void RHIDeviceContext::SetSingleViewPort(D3D11_VIEWPORT viewport)
{
	dxContext->RSSetViewports(1, &viewport);
}

void RHIDeviceContext::SetShader(ShaderProgram* shader)
{
	dxContext->VSSetShader(shader->dxVertexShader, nullptr, 0U);
	dxContext->PSSetShader(shader->dxFragmentShader, nullptr, 0U);
	dxContext->IASetInputLayout(shader->dxInputLayout);
}

void RHIDeviceContext::SetComputeShader(ComputeShader* computeShader)
{
	dxContext->CSSetShader(computeShader->dxComputeShader, nullptr, 0U);
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState* depthStencilState)
{
	dxContext->OMSetDepthStencilState(depthStencilState->dxState, 0U);
}

void RHIDeviceContext::SetColorTarget(RHITexture2D* targetTexture, RHITexture2D* depthStencilTarget)
{
	dxContext->OMSetRenderTargets(1, &targetTexture->m_dxRenderTargetView, depthStencilTarget != nullptr ? depthStencilTarget->m_dxDepthStencilView : nullptr);
}

void RHIDeviceContext::SetTexture2D(unsigned int textureIndex, const RHITexture2D* texture)
{
	dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxShaderResourceView);
	dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxShaderResourceView);
}

void RHIDeviceContext::SetSampler(unsigned int samplerIndex, Sampler* sampler)
{
	dxContext->VSSetSamplers(samplerIndex, 1, &sampler->dxSampler);
	dxContext->PSSetSamplers(samplerIndex, 1, &sampler->dxSampler);
}

void RHIDeviceContext::SetConstantBuffer(unsigned int constBufferIndex, ConstantBuffer* buffer)
{
	dxContext->VSSetConstantBuffers(constBufferIndex, 1, &buffer->dxBuffer);
	dxContext->PSSetConstantBuffers(constBufferIndex, 1, &buffer->dxBuffer);
}

void RHIDeviceContext::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY d3d_prim)
{
	dxContext->IASetPrimitiveTopology(d3d_prim);
}

void RHIDeviceContext::SetVertexBuffers(VertexBuffer* vbo)
{
	unsigned int stride = sizeof(Vertex3D);
	unsigned int offsets = 0;
	dxContext->IASetVertexBuffers(0, 1, &vbo->m_dxBuffer, &stride, &offsets);
}

//#TODO finish IndexBuffer Implementation
void RHIDeviceContext::SetIndexBuffers(IndexBuffer* ibo)
{
	UNUSED(ibo);
// 	unsigned int stride = sizeof(unsigned int);
// 	unsigned int offsets = 0;
// 	dxContext->IASetIndexBuffer(0, 1, &ibo->m_dxBuffer, &stride, &offsets);
}

void RHIDeviceContext::SetStructuredBuffer(unsigned int bindPoint, StructuredBuffer* buffer)
{
	// #TODO: check for null, and unbind if so.
	dxContext->VSSetShaderResources(bindPoint, 1, &buffer->dxShaderResourceView);
	dxContext->PSSetShaderResources(bindPoint, 1, &buffer->dxShaderResourceView);
}

void RHIDeviceContext::DrawNumVertex(unsigned int vertexCount, unsigned int startIndex)
{
	dxContext->Draw(vertexCount, startIndex);
}

void RHIDeviceContext::DispatchComputeShaderJob(ComputeShaderJob* job)
{
	auto srvIter = job->m_dxShaderResourceViews.begin();

	while (srvIter != job->m_dxShaderResourceViews.end()) {
		dxContext->CSSetShaderResources(srvIter->first, 1, &srvIter->second);
		++srvIter;
	}

	//////////////////////////////////////////////////////////////////////////

	auto samplerIter = job->m_dxSamplerStates.begin();

	while (samplerIter != job->m_dxSamplerStates.end()) {
		dxContext->CSSetSamplers(samplerIter->first, 1, &samplerIter->second);
		++samplerIter;
	}

	//////////////////////////////////////////////////////////////////////////

	auto uavIter = job->m_dxUnorderedAccessView.begin();

	while (uavIter != job->m_dxUnorderedAccessView.end()) {
		dxContext->CSSetUnorderedAccessViews(uavIter->first, 1, &uavIter->second, nullptr);
		++uavIter;
	}

	dxContext->Dispatch(job->m_gridSize.m_x, job->m_gridSize.m_y, job->m_gridSize.m_z);

	uavIter = job->m_dxUnorderedAccessView.begin();

	while (uavIter != job->m_dxUnorderedAccessView.end()) {
		ID3D11UnorderedAccessView* ptr = nullptr;
		dxContext->CSSetUnorderedAccessViews(uavIter->first, 1, &ptr, nullptr);
		++uavIter;
	}
}
