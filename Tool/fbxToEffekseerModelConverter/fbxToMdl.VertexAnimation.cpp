
#include "fbxToMdl.VertexAnimation.h"

#include <fstream>
#include <iostream>

namespace fbxToEfkMdl
{
	struct BoneConnectorState
	{
		int32_t							NodeIndex;
	};

	struct MeshState
	{
		std::shared_ptr<Mesh>			Target;
		std::vector<BoneConnectorState>	Connectors;
	};

	struct NodeState
	{
		float Values[9];
		std::shared_ptr<KeyFrameAnimation>	Animations[9];

		FbxMatrix	MatLocal;
		FbxMatrix	MatGlobal;
		std::shared_ptr<Node>	ParentNode;
		std::shared_ptr<Node>	TargetNode;
	};

	std::vector<MeshState> GetAllMeshes(std::shared_ptr<Node> node)
	{
		std::vector<MeshState> ret;

		if (node->MeshData != nullptr)
		{
			MeshState m;
			m.Target = node->MeshData;

			for (auto b : m.Target->BoneConnectors)
			{
				BoneConnectorState bs;
				bs.NodeIndex = -1;
				m.Connectors.push_back(bs);
			}

			ret.push_back(m);
		}

		for (auto c : node->Children)
		{
			auto ms = GetAllMeshes(c);

			for (auto m : ms)
			{
				ret.push_back(m);
			}
		}

		return ret;
	}

	std::vector<NodeState> GetAllNodes(std::shared_ptr<Node> node, std::shared_ptr<Node> parentNode)
	{
		std::vector<NodeState> ret;
		ret.push_back(NodeState());
		(ret.end() - 1)->ParentNode = parentNode;
		(ret.end() - 1)->TargetNode = node;

		for (auto c : node->Children)
		{
			auto ms = GetAllNodes(c, node);

			for (auto m : ms)
			{
				ret.push_back(m);
			}
		}

		return ret;
	}

	void VertexAnimation::Export(const char* path, std::shared_ptr<Scene> scene)
	{
		auto meshes = GetAllMeshes(scene->Root);
		auto anim = scene->AnimationClips[0];
		auto nodes = GetAllNodes(scene->Root, nullptr);
		auto frameCount = anim->FrameCount;

		// CalcIndex
		for (auto& mesh : meshes)
		{
			for (int32_t i = 0; i < mesh.Connectors.size(); i++)
			{
				for (int32_t j = 0; j < nodes.size(); j++)
				{
					if (mesh.Target->BoneConnectors[i].Name == nodes[j].TargetNode->Name)
					{
						mesh.Connectors[i].NodeIndex = j;
						break;
					}
				}
			}
		}

		// Assign animations
		for (auto a : anim->Animations)
		{
			for (auto& node : nodes)
			{
				if (a->Name != node.TargetNode->Name) continue;
				node.Animations[(int32_t)a->Target] = a;
				break;
			}
		}

		// Export model.
		// Preliminary
		const int Version = 5;
		int32_t modelCount = 1;
		float modelScale = 1.0f;

		std::ofstream fout;
		fout.open(path, std::ios::out | std::ios::binary);

		if (!fout)
		{
			printf("Failed to write a file..\n");
			return;
		}

		fout.write((const char*)&Version, sizeof(int32_t));
		fout.write((const char*)&modelScale, sizeof(int32_t));
		fout.write((const char*)&modelCount, sizeof(int32_t));
		fout.write((const char*)&frameCount, sizeof(int32_t));


		for (int32_t frame = 0; frame < frameCount; frame++)
		{
			// Calculate values
			for (auto& node : nodes)
			{
				for (int32_t j = 0; j < 9; j++)
				{
					if (node.Animations[j] != nullptr)
					{
						node.Values[j] = node.Animations[j]->Values[frame];
					}
					else
					{
						if (j == 0) node.Values[j] = node.TargetNode->Translation[0];
						if (j == 1) node.Values[j] = node.TargetNode->Translation[1];
						if (j == 2) node.Values[j] = node.TargetNode->Translation[2];
						if (j == 3) node.Values[j] = node.TargetNode->Rotation[0];
						if (j == 4) node.Values[j] = node.TargetNode->Rotation[1];
						if (j == 5) node.Values[j] = node.TargetNode->Rotation[2];
						if (j == 6) node.Values[j] = node.TargetNode->Scaling[0];
						if (j == 7) node.Values[j] = node.TargetNode->Scaling[1];
						if (j == 8) node.Values[j] = node.TargetNode->Scaling[2];
					}
				}
			}

			// Calculate local matrix
			for (auto& node : nodes)
			{
				FbxRotationOrder ro;
				ro.SetOrder(node.TargetNode->RotationOrder);
				FbxVector4 rv;
				rv[0] = node.Values[(int32_t)AnimationTarget::RX];
				rv[1] = node.Values[(int32_t)AnimationTarget::RY];
				rv[2] = node.Values[(int32_t)AnimationTarget::RZ];
				rv[3] = 1.0f;
				FbxAMatrix rm;
				ro.V2M(rm, rv);
				auto q = rm.GetQ();
				FbxVector4 r;
				r.SetXYZ(q);
				
				FbxMatrix mat;

				FbxVector4 t;
				t[0] = node.Values[(int32_t)AnimationTarget::TX];
				t[1] = node.Values[(int32_t)AnimationTarget::TY];
				t[2] = node.Values[(int32_t)AnimationTarget::TZ];

				FbxVector4 s;
				s[0] = node.Values[(int32_t)AnimationTarget::SX];
				s[1] = node.Values[(int32_t)AnimationTarget::SY];
				s[2] = node.Values[(int32_t)AnimationTarget::SZ];

				mat.SetTRS(t, r, s);

				node.MatLocal = mat;
				node.MatGlobal.SetIdentity();
			}
		
			// Calculate global matrix
			for (auto& node : nodes)
			{
				for (auto& node2 : nodes)
				{
					if (node2.TargetNode == node.ParentNode)
					{
						node.MatGlobal = node2.MatGlobal * node.MatLocal;
					}
				}
			}

			// Edit mesh


			int32_t vcount = 0;
			int32_t fcount = 0;

			for (auto mesh : meshes)
			{
				vcount += mesh.Target->Vertexes.size();
				fcount += mesh.Target->Faces.size();
			}

			fout.write((const char*)&vcount, sizeof(int32_t));

			for (auto& mesh : meshes)
			{
				std::vector<FbxMatrix> boneMat;
				for (int32_t i = 0; i < mesh.Connectors.size(); i++)
				{
					auto m = nodes[mesh.Connectors[i].NodeIndex].MatGlobal * mesh.Target->BoneConnectors[i].OffsetMatrix;
					boneMat.push_back(m);
				}

				for (auto v : mesh.Target->Vertexes)
				{
					auto getBoneMat = [&](int32_t i) -> FbxMatrix
					{
						FbxMatrix ret;
						ret.SetIdentity();
						if (i < 0) return ret;
						return boneMat[i];
					};

					auto m =
						getBoneMat(v.Weights[0].Index) * v.Weights[0].Value +
						getBoneMat(v.Weights[1].Index) * v.Weights[1].Value +
						getBoneMat(v.Weights[2].Index) * v.Weights[2].Value +
						getBoneMat(v.Weights[3].Index) * v.Weights[3].Value;

					auto position = m.MultNormalize(v.Position);

					float p[3];
					p[0] = (float)(position[0]);
					p[1] = (float)(position[1]);
					p[2] = (float)(position[2]);

					v.Normal[3] = 1.0f;
					auto normal = m.MultNormalize(v.Normal);

					float n[3];
					n[0] = (float)(normal[0]);
					n[1] = (float)(normal[1]);
					n[2] = (float)(normal[2]);

					v.Binormal[3] = 1.0f;
					auto binormal = m.MultNormalize(v.Binormal);

					float b[3];
					b[0] = (float)(binormal[0]);
					b[1] = (float)(binormal[1]);
					b[2] = (float)(binormal[2]);

					v.Tangent[3] = 1.0f;
					auto tangent = m.MultNormalize(v.Tangent);

					float t[3];
					t[0] = (float)(tangent[0]);
					t[1] = (float)(tangent[1]);
					t[2] = (float)(tangent[2]);

					float uv[2];
					uv[0] = (float)(v.UV[0]);
					uv[1] = (float)(v.UV[1]);

					uint8_t c[4];
					c[0] = (uint8_t)(v.VertexColor.mRed * 255);
					c[1] = (uint8_t)(v.VertexColor.mGreen * 255);
					c[2] = (uint8_t)(v.VertexColor.mBlue * 255);
					c[3] = (uint8_t)(v.VertexColor.mAlpha * 255);

					fout.write((const char*)p, sizeof(float) * 3);
					fout.write((const char*)n, sizeof(float) * 3);
					fout.write((const char*)b, sizeof(float) * 3);
					fout.write((const char*)t, sizeof(float) * 3);
					fout.write((const char*)uv, sizeof(float) * 2);
					fout.write((const char*)c, sizeof(uint8_t) * 4);
				}
			}

			fout.write((const char*)&fcount, sizeof(int32_t));
			int32_t foffset = 0;

			for (auto& mesh : meshes)
			{
				for (auto f : mesh.Target->Faces)
				{
					int32_t i0 = f.Index[0] + foffset;
					int32_t i1 = f.Index[1] + foffset;
					int32_t i2 = f.Index[2] + foffset;

					fout.write((const char*)&(i0), sizeof(int32_t));
					fout.write((const char*)&(i1), sizeof(int32_t));
					fout.write((const char*)&(i2), sizeof(int32_t));
				}

				foffset += mesh.Target->Faces.size();
			}
		}

		fout.close();
	}
}