#include "TestHelper.h"
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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(void, effekseer_report_test_result, (int result, const char* message), {
	let text = UTF8ToString(message);
	let status = result === 0 ? 'passed' : 'failed';
	if (Module.llgiTestResult && Module.llgiTestResult.status === 'failed') {
		console.log('EFFEKSEER_TEST_FAIL', Module.llgiTestResult.message);
		return;
	}
	if (status === 'passed' && Module.llgiLastWebGPUError) {
		status = 'failed';
		text = Module.llgiLastWebGPUError;
	}
	Module.llgiTestResult = { status: status, message: text };
	console.log(status === 'passed' ? 'EFFEKSEER_TEST_PASS' : 'EFFEKSEER_TEST_FAIL', text);
});
#endif

int main(int argc, char* argv[])
{
	[[maybe_unused]] const bool onCI = argc >= 2;

#if _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifdef BUILD_WITH_EASY_PROFILER
	EASY_PROFILER_ENABLE;
	profiler::startListen();
#endif

	Effekseer::SetLogger([](Effekseer::LogType type, const std::string& s) -> void
						 { std::cout << s << std::endl; });

	// You can specify "Test --filter=*.* to run a single test"
	auto parsed = TestHelper::ParseArg(argc, argv);
	TestHelper::Run(parsed);

#ifdef __EMSCRIPTEN__
	effekseer_report_test_result(0, "completed");
#endif

#if _WIN32
	//_CrtDumpMemoryLeaks();
#endif

#ifdef BUILD_WITH_EASY_PROFILER
	profiler::dumpBlocksToFile("effekseer_profile.prof");
#endif
	return 0;
}
