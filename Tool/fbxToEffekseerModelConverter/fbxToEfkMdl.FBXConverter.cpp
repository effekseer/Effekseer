
#include "fbxToEfkMdl.FBXConverter.h"

namespace fbxToEfkMdl
{
void FBXConverter::CalcTangentSpace(const Vertex& v1, const Vertex& v2, const Vertex& v3, FbxVector4& binormal, FbxVector4& tangent)
{
	binormal = FbxVector4();
	tangent = FbxVector4();

	FbxVector4 cp0[3];
	cp0[0] = FbxVector4(v1.Position[0], v1.UV[0], v1.UV[1]);
	cp0[1] = FbxVector4(v1.Position[1], v1.UV[0], v1.UV[1]);
	cp0[2] = FbxVector4(v1.Position[2], v1.UV[0], v1.UV[1]);

	FbxVector4 cp1[3];
	cp1[0] = FbxVector4(v2.Position[0], v2.UV[0], v2.UV[1]);
	cp1[1] = FbxVector4(v2.Position[1], v2.UV[0], v2.UV[1]);
	cp1[2] = FbxVector4(v2.Position[2], v2.UV[0], v2.UV[1]);

	FbxVector4 cp2[3];
	cp2[0] = FbxVector4(v3.Position[0], v3.UV[0], v3.UV[1]);
	cp2[1] = FbxVector4(v3.Position[1], v3.UV[0], v3.UV[1]);
	cp2[2] = FbxVector4(v3.Position[2], v3.UV[0], v3.UV[1]);

	double u[3];
	double v[3];

	for (int32_t i = 0; i < 3; i++)
	{
		auto v1 = cp1[i] - cp0[i];
		auto v2 = cp2[i] - cp1[i];
		auto abc = v1.CrossProduct(v2);

		if (abc[0] == 0.0f)
		{
			return;
		}
		else
		{
			u[i] = -abc[1] / abc[0];
			v[i] = -abc[2] / abc[0];
		}
	}

	tangent = FbxVector4(u[0], u[1], u[2]);
	tangent.Normalize();

	binormal = FbxVector4(v[0], v[1], v[2]);
	binormal.Normalize();
}

FbxVector4 FBXConverter::LoadPosition(FbxMesh* fbxMesh, int32_t ctrlPointIndex)
{
	FbxVector4 position;

	auto controlPoints = fbxMesh->GetControlPoints();

	position = controlPoints[ctrlPointIndex];

	return position;
}

FbxVector4 FBXConverter::LoadNormal(FbxMesh* fbxMesh, FbxLayerElementNormal* normals, int32_t vertexID, int32_t ctrlPointIndex)
{
	FbxVector4 normal;

	if (normals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (normals->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			normal = normals->GetDirectArray().GetAt(vertexID);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normals->GetIndexArray().GetAt(vertexID);
			normal = normals->GetDirectArray().GetAt(id);
		}
		break;
		}
	}
	else if (normals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		int id = 0;

		if (normals->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			id = ctrlPointIndex;
		}

		if (normals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			id = normals->GetIndexArray().GetAt(ctrlPointIndex);
		}

		normal = normals->GetDirectArray().GetAt(id);
	}

	return normal;
}

FbxVector2 FBXConverter::LoadUV(
	FbxMesh* fbxMesh, FbxLayerElementUV* uvs, int32_t vertexID, int32_t ctrlPointIndex, int32_t polygonIndex, int32_t polygonPointIndex)
{
	FbxVector2 uv;

	switch (uvs->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (uvs->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			uv = uvs->GetDirectArray().GetAt(ctrlPointIndex);
			break;
		}
		case FbxGeometryElement::eIndexToDirect:
		{
			auto id = uvs->GetIndexArray().GetAt(ctrlPointIndex);
			uv = uvs->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		auto textureUVIndex = fbxMesh->GetTextureUVIndex(polygonIndex, polygonPointIndex);
		switch (uvs->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			uv = uvs->GetDirectArray().GetAt(textureUVIndex);
			break;
		}
		}
		break;
	}
	default:
		break;
	}

	// 上下逆
	uv[1] = 1.0 - uv[1];

	return uv;
}

FbxColor FBXConverter::LoadVertexColor(FbxMesh* fbxMesh,
									   FbxLayerElementVertexColor* colors,
									   int32_t vertexID,
									   int32_t ctrlPointIndex,
									   int32_t polygonIndex,
									   int32_t polygonPointIndex)
{
	FbxColor color;

	switch (colors->GetMappingMode())
	{
	default:
		break;

	case FbxGeometryElement::eByControlPoint:
		switch (colors->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			color = colors->GetDirectArray().GetAt(ctrlPointIndex);
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			auto id = colors->GetIndexArray().GetAt(ctrlPointIndex);
			color = colors->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (colors->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			color = colors->GetDirectArray().GetAt(vertexID);
			break;

		case FbxGeometryElement::eIndexToDirect:
			auto id = colors->GetIndexArray().GetAt(vertexID);
			color = colors->GetDirectArray().GetAt(id);
			break;
		}

		break;
	}

	return color;
}

void FBXConverter::LoadSkin(FbxMesh* fbxMesh, std::vector<BoneConnector>& bcs, std::vector<Vertex>& vs)
{
	vs.resize(fbxMesh->GetControlPointsCount());

	auto skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (auto skinInd = 0; skinInd < skinCount; skinInd++)
	{
		auto skin = (FbxSkin*)fbxMesh->GetDeformer(skinInd, FbxDeformer::eSkin);

		auto clusterCount = skin->GetClusterCount();
		for (auto clusterInd = 0; clusterInd < clusterCount; clusterInd++)
		{
			auto cluster = skin->GetCluster(clusterInd);
			if (cluster->GetLink() == nullptr)
				continue;

			// get bone
			auto name = cluster->GetLink()->GetName();

			FbxAMatrix m1, m2;
			cluster->GetTransformMatrix(m1);
			cluster->GetTransformLinkMatrix(m2);

			int32_t id = bcs.size();

			BoneConnector connector;
			connector.Name = name;

			auto m2_inv = m2.Inverse();
			auto m = m2_inv * m1;

			connector.OffsetMatrix = m;

			bcs.push_back(connector);

			auto indexCount = cluster->GetControlPointIndicesCount();
			auto vindices = cluster->GetControlPointIndices();
			auto vweights = cluster->GetControlPointWeights();

			for (auto ind = 0; ind < indexCount; ind++)
			{
				Weight data;
				data.Index = id;
				data.Value = (float)vweights[ind];

				vs[vindices[ind]].Weights.push_back(data);
			}
		}
	}

	// Calculate weight
	for (auto& v : vs)
	{
		if (v.Weights.size() == 0)
		{
			Weight w;
			w.Index = -1;
			w.Value = 1.0;
			v.Weights.push_back(w);

			v.Weights.push_back(Weight());
			v.Weights.push_back(Weight());
			v.Weights.push_back(Weight());
		}
		else
		{

			v.Weights.push_back(Weight());
			v.Weights.push_back(Weight());
			v.Weights.push_back(Weight());

			std::sort(v.Weights.begin(), v.Weights.end(), Weight());

			float fSum = 0.0f;
			for (auto ind = 0; ind < 4; ind++)
			{
				fSum += v.Weights[ind].Value;
			}

			v.Weights[0].Value += 1.0f - fSum;
			v.Weights.resize(4);
		}
	}
}

std::shared_ptr<Mesh> FBXConverter::LoadMesh(FbxMesh* fbxMesh)
{
	assert(fbxMesh->GetLayerCount() > 0);

	auto node = fbxMesh->GetNode();
	auto layer = fbxMesh->GetLayer(0);

	if (layer->GetNormals() == nullptr)
	{
		fbxMesh->GenerateNormals(true);
	}

	auto uvs = layer->GetUVs();
	auto vcolors = layer->GetVertexColors();
	auto normals = layer->GetNormals();
	auto binormals = layer->GetBinormals();
	auto materials = layer->GetMaterials();

	auto controlPoints = fbxMesh->GetControlPoints();
	auto controlPointsCount = fbxMesh->GetControlPointsCount();

	auto polygonCount = fbxMesh->GetPolygonCount();

	std::vector<FbxFace> faces;

	// Load weights
	std::vector<BoneConnector> bcs_temp;
	std::vector<Vertex> vs_temp;
	LoadSkin(fbxMesh, bcs_temp, vs_temp);

	// generate face vertex
	int32_t vertexID = 0;
	for (int32_t polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
	{
		int polygonPointCount = fbxMesh->GetPolygonSize(polygonIndex);

		FbxFace face;

		for (int32_t polygonPointIndex = 0; polygonPointIndex < polygonPointCount; polygonPointIndex++)
		{
			auto ctrlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, polygonPointIndex);

			Vertex v;

			v.Position = LoadPosition(fbxMesh, ctrlPointIndex);

			v.Weights = vs_temp[ctrlPointIndex].Weights;

			if (normals != nullptr)
			{
				v.Normal = LoadNormal(fbxMesh, normals, vertexID, ctrlPointIndex);
			}

			if (uvs != nullptr)
			{
				v.UV = LoadUV(fbxMesh, uvs, vertexID, ctrlPointIndex, polygonIndex, polygonPointIndex);
			}
			else
			{
				// Auto generated
				v.UV[0] = v.Position[0] + v.Position[2];
				v.UV[1] = v.Position[1];
			}

			if (vcolors != nullptr)
			{
				v.VertexColor = LoadVertexColor(fbxMesh, vcolors, vertexID, ctrlPointIndex, polygonIndex, polygonPointIndex);
			}

			face.Vertecies.push_back(v);
			vertexID++;
		}

		faces.push_back(face);
	}

	// 面の表裏入れ替え
	for (auto& face : faces)
	{
		std::reverse(face.Vertecies.begin(), face.Vertecies.end());
	}

	// メッシュで使用可能な形式に変換

	// 頂点変換テーブル作成
	int32_t vInd = 0;
	std::map<Vertex, int32_t> v2ind;
	std::map<int32_t, Vertex> ind2v;

	for (auto& face : faces)
	{
		for (int32_t vi = 0; vi < (int32_t)face.Vertecies.size(); vi++)
		{
			auto vertex = face.Vertecies[vi];

			auto it = v2ind.find(vertex);
			if (it == v2ind.end())
			{
				v2ind[vertex] = vInd;
				ind2v[vInd] = vertex;
				vInd++;
			}
		}
	}

	// 設定
	auto mesh = std::make_shared<Mesh>();
	mesh->Name = node->GetName();
	mesh->BoneConnectors = bcs_temp;

	mesh->Vertexes.resize(vInd);
	for (auto& iv : ind2v)
	{
		mesh->Vertexes[iv.first] = iv.second;
	}

	for (auto& face : faces)
	{
		if (face.Vertecies.size() < 3)
			continue;

		if (face.Vertecies.size() == 3)
		{
			Face f;
			f.Index[0] = v2ind[face.Vertecies[0]];
			f.Index[1] = v2ind[face.Vertecies[1]];
			f.Index[2] = v2ind[face.Vertecies[2]];
			mesh->Faces.push_back(f);
		}

		if (face.Vertecies.size() == 4)
		{
			Face f0;
			f0.Index[0] = v2ind[face.Vertecies[0]];
			f0.Index[1] = v2ind[face.Vertecies[1]];
			f0.Index[2] = v2ind[face.Vertecies[2]];
			mesh->Faces.push_back(f0);

			Face f1;
			f1.Index[0] = v2ind[face.Vertecies[0]];
			f1.Index[1] = v2ind[face.Vertecies[2]];
			f1.Index[2] = v2ind[face.Vertecies[3]];
			mesh->Faces.push_back(f1);
		}
	}

	// Binormal,Tangent計算
	std::map<int32_t, VertexNormals> vInd2Normals;

	for (const auto& face : mesh->Faces)
	{
		FbxVector4 binormal, tangent;
		CalcTangentSpace(mesh->Vertexes[face.Index[0]], mesh->Vertexes[face.Index[1]], mesh->Vertexes[face.Index[2]], binormal, tangent);

		for (auto i = 0; i < 3; i++)
		{
			vInd2Normals[face.Index[i]].Binormal += binormal;
			vInd2Normals[face.Index[i]].Tangent += tangent;
			vInd2Normals[face.Index[i]].Count += 1;
		}
	}

	for (auto& vn : vInd2Normals)
	{
		vn.second.Binormal /= vn.second.Count;
		vn.second.Tangent /= vn.second.Count;
	}

	for (auto& vn : vInd2Normals)
	{
		mesh->Vertexes[vn.first].Binormal = vn.second.Binormal;
		mesh->Vertexes[vn.first].Tangent = vn.second.Tangent;

		// 適当な値を代入する
		if (mesh->Vertexes[vn.first].Binormal.Length() == 0.0f)
		{
			if (mesh->Vertexes[vn.first].Normal != FbxVector4(1, 0, 0))
			{
				mesh->Vertexes[vn.first].Binormal = FbxVector4(1, 0, 0);
				mesh->Vertexes[vn.first].Tangent = FbxVector4(0, 1, 0);
			}
			else
			{
				mesh->Vertexes[vn.first].Binormal = FbxVector4(0, 1, 0);
				mesh->Vertexes[vn.first].Tangent = FbxVector4(1, 0, 0);
			}
		}
	}

	return mesh;
}

std::shared_ptr<Node> FBXConverter::LoadHierarchy(std::shared_ptr<Node> parent, FbxNode* fbxNode, FbxManager* fbxManager)
{
	FbxMesh* mesh = nullptr;
	std::shared_ptr<Node> node = std::make_shared<Node>();

	node->Name = fbxNode->GetName();

	auto attribute_ = fbxNode->GetNodeAttribute();

	if (attribute_ != nullptr)
	{
		auto attributeType = attribute_->GetAttributeType();
		switch (attributeType)
		{
		case FbxNodeAttribute::eMesh:
			mesh = fbxNode->GetMesh();

			if (mesh->GetLayerCount() > 0)
			{
				if (!mesh->IsTriangleMesh())
				{
					FbxGeometryConverter converter(fbxManager);
					auto mesh_tri = (FbxMesh*)converter.Triangulate(mesh, false);

					if (mesh_tri != nullptr)
					{
						mesh = mesh_tri;
					}
				}

				node->MeshData = LoadMesh(mesh);
			}
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		default:
			break;
		}
	}
	else
	{
		// This is root.
	}

	// load transforms
	EFbxRotationOrder fbxRotationOrder;
	fbxNode->GetRotationOrder(FbxNode::eDestinationPivot, fbxRotationOrder);
	node->RotationOrder = fbxRotationOrder;

	// default transform
	auto lclT = fbxNode->LclTranslation.Get();
	auto lclR = fbxNode->LclRotation.Get();
	auto lclS = fbxNode->LclScaling.Get();

	node->Translation[0] = lclT[0];
	node->Translation[1] = lclT[1];
	node->Translation[2] = lclT[2];

	node->Rotation[0] = lclR[0];
	node->Rotation[1] = lclR[1];
	node->Rotation[2] = lclR[2];

	node->Scaling[0] = lclS[0];
	node->Scaling[1] = lclS[1];
	node->Scaling[2] = lclS[2];

	node->RotationPivot = fbxNode->GetRotationPivot(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	node->RotationOffset = fbxNode->GetRotationOffset(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	node->ScalingPivot = fbxNode->GetScalingPivot(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	node->ScalingOffset = fbxNode->GetScalingOffset(fbxsdk::FbxNode::EPivotSet::eSourcePivot);

	for (auto i = 0; i < fbxNode->GetChildCount(); i++)
	{
		auto childNode = LoadHierarchy(node, fbxNode->GetChild(i), fbxManager);
		if (childNode != nullptr)
		{
			node->Children.push_back(childNode);
		}
	}

	return node;
}

std::shared_ptr<KeyFrameAnimation> FBXConverter::LoadCurve(FbxAnimCurve* curve, int32_t frameStart, int32_t frameCount)
{
	auto keyFrameAnimation = std::make_shared<KeyFrameAnimation>();

	for (int32_t i = 0; i < frameCount; i++)
	{
		FbxTime time;
		time.SetFrame(i + frameStart, FbxTime::eFrames60);
		auto value = curve->Evaluate(time);
		keyFrameAnimation->Values.push_back(value);
	}

	return keyFrameAnimation;
}

std::vector<std::shared_ptr<KeyFrameAnimation>>
FBXConverter::LoadCurve(FbxAnimLayer* fbxAnimLayer, FbxNode* fbxNode, int32_t frameStart, int32_t frameCount)
{
	std::vector<std::shared_ptr<KeyFrameAnimation>> ret;
	auto boneName = fbxNode->GetName();
	auto transXCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	auto transYCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	auto transZCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	auto rotXCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	auto rotYCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	auto rotZCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	auto rot = fbxNode->LclRotation.GetCurveNode(fbxAnimLayer);
	auto lclR = fbxNode->LclRotation.Get();
	auto defRotX = lclR[0];
	auto defRotY = lclR[1];
	auto defRotZ = lclR[2];
	if (rot != nullptr)
	{
		for (size_t i = 0; i < rot->GetChannelsCount(); i++)
		{
			auto name = rot->GetChannelName(i);
			if (name == "X")
			{
				defRotX = rot->GetChannelValue(name, defRotX);
			}
			if (name == "Y")
			{
				defRotY = rot->GetChannelValue(name, defRotY);
			}
			if (name == "Z")
			{
				defRotZ = rot->GetChannelValue(name, defRotZ);
			}
		}
	}

	auto sclXCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	auto sclYCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	auto sclZCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (transXCurve != nullptr)
	{
		auto c = LoadCurve(transXCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::TX;
		ret.push_back(c);
	}

	if (transYCurve != nullptr)
	{
		auto c = LoadCurve(transYCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::TY;
		ret.push_back(c);
	}

	if (transZCurve != nullptr)
	{
		auto c = LoadCurve(transZCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::TZ;
		ret.push_back(c);
	}

	if (rotXCurve != nullptr)
	{
		auto c = LoadCurve(rotXCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::RX;
		ret.push_back(c);
	}

	if (rotYCurve != nullptr)
	{
		auto c = LoadCurve(rotYCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::RY;
		ret.push_back(c);
	}

	if (rotZCurve != nullptr)
	{
		auto c = LoadCurve(rotZCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::RZ;
		ret.push_back(c);
	}

	if (sclXCurve != nullptr)
	{
		auto c = LoadCurve(sclXCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::SX;
		ret.push_back(c);
	}

	if (sclYCurve != nullptr)
	{
		auto c = LoadCurve(sclYCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::SY;
		ret.push_back(c);
	}

	if (sclZCurve != nullptr)
	{
		auto c = LoadCurve(sclZCurve, frameStart, frameCount);
		c->Name = boneName;
		c->Target = AnimationTarget::SZ;
		ret.push_back(c);
	}

	for (auto i = 0; i < fbxNode->GetChildCount(); i++)
	{
		auto kfas = LoadCurve(fbxAnimLayer, fbxNode->GetChild(i), frameStart, frameCount);

		for (auto a : kfas)
		{
			ret.push_back(a);
		}
	}

	return ret;
}

std::shared_ptr<AnimationClip> FBXConverter::LoadAnimation(FbxAnimStack* fbxAnimStack, FbxNode* fbxRootNode)
{
	auto animClip = std::make_shared<AnimationClip>();

	const int32_t layerCount = fbxAnimStack->GetMemberCount<FbxAnimLayer>();
	if (layerCount == 0)
		return std::shared_ptr<AnimationClip>();

	auto animationName = fbxAnimStack->GetName();

	FbxTime startTime = fbxAnimStack->LocalStart;
	FbxTime endTime = fbxAnimStack->LocalStop;

	int hour, minute, second, frame, field, residual;

	startTime.GetTime(hour, minute, second, frame, field, residual, FbxTime::eFrames60);
	auto startFrame = 60 * (hour * 60 * 60 + minute * 60 + second) + frame;

	endTime.GetTime(hour, minute, second, frame, field, residual, FbxTime::eFrames60);
	auto endFrame = 60 * (hour * 60 * 60 + minute * 60 + second) + frame;

	animClip->Name = animationName;
	animClip->StartFrame = startFrame;
	animClip->EndFrame = endFrame;

	for (auto i = 0; i < layerCount; ++i)
	{
		auto layer = fbxAnimStack->GetMember<FbxAnimLayer>();
		auto kfas = LoadCurve(layer, fbxRootNode, animClip->StartFrame, animClip->EndFrame - animClip->StartFrame);

		for (auto a : kfas)
		{
			animClip->Animations.push_back(a);
		}
	}

	animClip->EndFrame = animClip->EndFrame - animClip->StartFrame;
	animClip->StartFrame = 0;

	return animClip;
}

std::shared_ptr<Scene> FBXConverter::LoadScene(FbxScene* fbxScene, FbxManager* fbxManager)
{
	// convert a coordinate system
	FbxAxisSystem axis(FbxAxisSystem::eOpenGL);
	axis.ConvertScene(fbxScene);

	auto scene = std::make_shared<Scene>();

	// import nodes
	auto root = fbxScene->GetRootNode();
	scene->Root = LoadHierarchy(nullptr, root, fbxManager);

	// import animations
	auto animStackCount = fbxScene->GetSrcObjectCount<FbxAnimStack>();

	for (auto animStackIndex = 0; animStackIndex < animStackCount; animStackIndex++)
	{
		auto animStack = fbxScene->GetSrcObject<FbxAnimStack>(animStackIndex);

		fbxScene->SetCurrentAnimationStack(animStack);
		auto animClip = LoadAnimation(animStack, root);

		if (animClip.get() == nullptr)
			continue;
		scene->AnimationClips.push_back(animClip);
	}

	return scene;
}
} // namespace fbxToEfkMdl