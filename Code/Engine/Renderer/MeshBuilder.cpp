#include "Engine/Renderer/MeshBuilder.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

MeshBuilder::MeshBuilder() :
	m_vertexStamp(Vertex3D())
{
	m_tempDrawInstruct = DrawInstruction_T();
}

void MeshBuilder::SetRenderer(SimpleRenderer* renderer)
{
	m_renderer = renderer;
}

size_t MeshBuilder::AddVertex(const Vector3& position)
{
	m_vertexStamp.m_position = position;
	//m_vertices.insert(m_vertices.begin(), m_vertexStamp);
	m_vertices.push_back(m_vertexStamp);
	return m_vertices.size() - 1;
}

void MeshBuilder::SetTangent(const Vector3& tangent)
{
	m_vertexStamp.m_tangent = tangent;
}

void MeshBuilder::SetBitangent(const Vector3& bitangent)
{
	m_vertexStamp.m_bitangent = bitangent;
}

void MeshBuilder::SetNormal(const Vector3& normal)
{ 
	m_vertexStamp.m_normal = normal;
}

void MeshBuilder::SetColor(const Rgba& color) 
{
	m_vertexStamp.m_color = color;
}

void MeshBuilder::SetUV(const Vector2& uv) 
{
	m_vertexStamp.m_texCoords = uv;
}

void MeshBuilder::SetBoneIndices(const IntVector4& indices)
{
	m_vertexStamp.m_boneIndices = indices;
}

void MeshBuilder::SetBoneWeights(const Vector4& weights)
{
	m_vertexStamp.m_boneWeights = weights;
}

void MeshBuilder::Begin(const char* materialName, ePrimitiveType type, bool isIndexed)
{
	m_tempDrawInstruct.m_materialName = materialName;
	m_tempDrawInstruct.m_primitive = type;
	m_tempDrawInstruct.m_startIndex = m_vertices.size();
	m_tempDrawInstruct.m_usesIndexBuffer = isIndexed;
}

void MeshBuilder::End()
{
	m_tempDrawInstruct.m_vertexCount = m_vertices.size() - m_tempDrawInstruct.m_startIndex;
	m_drawInstructions.push_back(m_tempDrawInstruct);
}

void MeshBuilder::Clear()
{
	m_drawInstructions.clear();
	m_vertices.clear();
	m_indices.clear();
}

// void MeshBuilder::SetUV(size_t uv_idx, vec2 const &uv) 
// { 
// 
// }