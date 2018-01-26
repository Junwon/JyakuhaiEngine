#include "Engine/Tools/FBX/fbx.h"

#include <vector>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Motion.hpp"
#include "Engine/Renderer/Pose.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

#include "ThirdParty/mikktspace/mikktspace.h"

#define UNUSED(x) (void)(x);

#if defined(TOOLS_BUILD) 

// If FBX IMPORTING ENABLED, make sure to include the library and header files.
// #define __PLACEMENT_NEW_INLINE 

void FbxListFile(const char* filename)
{
	FbxManager *fbx_manager = FbxManager::Create();
	if (fbx_manager == nullptr) {
		ASSERT_OR_DIE(fbx_manager != nullptr, "FBX Manager is NULL.");
		return;
	}

	fbx_manager->Destroy();

	FbxScene* scene = FbxLoadScene(filename, false);

	if (scene == nullptr)
	{
		DebuggerPrintlnf("Failed to load scene: %s", filename);
		return;
	}

	DebuggerPrintlnf("Listing File: %s", filename);
	FbxListScene(scene);

	FbxUnloadScene(scene);
}

//------------------------------------------------------------------------
FbxScene* FbxLoadScene(const char* filename, bool importEmbedded)
{
	// Manager - a running process of the FbxSdk -  could only ever load one of these
	// and call it good - though I usually just keep one around per import.
	FbxManager* fbxManager = FbxManager::Create();

	if (fbxManager == nullptr) {
		//ERROR_MSG("Could not create FBX Manager.");
		ASSERT_OR_DIE(nullptr, "Could not create FBX Manager.");
		return nullptr;
	}

	// Next, set the IO settings to configure what is importing
	// By default, import everything, but if you wanted to explicitly not import some parts
	// you could do that here)
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	ioSettings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, importEmbedded);

	fbxManager->SetIOSettings(ioSettings);


	// Next, create our importer (since we're loading an FBX, not saving one)
	// Second argument is the name - we don't need to name it.
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	bool result = importer->Initialize(filename,
		-1, // File format, -1 will let the program figure it out
		ioSettings);

	if (result) {
		// First, create a scene to be imported into
		// Doesn't need a name, we're not going to do anything with it
		FbxScene *scene = FbxScene::Create(fbxManager, "");

		// Import into the scene
		result = importer->Import(scene);
		importer->Destroy();
		importer = nullptr;

		if (result) {
			return scene;
		}
	}

	// Failed somewhere, so clean up after ourselves.
	FBX_SAFE_DESTROY(ioSettings);
	FBX_SAFE_DESTROY(importer);
	FBX_SAFE_DESTROY(fbxManager);

	return nullptr;
}

//------------------------------------------------------------------------
void FbxUnloadScene(FbxScene* scene)
{
	if (scene == nullptr) {
		return;
    }

	FbxManager* manager = scene->GetFbxManager();
	FbxIOSettings* ioSettings = manager->GetIOSettings();

	FBX_SAFE_DESTROY(scene);
	FBX_SAFE_DESTROY(ioSettings);
	FBX_SAFE_DESTROY(manager);
}

//------------------------------------------------------------------------
void FbxPrintNode(FbxNode* node, int depth)
{
	// Print the node's attributes.
	DebuggerPrintlnf("%*sNode [%s]", depth, " ", node->GetName());

	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxPrintAttribute(node->GetNodeAttributeByIndex(i), depth);
	}

	// Print the nodes children
	for (int32_t i = 0; i < node->GetChildCount(); ++i) {
		FbxPrintNode(node->GetChild(i), depth + 1);
	}
}

//------------------------------------------------------------------------
void FbxListScene(FbxScene* scene)
{
	// Print the node
	FbxNode* root = scene->GetRootNode();
	FbxPrintNode(root, 0);
}

//------------------------------------------------------------------------
void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth)
{
	if (pAttribute == nullptr) {
		return;
	}

	FbxNodeAttribute::EType type = pAttribute->GetAttributeType();

	const char* typeName = GetAttributeTypeName(type);
	const char* attrName = pAttribute->GetName();

	DebuggerPrintlnf("%*s- type='%s' name='%s'", depth, " ", typeName, attrName);
}

//------------------------------------------------------------------------
const char* GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
		case FbxNodeAttribute::eUnknown: return "unidentified";
		case FbxNodeAttribute::eNull: return "null";
		case FbxNodeAttribute::eMarker: return "marker";
		case FbxNodeAttribute::eSkeleton: return "skeleton";
		case FbxNodeAttribute::eMesh: return "mesh";
		case FbxNodeAttribute::eNurbs: return "nurbs";
		case FbxNodeAttribute::ePatch: return "patch";
		case FbxNodeAttribute::eCamera: return "camera";
		case FbxNodeAttribute::eCameraStereo: return "stereo";
		case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
		case FbxNodeAttribute::eLight: return "light";
		case FbxNodeAttribute::eOpticalReference: return "optical reference";
		case FbxNodeAttribute::eOpticalMarker: return "marker";
		case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary: return "boundary";
		case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case FbxNodeAttribute::eShape: return "shape";
		case FbxNodeAttribute::eLODGroup: return "lodgroup";
		case FbxNodeAttribute::eSubDiv: return "subdiv";
		default: return "unknown";
	}
}


//------------------------------------------------------------------------

//Loading a Mesh

//------------------------------------------------------------------------
bool FbxLoadMesh(MeshBuilder* mb, const char* filename, float scale, const Skeleton* skeleton)
{
	// First - load the scene - code is identical to when you listed the file
	FbxScene* scene = FbxLoadScene(filename, false);
	if (nullptr == scene) {
		return false;
	}

	// Second, we want to convert the basis for consistency
	ConvertSceneToEngineBasis(scene);

	// Third, our engine only deals with triangles, but authoring programs tend to deal with a lot of 
	// different surface patch types.  For we triangulate (the process of converting every non-triangle polygon to triangles )
	TriangulateScene(scene);

	// Alright, now we can start importing data, for this we'll need a matrix stack 
	// [well, it helps, you can do this without since we'll mostly be deailing with 
	// global transforms outside of what is called the "geometric transform" which is always local]

	// clear all old data before importing
	mb->Clear();

	// My matrix stack treats the top as being IDENTITY
	MatrixStack matStack;

	// Next, get the root node, and start importing
	FbxNode* root = scene->GetRootNode();

	// import data into our builder
	ImportMeshes(mb, root, matStack, skeleton, scale);

	// Clean up after ourself
	FbxUnloadScene(scene);

	// [HACK] After loading, I flip everything across the X axis
	// for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
	// but no mattter the transform I sent, it was always flipped on X [at best]
	//
	// Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
	//FlipXAxis(mb);
	return true;
}


//------------------------------------------------------------------------
void ConvertSceneToEngineBasis(FbxScene *scene)
{
	fbxsdk::FbxAxisSystem local_system(
		(fbxsdk::FbxAxisSystem::EUpVector) fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
		(fbxsdk::FbxAxisSystem::EFrontVector) fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd,  // by default points toward me.
		(fbxsdk::FbxAxisSystem::ECoordSystem) fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded);

	FbxAxisSystem scene_system = scene->GetGlobalSettings().GetAxisSystem();
	if (scene_system != local_system) {
		local_system.ConvertScene(scene);
	}
}


//------------------------------------------------------------------------
void TriangulateScene(FbxScene* scene)
{
	FbxGeometryConverter converter(scene->GetFbxManager());

	// Replace being true means it will destroy the old 
	// representation of the mesh after conversion.  Setting this to 
	// false would leave the old representations in.
	converter.Triangulate(scene, true);
}


//------------------------------------------------------------------------
void FlipXAxis(MeshBuilder* mb)
{
	/*
	// Flips all geoemetry across the X axis. You just want to flip the X coordinate of all vertex
	// data (positions,normals/tangents/bitangents)
	*/
	for (Vertex3D& vert : mb->m_vertices) {
		vert.m_position.x *= -1.f;
		vert.m_normal.x *= -1.f;
		vert.m_tangent.x *= -1.f;
		vert.m_bitangent.x *= 1.f;
	}
}


//------------------------------------------------------------------------
void ImportMeshes(MeshBuilder* out, FbxNode* node, MatrixStack& matStack, const Skeleton* skeleton, float scale)
{
	// first, we're traversing the graph, keep track of our current world transform
	// (I will do this by by pushing directly the world transform of this node to my stack)
	// (that is, this transform becomes the new top exactly)
	Matrix4 transform = GetNodeWorldTransform(node);
	matStack.PushDirect(transform);

	// First, traverse the scene graph, looking for FbxMesh node attributes.
	int attribCount = node->GetNodeAttributeCount();

	for (int ai = 0; ai < attribCount; ++ai) {
		FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(ai);
		if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh) {
			ImportMesh(out, matStack, (FbxMesh*)attrib, skeleton, scale);
		}
	}

	// Try to load any meshes that are children of this node
	int childCount = node->GetChildCount();

	for (int ci = 0; ci < childCount; ++ci) {
		FbxNode* child = node->GetChild(ci);
		ImportMeshes(out, child, matStack, skeleton, scale);
	}

	// we're done with this node, so its transform off the stack
	matStack.Pop();
}


//------------------------------------------------------------------------
static bool ImportMesh(MeshBuilder* out, MatrixStack& matStack, FbxMesh* mesh, const Skeleton* skeleton, float scale)
{
	// Should have been triangulated before this - sort of a late check 
	// [treat this as an ASSERT_OR_DIE]
	ASSERT_OR_DIE(mesh->IsTriangleMesh(), "Mesh is not triangulated.");

	// Load in the mesh - first, figure out how many polygons there are
	int polyCount = mesh->GetPolygonCount();

	if (polyCount <= 0) {
		return false;
	}

	std::vector<FBXSkinWeight_T> skinWeights;
	if (skeleton != nullptr) {
		CalculateSkinWeights(&skinWeights, mesh, skeleton);
	}

	// Geometric Transformations only apply to the current node
	// http://download.autodesk.com/us/fbx/20112/FBX_SDK_HELP/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
	Matrix4 geoTrans = GetGeometricTransform(mesh->GetNode());
	matStack.Push(geoTrans);

	// import the mesh data.
	Matrix4 transform = matStack.GetTop();

	// Starting a draw call.
	const char* matName = FbxGetMaterialName(mesh);
	out->Begin(matName, PRIMITIVE_TRIANGLES, false);

	for (int polyIDx = 0; polyIDx < polyCount; ++polyIDx){
		// For each polygon - get the number of vertices that make it up (should always be 3 for a triangulated mesh)
		int vertCount = mesh->GetPolygonSize(polyIDx);
		ASSERT_OR_DIE(vertCount == 3, "ImportMesh(): Vertices count is not 3."); // Triangle Meshes should ALWAYS have 3 verts per poly

		// Finally, import all the data for this vertex (for now, just position)
		for (int vertIDx = 0; vertIDx < vertCount; ++vertIDx)
		{
			FbxImportVertex(out, transform, mesh, skinWeights, polyIDx, vertIDx, scale);
		}
	}

	out->End();

	// pop the geometric transform
	matStack.Pop();
	return true;
}


//------------------------------------------------------------------------
const char* FbxGetMaterialName(FbxMesh* mesh)
{
	if (mesh != nullptr) {
		return "";
	} 

	return mesh->GetElementMaterial(0)->GetName();
}

//------------------------------------------------------------------------
Matrix4 GetNodeWorldTransform(FbxNode* node)
{
	FbxAMatrix fbxMat = node->EvaluateGlobalTransform();
	return ToEngineMatrix(fbxMat);
}

//------------------------------------------------------------------------
// If using my basis - you should convert the transform 
// to our at point of fetch.
//------------------------------------------------------------------------
Matrix4 ToEngineMatrix(const FbxMatrix& fbxMat)
{
	Matrix4 const flipX = Matrix4(-Vector3::X_AXIS, Vector3::Y_AXIS, Vector3::Z_AXIS);

	FbxDouble4 row0 = fbxMat.mData[0];
	FbxDouble4 row1 = fbxMat.mData[1];
	FbxDouble4 row2 = fbxMat.mData[2];
	FbxDouble4 row3 = fbxMat.mData[3];

	Matrix4 mat = Matrix4((float)row0.mData[0], (float)row1.mData[0], (float)row2.mData[0], (float)row3.mData[0], 
						  (float)row0.mData[1], (float)row1.mData[1], (float)row2.mData[1], (float)row3.mData[1],
						  (float)row0.mData[2], (float)row1.mData[2], (float)row2.mData[2], (float)row3.mData[2],
						  (float)row0.mData[3], (float)row1.mData[3], (float)row2.mData[3], (float)row3.mData[3]);
	
	return flipX * mat * flipX;
}

//------------------------------------------------------------------------
Matrix4 GetGeometricTransform(FbxNode* node)
{
	Matrix4 ret = Matrix4::IDENTITY;

	if ((node != nullptr) && (node->GetNodeAttribute() != nullptr)) {
		FbxEuler::EOrder order;

		node->GetRotationOrder(FbxNode::eSourcePivot, order);

		FbxVector4 const geoTrans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 const geoRot = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 const geoScale = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geoMat;
		FbxAMatrix matRot;
		matRot.SetR(geoRot, order);
		geoMat.SetTRS(geoTrans, matRot, geoScale);

		ret = ToEngineMatrix(geoMat);
   }

	return ret;
}


//------------------------------------------------------------------------
// #TODO : Finish for tangents, etc.
void FbxImportVertex(MeshBuilder* out, const Matrix4& transform, FbxMesh* mesh, const std::vector<FBXSkinWeight_T>& skinWeights, int32_t polyIDx, int32_t vertIDx, float scale)
{
	// Will demonstarate with normal, but this process is repeated for
	// uvs, colors, tangents, bitangents (called binormals in the SDK)
	Vector3 normal;
	if (FbxGetNormal(&normal, mesh, polyIDx, vertIDx, scale)) {
		// #NOTE : Swapped the product here.
		//normal = (transform * Vector4(normal, 0.0f)).XYZ();
		normal = (transform * Vector4(normal, 0.0f)).XYZ();
		normal *= scale;
		out->SetNormal(normal);
	}

	Vector3 tangent;
	tangent = CalculateTangent(normal);
	//out->SetTangent(tangent);
	
	Vector3 bitangent;
	bitangent = CalculateBitangent(normal, tangent);
	//out->SetBitangent(bitangent);

	Vector2 textureCoords;
	if (FbxGetUV(&textureCoords, mesh, polyIDx, vertIDx)) {
		textureCoords = (transform * Vector4(textureCoords, 0.f, 0.f)).XY();
		textureCoords.y = 1 - textureCoords.y;
		out->SetUV(textureCoords);
	}


	FbxSetSkinWeight(out, mesh, skinWeights, polyIDx, vertIDx);


	// If we get a position- push it.
	// This actually should always succeed.
	Vector3 position;
	if (FbxGetPosition(&position, transform, mesh, polyIDx, vertIDx, scale)) {
		out->AddVertex(position);
	}
}


//------------------------------------------------------------------------
void FbxSetSkinWeight(MeshBuilder* out, FbxMesh* mesh, const std::vector<FBXSkinWeight_T>& skinWeights, int32_t polyIndex, int32_t vertIndex)
{
	int controlIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);

	if (controlIndex < 0) {
		return;
	}

	IntVector4 boneIndicesAtControl = skinWeights[controlIndex].indices;
	Vector4 boneWeightsAtControl = skinWeights[controlIndex].weights;
		
	out->SetBoneIndices(boneIndicesAtControl);
	out->SetBoneWeights(boneWeightsAtControl);
}

//------------------------------------------------------------------------
bool FbxGetPosition(Vector3* outPos, const Matrix4& transform, FbxMesh* mesh, int polyIndex, int vertIndex, float scale)
{
	// First, get the control point index for this poly/vert pair.
	int controlIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);

	if (controlIndex < 0) {
		return false;
	}

	// Get the position for the control point.
	FbxVector4 fbxPos = mesh->GetControlPointAt(controlIndex);

	// convert to an engine vertex, and save it to the out variable
	Vector3 pos = FromFbxVector4ToVector4(fbxPos).XYZ();
	pos.x *= -1.f;
	*outPos = (transform * pos) * scale;
	
	// return success.
	return true;
}

bool FbxGetNormal(Vector3* out, FbxMesh* mesh, int polyIDx, int vertIDx, float scale)
{
	UNUSED(scale);

	// First, we need to get the geometry element we care about...
	// you can this of this as a "layer" of data for a mesh.  In this case
	// it is the normal data.

	// Note:  0 Index - meshes can potentially contain multiple layers
	// of the same type (UVs are the most common exmaple of this).  
	// For Normals, I just use the first one and continue on.
	FbxGeometryElementNormal* element = mesh->GetElementNormal(0);

	if (out == nullptr) {
		// no layer - no data
		return false;
	}

	// Next, we need to figure out how to pull the normal for this particular vertex out
	// of this layer. 
	// 
	// So, vnormal is just a packed array of normals, or potentially normals, and
	// and index buffer into those normals.
	//
	// Either way, you can think of vnormal as a packed collection of normals.
	// and we're trying to figure out which element of this array we want (elem_idx)

	// So first, get the element index.
	// (Is the data tied to the control point, or the polygon vertex)
	int elemIDx = 0;

	switch (element->GetMappingMode()) {
		case FbxGeometryElement::eByControlPoint: {
			// Get the normal by control point - most direct way.
			elemIDx = mesh->GetPolygonVertex(polyIDx, vertIDx);
		} break; // case: eByControlPoint

		case FbxGeometryElement::eByPolygonVertex: {
			// array is packed by polygon vertex - so each polygon is unique
			// (think sharp edges in the case of normals)
			elemIDx = mesh->GetPolygonVertexIndex(polyIDx) + vertIDx;
		} break; // case: eByPolygonVertex

		default:
			ASSERT_OR_DIE(nullptr, "Unknown mapping mode for FbxGetNormal."); // Unknown mapping mode
			return false;
	}

	// next, now that we have the element index, we figure out how this is used.
	// Is this a direct array, or an indexed array.
	// If we got an element index, fetch the data based on how it's referenced
	// (Directly, or by an index into an array);
	switch (element->GetReferenceMode()) {
		case FbxGeometryElement::eDirect: {
			// this is just an array - use elem index as an index into this array
			if (elemIDx < element->GetDirectArray().GetCount()) {
				*out = FromFbxVector4ToVector4(element->GetDirectArray().GetAt(elemIDx)).XYZ();
				return true;
			}
		} break;

		case FbxGeometryElement::eIndexToDirect: {
			if (elemIDx < element->GetIndexArray().GetCount()) {
				int index = element->GetIndexArray().GetAt(elemIDx);
				*out = FromFbxVector4ToVector4(element->GetDirectArray().GetAt(index)).XYZ();
				return true;
			}
		} break;

		default:
			ASSERT_OR_DIE(nullptr, "Unknown reference type for FbxGetNormal.");
			return false;
	}

	ASSERT_OR_DIE(nullptr, "Unknown reference type for FbxGetNormal.");
	return false;
}

Vector3 CalculateTangent(const Vector3& normal)
{
	SMikkTSpaceContext mikkTContext;
	SMikkTSpaceInterface mikkTInterface;

	return Vector3::ZERO;
}

Vector3 CalculateBitangent(const Vector3& normal, const Vector3& tangent)
{
	return Vector3::ZERO;
}

bool FbxGetUV(Vector2* out, FbxMesh* mesh, int polyIDx, int vertIDx)
{
	FbxGeometryElementUV* element = mesh->GetElementUV(0);

	if (out == nullptr) {
		return false;
	}

	if (element == nullptr) {
		return false;
	}

	// Next, we need to figure out how to pull the normal for this particular vertex out
	// of this layer. 
	// 
	// So, vnormal is just a packed array of normals, or potentially normals, and
	// and index buffer into those normals.
	//
	// Either way, you can think of vnormal as a packed collection of normals.
	// and we're trying to figure out which element of this array we want (elem_idx)

	int elemIDx = 0;

	switch (element->GetMappingMode()) {
		case FbxGeometryElement::eByControlPoint: {
			elemIDx = mesh->GetPolygonVertex(polyIDx, vertIDx);
		} 
		break;

		case FbxGeometryElement::eByPolygonVertex: {
			elemIDx = mesh->GetPolygonVertexIndex(polyIDx) + vertIDx;
		} 
		break;

		default:
			ASSERT_OR_DIE(nullptr, "Unknown mapping mode for FbxGetNormal.");
			return false;
	}

	switch (element->GetReferenceMode()) {
	case FbxGeometryElement::eDirect: {
		if (elemIDx < element->GetDirectArray().GetCount()) {
			*out = FromFbxVector2ToVector2(element->GetDirectArray().GetAt(elemIDx));
			return true;
		}
	} break;

	case FbxGeometryElement::eIndexToDirect: {
		if (elemIDx < element->GetIndexArray().GetCount()) {
			int index = element->GetIndexArray().GetAt(elemIDx);
			*out = FromFbxVector2ToVector2(element->GetDirectArray().GetAt(index));
			return true;
		}
	} break;

	default:
		ASSERT_OR_DIE(nullptr, "Unknown reference type for FbxGetUV.");
		return false;
	}

	ASSERT_OR_DIE(nullptr, "Unknown reference type for FbxGetUV.");
	return false;
}

Vector2 FromFbxVector2ToVector2(const FbxVector2& fbxVector)
{
	return Vector2((float)fbxVector.mData[0], (float)fbxVector.mData[1]);
}

Vector4 FromFbxVector4ToVector4(const FbxVector4& fbxVector)
{
	return Vector4((float)fbxVector.mData[0], (float)fbxVector.mData[1], (float)fbxVector.mData[2], (float)fbxVector.mData[3]);
}

//------------------------------------------------------------------------
// So skin weights are by control point - so we'll start off
// by collecting them all up front before we even start building our mesh
void CalculateSkinWeights(std::vector<FBXSkinWeight_T>* skinWeights,
	const FbxMesh* mesh,
	const Skeleton* skeleton)
{
	// default them all to zero
	unsigned int controlCount = (unsigned int)mesh->GetControlPointsCount();
	skinWeights->reserve(controlCount);
	skinWeights->resize(controlCount);

	for (unsigned int i = 0; i < controlCount; ++i) {
		(*skinWeights)[i].Reset();
	}

	if (skeleton == nullptr) {
		skinWeights->clear();
		return;
	}

	// for each deformer [probably will only be one] - figure out how it affects
	// my vertices
	int deformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int didx = 0; didx < deformerCount; ++didx) {
		FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);

		if (nullptr == skin) {
			continue;
		}

		// cluster is how a single joint affects the mesh, 
		// so go over the vertices it affects and keep track of how much!
		int clusterCount = skin->GetClusterCount();
		for (int cidx = 0; cidx < clusterCount; ++cidx) {
			FbxCluster* cluster = skin->GetCluster(cidx);
			const FbxNode* linkNode = cluster->GetLink();

			// Can't use it without a link node [shouldn't happen!]
			if (nullptr == linkNode) {
				continue;
			}

			// Find the joint - if this skeleton doesn't have a joint
			// then we skip this cluster!
			int jointIDx = skeleton->GetJointIndex(linkNode->GetName());

			if (jointIDx == INVALID_JOINT_INDEX) {
				continue;
			}

			// cluster stores things in an index buffer again,
			// it will store indices to the control points it affects, 
			// as well as how much weight we apply
			int* indices = cluster->GetControlPointIndices();
			int indexCount = cluster->GetControlPointIndicesCount();
			double* weights = cluster->GetControlPointWeights();

			for (int i = 0; i < indexCount; ++i) {
				int controlIDx = indices[i];
				double weight = weights[i];

				FBXSkinWeight_T* skinWeight = &(*skinWeights)[controlIDx];
				AddHighestWeight(skinWeight, jointIDx, (float)weight);
			}
		}
	}
}
 

//------------------------------------------------------------------------
// Keep track of this influencer if it influences more than an already existing
// bone. [all bones influence at 0 at the start]
void AddHighestWeight(FBXSkinWeight_T* skinWeight, int boneIDx, float weight)
{
	if (boneIDx < 0) {
		return;
	}

	float lowestWeight = skinWeight->weights.m_components[0];
	int lowestIndex = 0;

	for (int weightIndex = 1; weightIndex < 4; ++weightIndex) {
		if (skinWeight->weights.m_components[weightIndex] < lowestWeight) {
			lowestWeight = skinWeight->weights.m_components[weightIndex];
			lowestIndex = weightIndex;
		}
	}

	if (weight > lowestWeight) {
		skinWeight->indices.m_components[lowestIndex] = boneIDx;
		skinWeight->weights.m_components[lowestIndex] = weight;
	}
}

bool HasSkinWeights(const FbxMesh* mesh)
{
	int deformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	return deformerCount > 0;
}


//------------------------------------------------------------------------

//Loading a Skeleton

//------------------------------------------------------------------------
bool FbxLoadSkeleton(Skeleton* skel, const char* filename, float scale, const Vector3& translation)
{
	// Same as you did for Meshes.
	FbxScene* fbxScene = FbxLoadScene(filename, false);
	if (nullptr == fbxScene) {
		return false;
	}

	ConvertSceneToEngineBasis(fbxScene);

	// First, get the bind post for the scene
	skel->Clear();

	// Now, traverse the scene, and build the skeleton out with 
	// the bind pose positions
	FbxNode* root = fbxScene->GetRootNode();

	// Bind pose - the pose which assets are authored for.
	FbxPose* pose = GetBindPose(fbxScene);

	// Import the skeleton by traversing the scene.
	ImportSkeleton(skel, root, nullptr, nullptr, pose);

	// Flip the Axis 
	TransformSkeleton(skel, scale, translation);

	FbxUnloadScene(fbxScene);

	// Success if I've loaded in at least ONE bone/joint.
	return (skel->GetJointCount() > 0U);
}

void TransformSkeleton(Skeleton* skel, float scale, const Vector3& translation)
{
	for each(Joint* j in skel->m_joints){
		ScaleJointTransform(j, scale);
		TranslateJointTransform(j, translation);
	}
}

void ScaleJointTransform(Joint* j, float scale)
{
	Vector3 scaledVector = j->m_transform.m_translation.XYZ();
	scaledVector.ScaleUniform(scale);
	j->m_transform.m_translation = Vector4(scaledVector, 1.0f);
}

void TranslateJointTransform(Joint* j, const Vector3& translation)
{
	j->m_transform.m_translation += Vector4(translation, 0.f);
}

void FlipXAxis(Skeleton* skel)
{
	/*
	// Flips all geoemetry across the X axis. You just want to flip the X coordinate of all vertex
	// data (positions,normals/tangents/bitangents)
	*/
	for each(Joint* j in skel->m_joints){
		j->m_transform.m_iBasis *= -1.f;
	}
}

// Get the bind pose - that is, the pose that the resources are authored
// for. If none exists - we'll assume ONLY the bind pose exists, and use
// the global transformations.
FbxPose* GetBindPose(FbxScene *scene)
{
	int poseCount = scene->GetPoseCount();

	for (int i = 0; i < poseCount; ++i) {
		FbxPose* pose = scene->GetPose(i);

		if (pose->IsBindPose()) {
			// DEBUG - Print all nodes part of this pose
			int count = pose->GetCount();
			for (int j = 0; j < count; ++j) 
			{
				FbxNode *node = pose->GetNode(j);
				DebuggerPrintlnf("Pose Node: %s", node->GetName());
			}
		
			return pose;
		}
	}

	return nullptr;
}

// Imports the skeleton.
void ImportSkeleton(Skeleton* outSkeleton,  // skeleton I'm loading into
	FbxNode* node,							// Current node I'm looking at.
	FbxSkeleton* rootBone,					// The head bone in this skeleton (keeping track purely for future support for nested skeletons in an FBX)
	FbxSkeleton* parentBone,				// The bone to consider the parent of any subsequently loaded bone.
	FbxPose* pose)							// Pose we're loading.
{
	for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {
		// If this node is a skeleton node (a joint), 
		// then it should have a skeleton attribute. 
		FbxNodeAttribute* na = node->GetNodeAttributeByIndex(i);
		if (na->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
			// Cast it, and figure out the type of bone. 
			FbxSkeleton* skel = (FbxSkeleton*)na;
			FbxSkeleton::EType type = skel->GetSkeletonType();

			// From FBXSDK Documentation.
			// eRoot,			/*!< First element of a chain. */
			// eLimb,			/*!< Chain element. */
			// eLimbNode,		/*!< Chain element. */
			// eEffector		/*!< Last element of a chain. */

			// If this is a root - we better have not have another root higher 
			// up (for now).  This is just here to catch this happening so
			// I know if I should support it in the future - you could cut this.
			if ((type == FbxSkeleton::eRoot) || (type == FbxSkeleton::eEffector)) {
				// this is a root bone - so can treat it as the root
				// of a skeleton.
				// TODO: no support for nested skeletons yet.
				ASSERT_OR_DIE(rootBone, "Root bone is null.");
				rootBone = skel;
			} // else, it is a limb or limb node, which we treat the same

			  // See if this bone exists in the pose, and if so, 
			  // get its index in the pose.
			int poseNodeIDx = -1;

			if (pose != nullptr) {
				poseNodeIDx = pose->Find(node);
			}

			// Get the global transform for this bone
			Matrix4 boneTransform = Matrix4::IDENTITY;

			if (poseNodeIDx != -1) {
				// If it exists in the pose - use the poses transform.
				// Since we're only doing the bind pose now, the pose should ONLY
				// be storing global transforms (as per the documentation).
				//
				// When we update this code to load in animated poses, this will change.
				bool isLocal = pose->IsLocalMatrix(poseNodeIDx);
				ASSERT_OR_DIE(!isLocal, "Poses are local. Needs to be global");// for now, should always be global (pose transforms always are)

				FbxMatrix fbxMatrix = pose->GetMatrix(poseNodeIDx);
				boneTransform = ToEngineMatrix(fbxMatrix);
			}
			else {
				// Not in the pose - so we have two options
				if (parentBone == nullptr) {
					// We have no parent [first in the skeleton], so just use 
					// the global transform of the node.
					boneTransform = ToEngineMatrix(node->EvaluateGlobalTransform());
				}
				else {
					// Otherwise, calculate my world based on my parent transform
					// and my nodes local.
					
					// The reason behind this is EvaluateGlobalTransform actually
					// gets the animated transform, while getting the Pose transform
					// will get an unanimated transform.  So if we're a child of a transform
					// that was gotten from a pose, I should be evaluating off that.
					Matrix4 local = ToEngineMatrix(node->EvaluateLocalTransform());
					Matrix4 parentTransform = outSkeleton->GetJointTransform(GetBoneName(parentBone));
					boneTransform = parentTransform * local;
				}
			}

			// Add a joint.
			const char* boneName = GetBoneName(skel);
			const char* parentBoneName = (parentBone != nullptr) ? GetBoneName(parentBone) : "";
			outSkeleton->AddJoint(boneName, parentBoneName, boneTransform);

			// set this as the next nodes parent, and continue down the chain
			parentBone = skel;
			break;
		}
	}

	// import the rest
	for (int i = 0; i < node->GetChildCount(); ++i) {
		ImportSkeleton(outSkeleton, node->GetChild(i), rootBone, parentBone, pose);
	}
}

//------------------------------------------------------------------------
// Just a helper - checks for null - but otherwise gets the node name
static const char* GetNodeName(FbxNode const *node)
{
	if (nullptr == node) {
		return "";
	}
	else {
		return node->GetName();
	}
}


//------------------------------------------------------------------------
// Get the name to use for this bone (ideally, use the node, but if the bone
// doesn't exist, use the skeleton attributes name)
static const char* GetBoneName(const FbxSkeleton* skel)
{
	if (nullptr == skel) {
		return nullptr;
	}

	const char* nodeName = GetNodeName(skel->GetNode());
	if (!IsStringNullOrEmpty(nodeName)) {
		return skel->GetNode()->GetName();
	}
	else {
		return skel->GetName();
	}
}



//------------------------------------------------------------------------

//Loading Motion

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Just as an extra - if you want to use the FBX files frame rate, this
// is how you can fetch it.
static float GetNativeFrameRate(const FbxScene* scene)
{
	// Get the scenes authored framerate
	
	const FbxGlobalSettings& settings = scene->GetGlobalSettings();
	FbxTime::EMode timeMode = settings.GetTimeMode();

	double framerate;
	if (timeMode == FbxTime::eCustom) {
		framerate = settings.GetCustomFrameRate();
	}
	else {
		framerate = FbxTime::GetFrameRate(timeMode);
	}

	return (float)framerate;
}

//------------------------------------------------------------------------
bool FbxLoadMotion(Motion* motion, const Skeleton* skeleton, const char* filename, float scale, const Vector3& translation, unsigned int framerate)
{
	// Normal things
	FbxScene* scene = FbxLoadScene(filename, false);

	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Framerate - how often are we sampling this
	float fr = (float)framerate;


	// Next get the animation count.
	int animCount = scene->GetSrcObjectCount<FbxAnimStack>();
	if (animCount > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack* anim = scene->GetSrcObject<FbxAnimStack>(0);
		FbxImportMotion(motion, skeleton, scene, anim, fr);
	}
	
	TransformMotion(motion, scale, translation);

	FbxUnloadScene(scene);

	return (motion->GetDuration() > 0.0f);
}

void TransformMotion(Motion* motion, float scale, const Vector3& translation)
{
	Matrix4 minusXTransform = Matrix4::IDENTITY;
	minusXTransform.SetIBasis(-minusXTransform.GetIBasis());

	unsigned int frameCount = motion->GetFrameCount();

	for (unsigned int frameIDx = 0U; frameIDx < frameCount; ++frameIDx) {
		Pose* pose = motion->GetPoseAtFrame(frameIDx);

		for (unsigned int i = 0; i < pose->m_localTransforms.size(); ++i) {
			PoseTransform_T* localTransform = &pose->m_localTransforms[i];

			//Scaling
			localTransform->m_position *= scale;	
		}

		//Translation
		pose->m_localTransforms[0].m_position += translation;
	}
}

//------------------------------------------------------------------------
static bool FbxImportMotion(Motion* motion, const Skeleton* skeleton, FbxScene* scene, FbxAnimStack* anim, float framerate)
{
	// First, set the scene to use this animation - helps all the evaluation functions
	scene->SetCurrentAnimationStack(anim);

	// how far do we move through this
	FbxTime advance;
	advance.SetSecondDouble((double)(1.0f / framerate));

	// Get the start, end, and duration of this animation.
	FbxTime localStart = anim->LocalStart;
	FbxTime localEnd = anim->LocalStop;
	float localStartFloat = (float)localStart.GetSecondDouble();
	float localEndFl = (float)localEnd.GetSecondDouble();

	// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
	// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
	// Whatever, we'll always start at least 0.
	localStartFloat = Max(0.0f, localStartFloat);
	float durationFloat = localEndFl - localStartFloat;

	// no duration, no animation!
	if (durationFloat <= 0.0f) {
		return false;
	}

	// Alright, we have a motion, we start getting data from it
	const char* motionName = anim->GetName();
	// float timeSpan = durationFloat;

	// set some identifying information
	motion->SetName(motionName);
	motion->SetDuration(durationFloat);

	// number of frames should encompasses the duration, so say we are at 10hz,
	// but have an animation that is 0.21f seconds.  We need at least...
	//    3 intervals (Ceiling(10 * 0.21) = Ceiling(2.1) = 3)
	//    4 frames (0.0, 0.1, 0.2, 0.3)
	unsigned int frameCount = (unsigned int)Ceiling(durationFloat * framerate) + 1;


	// Now, for each joint in our skeleton
	// not the most efficient way to go about this, but whatever - tool step
	// and it is not slow enough to matter.
	unsigned int jointCount = skeleton->GetJointCount();

	for each (Pose* pose in motion->m_framePoses) {
		pose->SetLocalTransformLength(jointCount);
	}

	for (unsigned int jointIDx = 0; jointIDx < jointCount; ++jointIDx) {
		std::string name = skeleton->GetJointName(jointIDx);

		// get the node associated with this bone
		// I rely on the names to find this - so author of animations
		// should be sure to be using the same rig/rig names.
		// (this is only important for the import process, after that
		// everything is assuming matching indices)
		const char* nodeName = name.c_str();
		FbxNode* node = scene->FindNodeByName(nodeName);
		FbxNode* parent = nullptr;
		int parentIDx = skeleton->GetJointParentID(jointIDx);
		ASSERT_OR_DIE(node != nullptr, "Node is null when importing motion in FbxImportMotion().");

		// get the parent node
		if (parentIDx != INVALID_JOINT_INDEX) {
			std::string parentName = skeleton->GetJointName(parentIDx);
			parent = scene->FindNodeByName(parentName.c_str());
			ASSERT_OR_DIE(parent != nullptr, "Parent node is null when importing motion in FbxImportMotion().");
		}

		// Now, for this entire animation, evaluate the local transform for this bone at every interval
		// number of frames is 
		FbxTime evalTime = FbxTime(0);
		for (unsigned int frameIDx = 0; frameIDx < frameCount; ++frameIDx) {
			// Okay, get the pose we want
			Pose* pose = motion->GetPoseAtFrame(frameIDx);
			Matrix4 jointWorld = GetNodeWorldTransformAtTime(node, evalTime);
			Matrix4 jointLocal = jointWorld;
			if (nullptr != parent) {
				Matrix4 parentWorld = GetNodeWorldTransformAtTime(parent, evalTime);
				jointLocal = parentWorld.GetInverse() * jointWorld;
			}

			pose->m_localTransforms[jointIDx].SetLocalTransform(jointLocal);

			//Advance the clock
			evalTime += advance;
		
		}
	}

	return true;
}

//------------------------------------------------------------------------
Matrix4 GetNodeWorldTransformAtTime(FbxNode* node, FbxTime time)
{
	if (nullptr == node) {
		return Matrix4::IDENTITY;
	}

	FbxMatrix fbxMat = node->EvaluateGlobalTransform(time);
	return ToEngineMatrix(fbxMat);
}

#else 

// Stubs so that if we don't have the FBX SDK Available, our program does not crash
void FbxListFile(const char*) {}
bool FbxLoadMesh(MeshBuilder*, const char*, float, const Skeleton*) { return false;  }
bool FbxLoadSkeleton(Skeleton*, const char*, float, const Vector3&) { return false; }
bool FbxLoadMotion(Motion*, const Skeleton*, const char*, float, const Vector3&, unsigned int) { return false; }

#endif
