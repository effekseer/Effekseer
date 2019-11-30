
#include "fbxToMdl.VertexAnimation.h"
#include "Utils.h"
#include <fstream>
#include <iostream>

namespace fbxToEfkMdl
{
void VertexAnimation::Export(const char* path, std::shared_ptr<Scene> scene, std::shared_ptr<AnimationClip> anim, float modelScale)
{
	auto meshes = GetAllMeshes(scene->Root);
	auto nodes = GetAllNodes(scene->Root, nullptr);
	int32_t frameCount = 1;
	int32_t startFrame = 0;
	int32_t endFrame = 1;

	if (anim != nullptr)
	{
		startFrame = anim->StartFrame;
		endFrame = anim->EndFrame;
		frameCount = anim->EndFrame - anim->StartFrame;
	}

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
	if (anim != nullptr)
	{
		for (auto a : anim->Animations)
		{
			for (auto& node : nodes)
			{
				if (a->Name != node.TargetNode->Name)
					continue;
				node.Animations[(int32_t)a->Target] = a;
				break;
			}
		}
	}

	AssignAllDefaultGlobalMateixes(nodes);

	// Export model.
	// Preliminary
	const int Version = 5;
	int32_t modelCount = 1;

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

	for (int32_t frame = startFrame; frame < endFrame; frame++)
	{
		// Calculate values
		for (auto& node : nodes)
		{
			node.AssignDefaultValues();

			for (int32_t j = 0; j < 9; j++)
			{
				if (node.Animations[j] != nullptr)
				{
					node.Values[j] = node.Animations[j]->Values[frame];
				}
			}
		}

		// Calculate local matrix
		for (auto& node : nodes)
		{
			node.CalculateLocalMatrix();
			node.MatGlobal.SetIdentity();
		}

		CalculateAllGlobalMateixes(nodes);

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
			fbxToEfkMdl::NodeState nodeState;
			for (auto node : nodes)
			{
				if (node.TargetNode == mesh.MeshNode)
				{
					nodeState = node;
					break;
				}
			}

			std::vector<FbxMatrix> boneMat;
			for (int32_t i = 0; i < mesh.Connectors.size(); i++)
			{
				auto m = nodes[mesh.Connectors[i].NodeIndex].MatGlobal * mesh.Target->BoneConnectors[i].OffsetMatrix * nodeState.MatGlobal *
						 nodeState.MatGlobalDefault.Inverse();
				boneMat.push_back(m);
			}

			for (auto v : mesh.Target->Vertexes)
			{
				auto getBoneMat = [&](int32_t i) -> FbxMatrix {
					if (i < 0)
						return nodeState.MatGlobal;
					return boneMat[i];
				};

				auto m = getBoneMat(v.Weights[0].Index) * v.Weights[0].Value + getBoneMat(v.Weights[1].Index) * v.Weights[1].Value +
						 getBoneMat(v.Weights[2].Index) * v.Weights[2].Value + getBoneMat(v.Weights[3].Index) * v.Weights[3].Value;

				auto position = m.MultNormalize(v.Position);

				auto rot_m = m;

				rot_m.Set(0, 3, 0);
				rot_m.Set(1, 3, 0);
				rot_m.Set(2, 3, 0);
				rot_m.Set(3, 0, 0);
				rot_m.Set(3, 1, 0);
				rot_m.Set(3, 2, 0);

				float p[3];
				p[0] = (float)(position[0]) * modelScale;
				p[1] = (float)(position[1]) * modelScale;
				p[2] = (float)(position[2]) * modelScale;

				v.Normal[3] = 1.0f;
				auto normal = rot_m.MultNormalize(v.Normal);
				normal.Normalize();

				float n[3];
				n[0] = (float)(normal[0]);
				n[1] = (float)(normal[1]);
				n[2] = (float)(normal[2]);

				v.Binormal[3] = 1.0f;
				auto binormal = rot_m.MultNormalize(v.Binormal);
				binormal.Normalize();

				float b[3];
				b[0] = (float)(binormal[0]);
				b[1] = (float)(binormal[1]);
				b[2] = (float)(binormal[2]);

				v.Tangent[3] = 1.0f;
				auto tangent = rot_m.MultNormalize(v.Tangent);
				tangent.Normalize();

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

			foffset += mesh.Target->Vertexes.size();
		}
	}

	fout.close();
}
} // namespace fbxToEfkMdl