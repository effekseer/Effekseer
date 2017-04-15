
#include "fbxToEfkMdl.Base.h"
#include "fbxToEfkMdl.FBXConverter.h"

#include <iostream>
#include <fstream>

#if _DEBUG
#pragma comment(lib,"debug/libfbxsdk-mt.lib")
#else
#pragma comment(lib,"release/libfbxsdk-mt.lib")
#endif


int main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Effekseer Model Conveter\n");
		printf("Usage: fbxToEffekseerModelConverter InputFile(*.fbx) {OutputFile(*.efkmodel)} {options}\n");
		printf("");
		printf("Options: -modelcount : Max Render Count (1 - n)\n");
		printf("         -scale      : Scaling (ex. 0.5, 1.2)\n");
		printf("");
		printf("Examples: fbxToEffekseerModelConverter foo.fbx -scale 0.1\n");
		return -1;
	}

	std::map<std::string, std::string> args;

	for (int32_t i = 1; i < argc - 1; i++)
	{
		if (argv[i][0] == '-')
		{
			args[argv[i]] = argv[i + 1];
		}
	}

	std::string importPath = "";
	std::string exportPath = "";

	if (argc >= 2)
	{
		std::string p = argv[1];
		int ext_i = p.find_last_of(".");
		std::string ext = p.substr(ext_i, p.size() - ext_i);

		if (ext != ".fbx")
		{
			printf("InputFile is not fbx\n");
			return -1;
		}
		importPath = argv[1];
	}

	if (argc >= 3)
	{
		std::string p = argv[2];
		int ext_i = p.find_last_of(".");
		std::string ext = p.substr(ext_i, p.size() - ext_i);

		if (ext != ".efkmodel")
		{
			printf("OutputFile is not efkmodel\n");
			return -1;
		}
		exportPath = argv[2];
	}
	else
	{
		std::string p = argv[1];
		int ext_i = p.find_last_of(".");
		std::string p_ = p.substr(0, ext_i);
		exportPath = p_ + ".efkmodel";
	}

	int modelCount = 1;
	float modelScale = 1.0f;

	if (args.count("-modelcount") > 0)
	{
		modelCount = ::std::atoi(args["-modelcount"].c_str());
	}

	if (args.count("-scale") > 0)
	{
		modelScale = ::std::atof(args["-scale"].c_str());
	}

	fbxToEfkMdl::FBXConverter converter;

	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	fbxsdk::FbxImporter* fbxImporter = fbxsdk::FbxImporter::Create(sdkManager, "");
	if (!fbxImporter->Initialize(importPath.c_str(), -1, sdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString());
		system("PAUSE");
		exit(-1);
	}

	FbxScene* fbxScene = FbxScene::Create(sdkManager, "myScene");
	fbxImporter->Import(fbxScene);

	auto scene = converter.LoadScene(fbxScene, sdkManager);
	
	fbxScene->Destroy();
	fbxImporter->Destroy();
	sdkManager->Destroy();

	// Find mesh
	std::function<std::shared_ptr<fbxToEfkMdl::Mesh>(std::shared_ptr<fbxToEfkMdl::Node>)> findMesh = [&](std::shared_ptr<fbxToEfkMdl::Node> node) -> std::shared_ptr<fbxToEfkMdl::Mesh>
	{
		if (node->MeshData != nullptr) return node->MeshData;

		for (auto c : node->Children)
		{
			auto m = findMesh(c);
			if (m != nullptr)
			{
				return m;
			}
		}

		return nullptr;
	};

	std::shared_ptr<fbxToEfkMdl::Mesh> mesh = findMesh(scene->Root);


	// Export model.
	const int Version = 1;

	std::ofstream fout;
	fout.open(exportPath.c_str(), std::ios::out | std::ios::binary);
	
	if (!fout)
	{
		printf("Failed to write a file..\n");
		return -1;
	}

	fout.write((const char*)&Version, sizeof(int32_t));
	fout.write((const char*)&modelCount, sizeof(int32_t));
	
	if (mesh != nullptr)
	{
		auto vsize = (int32_t)mesh->Vertexes.size();
		fout.write((const char*)&vsize, sizeof(int32_t));

		for (auto v : mesh->Vertexes)
		{
			float p[3];
			p[0] = (float)(v.Position[0] * modelScale);
			p[1] = (float)(v.Position[1] * modelScale);
			p[2] = (float)(v.Position[2] * modelScale);

			float n[3];
			n[0] = (float)(v.Normal[0]);
			n[1] = (float)(v.Normal[1]);
			n[2] = (float)(v.Normal[2]);

			float b[3];
			b[0] = (float)(v.Binormal[0]);
			b[1] = (float)(v.Binormal[1]);
			b[2] = (float)(v.Binormal[2]);

			float t[3];
			t[0] = (float)(v.Tangent[0]);
			t[1] = (float)(v.Tangent[1]);
			t[2] = (float)(v.Tangent[2]);

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

		auto fsize = (int32_t)mesh->Faces.size();
		fout.write((const char*)&fsize, sizeof(int32_t));

		for (auto f : mesh->Faces)
		{
			fout.write((const char*)&f.Index[0], sizeof(int32_t));
			fout.write((const char*)&f.Index[1], sizeof(int32_t));
			fout.write((const char*)&f.Index[2], sizeof(int32_t));
		}
	}
	else
	{
		auto vsize = (int32_t)0;
		auto fsize = (int32_t)0;
		fout.write((const char*)&vsize, sizeof(int32_t));
		fout.write((const char*)&fsize, sizeof(int32_t));
	}

	fout.close();

    return 0;
}