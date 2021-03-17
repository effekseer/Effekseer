#include "FbxLoader.h"
#include "FbxCurveConverter.h"

#if _DEBUG
#pragma comment(lib, "debug/libfbxsdk.lib")
//#pragma comment(lib, "debug/libfbxsdk-mt.lib")
//#pragma comment(lib, "debug/libxml2-mt.lib")
//#pragma comment(lib, "debug/zlib-mt.lib")
#else
#pragma comment(lib, "release/libfbxsdk.lib")
//#pragma comment(lib, "release/libfbxsdk-mt.lib")
//#pragma comment(lib, "release/libxml2-mt.lib")
//#pragma comment(lib, "release/zlib-mt.lib")
#endif

int main(int argc, char *argv[])
{
	std::string lFilepath;// = "data//test_curve.fbx";
	lFilepath = argv[1];

	auto lFbxInfo = FbxLoader::Imoport(lFilepath);
	
	if (lFbxInfo == nullptr)
	{
		printf("Load Failed\n");

#ifdef  _DEBUG
		system("pause");
#endif //  _DEBUG
		
		return 1;
	}

	if (FbxCurveConverter::Export(lFilepath, *lFbxInfo) == false)
	{
		printf("Output Error\n");

#ifdef  _DEBUG
		system("pause");
#endif //  _DEBUG
		
		return 1;
	}

	delete lFbxInfo;

	return 0;
}
