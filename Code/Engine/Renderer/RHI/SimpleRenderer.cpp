#include "Engine/Renderer/RHI/SimpleRenderer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/DefaultShader.hpp"

#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/ComputeShader.hpp"
#include "Engine/Renderer/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/RHI/IndexBuffer.hpp"

#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/RasterState.hpp"

#include "Engine/Renderer/RHI/ConstantBuffer.hpp"
#include "Engine/Renderer/RHI/StructuredBuffer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Core/Performance/ProfilerSystem.hpp"

#include "Engine/Renderer/RHI/ComputeShaderJob.hpp"

std::vector< RHITexture2D* > SimpleRenderer::s_alreadyLoadedTextures;
SimpleRenderer* SimpleRenderer::s_instance = nullptr;

SimpleRenderer::SimpleRenderer() :
	m_width(0),
	m_height(0),
	m_defaultShader(nullptr),
	m_currentTarget(nullptr),
	device(nullptr),
	context(nullptr),
	m_defaultRasterState(nullptr),
	output(nullptr),
	matrixBuffer(nullptr),
	currentBlendState(nullptr),
	mainFont(nullptr),
	tempVBO(nullptr),
	m_boneStructure(nullptr)
{
	s_instance = this;
}

SimpleRenderer::~SimpleRenderer()
{
	s_instance = nullptr;
}

SimpleRenderer* SimpleRenderer::GetInstance()
{
	return s_instance;
}

void SimpleRenderer::Setup(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	RHIInstance* instance = RHIInstance::GetInstance();
	instance->CreateOutput(&device, &context, &output, width, height);

	output->window->SetTitle("");

	m_linearSampler = new Sampler(device);
	m_linearSampler->CreateSampler(FILTER_LINEAR, FILTER_LINEAR);
	SetSampler(0, m_linearSampler);

	m_pointSampler = new Sampler(device);
	m_pointSampler->CreateSampler(FILTER_POINT, FILTER_POINT);
	SetSampler(1, m_pointSampler);

	m_defaultRasterState = new RasterState(device);
	context->SetRasterState(m_defaultRasterState);

	m_timeBuffer.time = 0.0f;
	m_timeBuffer.zoom = 1.0f;
	m_timeConstant = new ConstantBuffer(device, &m_timeBuffer, sizeof(m_timeBuffer));
	SetConstantBuffer(TIME_BUFFER_INDEX, m_timeConstant); //Can bind once

	matrixData.eyePosition = Vector4::ZERO;
	matrixData.model = Matrix4::IDENTITY;
	matrixData.projection = Matrix4::IDENTITY;
	matrixData.view = Matrix4::IDENTITY;
	matrixBuffer = new ConstantBuffer(device, &matrixData, sizeof(matrixData));
	SetConstantBuffer(MATRIX_BUFFER_INDEX, matrixBuffer);

	m_lightConstant = new ConstantBuffer(device, &m_lightBuffer, sizeof(m_lightBuffer));
	SetConstantBuffer(LIGHT_BUFFER_INDEX, m_lightConstant);

	mainFont = new Font(FONT_NAME, FONT_DIR);

	tempVBO = new VertexBuffer(device, nullptr, 32, BUFFERUSAGE_DYNAMIC);

	Image image;
	image.CreateClear(1, 1, RgbaFloats::WHITE);
	m_whiteTexture = new RHITexture2D(device, Rgba::WHITE, D3D11_USAGE_DEFAULT);

	image.CreateClear(1, 1, RgbaFloats(0.5f, 0.5f, 1.f, 1.f));
	m_flatNormalTexture = new RHITexture2D(device, image, D3D11_USAGE_DEFAULT);

	m_defaultTarget = output->GetRenderTarget();

	m_defaultDepthStencil = new RHITexture2D(device, width, height, IMAGEFORMAT_D24S8, D3D11_USAGE_DEFAULT);
	m_currentDepthStencil = nullptr;

	depthStencilDesc.depthTestEnabled = false;
	depthStencilDesc.depthWritingEnabled = true;
	depthStencilState = new DepthStencilState(device, depthStencilDesc);
	context->SetDepthStencilState(depthStencilState);

	SetRenderTarget(nullptr, nullptr);

	m_defaultShader = device->CreateShaderFromString(DEFAULT_SHADER);
	context->SetShader(m_defaultShader);
}

void SimpleRenderer::Destroy()
{
	SAFE_DELETE(device);

	SAFE_DELETE(context);

	SAFE_DELETE(output);

	SAFE_DELETE(m_pointSampler);

	SAFE_DELETE(m_linearSampler);

	SAFE_DELETE(m_defaultRasterState);

	SAFE_DELETE(m_lightConstant);

	SAFE_DELETE(matrixBuffer);

	SAFE_DELETE(m_boneStructure);

	SAFE_DELETE(currentBlendState);

	SAFE_DELETE(m_timeConstant);

	SAFE_DELETE(mainFont);

	SAFE_DELETE(tempVBO);

	SAFE_DELETE(m_defaultDepthStencil);

	SAFE_DELETE(depthStencilState);

	SAFE_DELETE(m_whiteTexture);

	SAFE_DELETE(m_flatNormalTexture);

	SAFE_DELETE(m_defaultShader);

	for (int i = 0; i < (int)s_alreadyLoadedTextures.size(); ++i)
	{
		SAFE_DELETE(s_alreadyLoadedTextures[i]);
	}

	RHIInstance::GetInstance()->ReportLiveObjects();
}				

void SimpleRenderer::SetRenderTarget(RHITexture2D *colorTarget, RHITexture2D* depthTarget)
{
	if (colorTarget != nullptr) 
	{
		m_currentTarget = colorTarget;
	}
	else 
	{
		m_currentTarget = output->GetRenderTarget();
	}

	if (depthTarget == nullptr)
	{
		depthTarget = m_defaultDepthStencil;
	}

	m_currentDepthStencil = depthTarget;
	context->SetColorTarget(m_currentTarget, depthTarget);
}

void SimpleRenderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)

	context->SetSingleViewPort(viewport);
}

void SimpleRenderer::ProcessMessages()
{
	output->window->ProcessMessages();
}

// #TODO : finish it
void SimpleRenderer::SetViewportAsPercent(float x, float y, float w, float h)
{
	UNUSED(x);
	UNUSED(y);
	UNUSED(w);
	UNUSED(h);
}

void SimpleRenderer::ClearScreen(float red, float green, float blue, float alpha)
{
	ClearColor(RgbaFloats(red, green, blue, alpha));
}

void SimpleRenderer::ClearScreen(const Rgba& color)
{
	ClearColor(ConvertColorBytesToFloats(color));
}

void SimpleRenderer::ClearColor(RgbaFloats const &color)
{
	context->ClearColorTarget(m_currentTarget, color);
}

void SimpleRenderer::ClearColorTarget(RHITexture2D *target, RgbaFloats const &color)
{
	context->ClearColorTarget(target, color);
}

void SimpleRenderer::ClearDepth(float depth, UINT8 stencil)
{
	context->ClearDepthTarget(m_currentDepthStencil, depth, stencil);
}

void SimpleRenderer::Present()
{
	output->Present();
}

void SimpleRenderer::SetShader(ShaderProgram* shader)
{
	if (shader)
	{
		context->SetShader(shader);
	}
	else
	{
		context->SetShader(m_defaultShader);
	}
}
	
void SimpleRenderer::SetColorTarget(RHITexture2D* color, RHITexture2D* target)
{
	context->SetColorTarget(color, target);
}

void SimpleRenderer::SetTexture2D(unsigned int textureIndex, const RHITexture2D* texture)
{
	context->SetTexture2D(textureIndex, texture);
}

void SimpleRenderer::SetSampler(unsigned int samplerIndex, Sampler *sampler)
{
	context->SetSampler(samplerIndex, sampler);
}

void SimpleRenderer::SetConstantBuffer(unsigned int constBufferIndex, ConstantBuffer* buffer)
{
	context->SetConstantBuffer(constBufferIndex, buffer);
}

void SimpleRenderer::SetStructuredBuffer(unsigned int structBufferIndex, StructuredBuffer* buffer)
{
	context->SetStructuredBuffer(structBufferIndex, buffer);
}

void SimpleRenderer::SetStructuredBuffer(const void* buffer, unsigned int structureSize, unsigned int structureCount)
{
	if (m_boneStructure == nullptr)
	{
		m_boneStructure = new StructuredBuffer(device, buffer, structureSize, structureCount);
	}

	context->SetStructuredBuffer(STRUCTURED_BUFFER_INDEX, m_boneStructure);
	m_boneStructure->Update(context, buffer);
}

void SimpleRenderer::SetComputeShader(ComputeShader* cs)
{
	if (cs)
	{
		context->SetComputeShader(cs);
	}
}

void SimpleRenderer::DispatchComputeShaderJob(ComputeShaderJob* job)
{
	context->DispatchComputeShaderJob(job);
}

void SimpleRenderer::SetUIOrthoProjection(const Vector2& min, const Vector2& max)
{
	matrixData.view = Matrix4::IDENTITY;
	Matrix4 proj = Matrix4::MatrixOrthoProjection(min.x, max.x, min.y, max.y, 0.f, 1.0f);
	SetProjectionMatrix(proj);
}

void SimpleRenderer::SetUIOrthoProjection(float minX, float minY, float maxX, float maxY)
{
	matrixData.view = Matrix4::IDENTITY;
	Matrix4 proj = Matrix4::MatrixOrthoProjection(minX, maxX, minY, maxY, 0.f, 1.0f);
	SetProjectionMatrix(proj);
}

void SimpleRenderer::SetOrthoProjection(const Vector2& min, const Vector2& max)
{
	Matrix4 proj = Matrix4::MatrixOrthoProjection(min.x, max.x, min.y, max.y, 0.f, 1.0f);
	SetProjectionMatrix(proj);
}

void SimpleRenderer::SetOrthoProjection(float minX, float minY, float maxX, float maxY)
{
	Matrix4 proj = Matrix4::MatrixOrthoProjection(minX, maxX, minY, maxY, 0.f, 1.0f);
	SetProjectionMatrix(proj);
}

void SimpleRenderer::SetPerspectiveProjection(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	Matrix4 projection = Matrix4::MakePerspectiveProjection(fieldOfView, aspectRatio, nearPlane, farPlane);
	SetProjectionMatrix(projection);
}

void SimpleRenderer::SetProjectionMatrix(Matrix4& proj)
{
	matrixData.model = Matrix4::IDENTITY;
	matrixData.projection = proj.CreateTransposeMatrix();
	matrixBuffer->Update(context, &matrixData);
}

void SimpleRenderer::EnableBlend(eBlendFactor src, eBlendFactor dst)
{
	if (currentBlendState != nullptr)
	{
		if (blendState.enabled == true && blendState.srcFactor == src && blendState.dstFactor == dst)
		{
			return;
		}
	}

	SAFE_DELETE(currentBlendState);

	BlendState* bs = new BlendState(device, true, src, dst);
	context->SetBlendState(bs);

	blendState.enabled = true;
	blendState.srcFactor = src;
	blendState.dstFactor = dst;

	currentBlendState = bs;
}

void SimpleRenderer::DisableBlend()
{
	if (blendState.enabled)
	{
		return;
	}

	SAFE_DELETE(currentBlendState);
	BlendState* bs = new BlendState(device, false);
	context->SetBlendState(bs);

	blendState.enabled = true;

	currentBlendState = bs;
}

void SimpleRenderer::SetAmbientLight(float intensity, const RgbaFloats& color)
{
	m_lightBuffer.ambience = RgbaFloats(color.r, color.g, color.b, intensity);
	m_lightConstant->Update(context, &m_lightBuffer);
}

void SimpleRenderer::EnablePointLight(const Vector3& pos, const RgbaFloats& color, float intensity, int index, const Vector4& attenuation, const Vector4& specAttenuation)
{
	m_lightBuffer.pointLight[index].lightColor = RgbaFloats(color.r, color.g, color.b, intensity);
	m_lightBuffer.pointLight[index].lightPosition = Vector4(pos, 1.0f);
	m_lightBuffer.pointLight[index].attenuation = attenuation;
	m_lightBuffer.pointLight[index].specAttenuation = specAttenuation;

	m_lightConstant->Update(context, &m_lightBuffer);
}

void SimpleRenderer::EnableDirectionLight(const Vector3& pos, const RgbaFloats& color, float intensity, int index, const Vector3& lightDir, const Vector4& attenuation, const Vector4& specAttenuation)
{
	m_lightBuffer.pointLight[index].lightColor = RgbaFloats(color.r, color.g, color.b, intensity);
	m_lightBuffer.pointLight[index].lightPosition = Vector4(pos, 1.0f);
	m_lightBuffer.pointLight[index].attenuation = attenuation;
	m_lightBuffer.pointLight[index].specAttenuation = specAttenuation;
	m_lightBuffer.pointLight[index].lightDirection = lightDir;

	m_lightConstant->Update(context, &m_lightBuffer);
}

void SimpleRenderer::EnableDepthTest(bool enable)
{
	if (depthStencilDesc.depthTestEnabled != enable)
	{
		depthStencilDesc.depthTestEnabled = enable;
		SAFE_DELETE(depthStencilState);
		depthStencilState = new DepthStencilState(device, depthStencilDesc);
		context->SetDepthStencilState(depthStencilState);
	}
}

void SimpleRenderer::EnableDepthWrite(bool enable)
{
	if (depthStencilDesc.depthWritingEnabled != enable)
	{
		depthStencilDesc.depthWritingEnabled = enable;
		SAFE_DELETE(depthStencilState);
		depthStencilState = new DepthStencilState(device, depthStencilDesc);
		context->SetDepthStencilState(depthStencilState);
	}
}

void SimpleRenderer::EnableDepth(bool test, bool write)
{
	PROFILE_SCOPE_FUNCTION();
	if ((depthStencilDesc.depthWritingEnabled != write)
		|| (depthStencilDesc.depthTestEnabled != test)) {

		depthStencilDesc.depthWritingEnabled = write;
		depthStencilDesc.depthTestEnabled = test;
		SAFE_DELETE(depthStencilState);
		depthStencilState = new DepthStencilState(device, depthStencilDesc);
		context->SetDepthStencilState(depthStencilState);
	}
}

void SimpleRenderer::SetSpecularConstants(float const specPower, float const specFactor)
{
	m_lightBuffer.specFactor = specFactor;
	m_lightBuffer.specPower = specPower;

	m_lightConstant->Update(context, &m_lightBuffer);
}

void SimpleRenderer::Draw(ePrimitiveType topology, VertexBuffer* vbo, unsigned int const vertexCount, unsigned int startIndex) const
{
	D3D11_PRIMITIVE_TOPOLOGY d3dPrim;

	switch (topology)
	{
	case PRIMITIVE_LINES:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;

	case PRIMITIVE_LINESTRIP:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;

	case PRIMITIVE_TRIANGLESTRIP:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;

	case PRIMITIVE_TRIANGLES:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;

	case PRIMITIVE_POINTS:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break; 

	default:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	context->SetPrimitiveTopology(d3dPrim);
	context->SetVertexBuffers(vbo);
	context->DrawNumVertex(vertexCount, startIndex);
}

void SimpleRenderer::Draw(const DrawInstruction_T& dt, std::vector<Vertex3D> vertices)
{
	DrawVertexArray(dt.m_primitive, vertices);
}

void SimpleRenderer::Draw(const std::vector<DrawInstruction_T>& drawInstructions, std::vector<Vertex3D> vertices)
{
	if (vertices.size() > 0)
	{
		for each(DrawInstruction_T dt in drawInstructions)
		{
			Draw(dt, vertices);
		}
	}
}

void SimpleRenderer::DrawVertexArray(ePrimitiveType topology, const Vertex3D* vertices, unsigned int const vertexCount, unsigned int startIndex) const
{
	tempVBO->Update(context, vertices, vertexCount);
	Draw(topology, tempVBO, vertexCount, startIndex);
}

void SimpleRenderer::DrawVertexArray(ePrimitiveType topology, const std::vector<Vertex3D> vertices) const
{
	size_t size = vertices.size();
	tempVBO->Update(context, &vertices[0], size);
	Draw(topology, tempVBO, size);
}

void SimpleRenderer::DrawQuad2D(float xMin, float yMin, float xMax, float yMax, const Rgba& color)
{
	Vertex3D quadVertices[6] = 
	{
		Vertex3D(Vector3(xMin, yMin, 0.0f), Vector2(0.0f, 1.0f), color),
		Vertex3D(Vector3(xMax, yMin, 0.0f), Vector2(1.0f, 1.0f), color),
		Vertex3D(Vector3(xMin,  yMax, 0.0f), Vector2(0.0f, 0.0f), color),

		Vertex3D(Vector3(xMax, yMin, 0.0f), Vector2(1.0f, 1.0f), color),
		Vertex3D(Vector3(xMax,  yMax, 0.0f), Vector2(1.0f, 0.0f), color),
		Vertex3D(Vector3(xMin, yMax, 0.0f), Vector2(0.0f, 0.0f), color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawQuad2D(const AABB2D& bound, const Rgba& color)
{
	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.0f), Vector2(0.0f, 1.0f), color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(1.0f, 1.0f), color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.0f), Vector2(0.0f, 0.0f), color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(1.0f, 1.0f), color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_maxs.y, 0.0f), Vector2(1.0f, 0.0f), color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.0f), Vector2(0.0f, 0.0f), color)
	};

	SetTexture2D(nullptr);

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawQuad2D(const Vector2& bottomLeft, const Vector2& topRight, const Vector2& uvBottomLeft, const Vector2& uvTopRight, const Rgba& color)
{
	Vertex3D quadVertices[6] = 
	{
		Vertex3D(Vector3(bottomLeft.x, bottomLeft.y, 0.0f), Vector2(uvBottomLeft.x, uvTopRight.y), color),
		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.0f), Vector2(uvTopRight.x, uvTopRight.y), color),
		Vertex3D(Vector3(bottomLeft.x,  topRight.y, 0.0f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color),

		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.0f), Vector2(uvTopRight.x, uvTopRight.y), color),
		Vertex3D(Vector3(topRight.x,  topRight.y, 0.0f), Vector2(uvTopRight.x, uvBottomLeft.y), color),
		Vertex3D(Vector3(bottomLeft.x, topRight.y, 0.0f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawQuad2D(const AABB2D& bound, const Vector2& uvBottomLeft, const Vector2& uvTopRight, const Rgba& color)
{
	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.0f), Vector2(uvBottomLeft.x, uvTopRight.y), color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvTopRight.x, uvTopRight.y), color),
		Vertex3D(Vector3(bound.m_mins.x,  bound.m_maxs.y, 0.0f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvTopRight.x, uvTopRight.y), color),
		Vertex3D(Vector3(bound.m_maxs.x,  bound.m_maxs.y, 0.0f), Vector2(uvTopRight.x, uvBottomLeft.y), color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.0f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawQuad2D(const AABB2D& bound, const AABB2D& uvBounds, const Rgba& color)
{
	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(bound.m_mins.x,  bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(bound.m_maxs.x,  bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_mins.y), color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawQuad2D(const Vector2& midPoint, const Vector2& widthAndHeight, const AABB2D& uvBounds, const Rgba& color)
{
	Vector2 bottomLeft = Vector2(midPoint.x - widthAndHeight.x, midPoint.y - widthAndHeight.y);
	Vector2 topRight = Vector2(midPoint.x + widthAndHeight.x, midPoint.y + widthAndHeight.y);

	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bottomLeft.x, bottomLeft.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(bottomLeft.x,  topRight.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), color),

		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), color),
		Vertex3D(Vector3(topRight.x,  topRight.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_mins.y), color),
		Vertex3D(Vector3(bottomLeft.x, topRight.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawCircle2D(const Vector2& pos, float radius, const Rgba& color, float numSides)
{
	float degreesPerSide = 360.0f / numSides;

	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	Vector3 previousVector = Vector3(pos, 0.f);

	for (float degrees = 0.f; degrees <= 360.0f; degrees += degreesPerSide)
	{
		float radians = ConvertDegreesToRadians(degrees);
		float x = pos.x + (radius * cosf(radians));
		float y = pos.y + (radius * sinf(radians));

		if (degrees != 0.f) {
			vertexes.push_back(Vertex3D(previousVector, Vector2::ZERO, color));

			vertexes.push_back(Vertex3D(Vector3(x, y, 0.f), Vector2::ZERO, color));

			vertexes.push_back(Vertex3D(Vector3(pos.x, pos.y, 0.f), Vector2::ZERO, color));

			previousVector = Vector3(x, y, 0.f);

			vertexCount += 3;
		}
		else {
			previousVector = Vector3(x, y, 0.f);
		}
	}

	DrawVertexArray(PRIMITIVE_TRIANGLES, vertexes);
}

void SimpleRenderer::DrawBorderedQuad2D(const AABB2D& bound, const AABB2D& uvBounds, float borderThickness, const Rgba& quadColor, const Rgba& borderColor)
{
	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_maxs.y), quadColor),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), quadColor),
		Vertex3D(Vector3(bound.m_mins.x,  bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), quadColor),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_maxs.y), quadColor),
		Vertex3D(Vector3(bound.m_maxs.x,  bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_maxs.x, uvBounds.m_mins.y), quadColor),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.0f), Vector2(uvBounds.m_mins.x, uvBounds.m_mins.y), quadColor)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);

	DrawLine2D(bound.m_mins, bound.GetTopLeftPoint(), borderColor, borderThickness);
	DrawLine2D(bound.m_mins, bound.GetBottomRightPoint(), borderColor, borderThickness);
	DrawLine2D(bound.m_maxs, bound.GetTopLeftPoint(), borderColor, borderThickness);
	DrawLine2D(bound.m_maxs, bound.GetBottomRightPoint(), borderColor, borderThickness);
}

void SimpleRenderer::DrawTwoSidedQuad3D(const Vector3& origin, const Rgba& color)
{
	Vector3 right = Vector3(1.0f, 0.f, 0.f);
	Vector3 up = Vector3(0.f, 1.0f, 0.f);
	DrawQuad3D(origin, right, -1.f, 1.f, up, -1.f, 1.f, color);
	DrawQuad3D(origin, -right, -1.f, 1.f, up, -1.f, 1.f, color);
}

void SimpleRenderer::DrawQuad3D(const Vector3& origin, 
	const Vector3& right, float negXBound, float posXBound,
	const Vector3& up, float negYBound, float posYBound, const Rgba& color)
{
	Vector3 bottomLeft = origin + negXBound * right + negYBound * up;
	Vector3 topLeft = origin + negXBound * right + posYBound * up;
	Vector3 bottomRight = origin + posXBound * right + negYBound * up;
	Vector3 topRight = origin + posXBound * right + posYBound * up;

	Vector3 normal = up.CrossProduct(right);
	Vector3 tangent = right;
	Vector3 bitangent = up;

	Vertex3D quadVertices[6] = 
	{
		Vertex3D(bottomLeft, Vector2(0.0f, 1.0f), color, normal, tangent, bitangent),
		Vertex3D(topRight, Vector2(1.0f, 0.0f), color, normal, tangent, bitangent),
		Vertex3D(topLeft, Vector2(0.0f, 0.0f), color, normal, tangent, bitangent),
		Vertex3D(bottomLeft, Vector2(0.0f, 1.0f), color, normal, tangent, bitangent),
		Vertex3D(bottomRight, Vector2(1.0f, 1.0f), color, normal, tangent, bitangent),
		Vertex3D(topRight, Vector2(1.0f, 0.0f), color, normal, tangent, bitangent)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);
}

void SimpleRenderer::DrawFilledQuad3D(const Vector3& center,
									  const Vector3& right, 
									  const Vector3& up, float length, const Rgba& color)
{
	float radius = length * 0.5f;
	Vector3 bottomLeft = center - radius * right - radius * up;
	Vector3 bottomRight = center - radius * right + radius * up;
	Vector3 topLeft = center + radius * right - radius * up;
	Vector3 topRight = center + radius * right + radius * up;

	Vertex3D quadVertices[12] =
	{
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(topLeft, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(bottomRight, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),

		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(bottomRight, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(topLeft, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 12);
}

void SimpleRenderer::DrawCube(const Vector3& origin, const Matrix4& transform, const Rgba& color)
{
	Vector3 right = Vector3(transform.m_iBasis);
	Vector3 up = Vector3(transform.m_jBasis);
	Vector3 forward = Vector3(transform.m_kBasis);

	DrawQuad3D((origin - forward) + transform.m_translation.XYZ(), right, -1.0f, 1.0f, up, -1.0f, 1.0f, color);
	DrawQuad3D((origin + right) + transform.m_translation.XYZ(), forward, -1.0f, 1.0f, up, -1.0f, 1.0f, color);
	DrawQuad3D((origin + forward) + transform.m_translation.XYZ(), -right, -1.0f, 1.0f, up, -1.0f, 1.0f, color);
	DrawQuad3D((origin - right) + transform.m_translation.XYZ(), -forward, -1.0f, 1.0f, up, -1.0f, 1.0f, color);
	DrawQuad3D((origin + up) + transform.m_translation.XYZ(), right, -1.0f, 1.0f, forward, -1.0f, 1.0f, color);
	DrawQuad3D((origin - up) + transform.m_translation.XYZ(), -right, -1.0f, 1.0f, forward, -1.0f, 1.0f, color);
}

void SimpleRenderer::DrawCylinder(const Vector3& position, float radius, float halfLength)
{
	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	float degreesPerSide = 360.0f / 36.f;

	for (int i = -1; i < 0; i++)
	{
		vertexes.push_back(Vertex3D(Vector3(position.x, position.y + (halfLength * i), position.z), Vector2::ZERO, Rgba::WHITE));
		vertexCount++;

		for (float degrees = 0.0f; degrees <= 360.0f; degrees += degreesPerSide)
		{
			float radians = ConvertDegreesToRadians(degrees);
			float x = position.x + (radius * -sinf(radians));
			float z = position.z + (radius * cosf(radians));

			vertexes.push_back(Vertex3D(Vector3(x, position.y + (halfLength * i), z), Vector2::ZERO, Rgba::WHITE));
			vertexCount++;
		}
	}

	DrawVertexArray(PRIMITIVE_TRIANGLESTRIP, &vertexes[0], vertexCount);
}

void SimpleRenderer::DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, const Rgba& color, float thickness)
{
	if (thickness < 0)
	{
		thickness *= -1.f;
	}

	if (thickness == 1.0f)
	{
		Vertex3D lineVertices[2] =
		{
			Vertex3D(Vector3(startPoint.x, startPoint.y, 0.f), Vector2::ZERO, color),
			Vertex3D(Vector3(endPoint.x, endPoint.y, 0.f), Vector2::ZERO, color),
		};

		SetShader(nullptr);

		DrawVertexArray(PRIMITIVE_LINES, lineVertices, 2);
	}
	else if (thickness >= 0.f)
	{
		Vector2 deltaPoint = endPoint - startPoint;

		float deltaPointLength = deltaPoint.CalcLength();

		if (deltaPointLength == 0.f)
		{
			deltaPointLength = 1.f;
		}

		float sqrtInverse = 1 / deltaPointLength;

		Vector2 startRightOrtho = (Vector2(-deltaPoint.y, deltaPoint.x) * sqrtInverse * thickness * 0.5f) + startPoint;
		Vector2 startLeftOrtho = (Vector2(-deltaPoint.y, deltaPoint.x) * sqrtInverse * thickness * -0.5f) + startPoint;
		Vector2 endRightOrtho = (Vector2(deltaPoint.y, -deltaPoint.x) * sqrtInverse * thickness * 0.5f) + endPoint;
		Vector2 endLeftOrtho = (Vector2(deltaPoint.y, -deltaPoint.x) * sqrtInverse * thickness * -0.5f) + endPoint;

		Vertex3D lineVertices[6] =
		{
			Vertex3D(Vector3(startLeftOrtho, 0.f), Vector2::ZERO, color),
			Vertex3D(Vector3(endLeftOrtho, 0.f), Vector2::ZERO, color),
			Vertex3D(Vector3(startRightOrtho, 0.f), Vector2::ZERO, color),

			Vertex3D(Vector3(startLeftOrtho, 0.f), Vector2::ZERO, color),
			Vertex3D(Vector3(endRightOrtho, 0.f), Vector2::ZERO, color),
			Vertex3D(Vector3(endLeftOrtho, 0.f), Vector2::ZERO, color)
		};

		DrawVertexArray(PRIMITIVE_TRIANGLES, lineVertices, 6);
	}
}

void SimpleRenderer::DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, const Rgba& color)
{
	Vertex3D lineVertices[2] =
	{
		Vertex3D(Vector3(startPoint.x, startPoint.y, startPoint.z), Vector2::ZERO, color),
		Vertex3D(Vector3(endPoint.x, endPoint.y, endPoint.z), Vector2::ZERO, color),
	};

	SetShader(nullptr);

	DrawVertexArray(PRIMITIVE_LINES, lineVertices, 2);
}

void SimpleRenderer::DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, const Rgba& color, const Rgba& secondColor)
{
	Vertex3D lineVertices[2] =
	{
		Vertex3D(Vector3(startPoint.x, startPoint.y, startPoint.z), Vector2::ZERO, color),
		Vertex3D(Vector3(endPoint.x, endPoint.y, endPoint.z), Vector2::ZERO, secondColor),
	};

	SetShader(nullptr);

	DrawVertexArray(PRIMITIVE_LINES, lineVertices, 2);
}

void SimpleRenderer::DrawPointLight3D(const Vector3& pos, const Rgba& color)
{
	Vector3 right = Vector3(0.2f, 0.0f, 0.0f);
	Vector3 up = Vector3(0.0f, 0.2f, 0.0f);
	Vector3 forward = Vector3(0.0f, 0.0f, 0.2f);

	Vertex3D vertices[] = 
	{
		Vertex3D(pos - right, Vector2(0.0f, 0.0f), color),
		Vertex3D(pos + right, Vector2(0.0f, 0.0f), color),
		Vertex3D(pos - up, Vector2(0.0f, 0.0f), color),
		Vertex3D(pos + up, Vector2(0.0f, 0.0f), color),
		Vertex3D(pos - forward, Vector2(0.0f, 0.0f), color),
		Vertex3D(pos + forward, Vector2(0.0f, 0.0f), color),
	};

	DrawVertexArray(PRIMITIVE_LINES, vertices, 6);
}

void SimpleRenderer::DrawBasisAxes(const Vector3& position, float length)
{
	DrawLine3D(position, position + Vector3(length, 0.f, 0.f), Rgba::RED);
	DrawLine3D(position, position + Vector3(0.f, length, 0.f), Rgba::GREEN);
	DrawLine3D(position, position + Vector3(0.f, 0.f, length), Rgba::BLUE);
}

void SimpleRenderer::DrawQuadAtDefault()
{
	Vertex3D quad_vertices[6] = 
	{
		Vertex3D(Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f)),
		Vertex3D(Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f)),
		Vertex3D(Vector3(-1.0f,  1.0f, 0.0f), Vector2(0.0f, 0.0f)),

		Vertex3D(Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f)),
		Vertex3D(Vector3(1.0f,  1.0f, 0.0f), Vector2(1.0f, 0.0f)),
		Vertex3D(Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f))
	};

	VertexBuffer* vertexBuffer = device->CreateVertexBuffer(quad_vertices, 6);

	context->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->SetVertexBuffers(vertexBuffer);
	context->DrawNumVertex(6);

	if (vertexBuffer != nullptr)
	{
		delete vertexBuffer;
		vertexBuffer = nullptr;
	}
}

void SimpleRenderer::DrawMesh(Mesh* mesh) const
{
	for each(DrawInstruction_T draw in mesh->m_drawInstructions)
	{
		if (draw.m_usesIndexBuffer) 
		{
			DrawIndexed(draw.m_primitive, mesh->m_vertices, mesh->m_indices, draw.m_vertexCount, draw.m_startIndex);
		}
		else 
		{
			Draw(draw.m_primitive, mesh->m_vertices, draw.m_vertexCount, draw.m_startIndex);
		}
	}
}

void SimpleRenderer::DrawSkeleton(const Skeleton& skeleton)
{
	std::vector<Vertex3D> vertex;

	for each(Joint* j in skeleton.m_joints)
	{
		if (j != nullptr && j->m_parentJoint != nullptr)
		{
			Vector3 jointPos = j->GetJointTranslation();
			Vector3 parentPos = j->m_parentJoint->GetJointTranslation();
			DrawBone(jointPos, parentPos, vertex);
		}
	}

	SetShader(nullptr);
	DrawVertexArray(PRIMITIVE_LINES, vertex);
}

void SimpleRenderer::DrawSkeletonInstance(const SkeletonInstance& skeletonInst)
{
	std::vector<Vertex3D> vertex;

	for (unsigned int i = 0; i < skeletonInst.m_skeleton->m_joints.size(); ++i)
	{
		Joint* j = skeletonInst.m_skeleton->m_joints[i];

		if (j != nullptr && j->m_parentJoint != nullptr)
		{
			Vector3 jointPos = skeletonInst.GetJointGlobalTransform(i).GetTranslation();
			Vector3 parentJointPos = skeletonInst.GetJointGlobalTransform(skeletonInst.GetJointParentID(i)).GetTranslation();
			DrawBone(jointPos, parentJointPos, vertex);
		}
	}

	SetShader(nullptr);
	DrawVertexArray(PRIMITIVE_LINES, vertex);
}

void SimpleRenderer::DrawBone(const Vector3& startJointPos, const Vector3& parentJointPos, std::vector<Vertex3D>& vertex)
{
	float pointPlacePercentage = .6f;
	float crossLengthFactor = 0.3f;

	Vector3 distBetweenJoints = parentJointPos - startJointPos;
	Vector3 cornerPoint = (startJointPos + (distBetweenJoints * pointPlacePercentage));

	Vector3 initialCrossProductCalc = CrossProduct(cornerPoint.CalcNormalizedVector(), Vector3::Y_AXIS);

	Vector3 verticalCrossProductCalc = CrossProduct(startJointPos - cornerPoint, initialCrossProductCalc);

	Vector3 horizontalCrossProductCalc = CrossProduct(startJointPos - cornerPoint, verticalCrossProductCalc);

	verticalCrossProductCalc.SetLength(horizontalCrossProductCalc.CalcLength());

	horizontalCrossProductCalc *= crossLengthFactor;
	verticalCrossProductCalc *= crossLengthFactor;

	Vector3 northCorner = verticalCrossProductCalc + cornerPoint;
	Vector3 southCorner = -verticalCrossProductCalc + cornerPoint;

	Vector3 eastCorner = horizontalCrossProductCalc + cornerPoint;
	Vector3 westCorner = -horizontalCrossProductCalc + cornerPoint;

	vertex.push_back(Vertex3D(startJointPos, Vector2::ZERO, Rgba::YELLOW));
	vertex.push_back(Vertex3D(eastCorner, Vector2::ZERO, Rgba::YELLOW));

	vertex.push_back(Vertex3D(eastCorner, Vector2::ZERO, Rgba::YELLOW));
	vertex.push_back(Vertex3D(parentJointPos, Vector2::ZERO, Rgba::YELLOW));

	vertex.push_back(Vertex3D(startJointPos, Vector2::ZERO, Rgba::RED));
	vertex.push_back(Vertex3D(northCorner, Vector2::ZERO, Rgba::RED));

	vertex.push_back(Vertex3D(northCorner, Vector2::ZERO, Rgba::RED));
	vertex.push_back(Vertex3D(parentJointPos, Vector2::ZERO, Rgba::RED));

	vertex.push_back(Vertex3D(startJointPos, Vector2::ZERO, Rgba::BLUE));
	vertex.push_back(Vertex3D(westCorner, Vector2::ZERO, Rgba::BLUE));

	vertex.push_back(Vertex3D(westCorner, Vector2::ZERO, Rgba::BLUE));
	vertex.push_back(Vertex3D(parentJointPos, Vector2::ZERO, Rgba::BLUE));

	vertex.push_back(Vertex3D(startJointPos, Vector2::ZERO, Rgba::GREEN));
	vertex.push_back(Vertex3D(southCorner, Vector2::ZERO, Rgba::GREEN));

	vertex.push_back(Vertex3D(southCorner, Vector2::ZERO, Rgba::GREEN));
	vertex.push_back(Vertex3D(parentJointPos, Vector2::ZERO, Rgba::GREEN));
}

void SimpleRenderer::DrawFullScreenTransparentQuad(float transparencyPercentage, const Rgba& color)
{
	Rgba transparentColor = color;
	transparentColor.a = (unsigned char)(ClampFloat(transparencyPercentage, 0.f, 1.f) * 255);
	DrawQuad2D(AABB2D(0.f, 0.f, (float)m_width, (float)m_height), AABB2D::ZERO_TO_ONE, transparentColor);
}

bool SimpleRenderer::IsWindowRunning() const
{
	return output->window->IsOpen();
}

void SimpleRenderer::TurnOffWindow()
{
	output->window->SetIsRunning(false);
}

void SimpleRenderer::CloseWindow()
{
	output->window->Close();
}

Window* SimpleRenderer::GetWindow() const
{
	return output->window;
}

void SimpleRenderer::DrawIndexed(ePrimitiveType topology, VertexBuffer* vbo, IndexBuffer* ibo, unsigned int const vertexCount, unsigned int startIndex) const
{
	ASSERT_OR_DIE(nullptr, "Why are you using DrawIndexed() from SimpleRenderer? It's not implemented yet");
	UNUSED(topology);
	UNUSED(vbo);
	UNUSED(ibo);
	UNUSED(vertexCount);
	UNUSED(startIndex);
}

void SimpleRenderer::UpdateTimeConstant(float deltaSeconds, float zoom)
{
	m_timeBuffer.time += deltaSeconds;
	m_timeBuffer.zoom = zoom;
	m_timeConstant->Update(context, &m_timeBuffer);
}

//////////////////////////////////////////////////////////////////////////
RHITexture2D* SimpleRenderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	RHITexture2D* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
}

RHITexture2D* SimpleRenderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)s_alreadyLoadedTextures.size(); ++textureIndex)
	{
		RHITexture2D* texture = s_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}

	return nullptr;
}

RHITexture2D* SimpleRenderer::CreateTextureFromFile(const std::string& imageFilePath)
{
	// Load image data
	RHITexture2D* texture = new RHITexture2D(device, imageFilePath, D3D11_USAGE_DEFAULT);

	GUARANTEE_OR_DIE(texture != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));

	s_alreadyLoadedTextures.push_back(texture);
	return texture;
}

void SimpleRenderer::DrawTexturedAABB2D(const AABB2D& bounds, RHITexture2D* texture, const Rgba& tints)
{
	Vertex3D quadVertices[6] =
	{
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.0f), Vector2(0.f, 1.f), tints),
		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.0f), Vector2(1.f, 1.f), tints),
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.0f), Vector2(0.f, 0.f), tints),
							  				   																	 
		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.0f), Vector2(1.f, 1.f), tints),
		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.0f), Vector2(1.f, 0.f), tints),
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.0f), Vector2(0.f, 0.f), tints)
	};

	SetTexture2D(texture);

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 6);

	SetTexture2D(nullptr);
}

void SimpleRenderer::DrawTexturedAABB2DSprite(const AABB2D& bounds, const AABB2D& spriteBounds, const RHITexture2D* texture, bool isFlipped, const Rgba& tints)
{
	AABB2D UV = spriteBounds;

	if (isFlipped)
	{
		UV.FlipAlongY();
	}

	Vertex3D vertexes[6] =
	{
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2(UV.m_mins.x, UV.m_maxs.y), tints),
		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(UV.m_maxs.x, UV.m_maxs.y), tints),
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(UV.m_mins.x, UV.m_mins.y), tints),

		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(UV.m_maxs.x, UV.m_maxs.y), tints),
		Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2(UV.m_maxs.x, UV.m_mins.y), tints),
		Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(UV.m_mins.x, UV.m_mins.y), tints)
	};

	SetTexture2D(texture);

	DrawVertexArray(PRIMITIVE_TRIANGLES, vertexes, 6);

	SetTexture2D(nullptr);
}

void SimpleRenderer::SetFontTexture(const std::string& fontTextureDir)
{
	if (mainFont != nullptr)
	{
		mainFont->SetFontTexture(device, FONT_IMG_DIR.data());
	}
	else
	{
		mainFont->SetFontTexture(device, fontTextureDir.data());
	}
}

void SimpleRenderer::SetFont(const std::string& fontName, const std::string& fontDir)
{
	if (mainFont != nullptr)
	{
		delete mainFont;
		mainFont = nullptr;
	}

	mainFont = new Font(fontName, fontDir);
}

void SimpleRenderer::DrawText2D(const Vector2& position, std::string text, float fontSize, const Rgba& color, TextAlignment alignment, Font* font)
{
	DrawText2D(position.x, position.y, text, fontSize, color, alignment, font);
}

void SimpleRenderer::DrawText2D(float sx, float sy, std::string text, float fontSize, const Rgba& color, TextAlignment alignment, Font* font)
{
	if (font == nullptr)
	{
		if (mainFont->glyphTextures == nullptr)
		{
			SetFontTexture();
		}
		font = mainFont;
	}

	float centerOffset = 0.f;

	if (font != nullptr)
	{
		switch (alignment)
		{
		case CENTER_ALIGNED:
			centerOffset = (font->CalculateTextWidth(text) * 0.5f * fontSize);
			sx -= centerOffset;
			sy -= font->GetGlyph(text.data()[0])->height;
			break;
		};

		float cursor = sx;

		const char* c = text.c_str();

		int prevCharCode = -1;

		SetTexture2D(0, font->glyphTextures);

		while (*c != NULL)
		{	
			const GlyphInfo_T* glyph = mainFont->GetGlyph(*c);

			if (glyph == nullptr)
			{
				glyph = mainFont->GetInvalidGlyph();
			}

			if (*c == '\n')
			{
				sy -= font->lineHeight * fontSize;
				cursor = sx;
			}

			float glyphBase = mainFont->base * fontSize;
			float glyphWidth = glyph->width * fontSize;
			float glyphHeight = glyph->height * fontSize;
			float glyphXOffSet = glyph->xoffset * fontSize;
			float glyphYOffSet = glyph->yoffset * fontSize;
			float glyphXAdvance = glyph->xadvance * fontSize;

			Vector2 topLeft;
			Vector2 bottomLeft;
			Vector2 topRight;

			topLeft = Vector2(cursor + glyphXOffSet, sy + (glyphBase - glyphYOffSet));

			topRight = Vector2(topLeft.x + glyphWidth, topLeft.y);
			bottomLeft = Vector2(topLeft.x, topRight.y - glyphHeight);
			
			Vector2 uvBottomLeft = Vector2((glyph->x / (float)mainFont->scaleW), (glyph->y / (float)mainFont->scaleH));
			Vector2 uvTopRight = uvBottomLeft + Vector2((glyph->width / (float)mainFont->scaleW), (glyph->height / (float)mainFont->scaleH));

			DrawQuad2D(bottomLeft, topRight, uvBottomLeft, uvTopRight, color);

			cursor += glyphXAdvance;
			prevCharCode = *c;
			++c;

			int kerning = (int)(mainFont->GetKerning(prevCharCode, *c) * fontSize);
			cursor += (kerning);
		}
	}

	SetTexture2D(nullptr);
}

void SimpleRenderer::DrawText2DInAABB(const AABB2D& aabb, std::string text, float fontSize, const Rgba& color, TextAlignment alignment, float xPadding, float yPadding, Font* font)
{
	Vector2 center = aabb.CalcCenter();
	
	switch (alignment)
	{
	case CENTER_ALIGNED:
		DrawText2D(center.x + xPadding, center.y + yPadding, text, fontSize, color, alignment, font);
		break;

	case LEFT_ALIGNED:
		DrawText2D(aabb.m_mins.x + xPadding, aabb.m_maxs.y + yPadding, text, fontSize, color, alignment, font);
		break;
	}
	
}

void SimpleRenderer::DrawDebugLine3D(const Vector3& pointA, const Vector3& pointB, const Rgba& color)
{
	Vertex3D vertices[] =
	{
		Vertex3D(pointA, Vector2::ZERO, color),
		Vertex3D(pointB, Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_LINES, vertices, 2);
}

void SimpleRenderer::DrawDebugLine2D(const Vector2& pointA, const Vector2& pointB, const Rgba& color)
{
	Vertex3D vertices[] =
	{
		Vertex3D(Vector3(pointA, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(pointB, 0.f), Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_LINES, vertices, 2);
}

void SimpleRenderer::DrawDebugPoint2D(const Vector2& point, const Rgba& color)
{
	Vertex3D vertices[] =
	{
		Vertex3D(Vector3(point), Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_POINTS, vertices, 1);
}

void SimpleRenderer::DrawDebugPoint3D(const Vector3& point, const Rgba& color)
{
	Vertex3D vertices[] =
	{
		Vertex3D(point, Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_POINTS, vertices, 1);
}

void SimpleRenderer::DrawDebugQuad3D(const Vector3& bottomLeft, const Vector3& topRight, const Rgba& color)
{
	Vertex3D quadVertices[] =
	{
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(Vector3(topRight.x, bottomLeft.y, bottomLeft.z), Vector2::ZERO, color),

		Vertex3D(Vector3(topRight.x, bottomLeft.y, bottomLeft.z), Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),

		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(Vector3(bottomLeft.x, topRight.y, topRight.z), Vector2::ZERO, color),

		Vertex3D(Vector3(bottomLeft.x, topRight.y, topRight.z), Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_LINES, quadVertices, 8);
}

void SimpleRenderer::DrawSolidDebugQuad3D(const Vector3& center, float radius, const Rgba& color)
{
	Vector3 bottomLeft = Vector3(center.x - radius, center.y - radius, center.z);
	Vector3 bottomRight = Vector3(center.x + radius, center.y - radius, center.z);
	Vector3 topLeft = Vector3(center.x - radius, center.y + radius, center.z);
	Vector3 topRight = Vector3(center.x + radius, center.y + radius, center.z);

	Vertex3D quadVertices[12] =
	{
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(topLeft, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(bottomRight, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),

		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(bottomRight, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color),
		Vertex3D(topLeft, Vector2::ZERO, color),
		Vertex3D(topRight, Vector2::ZERO, color),
		Vertex3D(bottomLeft, Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, quadVertices, 12);
}

void SimpleRenderer::DrawDebugQuad2D(const Vector2& bottomLeft, const Vector2& topRight, const Rgba& color)
{
	Vertex3D quadVertices[] =
	{
		Vertex3D(Vector3(bottomLeft.x, bottomLeft.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(topRight.x, topRight.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(topRight.x, topRight.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bottomLeft.x, topRight.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bottomLeft.x, topRight.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bottomLeft.x, bottomLeft.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bottomLeft.x, bottomLeft.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(topRight.x, topRight.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(topRight.x, bottomLeft.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bottomLeft.x, topRight.y, 0.f), Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_LINES, quadVertices, 12);
}

void SimpleRenderer::DrawDebugQuad2D(const AABB2D& bound, const Rgba& color)
{
	Vertex3D quadVertices[] =
	{
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bound.m_mins.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_maxs.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(bound.m_maxs.x, bound.m_mins.y, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(bound.m_mins.x, bound.m_maxs.y, 0.f), Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_LINES, quadVertices, 12);
}

void SimpleRenderer::DrawDebugCircle2D(float centerX, float centerY, float radius, const Rgba& color, float numSides)
{
	DrawRegularPolygonOutline(centerX, centerY, numSides, radius, color);
}

void SimpleRenderer::DrawDebugCircle2D(const Vector2& center, float radius, const Rgba& color, float numSides)
{
	DrawRegularPolygonOutline(center.x, center.y, numSides, radius, color);
}

void SimpleRenderer::DrawRegularPolygonOutline(float centerX, float centerY, float numSides, float radius, const Rgba& color, float angleOffset)
{
	float degreesPerSide = 360.0f / numSides;

	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	for (float degrees = 0.0f; degrees <= 360.0f; degrees += degreesPerSide)
	{
		float radians = ConvertDegreesToRadians(degrees) + angleOffset;
		float x = centerX + (radius * cosf(radians));
		float y = centerY + (radius * sinf(radians));

		vertexes.push_back(Vertex3D(Vector3(x, y, 0.f), Vector2::ZERO, color));
		vertexCount++;
	}

	DrawVertexArray(PRIMITIVE_LINESTRIP, vertexes);
}

void SimpleRenderer::DrawArrow2D(const Vector2& startPoint, const Vector2& endPoint, float width, const Rgba& color)
{
	float segmentedWidth = width * 0.333333333333f;
	Vector2 direction = startPoint - endPoint;
	float arrowHalfLength = direction.CalcLength() * 0.5f;
	direction.Normalize();

	Vector2 perpendicular = Vector2(direction.y, -direction.x);
	Vector2 leftBottomPoint = endPoint + (-perpendicular * segmentedWidth);
	Vector2 rightBottomPoint = endPoint + (perpendicular * segmentedWidth);

	Vector2 midPoint = endPoint + (direction * arrowHalfLength);
	Vector2 leftCrevicePoint = midPoint + (-perpendicular * segmentedWidth);
	Vector2 leftWingPoint = midPoint + (-perpendicular * 2 * segmentedWidth);
	Vector2 rightCrevicePoint = midPoint + (perpendicular * segmentedWidth);
	Vector2 rightWingPoint = midPoint + (perpendicular * 2 * segmentedWidth);
	
	Vertex3D arrowVertices[] = 
	{
		Vertex3D(Vector3(startPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(leftWingPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(leftCrevicePoint, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(startPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(leftCrevicePoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(leftBottomPoint, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(startPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(leftBottomPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(rightBottomPoint, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(startPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(rightBottomPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(rightCrevicePoint, 0.f), Vector2::ZERO, color),

		Vertex3D(Vector3(startPoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(rightCrevicePoint, 0.f), Vector2::ZERO, color),
		Vertex3D(Vector3(rightWingPoint, 0.f), Vector2::ZERO, color)
	};

	DrawVertexArray(PRIMITIVE_TRIANGLES, arrowVertices, 15);
}

IntVector2 SimpleRenderer::GetResolution() const
{
	return output->window->m_resolution;
}

void SimpleRenderer::Translate3D(const Vector3& position)
{
	m_translationMatrix = Matrix4::CreateTranslation(-position);
	matrixData.eyePosition = Vector4(position, 0.f);
}

void SimpleRenderer::Scale3D(float uniformScale)
{
	m_scaleMatrix = Matrix4::CreateScale(Vector3(uniformScale, uniformScale, uniformScale));
}

void SimpleRenderer::Rotate3D(float yawDegrees, float pitchDegrees, float rollDegrees)
{
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutZ(rollDegrees));
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutX(pitchDegrees));
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutY(yawDegrees));
}

void SimpleRenderer::RotatePitch(float pitch)
{
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutX(pitch));
}

void SimpleRenderer::RotateYaw(float yaw)
{
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutY(yaw));
}

void SimpleRenderer::RotateRoll(float roll)
{
	m_rotationMatrix.ConcatenateTransform(Matrix4::CreateRotationDegreesAboutZ(roll));
}

void SimpleRenderer::UpdateViewMatrix(const Matrix4& viewMatrix)
{
	matrixData.view = viewMatrix;
}

Matrix4 SimpleRenderer::GetRotationMatrix() const
{
	return m_rotationMatrix;
}