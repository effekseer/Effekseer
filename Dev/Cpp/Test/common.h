
#ifndef	__COMMON_H__
#define	__COMMON_H__

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "../Effekseer/Effekseer.h"

#include <assert.h>
#include <string>

void PlayEffect();
extern ::Effekseer::Manager* g_manager;


class TestHelper
{
public:
	// e.g.) (D:\Proj\Effekseer\Dev\Cpp\Test\main.cpp, Resource/Laser01.efk) -> D:\Proj\Effekseer\Dev\Cpp\Test/Resource/Laser01.efk
	static std::u16string GetLocalFileHelper(const char* baseFilePath, const EFK_CHAR* localPath)
	{
		EFK_CHAR parentDir[512];
		GetParentDir(parentDir, baseFilePath);
		return std::u16string(parentDir) + u"/" + localPath;
	}

	static void GetParentDir(EFK_CHAR* dst, const char* src)
	{
		int i, last = -1;
		for (i = 0; src[i] != '\0'; i++)
		{
			if (src[i] == '/' || src[i] == '\\')
				last = i;
		}
		if (last >= 0)
		{
			for (i = 0; i < last; i++)
				dst[i] = src[i];
			dst[last] = L'\0';
		}
		else
		{
			dst[0] = L'\0';
		}
	}
};

//#define EFK_LOCALFILE(fileName) TestHelper::GetLocalFileHelper(__FILE__, fileName).c_str()
#define EFK_LOCALFILE(fileName) fileName

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif



