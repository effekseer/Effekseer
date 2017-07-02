
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

	FbxVector4 FBXConverter::LoadNormal(FbxLayerElementNormal* normals, int32_t vertexID, int32_t ctrlPointIndex)
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

		return normal;
	}

	FbxVector2 FBXConverter::LoadUV(FbxMesh* fbxMesh, FbxLayerElementUV* uvs, int32_t vertexID, int32_t ctrlPointIndex, int32_t polygonIndex, int32_t polygonPointIndex)
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

	FbxColor FBXConverter::LoadVertexColor(FbxMesh* fbxMesh, FbxLayerElementVertexColor* colors, int32_t vertexID, int32_t ctrlPointIndex, int32_t polygonIndex, int32_t polygonPointIndex)
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

	std::shared_ptr<Mesh> FBXConverter::LoadMesh(FbxMesh* fbxMesh)
	{
		assert(fbxMesh->GetLayerCount() > 0);

		auto node = fbxMesh->GetNode();
		auto layer = fbxMesh->GetLayer(0);
		auto uvs = layer->GetUVs();
		auto vcolors = layer->GetVertexColors();
		auto normals = layer->GetNormals();
		auto binormals = layer->GetBinormals();
		auto materials = layer->GetMaterials();

		auto controlPoints = fbxMesh->GetControlPoints();
		auto controlPointsCount = fbxMesh->GetControlPointsCount();

		auto polygonCount = fbxMesh->GetPolygonCount();

		std::vector<FbxFace> faces;

		// ポリゴン走査
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
				
				if (normals != nullptr)
				{
					v.Normal = LoadNormal(normals, vertexID, ctrlPointIndex);
				}

				if (uvs != nullptr)
				{
					v.UV = LoadUV(fbxMesh, uvs, vertexID, ctrlPointIndex, polygonIndex, polygonPointIndex);
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
			if (face.Vertecies.size() != 3) continue;

			for (auto& vertex : face.Vertecies)
			{
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

		mesh->Vertexes.resize(vInd);
		for (auto& iv : ind2v)
		{
			mesh->Vertexes[iv.first] = iv.second;
		}

		for (auto& face : faces)
		{
			if (face.Vertecies.size() != 3) continue;

			Face f;
			f.Index[0] = v2ind[face.Vertecies[0]];
			f.Index[1] = v2ind[face.Vertecies[1]];
			f.Index[2] = v2ind[face.Vertecies[2]];
			mesh->Faces.push_back(f);
		}

		// Binormal,Tangent計算
		std::map<int32_t, VertexNormals> vInd2Normals;

		for (const auto& face : mesh->Faces)
		{
			FbxVector4 binormal, tangent;
			CalcTangentSpace(
				mesh->Vertexes[face.Index[0]],
				mesh->Vertexes[face.Index[1]],
				mesh->Vertexes[face.Index[2]],
				binormal,
				tangent);

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
			//mesh->Vertexes[vn.first].Tangent = vn.second.Tangent;

			// 適当な値を代入する
			if (mesh->Vertexes[vn.first].Binormal.Length() == 0.0f)
			{
				if (mesh->Vertexes[vn.first].Normal != FbxVector4(1, 0, 0))
				{
					mesh->Vertexes[vn.first].Binormal = FbxVector4(1, 0, 0);
				}
				else
				{
					mesh->Vertexes[vn.first].Binormal = FbxVector4(0, 1, 0);
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

				if (!mesh->IsTriangleMesh())
				{
					FbxGeometryConverter converter(fbxManager);
					mesh = (FbxMesh*) converter.Triangulate(mesh, false);
				}

				node->MeshData = LoadMesh(mesh);
				break;
			case FbxNodeAttribute::eSkeleton:
				break;
			default:
				return std::shared_ptr<Node>();
				break;
			}
		}
		else
		{
			// ルート
		}

		// 子の走査
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

	std::shared_ptr<Scene> FBXConverter::LoadScene(FbxScene* fbxScene, FbxManager* fbxManager)
	{
		// 座標系変換
		FbxAxisSystem axis(FbxAxisSystem::eOpenGL);
		axis.ConvertScene(fbxScene);

		auto scene = std::make_shared<Scene>();

		// ノード
		auto root = fbxScene->GetRootNode();
		scene->Root = LoadHierarchy(nullptr, root, fbxManager);

		return scene;
	}
}