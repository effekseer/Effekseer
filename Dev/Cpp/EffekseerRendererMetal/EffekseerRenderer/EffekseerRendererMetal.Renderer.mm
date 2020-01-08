#include "EffekseerRendererMetal.Renderer.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.CommandListMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.GraphicsMetal.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

#import <Metal/Metal.h>

namespace EffekseerRendererMetal
{

::EffekseerRenderer::Renderer* Create(id<MTLDevice> device,
									  bool isReversedDepth,
									  int32_t squareMaxCount)
{
	/*
	std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>()> getScreenFunc =
		[]() -> std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*> {
		return std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>();
	};

	auto graphics = new LLGI::GraphicsDX12(
		device, getScreenFunc, []() -> void {}, commandQueue, swapBufferCount);

	::EffekseerRendererLLGI::RendererImplemented* renderer = new ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount);

	auto allocate_ = [](std::vector<LLGI::DataStructure>& ds, const unsigned char* data, int32_t size) -> void {
		ds.resize(1);
		ds[0].Size = size;
		ds[0].Data = data;
		return;
	};

	allocate_(renderer->fixedShader_.StandardTexture_VS, Standard_VS::g_VS, sizeof(Standard_VS::g_VS));
	allocate_(renderer->fixedShader_.StandardTexture_PS, Standard_PS::g_PS, sizeof(Standard_PS::g_PS));

	allocate_(renderer->fixedShader_.StandardDistortedTexture_VS, Standard_Distortion_VS::g_VS, sizeof(Standard_Distortion_VS::g_VS));
	allocate_(renderer->fixedShader_.StandardDistortedTexture_PS, Standard_Distortion_PS::g_PS, sizeof(Standard_Distortion_PS::g_PS));

	allocate_(renderer->fixedShader_.ModelShaderTexture_VS, ShaderTexture_::g_VS, sizeof(ShaderTexture_::g_VS));
	allocate_(renderer->fixedShader_.ModelShaderTexture_PS, ShaderTexture_::g_PS, sizeof(ShaderTexture_::g_PS));

	allocate_(renderer->fixedShader_.ModelShaderLightingTextureNormal_VS,
			  ShaderLightingTextureNormal_::g_VS,
			  sizeof(ShaderLightingTextureNormal_::g_VS));
	allocate_(renderer->fixedShader_.ModelShaderLightingTextureNormal_PS,
			  ShaderLightingTextureNormal_::g_PS,
			  sizeof(ShaderLightingTextureNormal_::g_PS));

	allocate_(
		renderer->fixedShader_.ModelShaderDistortionTexture_VS, ShaderDistortionTexture_::g_VS, sizeof(ShaderDistortionTexture_::g_VS));
	allocate_(
		renderer->fixedShader_.ModelShaderDistortionTexture_PS, ShaderDistortionTexture_::g_PS, sizeof(ShaderDistortionTexture_::g_PS));

	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(renderTargetCount);

	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = LLGI::ConvertFormat(renderTargetFormats[i]);
	}

	key.HasDepth = hasDepth;

	auto pipelineState = graphics->CreateRenderPassPipelineState(key);

	if (renderer->Initialize(graphics, pipelineState, isReversedDepth))
	{
		ES_SAFE_RELEASE(graphics);
		ES_SAFE_RELEASE(pipelineState);
		return renderer;
	}

	ES_SAFE_RELEASE(graphics);
	ES_SAFE_RELEASE(pipelineState);

	ES_SAFE_DELETE(renderer);
*/
	return nullptr;
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, id<MTLTexture> texture)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsMetal*>(r->GetGraphics());
	auto texture_ = g->CreateTexture((uint64_t)texture);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture_;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 0;
	textureData->Height = 0;
	return textureData;
}

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData)
{
	auto texture = (LLGI::Texture*)textureData->UserPtr;
	texture->Release();
	delete textureData;
}

void FlushAndWait(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsMetal*>(r->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsMetal*>(r->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsMetal*>(r->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLCommandBuffer> commandBuffer)
{
	assert(commandList != nullptr);

	LLGI::PlatformContextDX12 context;
	context.commandList = dx12CommandList;

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->BeginWithPlatform(&context);
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->EndWithPlatform();
}

} // namespace EffekseerRendererMetal