#include "Engine/Renderer/RHI/RHIDevice.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/Window.hpp"

#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/Image.hpp"

#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/RHI/IndexBuffer.hpp"
#include <vector>

#define UNUSED(x) (void)(x);

RHIDevice::RHIDevice(RHIInstance *owner, ID3D11Device *dxd) :
	instance(owner),
	dxDevice(dxd),
	immediateContext(nullptr)
{
}

RHIDevice::~RHIDevice()
{
	DX_SAFE_RELEASE(dxDevice);
}

RHIDeviceContext* RHIDevice::GetImmediateContext()
{
	return immediateContext;
}

ID3DBlob* RHIDevice::CompileHLSLToShaderBlob(char const* optFilename,	// optional: used for error messages
	void const* sourceCode,												// buffer containing source code.
	size_t const sourceCodeSize,										// size of the above buffer.
	char const* entrypoint,												// Name of the Function we treat as the entry point for this stage
	char const* target)													// What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// Can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;   // Only do this if this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Default is level 1
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile(sourceCode,
		sourceCodeSize,						// Plain text source code
		optFilename,						// Optional, used for error messages
		nullptr,                            // Pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // Iinclude rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compileFlags,						// Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors);                           // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*)errors->GetBufferPointer();
			DebuggerPrintlnf("Failed to compile shader [%s].  Compiler gave the following output;\n%s",
				optFilename,
				error_string);
			DX_SAFE_RELEASE(errors);
		}
	}

	// will be nullptr if it failed to compile
	return code;
}

ShaderProgram* RHIDevice::CreateShaderFromHLSLFile(char const* filename)
{
	std::vector<unsigned char> codeBuffer;
	if (!ReadBufferFromFile(codeBuffer, filename))
	{
		ASSERT_OR_DIE(0, "File not found when creating shader from HLSL");
		return nullptr;
	}

	return CreateShaderProgram(filename, &codeBuffer[0], codeBuffer.size(), filename);
}

ShaderProgram* RHIDevice::CreateShaderFromString(const char* shader)
{
	std::vector<char> codeBuffer(shader, shader + strlen(shader));

	return CreateShaderProgram(nullptr, &codeBuffer[0], codeBuffer.size(), shader);
}

VertexBuffer* RHIDevice::CreateVertexBuffer(Vertex3D* vertices, unsigned int vertexCount)
{
	VertexBuffer* buffer = new VertexBuffer(this, vertices, vertexCount);
	return buffer;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(unsigned int* indices, unsigned int indexCount)
{
	IndexBuffer* buffer = new IndexBuffer(this, indices, indexCount);
	return buffer;
}

RHIOutput* RHIDevice::CreateOutput(Window *windowParam, eRHIOutputMode const mode)
{
	UNUSED(windowParam);
	UNUSED(mode);
	return nullptr;
}

//#TODO : double check
RHIOutput* RHIDevice::CreateOutput(UINT const px_width, UINT const px_height, eRHIOutputMode const mode)
{
// 	Window* window = new Window();
// 	window->width = px_width;
// 	window->height = px_height;
// 	return new RHIOutput(this, window, mode);
	UNUSED(px_width);
	UNUSED(px_height);
	UNUSED(mode);
	return nullptr;
}

RHITexture2D* RHIDevice::CreateRenderTarget(unsigned int width, unsigned int height)
{
	RHITexture2D* tex = new RHITexture2D(this, width, height, IMAGEFORMAT_RGBA8);
	return tex;
}

RHITexture2D* RHIDevice::CreateDepthTarget(unsigned int width, unsigned int height)
{
	RHITexture2D* tex = new RHITexture2D(this, width, height, IMAGEFORMAT_D24S8);
	return tex;
}

ShaderProgram* RHIDevice::CreateShaderProgram(char const* filename, const void* sourceCode, size_t sourceCodeSize, const char* debugName)
{
	ID3DBlob* vsByteCode = nullptr;
	ID3DBlob* fsByteCode = nullptr;

	vsByteCode = CompileHLSLToShaderBlob(filename, sourceCode, sourceCodeSize, "VertexFunction", "vs_5_0");
	fsByteCode = CompileHLSLToShaderBlob(filename, sourceCode, sourceCodeSize, "FragmentFunction", "ps_5_0");

	if ((vsByteCode != nullptr) && (fsByteCode != nullptr)) {

		ID3D11VertexShader* vs = nullptr;
		ID3D11PixelShader* fs = nullptr;

		dxDevice->CreateVertexShader(vsByteCode->GetBufferPointer(),
									 vsByteCode->GetBufferSize(),
									 nullptr,
									 &vs);

		dxDevice->CreatePixelShader(fsByteCode->GetBufferPointer(),
									fsByteCode->GetBufferSize(),
									nullptr,
									&fs);

		ShaderProgram* program = new ShaderProgram(vs, fs, vsByteCode, fsByteCode);
		RHIInstance::SetDebugName(debugName, program->dxFragmentShader);
		program->CreateInputLayout(this);
		return program;
	}

	return nullptr;
}