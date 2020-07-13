#include "FbxLoader.h"
#include "FbxCurveConverter.h"

void main(int argc, char *argv[])
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
		
		return;
	}

	if (FbxCurveConverter::Export(lFilepath, *lFbxInfo) == false)
	{
		printf("Output Error\n");

#ifdef  _DEBUG
		system("pause");
#endif //  _DEBUG
		
		return;
	}

	delete lFbxInfo;

	return;
}
