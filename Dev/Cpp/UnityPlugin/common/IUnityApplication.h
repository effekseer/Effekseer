#pragma once
#include "IUnityInterface.h"

UNITY_DECLARE_INTERFACE(IUnityApplication)
{
	const char* (UNITY_INTERFACE_API * GetStreamingAssetsPath)();
};
UNITY_REGISTER_INTERFACE_GUID(0xA65C172628A82E42ULL,0x9E42B1FF524FF826ULL,IUnityApplication)
