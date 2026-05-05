#if defined(__EFFEKSEER_BUILD_WEBGPU__) && defined(__EMSCRIPTEN__)

#include "../TestHelper.h"
#include <Runtime/EffectPlatformWebGPU.h>

#include <cstdlib>
#include <iostream>

namespace
{

size_t CountDifferentPixels(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
{
	EXPECT_TRUE(a.size() == b.size());

	size_t count = 0;
	for (size_t i = 0; i < a.size(); i += 4)
	{
		const auto dr = std::abs(static_cast<int>(a[i + 0]) - static_cast<int>(b[i + 0]));
		const auto dg = std::abs(static_cast<int>(a[i + 1]) - static_cast<int>(b[i + 1]));
		const auto db = std::abs(static_cast<int>(a[i + 2]) - static_cast<int>(b[i + 2]));
		const auto da = std::abs(static_cast<int>(a[i + 3]) - static_cast<int>(b[i + 3]));
		if (dr + dg + db + da > 24)
		{
			count++;
		}
	}
	return count;
}

void WebGPUBrowserSimpleEffectPresentationTest()
{
	EffectPlatformInitializingParameter param;
	param.VSync = false;
	param.WindowSize = {320, 240};

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);

	EXPECT_TRUE(platform->Update());
	const auto background = platform->CaptureScreenPixels();
	EXPECT_TRUE(background.size() == static_cast<size_t>(param.WindowSize[0] * param.WindowSize[1] * 4));

	platform->Play(u"/TestData/Effects/10/SimpleLaser.efk");
	for (int frame = 0; frame < 30; frame++)
	{
		EXPECT_TRUE(platform->Update());
	}

	const auto rendered = platform->CaptureScreenPixels();
	EXPECT_TRUE(rendered.size() == background.size());

	const auto changedPixels = CountDifferentPixels(background, rendered);
	if (changedPixels <= 100)
	{
		std::cout << "Changed pixels were too few: " << changedPixels << std::endl;
	}
	EXPECT_TRUE(changedPixels > 100);

	platform->Terminate();
}

} // namespace

TestRegister Runtime_WebGPUBrowserSimpleEffectPresentationTest(
	"Runtime.WebGPUBrowserSimpleEffectPresentation",
	[]() -> void { WebGPUBrowserSimpleEffectPresentationTest(); });

#endif
