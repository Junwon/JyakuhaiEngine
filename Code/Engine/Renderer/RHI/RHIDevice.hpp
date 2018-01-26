#pragma once

#include "Engine/Math/Vector3.hpp"

#include "Engine/Renderer/RHI/RHITypes.hpp"
#include "Engine/Renderer/RHI/DX11.hpp"

class Window;
class RHIInstance;
class RHIDeviceContext;
class RHIOutput;
class RHITexture2D;

class ShaderProgram;
class VertexBuffer;
class IndexBuffer;

// Created from RenderInstance when initial output is created
class RHIDevice
{
public:
	RHIDevice(RHIInstance* owner, ID3D11Device* dxd);
	~RHIDevice();

	RHIDeviceContext* GetImmediateContext();

	ShaderProgram* CreateShaderFromHLSLFile(const char* filename);
	ShaderProgram* CreateShaderFromString(const char* filename);
	VertexBuffer* CreateVertexBuffer(Vertex3D* vertices, unsigned int vertexCount);
	IndexBuffer* CreateIndexBuffer(unsigned int* indices, unsigned int indexCount);

	RHIOutput* CreateOutput(Window* window, eRHIOutputMode const mode = RENDEROUTPUT_WINDOWED);
	RHIOutput* CreateOutput(UINT const px_width, UINT const px_height, eRHIOutputMode const mode = RENDEROUTPUT_WINDOWED);

	RHITexture2D* CreateRenderTarget(unsigned int width, unsigned int height);
	RHITexture2D* CreateDepthTarget(unsigned int width, unsigned int height);

public:
	ID3DBlob* CompileHLSLToShaderBlob(char const* optFilename, void const* sourceCode, size_t const sourceCodeSize,
										 char const* entrypoint, char const* target);

public:
	RHIDeviceContext* immediateContext;
	RHIInstance* instance;

	ID3D11Device* dxDevice;

private:
	ShaderProgram* CreateShaderProgram(char const* filename, const void* sourceCode, size_t sourceCodeSize, const char* debugName);
};

//static ID3DBlob* CompileHLSLToShaderBlob(char const* optFilename, void const* sourceCode, size_t const sourceCodeSize,
//										 char const* entrypoint, char const* target);