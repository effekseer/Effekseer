#if defined(__EFFEKSEER_BUILD_WEBGPU__) && defined(__EMSCRIPTEN__)

#include "../TestHelper.h"
#include <Runtime/EffectPlatformWebGPU.h>

#include <emscripten.h>

#include <cstdlib>
#include <iostream>

namespace
{

EM_JS(int, effekseer_request_webgpu_browser_capture, (const char* name), {
	if (!Module.llgiWebGPUCaptureEnabled) {
		return 0;
	}

	Module.llgiWebGPUCaptureRequests = Module.llgiWebGPUCaptureRequests || [];
	Module.llgiWebGPUCaptureResults = Module.llgiWebGPUCaptureResults || {};
	const id = Module.llgiWebGPUCaptureNextId || 1;
	Module.llgiWebGPUCaptureNextId = id + 1;
	Module.llgiWebGPUCaptureRequests.push({
		id: id,
		name: UTF8ToString(name),
	});
	return id;
});

EM_JS(int, effekseer_get_webgpu_browser_capture_status, (int requestId), {
	if (requestId === 0) {
		return 1;
	}

	const results = Module.llgiWebGPUCaptureResults || {};
	const result = results[String(requestId)] || results[requestId];
	if (!result) {
		return 0;
	}

	return result.status === 'passed' ? 1 : -1;
});

void CaptureWebGPUBrowserCanvas(const char* name)
{
	const auto requestId = effekseer_request_webgpu_browser_capture(name);
	if (requestId == 0)
	{
		return;
	}

	for (int retry = 0; retry < 600; retry++)
	{
		const auto status = effekseer_get_webgpu_browser_capture_status(requestId);
		if (status > 0)
		{
			return;
		}
		if (status < 0)
		{
			std::cout << name << " WebGPU browser capture failed." << std::endl;
			EXPECT_TRUE(false);
			return;
		}
		emscripten_sleep(16);
	}

	std::cout << name << " WebGPU browser capture timed out." << std::endl;
	EXPECT_TRUE(false);
}

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

void WebGPUBrowserEffectPresentationTest(const char16_t* effectPath, int frameCount, size_t minimumChangedPixels)
{
	EffectPlatformInitializingParameter param;
	param.VSync = false;
	param.WindowSize = {320, 240};

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);

	EXPECT_TRUE(platform->Update());
	const auto background = platform->CaptureScreenPixels();
	EXPECT_TRUE(background.size() == static_cast<size_t>(param.WindowSize[0] * param.WindowSize[1] * 4));

	platform->Play(effectPath);
	for (int frame = 0; frame < frameCount; frame++)
	{
		EXPECT_TRUE(platform->Update());
	}

	const auto rendered = platform->CaptureScreenPixels();
	EXPECT_TRUE(rendered.size() == background.size());

	const auto changedPixels = CountDifferentPixels(background, rendered);
	if (changedPixels <= minimumChangedPixels)
	{
		std::cout << "Changed pixels were too few: " << changedPixels << std::endl;
	}
	EXPECT_TRUE(changedPixels > minimumChangedPixels);

	platform->Terminate();
}

void SetSoftParticleCamera(EffectPlatform* platform)
{
	Effekseer::Matrix44 cameraMat;
	cameraMat.LookAtRH({0, 3, 10}, {0, 0, 0}, {0, 1, 0});
	platform->GetRenderer()->SetCameraMatrix(cameraMat);
}

void WebGPUBrowserSoftParticlePresentationTest(EffekseerRenderer::UVStyle backgroundUVStyle, const char* testName)
{
	EffectPlatformInitializingParameter param;
	param.VSync = false;
	param.WindowSize = {320, 240};

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);
	SetSoftParticleCamera(platform.get());
	platform->GetRenderer()->SetBackgroundTextureUVStyle(backgroundUVStyle);
	platform->GenerateGroundDepth();

	EXPECT_TRUE(platform->Draw());
	const auto background = platform->CaptureScreenPixels();
	EXPECT_TRUE(background.size() == static_cast<size_t>(param.WindowSize[0] * param.WindowSize[1] * 4));

	srand(0);
	platform->Play(u"/TestData/Effects/16/SoftParticle01.efkefc");
	for (int frame = 0; frame < 30; frame++)
	{
		EXPECT_TRUE(platform->Update());
	}

	CaptureWebGPUBrowserCanvas(testName);

	const auto rendered = platform->CaptureScreenPixels();
	EXPECT_TRUE(rendered.size() == background.size());

	const auto changedPixels = CountDifferentPixels(background, rendered);
	if (changedPixels <= 100)
	{
		std::cout << testName << " changed pixels were too few: " << changedPixels << std::endl;
	}
	EXPECT_TRUE(changedPixels > 100);

	platform->Terminate();
}

void WebGPUBrowserSimpleEffectPresentationTest()
{
	WebGPUBrowserEffectPresentationTest(u"/TestData/Effects/10/SimpleLaser.efk", 30, 100);
}

void WebGPUBrowserDistortionPresentationTest()
{
	WebGPUBrowserEffectPresentationTest(u"/TestData/Effects/10/Distortions1.efk", 30, 100);
}

void WebGPUBrowserSoftParticleNotFlippedPresentationTest()
{
	WebGPUBrowserSoftParticlePresentationTest(EffekseerRenderer::UVStyle::Normal, "SoftParticle01_NotFlipped_WebGPU_Browser");
}

void WebGPUBrowserSoftParticleFlippedPresentationTest()
{
	WebGPUBrowserSoftParticlePresentationTest(EffekseerRenderer::UVStyle::VerticalFlipped, "SoftParticle01_Flipped_WebGPU_Browser");
}

} // namespace

TestRegister Runtime_WebGPUBrowserSimpleEffectPresentationTest(
	"Runtime.WebGPUBrowserSimpleEffectPresentation",
	[]() -> void { WebGPUBrowserSimpleEffectPresentationTest(); });

TestRegister Runtime_WebGPUBrowserDistortionPresentationTest(
	"Runtime.WebGPUBrowserDistortionPresentation",
	[]() -> void { WebGPUBrowserDistortionPresentationTest(); });

TestRegister Runtime_WebGPUBrowserSoftParticleNotFlippedPresentationTest(
	"Runtime.WebGPUBrowserSoftParticleNotFlippedPresentation",
	[]() -> void { WebGPUBrowserSoftParticleNotFlippedPresentationTest(); });

TestRegister Runtime_WebGPUBrowserSoftParticleFlippedPresentationTest(
	"Runtime.WebGPUBrowserSoftParticleFlippedPresentation",
	[]() -> void { WebGPUBrowserSoftParticleFlippedPresentationTest(); });

#endif
