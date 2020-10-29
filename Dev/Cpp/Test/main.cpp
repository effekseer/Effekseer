#include "TestHelper.h"

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

	// You can specify "Test --filter=*.* to run a single test"
	auto parsed = TestHelper::ParseArg(argc, argv);
	TestHelper::Run(parsed);

#if _WIN32
	//_CrtDumpMemoryLeaks();
#endif

	return 0;
}
