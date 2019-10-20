#include "EffekseerRendererDX12.Renderer.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace Standard_VS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
} // namespace Standard_VS

namespace Standard_PS
{
static
#include "Shader/EffekseerRenderer.Standard_PS.h"
} // namespace Standard_PS

namespace Standard_Distortion_VS
{
static
#include "Shader/EffekseerRenderer.Standard_Distortion_VS.h"
} // namespace Standard_Distortion_VS

namespace Standard_Distortion_PS
{
static
#include "Shader/EffekseerRenderer.Standard_Distortion_PS.h"
} // namespace Standard_Distortion_PS

namespace ShaderLightingTextureNormal_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h"

	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h"

} // namespace ShaderLightingTextureNormal_

namespace ShaderTexture_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h"

	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h"
} // namespace ShaderTexture_

namespace ShaderDistortionTexture_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"

	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortion_PS.h"
} // namespace ShaderDistortionTexture_


namespace EffekseerRendererDX12
{

::EffekseerRenderer::Renderer* Create(ID3D12Device* device,
									  int32_t swapBufferCount,
									  ID3D12CommandQueue* commandQueue,
									  std::function<void()> flushAndWaitQueueFunc,
									  bool isReversedDepth,
									  int32_t squareMaxCount)
{

	/*
	ID3D12Device* device,
				 std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, ID3D12Resource*>()> getScreenFunc,
				 std::function<void()> waitFunc,
				 ID3D12CommandQueue* commandQueue,
				 int32_t swapBufferCount);
	*/
	std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>()> getScreenFunc =
		[]() -> std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*> {
		return std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>();
	};

	auto graphics = new LLGI::GraphicsDX12(device, getScreenFunc, flushAndWaitQueueFunc, commandQueue, swapBufferCount);

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

	allocate_(renderer->fixedShader_.ModelShaderLightingTextureNormal_VS, ShaderLightingTextureNormal_::g_VS, sizeof(ShaderLightingTextureNormal_::g_VS));
	allocate_(renderer->fixedShader_.ModelShaderLightingTextureNormal_PS, ShaderLightingTextureNormal_::g_PS, sizeof(ShaderLightingTextureNormal_::g_PS));

	allocate_(renderer->fixedShader_.ModelShaderDistortionTexture_VS, ShaderDistortionTexture_::g_VS, sizeof(ShaderDistortionTexture_::g_VS));
	allocate_(renderer->fixedShader_.ModelShaderDistortionTexture_PS, ShaderDistortionTexture_::g_PS, sizeof(ShaderDistortionTexture_::g_PS));

	if (renderer->Initialize(graphics, isReversedDepth))
	{
		ES_SAFE_RELEASE(graphics);
		return renderer;
	}

	ES_SAFE_DELETE(renderer);

	return nullptr;
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, ID3D12Resource* texture)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
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
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->Begin();
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->End();
}

void ExecuteCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetGraphics()->Execute(c->GetInternal());
}

} // namespace EffekseerRendererDX12