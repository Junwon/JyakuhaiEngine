#include "Engine/Renderer/RHI/ComputeShaderJob.hpp"

#include "Engine/Renderer/RHI/ComputeShader.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/ConstantBuffer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

ComputeShaderJob::ComputeShaderJob() :
	m_shaderToRun(nullptr)
{
	m_rhiContext = nullptr;

	m_uintBuffer = nullptr;
	m_floatBuffer = nullptr;
	m_vector2Buffer = nullptr;
	m_vector4Buffer = nullptr;
	m_colorBuffer = nullptr;
}

ComputeShaderJob::ComputeShaderJob(ComputeShader* cs) :
	m_shaderToRun(cs)
{
	m_rhiContext = cs->m_rhiDevice->GetImmediateContext();

	SetConstantBuffer(cs);
}

ComputeShaderJob::~ComputeShaderJob()
{
	SAFE_DELETE(m_uintBuffer);
	SAFE_DELETE(m_floatBuffer);
	SAFE_DELETE(m_vector2Buffer);
	SAFE_DELETE(m_vector4Buffer);
	SAFE_DELETE(m_colorBuffer);
}

void ComputeShaderJob::SetConstantBuffer(ComputeShader* cs)
{
	m_uintBuffer = new ConstantBuffer(cs->m_rhiDevice, &m_uintData, sizeof(m_uintData));
	m_rhiContext->dxContext->CSSetConstantBuffers(UINT_BUFFER_INDEX, 1, &m_uintBuffer->dxBuffer);

	m_floatBuffer = new ConstantBuffer(cs->m_rhiDevice, &m_floatData, sizeof(m_floatData));
	m_rhiContext->dxContext->CSSetConstantBuffers(FLOAT_BUFFER_INDEX, 1, &m_floatBuffer->dxBuffer);

	m_vector2Buffer = new ConstantBuffer(cs->m_rhiDevice, &m_vector2Data, sizeof(m_vector2Data));
	m_rhiContext->dxContext->CSSetConstantBuffers(VEC2_BUFFER_INDEX, 1, &m_vector2Buffer->dxBuffer);

	m_vector4Buffer = new ConstantBuffer(cs->m_rhiDevice, &m_vector4Data, sizeof(m_vector4Data));
	m_rhiContext->dxContext->CSSetConstantBuffers(VEC4_BUFFER_INDEX, 1, &m_vector4Buffer->dxBuffer);

	m_colorBuffer = new ConstantBuffer(cs->m_rhiDevice, &m_colorData, sizeof(m_colorData));
	m_rhiContext->dxContext->CSSetConstantBuffers(COLOR_BUFFER_INDEX, 1, &m_colorBuffer->dxBuffer);
}

void ComputeShaderJob::SetShader(ComputeShader* cs)
{
	m_shaderToRun = cs;
	m_rhiContext = cs->m_rhiDevice->GetImmediateContext();
}

void ComputeShaderJob::SetGridDimensions(unsigned int x, unsigned int y, unsigned int z)
{
	m_gridSize = UnsignedInt3_T(x, y, z);
	m_rhiContext->DispatchComputeShaderJob(this);
}

void ComputeShaderJob::SetResource(unsigned int index, RHITexture2D* res)
{
	m_dxShaderResourceViews[index] = res->m_dxShaderResourceView;
}

void ComputeShaderJob::SetSampler(unsigned int index, Sampler* sampler)
{
	m_dxSamplerStates[index] = sampler->dxSampler;
}

void ComputeShaderJob::SetReadWriteUAVResource(unsigned int index, RHITexture2D* res)
{
	m_dxUnorderedAccessView[index] = res->m_dxUnorderedAccessView;
}

void ComputeShaderJob::SetUnsignedInt(const unsigned int& v, const char*)
{
	m_uintData.x = v;
	m_uintBuffer->Update(m_rhiContext, &m_uintData);
}

void ComputeShaderJob::SetFloat(const float& v0, const float& v1, const float& v2, const float& v3, const char*)
{
	m_floatData.x = v0;
	m_floatData.y = v1;
	m_floatData.z = v2;
	m_floatData.w = v3;
	m_floatBuffer->Update(m_rhiContext, &m_floatData);
}

void ComputeShaderJob::SetVector2(const Vector2& v, const char*)
{
	m_vector2Data = Vector4(v, 0.f, 0.f);
	m_vector2Buffer->Update(m_rhiContext, &m_vector2Data);
}

void ComputeShaderJob::SetVector4(const Vector4& v, const char*)
{
	m_vector4Data = v;
	m_vector4Buffer->Update(m_rhiContext, &m_vector4Data);
}

void ComputeShaderJob::SetColor(const Rgba& v, const char*)
{
	RgbaFloats rgbaFloats = v.GetAsRgbaFloats();
	m_colorData.r = rgbaFloats.r;
	m_colorData.g = rgbaFloats.g;
	m_colorData.b = rgbaFloats.b;
	m_colorData.a = rgbaFloats.a;

	m_colorBuffer->Update(m_rhiContext, &m_colorData);
}
