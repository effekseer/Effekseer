#include "TestHelper.h"
#include <Effekseer.h>
#include <iostream>

#if _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif

int main(int argc, char* argv[])
{
	bool onCI = argc >= 2;

#if _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Effekseer::SetLogger([](Effekseer::LogType type, const std::string& s) -> void { std::cout << s << std::endl; });

	// You can specify "Test --filter=*.* to run a single test"
	auto parsed = TestHelper::ParseArg(argc, argv);
	TestHelper::Run(parsed);

#if _WIN32
	//_CrtDumpMemoryLeaks();
#endif

	return 0;
}
