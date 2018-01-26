#pragma once
#pragma warning(disable: 4505)

#include "Engine/Renderer/Skeleton.hpp"

class Vector3;
class Vector4;
class Matrix4;
class IntVector4;
class MatrixStack;
class MeshBuilder;
class Motion;

#if !defined( __TOOLS_FBXLOAD__ )
#define __TOOLS_FBXLOAD__

typedef __int32 int32_t;

struct FBXSkinWeight_T
{
	FBXSkinWeight_T() :
		indices(IntVector4::ZERO),
		weights(Vector4::ZERO)
	{}

	void Reset()
	{
		indices = IntVector4::ZERO;
		weights = Vector4::ZERO;
	}

	IntVector4 indices;
	Vector4 weights;
};

void FbxListFile(const char* filename);
bool FbxLoadMesh(MeshBuilder* mb, const char* filename, float scale = 1.f, const Skeleton* skeleton = nullptr);
bool FbxLoadSkeleton(Skeleton* skel, const char* filename, float scale = 1.f, const Vector3& translation = Vector3::ZERO);
bool FbxLoadMotion(Motion* motion, const Skeleton* skeleton, const char* filename, float scale = 1.f, const Vector3& translation = Vector3::ZERO, unsigned int framerate = 10);

#if defined(TOOLS_BUILD) 

#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk-md.lib")

#define __PLACEMENT_NEW_INLINE 

//Scene Functions
FbxScene* FbxLoadScene(const char* filename, bool importEmbedded);
void FbxUnloadScene(FbxScene* scene);
void FbxPrintNode(FbxNode* node, int depth);
void FbxListScene(FbxScene* scene);
void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth);
const char* GetAttributeTypeName(FbxNodeAttribute::EType type);

//Mesh Functions
void ConvertSceneToEngineBasis(FbxScene *scene);
void TriangulateScene(FbxScene* scene);
void FlipXAxis(MeshBuilder* mb);
static bool ImportMesh(MeshBuilder* out, MatrixStack& matStack, FbxMesh* mesh, const Skeleton* skeleton = nullptr, float scale = 1.f);
void ImportMeshes(MeshBuilder* out, FbxNode* node, MatrixStack& matStack, const Skeleton* skeleton = nullptr, float scale = 1.f);
const char* FbxGetMaterialName(FbxMesh* mesh);
Matrix4 GetNodeWorldTransform(FbxNode* node);
Matrix4 ToEngineMatrix(const FbxMatrix& fbxMat);
Matrix4 GetGeometricTransform(FbxNode* node);
void FbxImportVertex(MeshBuilder* out, const Matrix4& transform, FbxMesh* mesh, const std::vector<FBXSkinWeight_T>& skinWeights, int32_t polyIDx, int32_t vertIDx, float scale = 1.f);
void FbxSetSkinWeight(MeshBuilder* out, FbxMesh* mesh, const std::vector<FBXSkinWeight_T>& skinWeights, int32_t polyIDx, int32_t vertIDx);
bool FbxGetPosition(Vector3* outPos, const Matrix4& transform, FbxMesh* mesh, int polyIDx, int vertIDx, float scale = 1.f);
bool FbxGetNormal(Vector3* out, FbxMesh* mesh, int polyIDx, int vertIDx, float scale = 1.f);
Vector3 CalculateTangent(const Vector3& normal);
Vector3 CalculateBitangent(const Vector3& normal, const Vector3& tangent);
bool FbxGetUV(Vector2* out, FbxMesh* mesh, int polyIDx, int vertIDx);
Vector2 FromFbxVector2ToVector2(const FbxVector2& fbxVector);
Vector4 FromFbxVector4ToVector4(const FbxVector4& fbxVector);
void CalculateSkinWeights(std::vector<FBXSkinWeight_T>* skinWeights, const FbxMesh* mesh, const Skeleton* skeleton);
void AddHighestWeight(FBXSkinWeight_T* skinWeight, int boneIDx, float weight);
bool HasSkinWeights(const FbxMesh* mesh);

//Skeleton Functions
void TransformSkeleton(Skeleton* skel, float scale, const Vector3& translation);
void ScaleJointTransform(Joint* j, float scale);
void TranslateJointTransform(Joint* j, const Vector3& translation);
void FlipXAxis(Skeleton* skel);
FbxPose* GetBindPose(FbxScene *scene);
void ImportSkeleton(Skeleton* outSkeleton, FbxNode* node, FbxSkeleton* rootBone, FbxSkeleton* parentBone, FbxPose* pose);
static const char* GetNodeName(FbxNode const *node);
static const char* GetBoneName(const FbxSkeleton* skel);

//Motion Functions
static float GetNativeFrameRate(const FbxScene* scene);
static bool FbxImportMotion(Motion* motion, const Skeleton* skeleton, FbxScene* scene, FbxAnimStack* anim, float framerate);
Matrix4 GetNodeWorldTransformAtTime(FbxNode* node, FbxTime time);
void TransformMotion(Motion* motion, float scale, const Vector3& translation);

#endif

#endif
