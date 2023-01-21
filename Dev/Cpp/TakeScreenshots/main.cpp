#include <Effekseer.h>
#include <iostream>

#if _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif

#ifdef BUILD_WITH_EASY_PROFILER
#define EASY_PROFILER_STATIC
#include <easy/profiler.h>
#endif

#include "TakeScreenshots.h"

int main(int argc, char* argv[])
{
	bool onCI = argc >= 2;

#if _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifdef BUILD_WITH_EASY_PROFILER
	EASY_PROFILER_ENABLE;
	profiler::startListen();
#endif

	Effekseer::SetLogger([](Effekseer::LogType type, const std::string& s) -> void
						 { std::cout << s << std::endl; });

	TakeScreenshots();

#if _WIN32
	//_CrtDumpMemoryLeaks();
#endif

#ifdef BUILD_WITH_EASY_PROFILER
	profiler::dumpBlocksToFile("effekseer_profile.prof");
#endif
	return 0;
}

