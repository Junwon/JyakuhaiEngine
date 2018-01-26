#pragma once

#include "Engine/Renderer/RHI/RHITypes.hpp"
#include <vector>

class VertexBuffer;
class IndexBuffer;
class Vector2;

struct DrawInstruction_T
{
	DrawInstruction_T() :
		m_materialName(""),
		m_startIndex(0),
		m_vertexCount(0),
		m_primitive(PRIMITIVE_TRIANGLES),
		m_usesIndexBuffer(false)
	{}

	const char* m_materialName;
	int m_startIndex;
	int m_vertexCount;
	ePrimitiveType m_primitive;
	bool m_usesIndexBuffer;
};

class Mesh
{
	// Optional CPU Buffers
// 	std::vector<Vector2> vertices;
// 	std::vector<unsigned int> indices;
public:
	std::vector<DrawInstruction_T> m_drawInstructions;

	// GPU Buffers
	VertexBuffer* m_vertices;
	IndexBuffer* m_indices;

};