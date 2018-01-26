#include "Engine/Renderer/Renderer.hpp"

#pragma comment( lib, "opengl32" )
#pragma comment( lib, "glu32" )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <gl/Glu.h>
#include <string>

#include "ThirdParty/stb/stb_image.h"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


PFNGLGENBUFFERSPROC		glGenBuffers		= nullptr;
PFNGLBINDBUFFERPROC		glBindBuffer		= nullptr;
PFNGLBUFFERDATAPROC		glBufferData		= nullptr;
PFNGLGENERATEMIPMAPPROC	glGenerateMipmap	= nullptr;
PFNGLDELETEBUFFERSPROC	glDeleteBuffers		= nullptr;

Renderer::Renderer()
{
	glGenBuffers		= (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer		= (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData		= (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glGenerateMipmap	= (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	unsigned char whiteTexture[] = {255, 255, 255, 255};
	m_defaultTexture = new Texture();
	m_defaultTexture->m_textureID = CreateOpenGLTexture(whiteTexture, 1, 1, 4);
	m_defaultTexture->m_texelDimensions = IntVector2(1, 1);
	m_defaultTexture->m_imageFilePath = "";
		
}

void Renderer::BeginFrame()
{

}

void Renderer::EndFrame()
{

}

void Renderer::ClearScreen(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearScreen(const Rgba& clearColor) 
{
	float r, g, b, a;
	clearColor.GetAsFloats(r, g, b, a);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);
}

void Renderer::SetOrthoProjection(float left, float right, float bottom, float top)
{
	glLoadIdentity();
	glOrtho(left, right, bottom, top, 0.f, 1.f);
}

void Renderer::EnableBackFaceCulling(bool enableBackCulling)
{
	if (enableBackCulling)
	{
		// 	glFrontFace(GL_CCW);
		// 	glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void Renderer::EnableDepthTesting(bool enableDepthTesting)
{
	if (enableDepthTesting)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void Renderer::EnableAlphaTesting(bool enableAlphaTesting)
{
	if (enableAlphaTesting)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}
}

void Renderer::RotateAroundPosition2D(const Vector2& position, float degree)
{
	Translate2D(position);
	Rotate2D(degree);
	Translate2D(-position);
}

//////////////////////////////////////////////////////////////////////////
//
// DRAW FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

void Renderer::DrawAxes(float axisLength)
{
	Vertex3D vertexes[6];

	vertexes[0] = Vertex3D(Vector3(0.f, 0.f, 0.f), Vector2::ZERO, Rgba::RED);
	vertexes[1] = Vertex3D(Vector3(axisLength, 0.f, 0.f), Vector2::ZERO, Rgba::RED);

	vertexes[2] = Vertex3D(Vector3(0.f, 0.f, 0.f), Vector2::ZERO, Rgba::GREEN);
	vertexes[3] = Vertex3D(Vector3(0.f, axisLength, 0.f), Vector2::ZERO, Rgba::GREEN);

	vertexes[4] = Vertex3D(Vector3(0.f, 0.f, 0.f), Vector2::ZERO, Rgba::BLUE);
	vertexes[5] = Vertex3D(Vector3(0.f, 0.f, axisLength), Vector2::ZERO, Rgba::BLUE);

	DrawVertexArray3D_PCT(vertexes, 6, GL_LINES, nullptr, 5.0f);
}

BitmapFont* Renderer::CreateOrGetFont(const std::string& bitmapFontName)
{
	BitmapFont* font = GetFont(bitmapFontName);
	if (font)
		return font;

	font = CreateFontFromFile(bitmapFontName);
	return font;
}

BitmapFont* Renderer::GetFont(const std::string& bitmapFontName)
{
	std::map< std::string, BitmapFont* >::iterator it = m_loadedFonts.find(bitmapFontName);

	if (it == m_loadedFonts.end())
	{
		return nullptr;
	}
	else
	{
		BitmapFont* font = it->second;
		if (bitmapFontName == font->m_fontFilePath)
			return font;
		else
			return nullptr;
	}
}

BitmapFont* Renderer::CreateFontFromFile(const std::string& bitmapFontName)
{
	Texture* fontTexture = CreateOrGetTexture(bitmapFontName);

	BitmapFont* font = new BitmapFont(bitmapFontName);
	font->m_spriteSheet = new SpriteSheet(*fontTexture, BitmapFont::FONT_SHEET_DIMENSIONS, BitmapFont::FONT_SHEET_DIMENSIONS);
	m_loadedFonts[bitmapFontName] = font;

	return font;
}

void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, float cellAspectScale, const BitmapFont* font)
{
	//cellAspectScale unused for now
	cellAspectScale = 0.f;

	const BitmapFont* activeFont = font;

	if (activeFont == nullptr)
	{
		ASSERT_OR_DIE(m_loadedFonts.size() > 0, "DrawText2D requires a loaded font.");
		activeFont = m_loadedFonts.begin()->second;
	}

	int vertexSize = (int)asciiText.length() * 4;
	std::vector<Vertex3D> vertexes;

	for (unsigned int i = 0; i < asciiText.length(); ++i)
	{
		AABB2D texCoordsGlyph = activeFont->GetTexCoordsForGlyph(asciiText[i]);

		vertexes.push_back(Vertex3D(Vector3(startBottomLeft.x + (cellHeight * i), startBottomLeft.y - cellHeight, 0.f),
			Vector2(texCoordsGlyph.m_mins.x, texCoordsGlyph.m_mins.y),
			tint));

		vertexes.push_back(Vertex3D(Vector3(startBottomLeft.x + (cellHeight * i), startBottomLeft.y, 0.f),
			Vector2(texCoordsGlyph.m_mins.x, texCoordsGlyph.m_maxs.y),
			tint));

		vertexes.push_back(Vertex3D(Vector3(startBottomLeft.x + (cellHeight * (i + 1)), startBottomLeft.y, 0.f),
			Vector2(texCoordsGlyph.m_maxs.x, texCoordsGlyph.m_maxs.y),
			tint));

		vertexes.push_back(Vertex3D(Vector3(startBottomLeft.x + (cellHeight * (i + 1)), startBottomLeft.y - cellHeight, 0.f),
			Vector2(texCoordsGlyph.m_maxs.x, texCoordsGlyph.m_mins.y),
			tint));
	}

	DrawVertexArray3D_PCT(&vertexes[0], vertexSize, GL_QUADS, activeFont->m_spriteSheet->GetTexture());
}

void Renderer::DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	Vertex3D vertexes[2];

	vertexes[0] = Vertex3D(Vector3(startX, startY, 0.f), Vector2::ZERO, startColor);
	vertexes[1] = Vertex3D(Vector3(endX, endY, 0.f), Vector2::ZERO, endColor);

	DrawVertexArray3D_PCT(vertexes, 2, GL_LINES, nullptr, lineThickness);

}

void Renderer::DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	Vertex3D vertexes[2];

	vertexes[0] = Vertex3D(Vector3(startPoint.x, startPoint.y, 0.f), Vector2::ZERO, startColor);
	vertexes[1] = Vertex3D(Vector3(endPoint.x, endPoint.y, 0.f), Vector2::ZERO, endColor);

	DrawVertexArray3D_PCT(vertexes, 2, GL_LINES, nullptr, lineThickness);
}

void Renderer::DrawLine2D(const LineSegment2D& line, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
// 	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
// 	glDisable(GL_DEPTH_TEST);  // disable depth-testing

	Vertex3D vertexes[2];

	vertexes[0] = Vertex3D(Vector3(line.m_startPoint.x, line.m_startPoint.y, 0.f), Vector2::ZERO, startColor);
	vertexes[1] = Vertex3D(Vector3(line.m_endPoint.x, line.m_endPoint.y, 0.f), Vector2::ZERO, endColor);

	DrawVertexArray3D_PCT(vertexes, 2, GL_LINES, nullptr, lineThickness);
}

//////////////////////////////////////////////////////////////////////////

void Renderer::DrawLine3D(const Vector3& start, const Vector3& end, const Rgba& color, float lineThickness)
{
	Vertex3D vertexes[2];

	vertexes[0] = Vertex3D(start, Vector2::ZERO, color);
	vertexes[1] = Vertex3D(end, Vector2::ZERO, color);

	DrawVertexArray3D_PCT(vertexes, 2, GL_LINES, nullptr, lineThickness);
}

void Renderer::DrawLines3D(const Vertex3D* vertexArray, int numVertexes, float lineWidth)
{
	DrawVertexArray3D_PCT(vertexArray, numVertexes, GL_LINE_LOOP, nullptr, lineWidth);
}

void Renderer::DrawLines3D(const std::vector<Vertex3D>& vertices, float lineWidth)
{
	DrawVertexArray3D_PCT(&vertices[0], (int)vertices.size(), GL_LINE_LOOP, nullptr, lineWidth);
}

//////////////////////////////////////////////////////////////////////////

void Renderer::DrawRegularPolygonOutline(float centerX, float centerY, float numSides, float radius, const Rgba& color, float angleOffset)
{
	float degreesPerSide = 360.0f / numSides;

	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	for (float degrees = 0.0f; degrees < 360.0f; degrees += degreesPerSide) 
	{
		float radians =  ConvertDegreesToRadians(degrees) + angleOffset;
		float x = centerX + (radius * cosf(radians));
		float y = centerY + (radius * sinf(radians));

		vertexes.push_back(Vertex3D(Vector3(x, y, 0.f), Vector2::ZERO, color));
		vertexCount++;
	}

	DrawVertexArray3D_PCT(&vertexes[0], vertexCount, GL_LINE_LOOP);
}

void Renderer::DrawRegularPolygonOutline(Vector2 centerVector, float numSides, float radius, const Rgba& color, float angleOffset)
{
	DrawRegularPolygonOutline(centerVector.x, centerVector.y, numSides, radius, color, angleOffset);
}

void Renderer::DrawShapeWithVertices(const Vector2 vertices[], int numOfVertices, Rgba color)
{
	std::vector<Vertex3D> vertexes;

	for (int i = 0; i < numOfVertices; ++i)
	{
		vertexes.push_back(Vertex3D(Vector3(vertices[i].x, vertices[i].y, 0.f), Vector2::ZERO, color));
	}

	DrawVertexArray3D_PCT(&vertexes[0], numOfVertices, GL_LINE_LOOP);
}

void Renderer::DrawFilledShapeWithStartingPoint(const Vector2& center, const Vector2 vertices[], int numOfVertices, const Rgba& startColor, const Rgba& endColor)
{
	std::vector<Vertex3D> vertexes;

	vertexes.push_back(Vertex3D(Vector3(center.x, center.y, 0.f), Vector2::ZERO, startColor));

	for (int i = 0; i < numOfVertices; ++i)
	{
		vertexes.push_back(Vertex3D(Vector3(vertices[i].x, vertices[i].y, 0.f), Vector2::ZERO, endColor));
	}

	DrawVertexArray3D_PCT(&vertexes[0], numOfVertices, GL_TRIANGLE_FAN);
}

//////////////////////////////////////////////////////////////////////////

void Renderer::DrawAABB2D(const AABB2D& bounds, const Rgba& color)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2::ZERO, color);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2::ZERO, color);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2::ZERO, color);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2::ZERO, color);

	DrawVertexArray3D_PCT(vertexes, 4, GL_QUADS);
}

void Renderer::DrawAABB2DOutline(const AABB2D& bounds, const Rgba& color, float lineWidth)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2::ZERO, color);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2::ZERO, color);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2::ZERO, color);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2::ZERO, color);

	DrawVertexArray3D_PCT(vertexes, 4, GL_LINE_LOOP, nullptr, lineWidth);
}

// void Renderer::DrawOBB2D(const OBB2D& bounds, const Rgba& color)
// {
// 	glDisable(GL_TEXTURE_2D);
// 	glColor4ub(color.r, color.g, color.b, color.a);
// 	glBegin(GL_QUADS);
// 	{
// 		Vector2 R = bounds.GetCenter() + bounds.GetOrientedWidth();
// 		Vector2 T = bounds.GetCenter() + bounds.GetOrientedHeight();
// 
// 	}
// 	glEnd();
// }

void Renderer::DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color, float numSides)
{
	DrawRegularPolygonOutline(centerX, centerY, numSides, radius, color);
}

void Renderer::DrawCircle2D(const Vector2& position, float radius, const Rgba& color, float numSides)
{
	DrawRegularPolygonOutline(position.x, position.y, numSides, radius, color);
}

void Renderer::DrawDisc2D(const Vector2& center, float radius, const Rgba& color, float numSides)
{
	float degreesPerSide = 360.0f / numSides;

	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	vertexes.push_back(Vertex3D(Vector3(center.x, center.y, 0.f), Vector2::ZERO, color));

	for (float degrees = 0.0f; degrees <= 360.0f; degrees += degreesPerSide)
	{
		float radians = ConvertDegreesToRadians(degrees);
		float x = center.x + (radius * cosf(radians));
		float y = center.y + (radius * sinf(radians));

		vertexes.push_back(Vertex3D(Vector3(x, y, 0.f), Vector2::ZERO, color));
		vertexCount++;
	}

	DrawVertexArray3D_PCT(&vertexes[0], vertexCount, GL_TRIANGLE_FAN);
}

void Renderer::DrawDisc2D(const Disc2D& disc, const Rgba& color, float numSides)
{
	float degreesPerSide = 360.0f / numSides;

	int vertexCount = 0;
	std::vector<Vertex3D> vertexes;

	Vector2 center = disc.m_center;
	float radius = disc.m_radius;

	vertexes.push_back(Vertex3D(Vector3(center.x, center.y, 0.f), Vector2::ZERO, color));

	for (float degrees = 0.0f; degrees <= 360.0f; degrees += degreesPerSide)
	{
		float radians = ConvertDegreesToRadians(degrees);
		float x = center.x + (radius * cosf(radians));
		float y = center.y + (radius * sinf(radians));

		vertexes.push_back(Vertex3D(Vector3(x, y, 0.f), Vector2::ZERO, color));
		vertexCount++;
	}

	DrawVertexArray3D_PCT(&vertexes[0], vertexCount, GL_TRIANGLE_FAN);
}

void Renderer::DrawFilledShapeWithVertices(const Vector2 vertices[], int numOfVertices, const Rgba& color)
{
	std::vector<Vertex3D> vertexes;

	for (int i = 0; i < numOfVertices; ++i)
	{
		vertexes.push_back(Vertex3D(Vector3(vertices[i].x, vertices[i].y, 0.f), Vector2::ZERO, color));
	}
	
	DrawVertexArray3D_PCT(&vertexes[0], numOfVertices, GL_POLYGON);
}

//////////////////////////////////////////////////////////////////////////

void Renderer::DrawPoint3D(const Vector3& pos, const Rgba& color, float size)
{
	std::vector<Vertex3D> vertexes;

	vertexes.push_back(Vertex3D(Vector3(pos.x, pos.y, pos.z), Vector2::ZERO, color));

	glPointSize(size);
	DrawVertexArray3D_PCT(&vertexes[0], 1, GL_POINTS);
}

void Renderer::DrawAABB3DOutline(const AABB3D& bounds, const Rgba& color, float lineWidth)
{
	Vertex3D vertexes[24];

	//Bottom Face
	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);

	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[3] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);

	vertexes[4] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[5] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);

	vertexes[6] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[7] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);

	//Side Edges
	vertexes[8] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[9] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[10] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[11] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[12] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[13] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[14] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z), Vector2::ZERO, color);
	vertexes[15] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);

	//Top Face
	vertexes[16] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);
	vertexes[17] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[18] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);
	vertexes[19] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[20] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);
	vertexes[21] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);

	vertexes[22] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z), Vector2::ZERO, color);
	vertexes[23] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z), Vector2::ZERO, color);

	DrawVertexArray3D_PCT(vertexes, 24, GL_LINES, nullptr, lineWidth);
}

void Renderer::DrawQuads3D(const Vertex3D* vertexArray, int numVertexes)
{
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride,  &vertexArray[0].m_position.x);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexArray[0].m_color);
	glTexCoordPointer(2, GL_FLOAT, stride, &vertexArray[0].m_texCoords);
	
	glDrawArrays(GL_QUADS, 0, numVertexes);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawQuads3DWithVBO(unsigned int vboID, int numVertexesInVBO)
{
	DrawVBO3D(vboID, numVertexesInVBO, GL_QUADS);
}

void Renderer::DrawVBO3D(unsigned int vboID, int numVerts)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride, (const GLvoid*)offsetof(Vertex3D, m_position));
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, (const GLvoid*)offsetof(Vertex3D, m_color));
	glTexCoordPointer(2, GL_FLOAT, stride, (const GLvoid*)offsetof(Vertex3D, m_texCoords));

	glDrawArrays(GL_QUADS, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::DrawVBO3D(unsigned int vboID, int numVerts, unsigned int primitiveType)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride, (const GLvoid*)offsetof(Vertex3D, m_position));
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, (const GLvoid*)offsetof(Vertex3D, m_color));
	glTexCoordPointer(2, GL_FLOAT, stride, (const GLvoid*)offsetof(Vertex3D, m_texCoords));

	glDrawArrays(primitiveType, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::InitializeVBO(unsigned int &vboID, std::vector<Vertex3D> vertexArray, int &numVertexesInVBO)
{
	if (vboID == 0)
	{
		glGenBuffers(1, &vboID);
	}

	size_t vertexArrayNumBytes = sizeof(Vertex3D) * vertexArray.size();
	numVertexesInVBO = (int)vertexArrayNumBytes;
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexArrayNumBytes, vertexArray.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::BindBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////

Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	Texture* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
}

void Renderer::BindTexture(const Texture* texture)
{
	if (texture)
	{
		m_currentTexture = texture;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->m_textureID);
	}
	else if(texture == nullptr)
	{
		m_currentTexture = m_defaultTexture;
		glBindTexture(GL_TEXTURE_2D, m_defaultTexture->m_textureID);
	}
}

Texture* Renderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex)
	{
		Texture* texture = m_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}

	return nullptr;
}

Texture* Renderer::CreateTextureFromFile(const std::string& imageFilePath)
{
	// Load image data
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = imageFilePath;
	texture->m_texelDimensions.SetXY(width, height);

	m_alreadyLoadedTextures.push_back(texture);
	return texture;
}

unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	// Create a texture ID (or "name" as OpenGL calls it) for this new texture
	unsigned int openGLTextureID = 0xFFFFFFFF;
	glGenTextures(1, &openGLTextureID);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture(GL_TEXTURE_2D, openGLTextureID);

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

    // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (bytesPerTexel == 3)
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageTexelBytes);	// Location of the actual pixel data bytes/buffer

	return openGLTextureID;
}

void Renderer::DrawTexturedAABB2D(const AABB2D& bounds, const Texture& texture, const Rgba& tints)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2(0.f, 1.f), tints);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(1.f, 1.f), tints);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2(1.f, 0.f), tints);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(0.f, 0.f), tints);

	DrawVertexArray3D_PCT(vertexes, 4, GL_QUADS, &texture);
}

void Renderer::DrawTexturedAABB2D(const AABB2D& bounds, const AABB2D& textureBounds, const Texture& texture, const Rgba& tints)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2(textureBounds.m_mins.x, textureBounds.m_maxs.y), tints);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(textureBounds.m_maxs.x, textureBounds.m_maxs.y), tints);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2(textureBounds.m_maxs.x, textureBounds.m_mins.y), tints);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(textureBounds.m_mins.x, textureBounds.m_mins.y), tints);

	DrawVertexArray3D_PCT(vertexes, 4, GL_QUADS, &texture);
}

void Renderer::DrawTexturedAABB2DSprite(const AABB2D& bounds, const AABB2D& spriteBounds, const Texture& texture, const Rgba& tints)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2(spriteBounds.m_mins.x, spriteBounds.m_maxs.y), tints);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(spriteBounds.m_maxs.x, spriteBounds.m_maxs.y), tints);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2(spriteBounds.m_maxs.x, spriteBounds.m_mins.y), tints);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(spriteBounds.m_mins.x, spriteBounds.m_mins.y), tints);

	DrawVertexArray3D_PCT(vertexes, 4, GL_QUADS, &texture);
}

void Renderer::DrawTexturedAABB3D(const AABB3D& bounds, const Texture& texture, const Rgba& tints)
{
	Vertex3D vertexes[4];

	vertexes[0] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f), Vector2(0.f, 1.f), tints);
	vertexes[1] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f), Vector2(1.f, 1.f), tints);
	vertexes[2] = Vertex3D(Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f), Vector2(1.f, 0.f), tints);
	vertexes[3] = Vertex3D(Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f), Vector2(0.f, 0.f), tints);

	DrawVertexArray3D_PCT(vertexes, 4, GL_QUADS, &texture);
}

void Renderer::DrawVertexArray3D_PCT(const Vertex3D* verts, int numVerts, int primitiveType, const Texture* texture, float lineThickness)
{
	BindTexture(texture);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride, &verts[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &verts[0].m_color);
	glTexCoordPointer(2, GL_FLOAT, stride, &verts[0].m_texCoords);

	glLineWidth(lineThickness);

	glDrawArrays(primitiveType, 0, numVerts);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//////////////////////////////////////////////////////////////////////////
//
// MATRIX FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

void Renderer::LoadMatrix(const Matrix4& matrixToLoad) 
{
	glLoadMatrixf(matrixToLoad.GetAsFloatArray());
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::Translate2D(float x, float y)
{
	glTranslatef(x, y, 0);
}

void Renderer::Translate2D(const Vector2& position)
{
	glTranslatef(position.x, position.y, 0);
}

void Renderer::Rotate2D(float anglesDegree)
{
	glRotatef(anglesDegree, 0.0f, 0.0f, 1.0f);
}

void Renderer::Scale2D(float x, float y)
{
	glScalef(x, y, 1);
}

void Renderer::Scale2D(const Vector2& scalesPerAxis)
{
	glScalef(scalesPerAxis.x, scalesPerAxis.y, 1);
}

void Renderer::Translate3D(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::Translate3D(const Vector3& position)
{
	glTranslatef(position.x, position.y, position.z);
}

void Renderer::Rotate3D(float anglesDegree, const Vector3& axis)
{
	glRotatef(anglesDegree, axis.x, axis.y, axis.z);
}

void Renderer::Scale3D(float x, float y, float z)
{
	glScalef(x, y, z);
}

void Renderer::Scale3D(const Vector3& scalesPerAxis)
{
	glScalef(scalesPerAxis.x, scalesPerAxis.y, scalesPerAxis.z);
}

void Renderer::LookAt3D(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}