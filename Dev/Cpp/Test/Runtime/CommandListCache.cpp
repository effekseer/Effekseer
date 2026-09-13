#include "../TestHelper.h"
#include <Runtime/EffectPlatformLLGI.h>
#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
#include <Runtime/EffectPlatformMetal.h>
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include <Runtime/EffectPlatformWebGPU.h>
#endif
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h>
#include <EffekseerRendererCommon/VertexBuffer.h>

#include <unordered_map>
#include <cstring>

namespace
{
void CommandListCache(EffectPlatformLLGI& platform)
{
	EffectPlatformInitializingParameter parameters;
	parameters.VSync = false;
	platform.Initialize(parameters);
	EXPECT_TRUE(platform.Update());
	const auto background = platform.CaptureScreenPixels();
	EXPECT_TRUE(!background.empty());
	platform.Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

	// Track buffers by native recording slot, independent of pool size.
	std::unordered_map<LLGI::CommandList*, std::weak_ptr<EffekseerRenderer::VertexBuffer>> buffers;
	int reuseCount = 0;
	bool effectVisible = false;
	for (int frame = 0; frame < 18; frame++)
	{
		EXPECT_TRUE(platform.BeginFrame());
		platform.GetManager()->Update();
		const bool withCompute = frame % 2 == 0;
		std::shared_ptr<EffekseerRenderer::VertexBuffer> computeBuffer;
		if (withCompute)
		{
			platform.BeginCompute();
			platform.GetManager()->Compute();
			computeBuffer = platform.GetRenderer()->GetImpl()->InternalVertexBuffer;
			EXPECT_TRUE(computeBuffer != nullptr);
			std::tuple<void*, int32_t> allocation;
			EXPECT_TRUE(computeBuffer->Allocate(16, 1, allocation));
			EXPECT_TRUE(std::get<1>(allocation) == 0);
			memset(std::get<0>(allocation), 0, 16);
			platform.EndCompute();
		}

		platform.BeginRendering();
		auto renderer = platform.GetRenderer();
		auto buffer = renderer->GetImpl()->InternalVertexBuffer;
		EXPECT_TRUE(buffer != nullptr);
		if (withCompute)
		{
			EXPECT_TRUE(buffer == computeBuffer);
		}
		std::tuple<void*, int32_t> allocation;
		EXPECT_TRUE(buffer->Allocate(16, 1, allocation));
		EXPECT_TRUE(std::get<1>(allocation) == (withCompute ? 16 : 0));
		memset(std::get<0>(allocation), 0, 16);
		auto native = platform.GetCurrentCommandList();
		EXPECT_TRUE(native != nullptr);
		auto found = buffers.find(native);
		if (found == buffers.end())
		{
			for (const auto& entry : buffers)
			{
				EXPECT_TRUE(entry.second.lock() != buffer);
			}
			buffers.emplace(native, buffer);
		}
		else
		{
			EXPECT_TRUE(found->second.lock() == buffer);
			reuseCount++;
		}

		EXPECT_TRUE(renderer->BeginRendering());
		Effekseer::Manager::DrawParameter draw;
		draw.ViewProjectionMatrix = renderer->GetCameraProjectionMatrix();
		draw.ZNear = 0.0f;
		draw.ZFar = 1.0f;
		platform.GetManager()->Draw(draw);
		EXPECT_TRUE(renderer->EndRendering());
		platform.EndRendering();
		// Submit the recording and read back actual GPU pixels.
		const auto pixels = platform.CaptureScreenPixels();
		EXPECT_TRUE(pixels.size() == background.size());
		effectVisible |= pixels != background;
	}
	EXPECT_TRUE(effectVisible);
	EXPECT_TRUE(buffers.size() > 1);
	EXPECT_TRUE(reuseCount > 0);
	platform.Terminate();
	for (const auto& buffer : buffers)
	{
		EXPECT_TRUE(buffer.second.expired());
	}
}

template <class Platform>
void RunCommandListCache()
{
	Platform platform;
	CommandListCache(platform);
}

#ifdef __EFFEKSEER_BUILD_DX12__
TestRegister registerCommandListCacheDX12("Runtime.CommandListCache.DX12",
	RunCommandListCache<EffectPlatformDX12>, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
TestRegister registerCommandListCacheVulkan("Runtime.CommandListCache.Vulkan",
	RunCommandListCache<EffectPlatformVulkan>, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
TestRegister registerCommandListCacheMetal("Runtime.CommandListCache.Metal",
	RunCommandListCache<EffectPlatformMetal>, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
TestRegister registerCommandListCacheWebGPU("Runtime.CommandListCache.WebGPU",
	RunCommandListCache<EffectPlatformWebGPU>, TestExecutionMode::FilterOnly);
#endif
} // namespace
