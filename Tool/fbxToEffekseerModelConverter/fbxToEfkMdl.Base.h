
#pragma once

#include <fbxsdk.h>

#include <algorithm>
#include <assert.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fbxToEfkMdl
{
class FBXConverter;
class Scene;
class Node;
class Mesh;
class BoneConnector;

class Node
{
public:
	std::string Name;
	std::shared_ptr<Mesh> MeshData;

	EFbxRotationOrder RotationOrder;
	float Translation[3];
	float Rotation[4];
	float Scaling[3];

	std::vector<std::shared_ptr<Node>> Children;
};

class Weight
{
public:
	float Value = 0.0f;
	int32_t Index = -1;

	bool operator()(const Weight& lhs, const Weight& rhs) { return lhs.Value > rhs.Value; }
};

enum class AnimationTarget : int32_t
{
	TX,
	TY,
	TZ,

	RX,
	RY,
	RZ,

	SX,
	SY,
	SZ,
};

struct KeyFrameAnimation
{
	AnimationTarget Target;
	std::string Name;
	std::vector<float> Values;
};

struct AnimationClip
{
	std::string Name;
	int32_t StartFrame;
	int32_t EndFrame;
	std::vector<std::shared_ptr<KeyFrameAnimation>> Animations;
};

class BoneConnector
{
public:
	std::string Name;
	FbxMatrix OffsetMatrix;
};

struct Vertex
{
	FbxVector4 Position;
	FbxVector4 Normal;
	FbxVector4 Tangent;
	FbxVector4 Binormal;
	FbxVector2 UV;
	FbxColor VertexColor;

	std::vector<Weight> Weights;

	bool operator<(const Vertex& r) const
	{
		auto& l = *this;
		if (l.Position[0] != r.Position[0])
			return l.Position[0] < r.Position[0];
		if (l.Position[1] != r.Position[1])
			return l.Position[1] < r.Position[1];
		if (l.Position[2] != r.Position[2])
			return l.Position[2] < r.Position[2];

		if (l.Normal[0] != r.Normal[0])
			return l.Normal[0] < r.Normal[0];
		if (l.Normal[1] != r.Normal[1])
			return l.Normal[1] < r.Normal[1];
		if (l.Normal[2] != r.Normal[2])
			return l.Normal[2] < r.Normal[2];

		if (l.Tangent[0] != r.Tangent[0])
			return l.Tangent[0] < r.Tangent[0];
		if (l.Tangent[1] != r.Tangent[1])
			return l.Tangent[1] < r.Tangent[1];
		if (l.Tangent[2] != r.Tangent[2])
			return l.Tangent[2] < r.Tangent[2];

		if (l.Binormal[0] != r.Binormal[0])
			return l.Binormal[0] < r.Binormal[0];
		if (l.Binormal[1] != r.Binormal[1])
			return l.Binormal[1] < r.Binormal[1];
		if (l.Binormal[2] != r.Binormal[2])
			return l.Binormal[2] < r.Binormal[2];

		if (l.UV[0] != r.UV[0])
			return l.UV[0] < r.UV[0];
		if (l.UV[1] != r.UV[1])
			return l.UV[1] < r.UV[1];

		if (l.VertexColor.mRed != r.VertexColor.mRed)
			return l.VertexColor.mRed < r.VertexColor.mRed;
		if (l.VertexColor.mGreen != r.VertexColor.mGreen)
			return l.VertexColor.mGreen < r.VertexColor.mGreen;
		if (l.VertexColor.mBlue != r.VertexColor.mBlue)
			return l.VertexColor.mBlue < r.VertexColor.mBlue;
		if (l.VertexColor.mAlpha != r.VertexColor.mAlpha)
			return l.VertexColor.mAlpha < r.VertexColor.mAlpha;

		return false;
	}
};

struct Face
{
	int32_t Index[3];

	Face()
	{
		Index[0] = -1;
		Index[1] = -1;
		Index[2] = -1;
	}
};

class Mesh
{
private:
public:
	std::string Name;

	std::vector<Vertex> Vertexes;
	std::vector<Face> Faces;

	std::vector<BoneConnector> BoneConnectors;
};

class Scene
{
public:
	std::shared_ptr<Node> Root;
	std::vector<std::shared_ptr<AnimationClip>> AnimationClips;
};

struct BoneConnectorState
{
	int32_t NodeIndex;
};

struct MeshState
{
	std::shared_ptr<Mesh> Target;
	std::shared_ptr<Node> MeshNode;
	std::vector<BoneConnectorState> Connectors;
};

} // namespace fbxToEfkMdl

namespace fbxToEfkMdl
{

struct NodeState
{
	float Values[9];
	std::shared_ptr<KeyFrameAnimation> Animations[9];

	FbxMatrix MatLocal;
	FbxMatrix MatGlobal;
	FbxMatrix MatGlobalDefault;
	std::shared_ptr<Node> ParentNode;
	std::shared_ptr<Node> TargetNode;

	void AssignDefaultValues()
	{
		for (int32_t j = 0; j < 9; j++)
		{
			if (j == 0)
				Values[j] = TargetNode->Translation[0];
			if (j == 1)
				Values[j] = TargetNode->Translation[1];
			if (j == 2)
				Values[j] = TargetNode->Translation[2];
			if (j == 3)
				Values[j] = TargetNode->Rotation[0];
			if (j == 4)
				Values[j] = TargetNode->Rotation[1];
			if (j == 5)
				Values[j] = TargetNode->Rotation[2];
			if (j == 6)
				Values[j] = TargetNode->Scaling[0];
			if (j == 7)
				Values[j] = TargetNode->Scaling[1];
			if (j == 8)
				Values[j] = TargetNode->Scaling[2];
		}
	}

	void CalculateLocalMatrix()
	{
		FbxRotationOrder ro;
		ro.SetOrder(TargetNode->RotationOrder);
		FbxVector4 rv;
		rv[0] = Values[(int32_t)AnimationTarget::RX];
		rv[1] = Values[(int32_t)AnimationTarget::RY];
		rv[2] = Values[(int32_t)AnimationTarget::RZ];
		rv[3] = 1.0f;
		FbxAMatrix rm;
		ro.V2M(rm, rv);
		auto q = rm.GetQ();
		FbxVector4 r;
		r.SetXYZ(q);

		FbxMatrix mat;

		FbxVector4 t;
		t[0] = Values[(int32_t)AnimationTarget::TX];
		t[1] = Values[(int32_t)AnimationTarget::TY];
		t[2] = Values[(int32_t)AnimationTarget::TZ];

		FbxVector4 s;
		s[0] = Values[(int32_t)AnimationTarget::SX];
		s[1] = Values[(int32_t)AnimationTarget::SY];
		s[2] = Values[(int32_t)AnimationTarget::SZ];

		mat.SetTRS(t, r, s);

		MatLocal = mat;
	}
};

} // namespace fbxToEfkMdl