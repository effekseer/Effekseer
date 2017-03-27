
#include "fbxToEfkMdl.Base.h"
#include "fbxToEfkMdl.FBXConverter.h"

#if _DEBUG
#pragma comment(lib,"debug/libfbxsdk-mt.lib")
#else
#pragma comment(lib,"release/libfbxsdk-mt.lib")
#endif


int main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Lack arges");
		return -1;
	}

	const char* importPath = argv[1];
	const char* exportPath = argv[2];

	fbxToEfkMdl::FBXConverter converter;

	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	fbxsdk::FbxImporter* fbxImporter = fbxsdk::FbxImporter::Create(sdkManager, "");
	if (!fbxImporter->Initialize(importPath, -1, sdkManager->GetIOSettings()))
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

    return 0;
}