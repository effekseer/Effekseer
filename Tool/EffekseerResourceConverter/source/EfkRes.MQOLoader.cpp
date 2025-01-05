#include <algorithm>
#include <mqoio_loader.h>
#include "EfkRes.Utils.h"
#include "EfkRes.MQOLoader.h"

namespace efkres
{

namespace
{

struct TriangleFace
{
	std::array<int, 3> Indexes;
	std::array<mqoio::Vector2D, 3> UV;
	std::array<mqoio::Color, 3> Colors;
	int MaterialIndex = -1;
};

Vec2 Convert(mqoio::Vector2D v)
{
	return { v.X, v.Y };
}

Vec3 Convert(mqoio::Vector3D v)
{
	return { v.X, v.Y, v.Z };
}

Vec4 Convert(mqoio::Vector4D v)
{
	return { v.X, v.Y, v.Z, v.W };
}

Vec4 Convert(mqoio::Color v)
{
	return { v.R, v.G, v.B, v.A };
}

}

std::optional<Model> MQOLoader::LoadModel(std::string_view filepath)
{
    mqoio::Loader mqoLoader;
    auto loadResult = mqoLoader.Load(std::string(filepath));
    if (!loadResult.has_value())
    {
        return std::nullopt;
    }
    mqoio::Data mqoData(std::move(loadResult.value()));

    Model model;

	for (auto& mqoObj : mqoData.Objects)
    {
		if (!mqoObj.Visible)
		{
			continue;
		}

        Mesh& mesh = model.meshes.emplace_back();
        
        std::vector<TriangleFace> triFaces;

		for (auto& mqoVertex : mqoObj.Vertexes)
		{
			auto& vertex = mesh.vertices.emplace_back();
			vertex.position = Convert(mqoVertex);
		}

        for (auto& mqoFace : mqoObj.Faces)
        {
			size_t vcount = mqoFace.Indexes.size();
			for (size_t v = 3; v <= vcount; v++)
			{
				size_t offset = v - 3;
				auto& triFace = triFaces.emplace_back();

				triFace.Indexes[0] = mqoFace.Indexes[0];
				triFace.Indexes[1] = mqoFace.Indexes[1 + offset];
				triFace.Indexes[2] = mqoFace.Indexes[2 + offset];

				if (!mqoFace.Colors.empty())
				{
					triFace.Colors[0] = mqoFace.Colors[0];
					triFace.Colors[1] = mqoFace.Colors[1 + offset];
					triFace.Colors[2] = mqoFace.Colors[2 + offset];
				}
				else
				{
					triFace.Colors[0] = mqoio::Color{ 1.0, 1.0, 1.0, 1.0 };
					triFace.Colors[1] = mqoio::Color{ 1.0, 1.0, 1.0, 1.0 };
					triFace.Colors[2] = mqoio::Color{ 1.0, 1.0, 1.0, 1.0 };
				}

				if (!mqoFace.UV.empty())
				{
					triFace.UV[0] = mqoFace.UV[0];
					triFace.UV[1] = mqoFace.UV[1 + offset];
					triFace.UV[2] = mqoFace.UV[2 + offset];
				}
			}
        }

		// 法線による分割
		{
			std::vector<Vec3> normals;
			normals.resize(triFaces.size());

			std::vector<std::vector<int>> vertexToFaceList;
			vertexToFaceList.resize(mesh.vertices.size());

			// 法線を求める+所属頂点を設定
			for (int faceIndex = 0; faceIndex < triFaces.size(); faceIndex++)
			{
				auto& face = triFaces[faceIndex];
				int vidx0 = face.Indexes[0];
				int vidx1 = face.Indexes[1];
				int vidx2 = face.Indexes[2];

				auto v1 = mesh.vertices[vidx0].position - mesh.vertices[vidx1].position;
				auto v2 = mesh.vertices[vidx2].position - mesh.vertices[vidx1].position;
				normals[faceIndex] = CrossProduct(v1, v2).Normalized();

				vertexToFaceList[vidx0].push_back(faceIndex);
				vertexToFaceList[vidx1].push_back(faceIndex);
				vertexToFaceList[vidx2].push_back(faceIndex);
			}

			// 面の法線から頂点分離
			for (int vertexIndex = 0; vertexIndex < vertexToFaceList.size(); vertexIndex++)
			{
				auto& faceIndexList = vertexToFaceList[vertexIndex];

				std::vector<std::vector<int>> faceGroups;

				// 分別
				for (int faceIndex : faceIndexList)
				{
					bool setted = false;
					for (auto& faceGroup : faceGroups)
					{
						Vec3 cross = CrossProduct(normals[faceGroup[0]], normals[faceIndex]);
						double s = cross.Length();
						double c = DotProduct(normals[faceGroup[0]], normals[faceIndex]);

						if (atan2(s, c) < mqoObj.Facet / 180.0 * 3.14)
						{
							faceGroup.push_back(faceIndex);
							setted = true;
							break;
						}
					}

					if (!setted)
					{
						auto& faceGroup = faceGroups.emplace_back();
						faceGroup.push_back(faceIndex);
					}
				}

				// 分離
				if (faceGroups.size() > 1)
				{
					for (int i = 1; i < faceGroups.size(); i++)
					{
						// 新規頂点
						auto& newVertex = mesh.vertices.emplace_back();
						int oldV = vertexIndex;
						int newV = (int)mesh.vertices.size() - 1;
						newVertex = mesh.vertices[oldV];

						for (auto faceIndex : faceGroups[i])
						{
							auto& mqoFace = triFaces[faceIndex];
							if (mqoFace.Indexes[0] == oldV) mqoFace.Indexes[0] = newV;
							if (mqoFace.Indexes[1] == oldV) mqoFace.Indexes[1] = newV;
							if (mqoFace.Indexes[2] == oldV) mqoFace.Indexes[2] = newV;
						}
					}
				}
			}

			// クリア
			for (auto& faceList : vertexToFaceList)
			{
				faceList.clear();
			}

			// 所属頂点を設定
			for (int faceIndex = 0; faceIndex < triFaces.size(); faceIndex++)
			{
				auto& face = triFaces[faceIndex];

				for (int v = 0; v < 3; v++)
				{
					if (face.Indexes[v] >= vertexToFaceList.size())
					{
						vertexToFaceList.resize((size_t)face.Indexes[v] + 1);
					}
					vertexToFaceList[face.Indexes[v]].push_back(faceIndex);
				}
			}

			// 法線を設定
			for (int vertexIndex = 0; vertexIndex < vertexToFaceList.size(); vertexIndex++)
			{
				auto& faceIndexList = vertexToFaceList[vertexIndex];
				auto& vertex = mesh.vertices[vertexIndex];
				
				for (int faceIndex : faceIndexList)
				{
					vertex.normal += normals[faceIndex];
				}
				vertex.normal = vertex.normal.Normalized();
			}
		}

		// UV,Color設定
		{
			// 元の頂点とそこから派生した頂点インデックスへのDictoinary
			std::vector<std::vector<int>> originalToVertex;
			originalToVertex.resize(mesh.vertices.size());

			// 面の値を元に頂点を差し替え
			for (int faceIndex = 0; faceIndex < triFaces.size(); faceIndex++)
			{
				auto& mqoFace = triFaces[faceIndex];

				for (int v = 0; v < 3; v++)
				{
					auto& vertex = mesh.vertices[mqoFace.Indexes[v]];
					auto& o2v = originalToVertex[mqoFace.Indexes[v]];

					if (o2v.size() == 0)
					{
						// 新規
						vertex.uv1 = Convert(mqoFace.UV[v]);
						vertex.color = Convert(mqoFace.Colors[v]);
						o2v.push_back(mqoFace.Indexes[v]);
					}
					else
					{
						// 探索
						bool found = false;
						for (int vind : o2v)
						{
							if (mesh.vertices[vind].uv1 == Convert(mqoFace.UV[v]) &&
								mesh.vertices[vind].color == Convert(mqoFace.Colors[v]))
							{
								mqoFace.Indexes[v] = vind;
								found = true;
								break;
							}
						}

						if (!found)
						{
							// 追加
							mqoFace.Indexes[v] = (int)mesh.vertices.size();
							o2v.push_back((int)mesh.vertices.size());
							MeshVertex newVertex = vertex;
							newVertex.uv1 = Convert(mqoFace.UV[v]);
							newVertex.color = Convert(mqoFace.Colors[v]);
							mesh.vertices.emplace_back(newVertex);
						}
					}
				}
			}
		}

		for (auto& mqoFace : triFaces)
		{
			auto& face = mesh.faces.emplace_back();
			face.indices[0] = mqoFace.Indexes[0];
			face.indices[1] = mqoFace.Indexes[1];
			face.indices[2] = mqoFace.Indexes[2];

			MeshVertex& v1 = mesh.vertices[face.indices[0]];
			MeshVertex& v2 = mesh.vertices[face.indices[1]];
			MeshVertex& v3 = mesh.vertices[face.indices[2]];
			CalcTangentSpace(v1, v2, v3);
		}

		// Apply mirror (Axis: x,y,z)
		if (mqoObj.Mirror > 0)
		{
			int mirrorAxis = mqoObj.MirrorAxis;
			if (mirrorAxis == 0)
			{
				mirrorAxis = 1;
			}

			for (int axis = 0; axis < 3; axis++)
			{
				if ((mirrorAxis & (1 << axis)) != 0)
				{
					size_t vertexCount = mesh.vertices.size();
					for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
					{
						MeshVertex vertex = mesh.vertices[vertexIndex];
						vertex.position.v[axis] = -vertex.position.v[axis];
						vertex.normal.v[axis] = -vertex.normal.v[axis];
						vertex.tangent.v[axis] = -vertex.tangent.v[axis];
						vertex.binormal.v[axis] = -vertex.binormal.v[axis];
						mesh.vertices.emplace_back(vertex);
					}

					size_t faceCount = mesh.faces.size();
					for (size_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
					{
						MeshFace face = mesh.faces[faceIndex];
						std::swap(face.indices[1], face.indices[2]);
						for (int& index : face.indices)
						{
							index += vertexCount;
						}
						mesh.faces.emplace_back(face);
					}
				}
			}
		}
    }

	for (auto& mesh : model.meshes)
	{
		auto& node = model.nodes.emplace_back();
		node.mesh = &mesh;
		node.transform = Mat43::Identity();
	}

    return std::move(model);
}

}
