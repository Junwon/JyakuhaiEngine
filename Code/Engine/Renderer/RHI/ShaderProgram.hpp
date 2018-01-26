#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/RHI.hpp"

enum eShaderStage
{
	SHADER_VERTEX,    // maps to target "vs_5_0", or Vertex Shader Model 5 (currently latest)
	SHADER_FRAGMENT,  // maps to target "ps_5_0", or Pixel Shader Model 5 (currently latest)
};

class ShaderProgram
{
public:
	ShaderProgram(ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3DBlob* vs_byteCode, ID3DBlob* ps_byteCode) :
		dxVertexShader(vs),
		dxFragmentShader(ps),
		vsByteCode(vs_byteCode),
		fsByteCode(ps_byteCode),
		dxInputLayout(nullptr)
	{};

	~ShaderProgram()
	{
		DX_SAFE_RELEASE(dxVertexShader);
		DX_SAFE_RELEASE(dxFragmentShader);
		DX_SAFE_RELEASE(dxInputLayout);
		DX_SAFE_RELEASE(vsByteCode);
		DX_SAFE_RELEASE(fsByteCode);
	}

	// feel free to add in more methods to help with the creation process if you want.
	void CreateInputLayout(RHIDevice* device);

	inline bool IsValid() const { return (dxVertexShader != nullptr) && (dxFragmentShader != nullptr); }

public:
	// All the steps to this (could be split out to a ShaderStage)
	ID3D11VertexShader* dxVertexShader;
	ID3D11PixelShader* dxFragmentShader;

	// Input Layout - for now, we're going 
	// to assume every shader makes their own
	// for simplicity, but honestly you should make these
	// based on need
	ID3D11InputLayout* dxInputLayout;

	// [OPTIONAL] ByteCode - only need to keep it around if using Reflection
	ID3DBlob* vsByteCode;
	ID3DBlob* fsByteCode;

	std::string m_name;
};
