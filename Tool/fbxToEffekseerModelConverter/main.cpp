
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

	// Export model.
	const int Version = 1;

	std::ofstream fout;
	fout.open(exportPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	if (!fout)
	{
		printf("Failed to write a file..\n");
		return -1;
	}

	fout << Version;

	fout << modelCount;

	fout << (int32_t)scene->Root->MeshData->Vertexes.size();
	
	for (auto v : scene->Root->MeshData->Vertexes)
	{
		fout << (float)(v.Position[0] * modelScale);
		fout << (float)(v.Position[1] * modelScale);
		fout << (float)(v.Position[2] * modelScale);

		fout << (float)(v.Normal[0]);
		fout << (float)(v.Normal[1]);
		fout << (float)(v.Normal[2]);

		fout << (float)(v.Binormal[0]);
		fout << (float)(v.Binormal[1]);
		fout << (float)(v.Binormal[2]);

		fout << (float)(v.Tangent[0]);
		fout << (float)(v.Tangent[1]);
		fout << (float)(v.Tangent[2]);

		fout << (float)(v.UV[0]);
		fout << (float)(v.UV[1]);

		fout << (uint8_t)(v.VertexColor.mRed * 255);
		fout << (uint8_t)(v.VertexColor.mGreen * 255);
		fout << (uint8_t)(v.VertexColor.mBlue * 255);
		fout << (uint8_t)(v.VertexColor.mAlpha * 255);
	}

	fout << (int32_t)scene->Root->MeshData->Faces.size();

	for (auto f : scene->Root->MeshData->Faces)
	{
		fout << f.Index[0];
		fout << f.Index[1];
		fout << f.Index[2];
	}

	fout.close();

    return 0;
}