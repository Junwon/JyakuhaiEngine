#pragma once

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/RHI/SimpleRenderer.hpp"

class MeshBuilder
{
public:
	MeshBuilder();

public:
	void SetRenderer(SimpleRenderer* renderer);

	size_t AddVertex(const Vector3& position);

	void SetTangent(const Vector3& tangent);
	void SetBitangent(const Vector3& bitangent);
	void SetNormal(const Vector3& normal);
	void SetColor(const Rgba& color);
	void SetUV(const Vector2& uv);

	void SetBoneIndices(const IntVector4& indices);
	void SetBoneWeights(const Vector4& weights);
	void Begin(const char* materialName, ePrimitiveType type, bool isIndexed);
	void End();
	void Clear();

public:
	std::vector<Vertex3D> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<DrawInstruction_T> m_drawInstructions;
	DrawInstruction_T m_tempDrawInstruct;

private:
	Vertex3D m_vertexStamp;
	SimpleRenderer* m_renderer;
};