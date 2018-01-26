#pragma once
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vector3.hpp"
#include <vector>

class RHITexture2D;
class RHIDevice;

class OBJModel
{

public:
	OBJModel(RHIDevice* owner);
	~OBJModel();

	void LoadOBJFile(const char* dir);
	void LoadDiffuse(const char* dir);
	void LoadSpec(const char* dir);

	RHITexture2D* GetDiffuse() const;
	RHITexture2D* GetSpecular() const;
	std::vector<Vertex3D> GetVertices() const;

public:
	std::vector<Vertex3D> m_vertex;

	RHIDevice* m_device;
	RHITexture2D* m_diffuse;
	RHITexture2D* m_spec;

private:
	std::vector<std::string> m_outBuffer;

	std::vector<Vector3> m_tempPositions;
	std::vector<Vector2> m_tempUVs;
	std::vector<Vector3> m_tempNormals;
	std::vector<Vector3> m_faceIndices;

private:
	void LoadInVertices();
	void LoadInUVs();
	void LoadInNormals();
	void LoadInFaceIndexBuffer();
	void LoadVerticesAsTriangle(std::vector<int> temp);
	void LoadVerticesAsQuad(std::vector<int> temp);
	int GetIndexOfFirstOccurenceStartingFromIndex(const std::string& key);
};