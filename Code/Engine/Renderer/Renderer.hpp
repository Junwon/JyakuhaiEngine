#pragma once
#include <vector>
#include <map>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/AABB3D.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/OBB2D.hpp"
#include "Engine/Math/LineSegment2D.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"

#include "Engine/Core/Rgba.hpp"

class Renderer
{
public:
	Renderer();

	void						BeginFrame();
	void						EndFrame();

	void						ClearScreen(float red, float green, float blue, float alpha = 1.0f);
	void						ClearScreen(const Rgba& clearColor);
	void						ClearDepth();

	void						SetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	void						SetOrthoProjection(float left, float right, float bottom, float top);

	void						EnableBackFaceCulling(bool enableBackCulling);
	void						EnableDepthTesting(bool enableDepthTesting);
	void						EnableAlphaTesting(bool enableAlphaTesting);

	void						RotateAroundPosition2D(const Vector2& position, float degree);

	//
	// DRAW FUNCTIONS

	void						DrawAxes(float axisLength);

	BitmapFont*					CreateOrGetFont(const std::string& bitmapFontName);
	void						DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint = Rgba::WHITE, float cellAspectScale = 1.f, const BitmapFont* font = nullptr);

	void						DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& startColor = Rgba::WHITE, const Rgba& endColor = Rgba::WHITE, float lineThickness = 1.0f);
	void						DrawLine2D(const LineSegment2D& line, const Rgba& startColor = Rgba::WHITE, const Rgba& endColor = Rgba::WHITE, float lineThickness = 1.0f);
	void						DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, const Rgba& startColor, const Rgba& endColor = Rgba::WHITE, float lineThickness = 1.0f);

	void						DrawLine3D(const Vector3& start, const Vector3& end, const Rgba& color = Rgba::WHITE, float lineWidth = 1.0f);
	void						DrawLines3D(const Vertex3D* vertexArray, int numVertexes, float lineWidth = 1.0f);
	void						DrawLines3D(const std::vector<Vertex3D>& vertices, float lineWidth = 1.0f);

	void						DrawRegularPolygonOutline(float centerX, float centerY, float numSides, float radius, const Rgba& color = Rgba::WHITE, float angleOffset = 0.0f);
	void						DrawRegularPolygonOutline(Vector2 centerVector, float numSides, float radius, const Rgba& color = Rgba::WHITE, float angleOffset = 0.0f);
	void						DrawShapeWithVertices(const Vector2 vertices[], int numOfVertices, Rgba color);
	void						DrawFilledShapeWithStartingPoint(const Vector2& center, const Vector2 vertices[], int numOfVertices, const Rgba& startColor = Rgba::WHITE, const Rgba& endColor = Rgba::WHITE);

	void						DrawAABB2D(const AABB2D& bounds, const Rgba& color = Rgba::WHITE);
	void						DrawAABB2DOutline(const AABB2D& bounds, const Rgba& color, float lineWidth = 1.0f);
/*	void						DrawOBB2D(const OBB2D& bounds, const Rgba& color);*/
	void						DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color = Rgba::WHITE, float numSides = 36.0f);
	void						DrawCircle2D(const Vector2& position, float radius, const Rgba& color, float numSides = 36.f);
	void						DrawDisc2D(const Vector2& center, float radius, const Rgba& color = Rgba::WHITE, float numSides = 36.f);
	void						DrawDisc2D(const Disc2D& disc, const Rgba& color = Rgba::WHITE, float numSides = 36.f);
	void						DrawFilledShapeWithVertices(const Vector2 vertices[], int numOfVertices, const Rgba& color = Rgba::WHITE);

	void						DrawPoint3D(const Vector3& pos, const Rgba& color = Rgba::WHITE, float size = 1.0f);
	void						DrawAABB3DOutline(const AABB3D& bounds, const Rgba& color, float lineWidth);
/*	void						DrawAABB3D(const AABB3D& bounds, const Rgba& color = Rgba::WHITE, float lineWidth = 1.0f);*/
	void						DrawQuads3D(const Vertex3D* vertexArray, int numVertexes);
	void						DrawQuads3DWithVBO(unsigned int vboID, int numVertexesInVBO);
	void						DrawVBO3D(unsigned int vboID, int numVerts, unsigned int primitiveType);
	void						DrawVBO3D(unsigned int vboID, int numVerts);
	void						InitializeVBO(unsigned int &vboID, std::vector<Vertex3D> vertexArray, int &numVertexesInVBO);
	void						BindBuffer();

	Texture*					CreateOrGetTexture(const std::string& imageFilePath);
	void						BindTexture(const Texture* texture);
	void						DrawTexturedAABB2D(const AABB2D& bounds, const Texture& texture, const Rgba& tints = Rgba::WHITE);
	void						DrawTexturedAABB2D(const AABB2D& bounds, const AABB2D& textureBounds, const Texture& texture, const Rgba& tints = Rgba::WHITE);
	void						DrawTexturedAABB2DSprite(const AABB2D& bounds, const AABB2D& spriteBounds, const Texture& texture, const Rgba& tints = Rgba::WHITE);
	void						DrawTexturedAABB3D(const AABB3D& bounds, const Texture& texture, const Rgba& tints = Rgba::WHITE);

	//
	// MATRIX FUNCTIONS
	void						LoadMatrix(const Matrix4& matrixToLoad);
	void						PushMatrix();
	void						PopMatrix();
	void						Translate2D(float x, float y);
	void						Translate2D(const Vector2& position);
	void						Rotate2D(float anglesDegree);
	void						Scale2D(float x, float y);
	void						Scale2D(const Vector2& scalesPerAxis);
	void						Translate3D(const Vector3& position);
	void						Translate3D(float x, float y, float z);
	void						Rotate3D(float anglesDegree, const Vector3& axis);
	void						Scale3D(float x, float y, float z);
	void						Scale3D(const Vector3& scalesPerAxis);

	void LookAt3D(const Vector3& eye, const Vector3& center, const Vector3& up);
private:
	Texture*					CreateTextureFromFile(const std::string& imageFilePath);
	Texture*					GetTexture(const std::string& imageFilePath);

	BitmapFont*					CreateFontFromFile(const std::string& bitmapFontName);
	BitmapFont*					GetFont(const std::string& bitmapFontName);

	unsigned int				CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
	
	void						DrawVertexArray3D_PCT(const Vertex3D* verts, int numVerts, int primitiveType, const Texture* texture = nullptr, float lineThickness = 5.f);
private:
	std::vector< Texture* >					m_alreadyLoadedTextures;
	const Texture*							m_currentTexture;
	std::map< std::string, BitmapFont* >	m_loadedFonts;

	Texture*							m_defaultTexture;
};
