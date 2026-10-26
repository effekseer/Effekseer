#include "Common.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include "GraphicsDevice.h"

namespace EffekseerRenderer
{

void ChangeRenderPassPipelineState(EffekseerRenderer::RendererRef renderer, RenderPassPipelineStateKey key)
{
	auto r = renderer.DownCast<EffekseerRendererLLGI::RendererImplemented>();

	LLGI::RenderPassPipelineStateKey llgiKey;

	llgiKey.RenderTargetFormats.resize(key.RenderTargetCount);

	for (size_t i = 0; i < llgiKey.RenderTargetFormats.size(); i++)
	{
		llgiKey.RenderTargetFormats.at(i) = EffekseerRendererLLGI::ConvertTextureFormat(key.RenderTargetFormats[i]);
	}
	llgiKey.DepthFormat = EffekseerRendererLLGI::ConvertTextureFormat(key.DepthFormat);

	r->ChangeRenderPassPipelineState(llgiKey);
}

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::Graphics*>(gd->GetGraphics());
	g->WaitFinish();
}

Effekseer::RefPtr<EffekseerRenderer::CommandList> CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
																	Effekseer::RefPtr<::EffekseerRenderer::SingleFrameMemoryPool> memoryPool)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::Graphics*>(gd->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool.Get());
	auto commandList = g->CreateCommandList(mp->GetInternal());
	if (!commandList)
	{
		return nullptr;
	}
	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(g, commandList, mp->GetInternal(), mp->GetGpuParticleMemoryPool());
	ES_SAFE_RELEASE(commandList);
	if (!ret->Initialize(mp->GetInternal()->GetSwapBufferCount()))
	{
		return nullptr;
	}
	return ret;
}

Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::Graphics*>(gd->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	// Reserve 8 MiB per frame for GPU particle constants, independently of renderer allocations.
	auto gpuParticleMemoryPool = LLGI::CreateSharedPtr(g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128));
	if (!mp || !gpuParticleMemoryPool)
	{
		ES_SAFE_RELEASE(mp);
		return nullptr;
	}
	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::SingleFrameMemoryPool>(mp, std::move(gpuParticleMemoryPool));
	ES_SAFE_RELEASE(mp);
	return ret;
}

} // namespace EffekseerRenderer
