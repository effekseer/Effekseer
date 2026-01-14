#include "FbxCurveConverter.h"
#include "FbxLoader.h"

#include <iostream>

#if _DEBUG
#pragma comment(lib, "debug/libfbxsdk.lib")
// #pragma comment(lib, "debug/libfbxsdk-mt.lib")
// #pragma comment(lib, "debug/libxml2-mt.lib")
// #pragma comment(lib, "debug/zlib-mt.lib")
#else
#pragma comment(lib, "release/libfbxsdk.lib")
// #pragma comment(lib, "release/libfbxsdk-mt.lib")
// #pragma comment(lib, "release/libxml2-mt.lib")
// #pragma comment(lib, "release/zlib-mt.lib")
#endif

#ifdef _WIN32
#include <cstring>
#include <string>
#include <windows.h>

std::string ANSI_to_UTF8(std::string inANSI)
{
	wchar_t tempWideChar[1024];
	char outUTF8[1024];
	MultiByteToWideChar(CP_ACP, 0, inANSI.c_str(), -1, tempWideChar, 1024);
	WideCharToMultiByte(CP_UTF8, 0, tempWideChar, -1, outUTF8, 1024, NULL, NULL);
	return outUTF8;
}

#endif

int main(int argc, char* argv[])
{
	std::string import_path; // = "data//test_curve.fbx";
	import_path = argv[1];

	std::string export_path = import_path;
	std::string::size_type ext_pos = export_path.find_last_of('.');

	export_path = export_path.substr(0, ext_pos);
	export_path.append(".efkcurve");

	std::cout << "Import path : " << import_path << std::endl;
	std::cout << "Export path : " << export_path << std::endl;

#ifdef _WIN32
	import_path = ANSI_to_UTF8(import_path);
#endif

	auto lFbxInfo = FbxLoader::Imoport(import_path);

	if (lFbxInfo == nullptr)
	{
		std::cout << "Error : Failed to load" << std::endl;
		return 1;
	}

	if (!FbxCurveConverter::Export(export_path, *lFbxInfo))
	{
		return 1;
	}

	return 0;
}
