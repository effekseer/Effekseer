#include "TestPlatforms.h"
#include "../TestHelper.h"

#include <EffekseerToolRuntime/ImageRenderer.h>
#include <EffekseerToolRuntime/LineRenderer.h>
#include <EffekseerToolRuntime/PostEffects.h>
#include <EffekseerToolRuntime/RenderImage.h>
#include <RenderingEnvironment/RenderingEnvironmentGL.h>
#include <Runtime/EffectPlatform.h>
#include <Runtime/EffectPlatformGL.h>

#ifdef _WIN32
#include <Runtime/EffectPlatformDX11.h>
#include <RenderingEnvironment/RenderingEnvironmentDX11.h>
#endif

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include <Runtime/EffectPlatformWebGPU.h>
#endif

#ifdef __APPLE__
#include <Runtime/EffectPlatformMetal.h>
#endif

#include <memory>
#include <string>
#include <vector>

namespace
{

void RenderLineOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto lineRenderer = std::make_shared<Effekseer::ToolRuntime::LineRenderer>(graphicsDevice);
	EXPECT_TRUE(lineRenderer != nullptr);
	EXPECT_TRUE(lineRenderer->GetIsValid());

	auto cameraMatrix = Effekseer::Matrix44().LookAtRH({0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	auto projectionMatrix = Effekseer::Matrix44().PerspectiveFovRH(30.0f * 3.1415926535f / 180.0f, 1.0f, 0.1f, 100.0f);
	if (graphicsDevice->GetDeviceName() == "OpenGL")
	{
		projectionMatrix.PerspectiveFovRH_OpenGL(30.0f * 3.1415926535f / 180.0f, 1.0f, 0.1f, 100.0f);
	}

	auto screenRenderPass = window->GetScreenRenderPass();
	graphicsDevice->BeginRenderPass(screenRenderPass, true, true, Effekseer::Color(0, 0, 0, 255));
	lineRenderer->ClearCache();
	lineRenderer->DrawLine({-0.5f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, Effekseer::Color(255, 255, 255, 255));
	lineRenderer->Render(cameraMatrix, projectionMatrix);
	graphicsDevice->EndRenderPass();
	window->Present();
}

Effekseer::Backend::TextureRef CreateColorTexture(const Effekseer::Backend::GraphicsDeviceRef& graphicsDevice, int32_t width, int32_t height)
{
	Effekseer::Backend::TextureParameter parameter;
	parameter.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	parameter.Size = {width, height};

	Effekseer::CustomVector<uint8_t> initialData;
	initialData.resize(width * height * 4);
	for (int32_t y = 0; y < height; y++)
	{
		for (int32_t x = 0; x < width; x++)
		{
			const auto offset = (x + y * width) * 4;
			initialData[offset + 0] = static_cast<uint8_t>(x * 255 / Effekseer::Max(1, width - 1));
			initialData[offset + 1] = static_cast<uint8_t>(y * 255 / Effekseer::Max(1, height - 1));
			initialData[offset + 2] = 64;
			initialData[offset + 3] = 255;
		}
	}

	return graphicsDevice->CreateTexture(parameter, initialData);
}

Effekseer::Backend::TextureRef CreateRenderTarget(const Effekseer::Backend::GraphicsDeviceRef& graphicsDevice, int32_t width, int32_t height)
{
	Effekseer::Backend::TextureParameter parameter;
	parameter.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	parameter.Size = {width, height};
	parameter.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;
	return graphicsDevice->CreateTexture(parameter);
}

EffectPlatformInitializingParameter CreateToolRuntimeComparisonParameter()
{
	EffectPlatformInitializingParameter param;
	param.VSync = false;
	param.InstanceCount = 1;
	param.SpriteCount = 1;
	param.WindowSize = {128, 128};
	param.BackgroundPattern = BackgroundPatternType::NonPeriodicGradient;
	return param;
}

void DrawFullscreenTexture(
	const std::shared_ptr<Effekseer::ToolRuntime::ImageRenderer>& imageRenderer,
	Effekseer::Backend::TextureRef texture)
{
	const Effekseer::Vector3D positions[] = {
		{-0.9f, 0.9f, 0.0f},
		{0.9f, 0.9f, 0.0f},
		{-0.9f, -0.9f, 0.0f},
		{0.9f, -0.9f, 0.0f},
	};

	const Effekseer::Vector2D uvs[] = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};

	const Effekseer::Color colors[] = {
		Effekseer::Color(255, 255, 255, 255),
		Effekseer::Color(255, 255, 255, 255),
		Effekseer::Color(255, 255, 255, 255),
		Effekseer::Color(255, 255, 255, 255),
	};

	imageRenderer->DrawBackendTexture(positions, uvs, colors, texture);
}

void DrawComparisonImageQuad(const std::shared_ptr<Effekseer::ToolRuntime::ImageRenderer>& imageRenderer)
{
	const Effekseer::Vector3D positions[] = {
		{-0.82f, 0.72f, 0.0f},
		{0.75f, 0.58f, 0.0f},
		{-0.65f, -0.78f, 0.0f},
		{0.86f, -0.62f, 0.0f},
	};

	const Effekseer::Vector2D uvs[] = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};

	const Effekseer::Color colors[] = {
		Effekseer::Color(255, 32, 16, 220),
		Effekseer::Color(32, 255, 64, 220),
		Effekseer::Color(32, 64, 255, 220),
		Effekseer::Color(255, 255, 255, 220),
	};

	imageRenderer->Draw(positions, uvs, colors, nullptr);
}

void DrawComparisonLines(const std::shared_ptr<Effekseer::ToolRuntime::LineRenderer>& lineRenderer)
{
	lineRenderer->DrawLine({-0.55f, 0.0f, 0.0f}, {0.55f, 0.0f, 0.0f}, Effekseer::Color(255, 64, 64, 255));
	lineRenderer->DrawLine({0.0f, -0.55f, 0.0f}, {0.0f, 0.55f, 0.0f}, Effekseer::Color(64, 255, 64, 255));
	lineRenderer->DrawLine({-0.45f, -0.35f, 0.0f}, {0.45f, 0.35f, 0.0f}, Effekseer::Color(64, 128, 255, 255));
	lineRenderer->DrawLine({-0.5f, 0.35f, 0.0f}, {0.5f, -0.35f, 0.0f}, Effekseer::Color(255, 255, 255, 255));
}

template <class TPlatform, class TDraw>
void RenderToolRuntimeComparisonToScreenshot(const char* screenshotPath, TDraw&& draw)
{
	auto platform = std::make_shared<TPlatform>();
	platform->Initialize(CreateToolRuntimeComparisonParameter());

	auto renderer = platform->GetRenderer();
	EXPECT_TRUE(renderer != nullptr);

	auto graphicsDevice = renderer->GetGraphicsDevice();
	EXPECT_TRUE(graphicsDevice != nullptr);

	EXPECT_TRUE(platform->Draw());
	EXPECT_TRUE(platform->BeginFrame());

	draw(platform, renderer, graphicsDevice);

	EXPECT_TRUE(platform->TakeScreenshot(screenshotPath));
	graphicsDevice.Reset();
	renderer.Reset();
	platform->Terminate();
}

template <class TPlatform>
void RenderLineComparisonToScreenshot(const char* screenshotPath)
{
	RenderToolRuntimeComparisonToScreenshot<TPlatform>(
		screenshotPath,
		[](const std::shared_ptr<TPlatform>& platform, EffekseerRenderer::RendererRef renderer, Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
		{
			auto lineRenderer = std::make_shared<Effekseer::ToolRuntime::LineRenderer>(graphicsDevice);
			EXPECT_TRUE(lineRenderer != nullptr);
			EXPECT_TRUE(lineRenderer->GetIsValid());

			lineRenderer->ClearCache();
			DrawComparisonLines(lineRenderer);

			platform->BeginRendering();
			lineRenderer->Render(Effekseer::Matrix44().Indentity(), Effekseer::Matrix44().Indentity());
			platform->EndRendering();
		});
}

template <class TPlatform>
void RenderImageRendererComparisonToScreenshot(const char* screenshotPath)
{
	RenderToolRuntimeComparisonToScreenshot<TPlatform>(
		screenshotPath,
		[](const std::shared_ptr<TPlatform>& platform, EffekseerRenderer::RendererRef renderer, Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
		{
			(void)renderer;
			auto imageRenderer = Effekseer::ToolRuntime::ImageRenderer::Create(graphicsDevice);
			EXPECT_TRUE(imageRenderer != nullptr);
			EXPECT_TRUE(imageRenderer->GetIsValid());

			imageRenderer->ClearCache();
			DrawComparisonImageQuad(imageRenderer);

			platform->BeginRendering();
			imageRenderer->Render();
			platform->EndRendering();
		});
}

template <class TPlatform>
void RenderPostEffectsComparisonToScreenshot(const char* screenshotPath)
{
	RenderToolRuntimeComparisonToScreenshot<TPlatform>(
		screenshotPath,
		[](const std::shared_ptr<TPlatform>& platform, EffekseerRenderer::RendererRef renderer, Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
		{
			(void)renderer;
			constexpr int32_t size = 128;
			auto source = CreateColorTexture(graphicsDevice, size, size);
			auto firstTarget = CreateRenderTarget(graphicsDevice, size, size);
			auto secondTarget = CreateRenderTarget(graphicsDevice, size, size);
			EXPECT_TRUE(source != nullptr);
			EXPECT_TRUE(firstTarget != nullptr);
			EXPECT_TRUE(secondTarget != nullptr);

			auto tonemap = std::make_unique<Effekseer::ToolRuntime::TonemapPostEffect>(graphicsDevice);
			EXPECT_TRUE(tonemap->GetIsValid());
			tonemap->SetParameters(Effekseer::ToolRuntime::TonemapPostEffect::Algorithm::Off, 1.0f);
			tonemap->Render(firstTarget, source);

			auto bloom = std::make_unique<Effekseer::ToolRuntime::BloomPostEffect>(graphicsDevice);
			EXPECT_TRUE(bloom->GetIsValid());
			bloom->SetParameters(0.4f, 0.25f, 0.15f);
			bloom->Render(secondTarget, firstTarget);

			auto linearToSRGB = std::make_unique<Effekseer::ToolRuntime::LinearToSRGBPostEffect>(graphicsDevice);
			EXPECT_TRUE(linearToSRGB->GetIsValid());
			linearToSRGB->Render(firstTarget, secondTarget);

			auto imageRenderer = Effekseer::ToolRuntime::ImageRenderer::Create(graphicsDevice);
			EXPECT_TRUE(imageRenderer != nullptr);
			EXPECT_TRUE(imageRenderer->GetIsValid());
			imageRenderer->ClearCache();
			DrawFullscreenTexture(imageRenderer, firstTarget);

			platform->BeginRendering();
			imageRenderer->Render();
			platform->EndRendering();
		});
}

void ResizeRenderImageOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto renderImage = Effekseer::ToolRuntime::RenderImage::Create(window->GetGraphicsDevice());
	EXPECT_TRUE(renderImage != nullptr);

	renderImage->Resize(64, 32);
	EXPECT_TRUE(renderImage->GetWidth() == 64);
	EXPECT_TRUE(renderImage->GetHeight() == 32);
	EXPECT_TRUE(renderImage->GetTexture() != nullptr);

	renderImage->Resize(64, 32);
	EXPECT_TRUE(renderImage->GetWidth() == 64);
	EXPECT_TRUE(renderImage->GetHeight() == 32);

	renderImage->Resize(16, 8);
	EXPECT_TRUE(renderImage->GetWidth() == 16);
	EXPECT_TRUE(renderImage->GetHeight() == 8);
}

void RenderImageRendererOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto imageRenderer = Effekseer::ToolRuntime::ImageRenderer::Create(graphicsDevice);
	EXPECT_TRUE(imageRenderer != nullptr);
	EXPECT_TRUE(imageRenderer->GetIsValid());

	const Effekseer::Vector3D positions[] = {
		{-0.8f, 0.8f, 0.0f},
		{0.8f, 0.8f, 0.0f},
		{-0.8f, -0.8f, 0.0f},
		{0.8f, -0.8f, 0.0f},
	};

	const Effekseer::Vector2D uvs[] = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};

	const Effekseer::Color colors[] = {
		Effekseer::Color(255, 0, 0, 255),
		Effekseer::Color(0, 255, 0, 255),
		Effekseer::Color(0, 0, 255, 255),
		Effekseer::Color(255, 255, 255, 255),
	};

	auto screenRenderPass = window->GetScreenRenderPass();
	graphicsDevice->BeginRenderPass(screenRenderPass, true, true, Effekseer::Color(0, 0, 0, 255));
	imageRenderer->ClearCache();
	imageRenderer->Draw(positions, uvs, colors, nullptr);
	imageRenderer->Render();
	graphicsDevice->EndRenderPass();
	window->Present();
}

void RenderPostEffectsOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto source = CreateColorTexture(graphicsDevice, 32, 32);
	auto firstTarget = CreateRenderTarget(graphicsDevice, 32, 32);
	auto secondTarget = CreateRenderTarget(graphicsDevice, 32, 32);

	EXPECT_TRUE(source != nullptr);
	EXPECT_TRUE(firstTarget != nullptr);
	EXPECT_TRUE(secondTarget != nullptr);

	auto linearToSRGB = std::make_unique<Effekseer::ToolRuntime::LinearToSRGBPostEffect>(graphicsDevice);
	EXPECT_TRUE(linearToSRGB->GetIsValid());
	linearToSRGB->Render(firstTarget, source);

	auto tonemap = std::make_unique<Effekseer::ToolRuntime::TonemapPostEffect>(graphicsDevice);
	EXPECT_TRUE(tonemap->GetIsValid());
	tonemap->SetParameters(Effekseer::ToolRuntime::TonemapPostEffect::Algorithm::Off, 1.0f);
	tonemap->Render(secondTarget, firstTarget);
	tonemap->SetParameters(Effekseer::ToolRuntime::TonemapPostEffect::Algorithm::Reinhard, 1.0f);
	tonemap->Render(firstTarget, secondTarget);

	auto bloom = std::make_unique<Effekseer::ToolRuntime::BloomPostEffect>(graphicsDevice);
	EXPECT_TRUE(bloom->GetIsValid());
	bloom->SetParameters(0.5f, 0.3f, 0.1f);
	bloom->Render(firstTarget, source);
}

void RenderLinearToSRGBOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto source = CreateColorTexture(graphicsDevice, 32, 32);
	auto target = CreateRenderTarget(graphicsDevice, 32, 32);
	EXPECT_TRUE(source != nullptr);
	EXPECT_TRUE(target != nullptr);

	auto effect = std::make_unique<Effekseer::ToolRuntime::LinearToSRGBPostEffect>(graphicsDevice);
	EXPECT_TRUE(effect->GetIsValid());
	effect->Render(target, source);
}

void RenderTonemapOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto source = CreateColorTexture(graphicsDevice, 32, 32);
	auto target = CreateRenderTarget(graphicsDevice, 32, 32);
	EXPECT_TRUE(source != nullptr);
	EXPECT_TRUE(target != nullptr);

	auto effect = std::make_unique<Effekseer::ToolRuntime::TonemapPostEffect>(graphicsDevice);
	EXPECT_TRUE(effect->GetIsValid());
	effect->SetParameters(Effekseer::ToolRuntime::TonemapPostEffect::Algorithm::Off, 1.0f);
	effect->Render(target, source);
}

void RenderBloomOnce(const std::shared_ptr<RenderingEnvironment>& window)
{
	auto graphicsDevice = window->GetGraphicsDevice();
	auto source = CreateColorTexture(graphicsDevice, 32, 32);
	auto target = CreateRenderTarget(graphicsDevice, 32, 32);
	EXPECT_TRUE(source != nullptr);
	EXPECT_TRUE(target != nullptr);

	auto effect = std::make_unique<Effekseer::ToolRuntime::BloomPostEffect>(graphicsDevice);
	EXPECT_TRUE(effect->GetIsValid());
	effect->SetParameters(0.5f, 0.3f, 0.1f);
	effect->Render(target, source);
}

template <class TPlatform>
void InitializeLineRendererOnEffectPlatform()
{
	EffectPlatformInitializingParameter param;
	param.VSync = false;
	param.InstanceCount = 1;
	param.SpriteCount = 1;
	param.WindowSize = {64, 64};

	auto platform = std::make_shared<TPlatform>();
	platform->Initialize(param);

	auto renderer = platform->GetRenderer();
	EXPECT_TRUE(renderer != nullptr);

	auto lineRenderer = std::make_shared<Effekseer::ToolRuntime::LineRenderer>(renderer->GetGraphicsDevice());
	EXPECT_TRUE(lineRenderer != nullptr);
	EXPECT_TRUE(lineRenderer->GetIsValid());

	lineRenderer.reset();
	renderer.Reset();
	platform->Terminate();
}

} // namespace


namespace
{
template <class Environment, class Platform>
void RegisterToolRuntimeTests(const std::string& backend)
{
	auto registerEnvironmentTest = [&backend](const char* test, auto run)
	{
		const auto name = "ToolRuntime." + std::string(test) + "." + backend;
		TestHelper::RegisterTest(name.c_str(), [run, name]
								 { run(std::make_shared<Environment>(std::array<int, 2>({64, 64}), name.c_str())); },
								 TestExecutionMode::FilterOnly);
	};
	registerEnvironmentTest("LineRenderer", RenderLineOnce);
	registerEnvironmentTest("RenderImage", ResizeRenderImageOnce);
	registerEnvironmentTest("ImageRenderer", RenderImageRendererOnce);
	registerEnvironmentTest("PostEffects", RenderPostEffectsOnce);
	registerEnvironmentTest("PostEffects.LinearToSRGB", RenderLinearToSRGBOnce);
	registerEnvironmentTest("PostEffects.Tonemap", RenderTonemapOnce);
	registerEnvironmentTest("PostEffects.Bloom", RenderBloomOnce);
	auto registerScreenshot = [&backend](const char* test, void (*run)(const char*))
	{
		const auto name = "ToolRuntime.RenderingComparison." + std::string(test) + "." + backend;
		const auto path = "ToolRuntime_" + std::string(test) + "_" + backend + ".png";
		TestHelper::RegisterTest(name.c_str(), [run, path]
								 { run(path.c_str()); },
								 TestExecutionMode::FilterOnly);
	};
	registerScreenshot("Line", RenderLineComparisonToScreenshot<Platform>);
	registerScreenshot("ImageRenderer", RenderImageRendererComparisonToScreenshot<Platform>);
	registerScreenshot("PostEffects", RenderPostEffectsComparisonToScreenshot<Platform>);
}

struct RegisterToolTests
{
	RegisterToolTests()
	{
		RegisterToolRuntimeTests<RenderingEnvironmentGL, EffectPlatformGL>("GL");
#ifdef _WIN32
		RegisterToolRuntimeTests<RenderingEnvironmentDX11, EffectPlatformDX11>("DX11");
#endif
		ForEachLLGITestPlatform([](auto type, const char* backend)
								{
			using Platform = typename decltype(type)::Type;
			const std::string name = backend;
			TestHelper::RegisterTest(("ToolRuntime.LineRenderer." + name).c_str(),
				InitializeLineRendererOnEffectPlatform<Platform>, TestExecutionMode::FilterOnly);
			if (name == "DX12" || name == "Vulkan")
				TestHelper::RegisterTest(("ToolRuntime.RenderingComparison.Line." + name).c_str(), [name] {
					RenderLineComparisonToScreenshot<Platform>(("ToolRuntime_Line_" + name + ".png").c_str());
				}, TestExecutionMode::FilterOnly); });
	}
} registerToolTests;
} // namespace
