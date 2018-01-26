#include "Engine/Renderer/RHI/OBJModel.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RHI/DX11.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"

OBJModel::OBJModel(RHIDevice* owner) :
	m_device(owner)
{

}

OBJModel::~OBJModel()
{
	SAFE_DELETE(m_spec);
	SAFE_DELETE(m_diffuse);
}

void OBJModel::LoadOBJFile(const char* dir)
{
	if (strlen(dir) > 0 && ReadWhiteSpacesDelimitedBufferFromFile(m_outBuffer, dir)) {
		LoadInVertices();
		LoadInUVs();
		LoadInNormals();
		LoadInFaceIndexBuffer();
	}
	else {
		ASSERT_OR_DIE(false, "Could not load OBJ file.")
	}
}

void OBJModel::LoadInVertices()
{
	unsigned index = GetIndexOfFirstOccurenceStartingFromIndex("v");

	while (m_outBuffer[index] == "v") {
		float x = std::stof(m_outBuffer[index + 1]);
		float y = std::stof(m_outBuffer[index + 2]);
		float z = std::stof(m_outBuffer[index + 3]);
		m_tempPositions.push_back(Vector3(x, y, z));
		index += 4;
	}
}

void OBJModel::LoadInUVs()
{
	unsigned index = GetIndexOfFirstOccurenceStartingFromIndex("vt");
	
	while (m_outBuffer[index] == "vt") {
		float x = std::stof(m_outBuffer[index + 1]);
		float y = std::stof(m_outBuffer[index + 2]);
		m_tempUVs.push_back(Vector2(x, y * -1.f));
		index += 4;
	}
}

void OBJModel::LoadInNormals()
{  
	unsigned index = GetIndexOfFirstOccurenceStartingFromIndex("vn");
	
	while (m_outBuffer[index] == "vn") {
		float x = std::stof(m_outBuffer[index + 1]) * 1.f;
		float y = std::stof(m_outBuffer[index + 2]) * -1.f;
		float z = std::stof(m_outBuffer[index + 3]) * 1.f;
		m_tempNormals.push_back(Vector3(x, y, z));
		index += 4;
	}
}

void OBJModel::LoadInFaceIndexBuffer()
{
	unsigned index = GetIndexOfFirstOccurenceStartingFromIndex("f");

	std::string delimiter = "/";
	std::vector<int> tempVector;

	while (m_outBuffer[index] == "f") {
		SplitIntoIntBuffer(tempVector, m_outBuffer[++index], delimiter);
		SplitIntoIntBuffer(tempVector, m_outBuffer[++index], delimiter);
		SplitIntoIntBuffer(tempVector, m_outBuffer[++index], delimiter);

		if (m_outBuffer[++index] != "f") {
			SplitIntoIntBuffer(tempVector, m_outBuffer[index], delimiter);
			LoadVerticesAsQuad(tempVector);
			++index;
		}
		else {
			LoadVerticesAsTriangle(tempVector);
		}
		
		
		tempVector.clear();
	}
}

void OBJModel::LoadVerticesAsTriangle(std::vector<int> temp)
{
	Vertex3D vertexOne = Vertex3D(m_tempPositions[temp[0] - 1], m_tempUVs[temp[1] - 1], Rgba::WHITE, m_tempNormals[temp[2] - 1]);
	Vertex3D vertexTwo = Vertex3D(m_tempPositions[temp[3] - 1], m_tempUVs[temp[4] - 1], Rgba::WHITE, m_tempNormals[temp[5] - 1]);
	Vertex3D vertexThree = Vertex3D(m_tempPositions[temp[6] - 1], m_tempUVs[temp[7] - 1], Rgba::WHITE, m_tempNormals[temp[8] - 1]);

	m_vertex.push_back(vertexOne);
	m_vertex.push_back(vertexTwo);
	m_vertex.push_back(vertexThree);
}

void OBJModel::LoadVerticesAsQuad(std::vector<int> temp)
{
	Vertex3D bottomLeft = Vertex3D(m_tempPositions[temp[0] - 1], m_tempUVs[temp[1] - 1], Rgba::WHITE, m_tempNormals[temp[2] - 1]);
	Vertex3D topLeft = Vertex3D(m_tempPositions[temp[3] - 1], m_tempUVs[temp[4] - 1], Rgba::WHITE, m_tempNormals[temp[5] - 1]);
	Vertex3D topRight = Vertex3D(m_tempPositions[temp[6] - 1], m_tempUVs[temp[7] - 1], Rgba::WHITE, m_tempNormals[temp[8] - 1]);
	Vertex3D bottomRight = Vertex3D(m_tempPositions[temp[9] - 1], m_tempUVs[temp[10] - 1], Rgba::WHITE, m_tempNormals[temp[11] - 1]);

	m_vertex.push_back(bottomLeft);
	m_vertex.push_back(topRight);
	m_vertex.push_back(topLeft);

	m_vertex.push_back(bottomLeft);
	m_vertex.push_back(bottomRight);
	m_vertex.push_back(topRight);
}

int OBJModel::GetIndexOfFirstOccurenceStartingFromIndex(const std::string& key)
{
	unsigned int index = 0;

	while (m_outBuffer[index] != key && index < m_outBuffer.size()) {
		index++;
	}

	return index;
}

void OBJModel::LoadDiffuse(const char* dir)
{
	if (strlen(dir) > 0) {
		m_diffuse = new RHITexture2D(m_device, dir);
	}
	else {
		ASSERT_OR_DIE(false, "Could not load OBJ diffuse texture.")
	}
}

void OBJModel::LoadSpec(const char* dir)
{
	if (strlen(dir) > 0) {
		m_spec = new RHITexture2D(m_device, dir);
	}
	else {
		ASSERT_OR_DIE(false, "Could not load OBJ specular texture.")
	}
}

RHITexture2D* OBJModel::GetDiffuse() const
{
	return m_diffuse;
}

RHITexture2D* OBJModel::GetSpecular() const
{
	return m_spec;
}

std::vector<Vertex3D> OBJModel::GetVertices() const
{
	return m_vertex;
}