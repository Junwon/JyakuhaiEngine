#pragma once

#include "Engine/Renderer/RHI/RHI.hpp"
#include "Engine/Renderer/RHI/RHITypes.hpp"
#include "Engine/Renderer/RHI/BlendState.hpp"
#include "Engine/Renderer/Font.hpp"

#include "Engine/Renderer/RHI/DepthStencilState.hpp"

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/SkeletonInstance.hpp"

class ShaderProgram;
class ComputeShader;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class RasterState;
class ConstantBuffer;
class StructuredBuffer;
class Font;
class Mesh;
class AABB2D;
class ComputeShaderJob;

#define MATRIX_BUFFER_INDEX (0)
#define TIME_BUFFER_INDEX (1)
#define LIGHT_BUFFER_INDEX (2)
#define STRUCTURED_BUFFER_INDEX (3)

struct TimeConstant_T
{
	float time;
	float zoom;
	Vector2 padding;
};

struct MatrixBuffer_T
{
	MatrixBuffer_T() :
		eyePosition(Vector4::ZERO)
	{}
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;
	Vector4 eyePosition;
};

struct BlendState_T
{
	bool enabled;
	eBlendFactor srcFactor;
	eBlendFactor dstFactor;
};

struct PointLight_T
{
	PointLight_T() :
		lightColor(1.f, 1.f, 1.f, 0.f),
		lightPosition(0.f, 0.f, 0.f, 1.f),
		attenuation(1.f, 0.f, 0.f, 0.f),
		specAttenuation(1.f, 0.f, 0.f, 0.f),
		lightDirection(Vector3::ZERO)
	{}
	RgbaFloats lightColor;
	Vector4 lightPosition;
	Vector4 attenuation;
	Vector4 specAttenuation;
	Vector3 lightDirection;
	float filler;
};

struct LightBuffer_T
{
	LightBuffer_T(): 
		ambience(1.f, 1.f, 1.f, 1.f),
		specFactor(0.f),
		specPower(1.0)
	{}
	RgbaFloats ambience;
	PointLight_T pointLight[10];
	float specFactor;
	float specPower;
	Vector2 unusedPadding;
};

const std::string FONT_NAME = "Calibri Light"; // "Calibri Light" ,"Arial Black"
const std::string FONT_DIR = "Data/Fonts/calibri_light32.fnt"; // "Data/Fonts/calibri_light32.fnt" "Data/Fonts/arial.fnt"
const std::string FONT_IMG_DIR = "Data/Fonts/calibri_light32.png"; // "Data/Fonts/calibri_light32.png" "Data/Fonts/arial.png"

class SimpleRenderer
{
public:
	SimpleRenderer();
	~SimpleRenderer();

	static SimpleRenderer* GetInstance();
	// [A02] However you want this to initialize, is up to you
	// For example, all of the following are perfectly good ways to expect
	// this to be used;
	// void setup( RHIOutput *output );
	void Setup(unsigned int width, unsigned int height);
	// void setup( Window *window );
	// void setup( Texture2D *default_render_target );

	void Destroy();

	void SetRenderTarget(RHITexture2D *color_target, RHITexture2D* depthTarget = nullptr);

	void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	void ProcessMessages();

	void SetViewportAsPercent(float x, float y, float w, float h);

	void ClearScreen(float red, float green, float blue, float alpha = 1.0f);
	void ClearScreen(const Rgba& color);
	void ClearColor(RgbaFloats const &color);
	void ClearColorTarget(RHITexture2D *target, RgbaFloats const &color);

	void ClearDepth(float depth = 1.0f, UINT8 stencil = 0);
	void Present();

	void SetShader(ShaderProgram* shader);

	void SetColorTarget(RHITexture2D* color, RHITexture2D* target);
	void SetTexture2D(unsigned int textureIndex, const RHITexture2D* texture);
	inline void SetTexture2D(const RHITexture2D* texture) { SetTexture2D(0, (texture == nullptr) ? m_whiteTexture : texture); }
	inline void SetDiffuse(RHITexture2D *texture) { SetTexture2D(0, (texture == nullptr) ? m_whiteTexture : texture); }
	inline void SetNormal(RHITexture2D *texture) { SetTexture2D(1, (texture == nullptr) ? m_flatNormalTexture : texture); }
	inline void SetSpec(RHITexture2D *texture) { SetTexture2D(2, (texture == nullptr) ? m_whiteTexture : texture); }

	void SetSampler(unsigned int sampler_index, Sampler *sampler);
	inline void SetSampler(Sampler* sampler) { SetSampler(0, sampler); }

	void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer);

	void SetStructuredBuffer(unsigned int structBufferIndex, StructuredBuffer* buffer);
	void SetStructuredBuffer(const void* buffer, unsigned int structureSize, unsigned int structureCount);

	void SetComputeShader(ComputeShader* cs = nullptr);

	void DispatchComputeShaderJob(ComputeShaderJob* job);

	void EnableDepthTest(bool enable);
	void EnableDepthWrite(bool enable);
	void EnableDepth(bool test = true, bool write = true);

	void SetProjectionMatrix(Matrix4& proj);
	void SetOrthoProjection(const Vector2& min, const Vector2& max);
	void SetOrthoProjection(float minX, float minY, float maxX, float maxY);
	void SetPerspectiveProjection(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	void SetUIOrthoProjection(const Vector2& min, const Vector2& max);
	void SetUIOrthoProjection(float minX, float minY, float maxX, float maxY);

	void EnableBlend(eBlendFactor src, eBlendFactor dst);
	void DisableBlend();

	void SetAmbientLight(float intensity, const RgbaFloats& color = RgbaFloats::WHITE);
	void EnablePointLight(const Vector3& pos, const RgbaFloats& color, float intensity = 1.0f, int index = 0, const Vector4& attenuation = Vector4(0.f, 0.f, 1.f, 0.f), const Vector4& specAttenuation = Vector4(0.f, 0.f, 1.f, 0.f));
	void EnableDirectionLight(const Vector3& pos, const RgbaFloats& color, float intensity = 1.0f, int index = 0, const Vector3& lightDir = Vector3::ZERO, const Vector4& attenuation = Vector4(0.f, 0.f, 1.f, 0.f), const Vector4& specAttenuation = Vector4(0.f, 0.f, 1.f, 0.f));
	inline void DisablePointLight() { EnablePointLight(Vector3::ZERO, RgbaFloats::WHITE, 0.f, 0, Vector4(1.0f, 0.f, 0.f, 0.f)); }
	void SetSpecularConstants(float const specPower, float const specFactor = 1.0f);

	void Draw(ePrimitiveType topology, VertexBuffer* vbo, unsigned int const vertexCount, unsigned int startIndex = 0) const;
	void Draw(const std::vector<DrawInstruction_T>& drawInstructions, std::vector<Vertex3D> vertices);
	void Draw(const DrawInstruction_T& dt, std::vector<Vertex3D> vertices);
	void DrawVertexArray(ePrimitiveType topology, const Vertex3D* vertices, unsigned int const vertexCount, unsigned int startIndex = 0) const;
	void DrawVertexArray(ePrimitiveType topology, const std::vector<Vertex3D> vertices) const;
	void DrawQuad2D(float xMin, float yMin, float xMax, float yMax, const Rgba& color = Rgba::WHITE);
	void DrawQuad2D(const Vector2& bottomLeft, const Vector2& topRight, const Vector2& uvBottomLeft, const Vector2& uvTopRight, const Rgba& color = Rgba::WHITE);
	void DrawQuad2D(const AABB2D& bound, const Vector2& uvBottomLeft, const Vector2& uvTopRight, const Rgba& color = Rgba::WHITE);
	void DrawQuad2D(const AABB2D& bound, const AABB2D& uvBounds, const Rgba& color = Rgba::WHITE);
	void DrawQuad2D(const Vector2& midPoint, const Vector2& widthAndHeight, const AABB2D& uvBounds, const Rgba& color = Rgba::WHITE);
	void DrawQuad2D(const AABB2D& bound, const Rgba& color = Rgba::WHITE);
	void DrawCircle2D(const Vector2& pos, float radius, const Rgba& color = Rgba::WHITE, float numSides = 36.f);
	void DrawBorderedQuad2D(const AABB2D& bound, const AABB2D& uvBounds, float borderThickness = 1.f, const Rgba& quadColor = Rgba::BLACK, const Rgba& borderColor = Rgba::WHITE);
	void DrawTwoSidedQuad3D(const Vector3& origin, const Rgba& color = Rgba::WHITE);
	void DrawQuad3D(const Vector3& origin, const Vector3& right, float negXBound, float posXBound, const Vector3& up, float negYBound, float posYBound, const Rgba& color = Rgba::WHITE);
	void DrawFilledQuad3D(const Vector3& center, const Vector3& right, const Vector3& up, float length = 1.f, const Rgba& color = Rgba::WHITE);
	void DrawCube(const Vector3& origin, const Matrix4& transform = Matrix4::IDENTITY, const Rgba& color = Rgba::WHITE);
	void DrawCylinder(const Vector3& position, float radius, float halfLength);
	void DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, const Rgba& color = Rgba::WHITE, float thickness = 1.f);
	void DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, const Rgba& color = Rgba::WHITE);
	void DrawLine3D(const Vector3& startPoint, const Vector3& endPoint, const Rgba& color, const Rgba& secondColor);
	void DrawPointLight3D(const Vector3& pos, const Rgba& color = Rgba::WHITE);
	void DrawBasisAxes(const Vector3& position, float length);
	void DrawQuadAtDefault();
	void DrawMesh(Mesh* mesh) const;
	void DrawSkeleton(const Skeleton& skeleton);
	void DrawSkeletonInstance(const SkeletonInstance& skeletonInst);
	void DrawIndexed(ePrimitiveType topology, VertexBuffer* vbo, IndexBuffer* ibo, unsigned int const vertexCount, unsigned int startIndex = 0) const;
	void SetFontTexture(const std::string& fontTextureDir = "");
	void SetFont(const std::string& fontName, const std::string& fontDir);
	void DrawTexturedAABB2D(const AABB2D& bounds, RHITexture2D* texture, const Rgba& tints = Rgba::WHITE);
	void DrawTexturedAABB2DSprite(const AABB2D& bounds, const AABB2D& spriteBounds, const RHITexture2D* texture, bool isFlipped = false, const Rgba& tints = Rgba::WHITE);
	void DrawText2D(float sx, float sy, std::string text, float fontSize = 1.0f, const Rgba& color = Rgba::WHITE, TextAlignment alignment = LEFT_ALIGNED, Font* font = nullptr);
	void DrawText2D(const Vector2& position, std::string text, float fontSize = 1.0f, const Rgba& color = Rgba::WHITE, TextAlignment alignment = LEFT_ALIGNED, Font* font = nullptr);
	void DrawText2DInAABB(const AABB2D& aabb, std::string text, float fontSize, const Rgba& color, TextAlignment alignment = CENTER_ALIGNED, float xPadding = 0.f, float yPadding = 0.f, Font* font = nullptr);
	void DrawDebugLine3D(const Vector3& pointA, const Vector3& pointB, const Rgba& color = Rgba::WHITE);
	void DrawDebugLine2D(const Vector2& pointA, const Vector2& pointB, const Rgba& color = Rgba::WHITE);
	void DrawDebugPoint2D(const Vector2& point, const Rgba& color = Rgba::WHITE);
	void DrawDebugPoint3D(const Vector3& point, const Rgba& color = Rgba::WHITE);
	void DrawDebugQuad3D(const Vector3& bottomLeft, const Vector3& topRight, const Rgba& color = Rgba::WHITE);
	void DrawSolidDebugQuad3D(const Vector3& center, float radius, const Rgba& color = Rgba::WHITE);
	void DrawDebugQuad2D(const Vector2& bottomLeft, const Vector2& topRight, const Rgba& color = Rgba::WHITE);
	void DrawDebugQuad2D(const AABB2D& bound, const Rgba& color = Rgba::WHITE);
	void DrawDebugCircle2D(float centerX, float centerY, float radius = 1.f, const Rgba& color = Rgba::WHITE, float numSides = 36.f);
	void DrawDebugCircle2D(const Vector2& center, float radius = 1.f, const Rgba& color = Rgba::WHITE, float numSides = 36.f);
	void DrawRegularPolygonOutline(float centerX, float centerY, float numSides, float radius, const Rgba& color = Rgba::WHITE, float angleOffset = 0.f);
	void DrawArrow2D(const Vector2& startPoint, const Vector2& endPoint, float width = 20.f, const Rgba& color = Rgba::WHITE);
	void DrawFullScreenTransparentQuad(float transparencyPercentage = 0.5f, const Rgba& color = Rgba::BLACK);
	IntVector2 GetResolution() const;
	void UpdateTimeConstant(float deltaSeconds, float zoom);
	

	RHITexture2D* CreateOrGetTexture(const std::string& imageFilePath);
	RHITexture2D* GetTexture(const std::string& imageFilePath);
	RHITexture2D* CreateTextureFromFile(const std::string& imageFilePath);

	void Translate3D(const Vector3& position);
	void Scale3D(float uniformScale);
	void Rotate3D(float yawDegrees, float pitchDegrees, float rollDegrees);
	void RotatePitch(float pitch);
	void RotateYaw(float yaw);
	void RotateRoll(float roll);
	void UpdateViewMatrix(const Matrix4& viewMatrix);
	Matrix4 GetRotationMatrix() const;

	bool IsWindowRunning() const;
	void TurnOffWindow();
	void CloseWindow();

	Window* GetWindow() const;

public:
	unsigned int m_width;
	unsigned int m_height;

	// RHI needed stuff
	RHIDeviceContext* context;
	RHIDevice* device;
	RHIOutput* output;

	ShaderProgram* m_defaultShader;

	// Whatever else you need to make this work.
	RHITexture2D* m_defaultTarget;
	RHITexture2D* m_currentTarget;
	RHITexture2D* m_defaultDepthStencil;
	RHITexture2D* m_currentDepthStencil;
	RHITexture2D* m_whiteTexture;
	RHITexture2D* m_flatNormalTexture;

	Sampler* m_pointSampler;
	Sampler* m_linearSampler;

	RasterState* m_defaultRasterState;

	TimeConstant_T m_timeBuffer;
	ConstantBuffer* m_timeConstant;

	LightBuffer_T m_lightBuffer;
	ConstantBuffer* m_lightConstant;

	MatrixBuffer_T matrixData;
	ConstantBuffer* matrixBuffer;

	StructuredBuffer* m_boneStructure;

	BlendState_T blendState;
	BlendState* currentBlendState;

	DepthStencilState_T depthStencilDesc;
	DepthStencilState* depthStencilState;

	Font* mainFont;

	Matrix4 m_translationMatrix;
	Matrix4 m_scaleMatrix;
	Matrix4 m_rotationMatrix;

private:
	static std::vector< RHITexture2D* >	s_alreadyLoadedTextures;
	VertexBuffer*					tempVBO;
	void DrawBone(const Vector3& startJointPos, const Vector3& parentJointPos, std::vector<Vertex3D>& vertex);

	static SimpleRenderer* s_instance;
};