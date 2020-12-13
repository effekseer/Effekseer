#include "EffekseerRendererDX12.Renderer.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.CommandListDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../EffekseerMaterialCompiler/DirectX12/EffekseerMaterialCompilerDX12.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace Sprite_Unlit_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_unlit_vs.h"
} // namespace Sprite_Unlit_VS

namespace Sprite_Unlit_PS_Ad
{
static
#include "ShaderHeader/ad_sprite_unlit_ps.h"
} // namespace Sprite_Unlit_PS

namespace Sprite_Distortion_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_distortion_vs.h"
} // namespace Sprite_Distortion_VS

namespace Sprite_Distortion_PS_Ad
{
static
#include "ShaderHeader/ad_sprite_distortion_ps.h"
} // namespace Sprite_Distortion_PS

namespace Sprite_Lit_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_lit_vs.h"
} // namespace Sprite_Lit_VS

namespace Sprite_Lit_PS_Ad
{
static
#include "ShaderHeader/ad_sprite_lit_ps.h"
} // namespace Sprite_Lit_PS

namespace Model_Unlit_VS_Ad
{
static
#include "ShaderHeader/ad_model_unlit_vs.h"
} // namespace Model_Unlit_VS

namespace Model_Unlit_PS_Ad
{
static
#include "ShaderHeader/ad_model_unlit_ps.h"
} // namespace Model_Unlit_PS

namespace Model_Distortion_VS_Ad
{
static
#include "ShaderHeader/ad_model_distortion_vs.h"
} // namespace Model_Distortion_VS

namespace Model_Distortion_PS_Ad
{
static
#include "ShaderHeader/ad_model_distortion_ps.h"
} // namespace Model_Distortion_PS

namespace Model_Lit_VS_Ad
{
static
#include "ShaderHeader/ad_model_lit_vs.h"
} // namespace Model_Lit_VS

namespace Model_Lit_PS_Ad
{
static
#include "ShaderHeader/ad_model_lit_ps.h"
} // namespace Model_Lit_PS

namespace Sprite_Unlit_VS
{
static
#include "ShaderHeader/sprite_unlit_vs.h"
} // namespace Sprite_Unlit_VS

namespace Sprite_Unlit_PS
{
static
#include "ShaderHeader/sprite_unlit_ps.h"
} // namespace Sprite_Unlit_PS

namespace Sprite_Distortion_VS
{
static
#include "ShaderHeader/sprite_distortion_vs.h"
} // namespace Sprite_Distortion_VS

namespace Sprite_Distortion_PS
{
static
#include "ShaderHeader/sprite_distortion_ps.h"
} // namespace Sprite_Distortion_PS

namespace Sprite_Lit_VS
{
static
#include "ShaderHeader/sprite_lit_vs.h"
} // namespace Sprite_Lit_VS

namespace Sprite_Lit_PS
{
static
#include "ShaderHeader/sprite_lit_ps.h"
} // namespace Sprite_Lit_PS

namespace Model_Unlit_VS
{
static
#include "ShaderHeader/model_unlit_vs.h"
} // namespace Model_Unlit_VS

namespace Model_Unlit_PS
{
static
#include "ShaderHeader/model_unlit_ps.h"
} // namespace Model_Unlit_PS

namespace Model_Distortion_VS
{
static
#include "ShaderHeader/model_distortion_vs.h"
} // namespace Model_Distortion_VS

namespace Model_Distortion_PS
{
static
#include "ShaderHeader/model_distortion_ps.h"
} // namespace Model_Distortion_PS

namespace Model_Lit_VS
{
static
#include "ShaderHeader/model_lit_vs.h"
} // namespace Model_Lit_VS

namespace Model_Lit_PS
{
static
#include "ShaderHeader/model_lit_ps.h"
} // namespace Model_Lit_PS

namespace EffekseerRendererDX12
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(ID3D12Device* device, ID3D12CommandQueue* commandQueue, int32_t swapBufferCount)
{
	std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>()> getScreenFunc =
		[]() -> std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*> {
		return std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>();
	};

	auto graphics = new LLGI::GraphicsDX12(
		device, getScreenFunc, []() -> void {}, commandQueue, swapBufferCount);

	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::Backend::GraphicsDevice>(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::EffekseerRenderer::RendererRef Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
									  DXGI_FORMAT* renderTargetFormats,
									  int32_t renderTargetCount,
									  DXGI_FORMAT depthFormat,
									  bool isReversedDepth,
									  int32_t squareMaxCount)
{
	auto renderer = ::Effekseer::MakeRefPtr<::EffekseerRendererLLGI::RendererImplemented>(squareMaxCount);

	auto allocate_ = [](std::vector<LLGI::DataStructure>& ds, const unsigned char* data, int32_t size) -> void {
		ds.resize(1);
		ds[0].Size = size;
		ds[0].Data = data;
		return;
	};

	allocate_(renderer->fixedShader_.AdvancedSpriteUnlit_VS, Sprite_Unlit_VS_Ad::g_main, sizeof(Sprite_Unlit_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedSpriteUnlit_PS, Sprite_Unlit_PS_Ad::g_main, sizeof(Sprite_Unlit_PS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedSpriteDistortion_VS, Sprite_Distortion_VS_Ad::g_main, sizeof(Sprite_Distortion_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedSpriteDistortion_PS, Sprite_Distortion_PS_Ad::g_main, sizeof(Sprite_Distortion_PS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedSpriteLit_VS, Sprite_Lit_VS_Ad::g_main, sizeof(Sprite_Lit_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedSpriteLit_PS, Sprite_Lit_PS_Ad::g_main, sizeof(Sprite_Lit_PS_Ad::g_main));

	allocate_(renderer->fixedShader_.AdvancedModelUnlit_VS, Model_Unlit_VS_Ad::g_main, sizeof(Model_Unlit_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedModelUnlit_PS, Model_Unlit_PS_Ad::g_main, sizeof(Model_Unlit_PS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedModelDistortion_VS, Model_Distortion_VS_Ad::g_main, sizeof(Model_Distortion_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedModelDistortion_PS, Model_Distortion_PS_Ad::g_main, sizeof(Model_Distortion_PS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedModelLit_VS, Model_Lit_VS_Ad::g_main, sizeof(Model_Lit_VS_Ad::g_main));
	allocate_(renderer->fixedShader_.AdvancedModelLit_PS, Model_Lit_PS_Ad::g_main, sizeof(Model_Lit_PS_Ad::g_main));

	allocate_(renderer->fixedShader_.SpriteUnlit_VS, Sprite_Unlit_VS::g_main, sizeof(Sprite_Unlit_VS::g_main));
	allocate_(renderer->fixedShader_.SpriteUnlit_PS, Sprite_Unlit_PS::g_main, sizeof(Sprite_Unlit_PS::g_main));

	allocate_(renderer->fixedShader_.SpriteDistortion_VS, Sprite_Distortion_VS::g_main, sizeof(Sprite_Distortion_VS::g_main));
	allocate_(renderer->fixedShader_.SpriteDistortion_PS, Sprite_Distortion_PS::g_main, sizeof(Sprite_Distortion_PS::g_main));

	allocate_(renderer->fixedShader_.SpriteLit_VS, Sprite_Lit_VS::g_main, sizeof(Sprite_Lit_VS::g_main));
	allocate_(renderer->fixedShader_.SpriteLit_PS, Sprite_Lit_PS::g_main, sizeof(Sprite_Lit_PS::g_main));

	allocate_(renderer->fixedShader_.ModelUnlit_VS, Model_Unlit_VS::g_main, sizeof(Model_Unlit_VS::g_main));
	allocate_(renderer->fixedShader_.ModelUnlit_PS, Model_Unlit_PS::g_main, sizeof(Model_Unlit_PS::g_main));

	allocate_(renderer->fixedShader_.ModelLit_VS,
			  Model_Lit_VS::g_main,
			  sizeof(Model_Lit_VS::g_main));
	allocate_(renderer->fixedShader_.ModelLit_PS,
			  Model_Lit_PS::g_main,
			  sizeof(Model_Lit_PS::g_main));

	allocate_(
		renderer->fixedShader_.ModelDistortion_VS, Model_Distortion_VS::g_main, sizeof(Model_Distortion_VS::g_main));
	allocate_(
		renderer->fixedShader_.ModelDistortion_PS, Model_Distortion_PS::g_main, sizeof(Model_Distortion_PS::g_main));

	renderer->platformType_ = Effekseer::CompiledMaterialPlatformType::DirectX12;
	renderer->materialCompiler_ = new Effekseer::MaterialCompilerDX12();

	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(renderTargetCount);

	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = LLGI::ConvertFormat(renderTargetFormats[i]);
	}

	key.DepthFormat = LLGI::ConvertFormat(depthFormat);

	auto gd = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	auto pipelineState = gd->GetGraphics()->CreateRenderPassPipelineState(key);

	if (renderer->Initialize(gd, pipelineState, isReversedDepth))
	{
		ES_SAFE_RELEASE(pipelineState);
		return renderer;
	}

	ES_SAFE_RELEASE(pipelineState);

	return nullptr;
}

::EffekseerRenderer::RendererRef Create(ID3D12Device* device,
									  ID3D12CommandQueue* commandQueue,
									  int32_t swapBufferCount,
									  DXGI_FORMAT* renderTargetFormats,
									  int32_t renderTargetCount,
									  DXGI_FORMAT depthFormat,
									  bool isReversedDepth,
									  int32_t squareMaxCount)
{
	auto graphicDevice = CreateGraphicsDevice(device, commandQueue, swapBufferCount);

	auto ret = Create(graphicDevice, renderTargetFormats, renderTargetCount, depthFormat, isReversedDepth, squareMaxCount);

	if (ret != nullptr)
	{
		return ret;
	}

	return nullptr;
}

Effekseer::TextureRef CreateTexture(::EffekseerRenderer::RendererRef renderer, ID3D12Resource* texture)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer.Get());
	return CreateTexture(::Effekseer::Backend::GraphicsDeviceRef(r->GetGraphicsDevice()), texture);
}

Effekseer::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ID3D12Resource* texture)
{
	auto g = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	return g->CreateTexture((uint64_t)texture, [] {});
}

void FlushAndWait(::EffekseerRenderer::RendererRef renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer.Get());
	FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef(r->GetGraphicsDevice()));
}

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::GraphicsDX12*>(gd->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::RendererRef renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer.Get());
	return CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef(r->GetGraphicsDevice()), memoryPool);
}

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::GraphicsDX12*>(gd->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::RendererRef renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer.Get());
	return CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef(r->GetGraphicsDevice()));
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto g = static_cast<LLGI::GraphicsDX12*>(gd->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList, ID3D12GraphicsCommandList* dx12CommandList)
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

} // namespace EffekseerRendererDX12