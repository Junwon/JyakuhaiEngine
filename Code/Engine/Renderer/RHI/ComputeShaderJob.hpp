#pragma once

#include <map>

#include "Engine/Renderer/RHI/DX11.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"

class ComputeShader;
class Sampler;
class RHITexture2D;
class RHIDeviceContext;
class ConstantBuffer;

struct UnsignedInt3_T
{
	UnsignedInt3_T() :
		m_x(0),
		m_y(0),
		m_z(0)
	{};

	UnsignedInt3_T(unsigned int x, unsigned int y, unsigned int z) :
		m_x(x),
		m_y(y),
		m_z(z)
	{};

	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_z;
};

struct UnsignedIntData_T
{
	UnsignedIntData_T() :
		x(0),
		y(0),
		z(0),
		w(0)
	{};

	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};

struct FloatData_T
{
	FloatData_T() :
		x(0.f),
		y(0.f),
		z(0.f),
		w(0.f)
	{};

	float x;
	float y;
	float z;
	float w;
};

#define UINT_BUFFER_INDEX (4)
#define FLOAT_BUFFER_INDEX (5)
#define VEC2_BUFFER_INDEX (6)
#define VEC4_BUFFER_INDEX (7)
#define COLOR_BUFFER_INDEX (8)

class ComputeShaderJob
{
public:
	ComputeShaderJob();
	ComputeShaderJob(ComputeShader* cs);
	~ComputeShaderJob();

public:
	void SetConstantBuffer(ComputeShader* cs);

public:
	// Sets the shader program to use for this job;
	void SetShader(ComputeShader* cs);

	// Dispach grid
	// These arguments are just forwarded to ID3D11DeviceContext::Dispatch
	// when we run the ("dispatch") job.
	void SetGridDimensions(unsigned int x, unsigned int y, unsigned int z = 1);

	void SetResource(unsigned int index, RHITexture2D* res);
	void SetSampler(unsigned int index, Sampler* sampler);

	// Set a UA resource view (bind points use register(u#) in the shader)
	// Use "rw" for read/write resource
	void SetReadWriteUAVResource(unsigned int index, RHITexture2D* res);


	// Sets the properties for this program by name
	void SetUnsignedInt(const unsigned int& v, const char* id = "");
	void SetFloat(const float& v0, const float& v1 = 0.f, const float& v2 = 0.f, const float& v3 = 0.f, const char* id = "");
	void SetVector2(const Vector2& v, const char* id = "");
	void SetVector4(const Vector4& v, const char* id = "");
	void SetColor(const Rgba& v, const char* id = "");
	// etc...

public:
	ComputeShader* m_shaderToRun;

	// The grid/block size use for a dispatch
	// Can store this with the job
	// RHIDeviceContext::Dispatch call - keep it with the job.
	UnsignedInt3_T m_gridSize;

	RHIDeviceContext* m_rhiContext;

	std::map<UINT, ID3D11ShaderResourceView*> m_dxShaderResourceViews;
	std::map<UINT, ID3D11SamplerState*> m_dxSamplerStates;
	std::map<UINT, ID3D11UnorderedAccessView*> m_dxUnorderedAccessView;

	UnsignedIntData_T m_uintData;
	FloatData_T m_floatData;
	Vector4 m_vector2Data;
	Vector4 m_vector4Data;
	RgbaFloats m_colorData;

	ConstantBuffer* m_uintBuffer;
	ConstantBuffer* m_floatBuffer;
	ConstantBuffer* m_vector2Buffer;
	ConstantBuffer* m_vector4Buffer;
	ConstantBuffer* m_colorBuffer;
};