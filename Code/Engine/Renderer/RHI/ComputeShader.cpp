#include "Engine/Renderer/RHI/ComputeShader.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

ComputeShader::ComputeShader(RHIDevice* device) :
	m_rhiDevice(device),
	dxComputeShader(nullptr)
{
}

ComputeShader::~ComputeShader()
{
	DX_SAFE_RELEASE(dxComputeShader);
}

void ComputeShader::Destroy()
{
	DX_SAFE_RELEASE(dxComputeShader);
}

bool ComputeShader::LoadFile(const char* filename, const char* entryPoint)
{
	std::vector<unsigned char> codeBuffer;
	if (!ReadBufferFromFile(codeBuffer, filename)) {
		ASSERT_OR_DIE(0, "File not found when creating shader from HLSL");
		return false;
	}

	ID3DBlob* csByteCode = nullptr;
	csByteCode = m_rhiDevice->CompileHLSLToShaderBlob(filename, &codeBuffer[0], codeBuffer.size(), entryPoint, "cs_5_0");

	if (csByteCode != nullptr) {
		dxComputeShader = nullptr;

		m_rhiDevice->dxDevice->CreateComputeShader(csByteCode->GetBufferPointer(), csByteCode->GetBufferSize(),
									  nullptr, &dxComputeShader);

		return true;
	}

	return false;
}