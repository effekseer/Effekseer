#include "../TestHelper.h"
#include <Runtime/EffectPlatformWebGPU.h>
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h>
#include <EffekseerRendererCommon/VertexBuffer.h>

#include <array>
#include <cstring>

namespace
{
void CommandListCacheWebGPU()
{
	EffectPlatformWebGPU platform;
	EffectPlatformInitializingParameter parameters;
	parameters.VSync = false;
	platform.Initialize(parameters);
	EXPECT_TRUE(platform.Update());
	const auto background = platform.CaptureScreenPixels();
	EXPECT_TRUE(!background.empty());
	platform.Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

	// The framework's native command-list pool contains three entries.
	std::array<std::weak_ptr<EffekseerRenderer::VertexBuffer>, 3> buffers;
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
		if (frame < 3)
		{
			buffers[frame] = buffer;
		}
		else
		{
			EXPECT_TRUE(buffers[frame % 3].lock() == buffer);
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
	platform.Terminate();
	for (const auto& buffer : buffers)
	{
		EXPECT_TRUE(buffer.expired());
	}
}

TestRegister registerCommandListCacheWebGPU(
	"Runtime.CommandListCacheWebGPU", CommandListCacheWebGPU, TestExecutionMode::FilterOnly);
} // namespace
