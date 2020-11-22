#include "EffekseerRendererMetal.Renderer.h"
#include "EffekseerRendererMetal.RendererImplemented.h"
//#include "EffekseerRendererMetal.MaterialLoader.h"
#include "EffekseerRendererMetal.VertexBuffer.h"
#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.IndexBuffer.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.Shader.h"
#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.MaterialLoader.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.CommandListMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.GraphicsMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.RenderPassMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.Metal_Impl.h"

#include "../../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.h"

// #include "Shaders.h"

#include "ShaderHeader/ad_sprite_distortion_ps.h"
#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_ps.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_ps.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"

#include "ShaderHeader/sprite_unlit_vs.h"
#include "ShaderHeader/sprite_unlit_ps.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_lit_ps.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_distortion_ps.h"

#include "ShaderHeader/model_unlit_vs.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/model_lit_vs.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_distortion_vs.h"
#include "ShaderHeader/model_distortion_ps.h"

#define GENERATE_VIEW(x) {{x, static_cast<int32_t>(sizeof(x))}};

namespace EffekseerRendererMetal
{

::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
{
    auto gd = static_cast<EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
    return EffekseerRendererLLGI::CreateTextureLoader(gd, fileInterface);
}

::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::Backend::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface)
{
    auto gd = static_cast<EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
    return EffekseerRendererLLGI::CreateModelLoader(gd, fileInterface);
}

::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::Backend::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface)
{
    auto gd = static_cast<EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
    auto compiler = new ::Effekseer::MaterialCompilerMetal();
    auto ret = new ::EffekseerRendererLLGI::MaterialLoader(gd, fileInterface, ::Effekseer::CompiledMaterialPlatformType::Metal, compiler);
    ES_SAFE_RELEASE(compiler);
    return ret;
}

::Effekseer::Backend::GraphicsDevice* CreateDevice()
{
    auto graphics = new LLGI::GraphicsMetal();
    graphics->Initialize(nullptr);

	auto ret = new EffekseerRendererLLGI::Backend::GraphicsDevice(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

static void CreateFixedShaderForMetal(EffekseerRendererLLGI::FixedShader* shader)
{
    assert(shader);
    if (!shader)
        return;

	shader->AdvancedSpriteUnlit_VS = GENERATE_VIEW(metal_ad_sprite_unlit_vs);
	shader->AdvancedSpriteUnlit_PS = GENERATE_VIEW(metal_ad_sprite_unlit_ps);
	shader->AdvancedSpriteLit_VS = GENERATE_VIEW(metal_ad_sprite_lit_vs);
	shader->AdvancedSpriteLit_PS = GENERATE_VIEW(metal_ad_sprite_lit_ps);
	shader->AdvancedSpriteDistortion_VS = GENERATE_VIEW(metal_ad_sprite_distortion_vs);
	shader->AdvancedSpriteDistortion_PS = GENERATE_VIEW(metal_ad_sprite_distortion_ps);

	shader->AdvancedModelUnlit_VS = GENERATE_VIEW(metal_ad_model_unlit_vs);
	shader->AdvancedModelUnlit_PS = GENERATE_VIEW(metal_ad_model_unlit_ps);
	shader->AdvancedModelLit_VS = GENERATE_VIEW(metal_ad_model_lit_vs);
	shader->AdvancedModelLit_PS = GENERATE_VIEW(metal_ad_model_lit_ps);
	shader->AdvancedModelDistortion_VS = GENERATE_VIEW(metal_ad_model_distortion_vs);
	shader->AdvancedModelDistortion_PS = GENERATE_VIEW(metal_ad_model_distortion_ps);

	shader->SpriteUnlit_VS = GENERATE_VIEW(metal_sprite_unlit_vs);
	shader->SpriteUnlit_PS = GENERATE_VIEW(metal_sprite_unlit_ps);
	shader->SpriteLit_VS = GENERATE_VIEW(metal_sprite_lit_vs);
	shader->SpriteLit_PS = GENERATE_VIEW(metal_sprite_lit_ps);
	shader->SpriteDistortion_VS = GENERATE_VIEW(metal_sprite_distortion_vs);
	shader->SpriteDistortion_PS = GENERATE_VIEW(metal_sprite_distortion_ps);

	shader->ModelUnlit_VS = GENERATE_VIEW(metal_model_unlit_vs);
	shader->ModelUnlit_PS = GENERATE_VIEW(metal_model_unlit_ps);
	shader->ModelLit_VS = GENERATE_VIEW(metal_model_lit_vs);
	shader->ModelLit_PS = GENERATE_VIEW(metal_model_lit_ps);
	shader->ModelDistortion_VS = GENERATE_VIEW(metal_model_distortion_vs);
	shader->ModelDistortion_PS = GENERATE_VIEW(metal_model_distortion_ps);
}

::EffekseerRenderer::Renderer* Create(
                                      ::Effekseer::Backend::GraphicsDevice* graphicsDevice,
                                      int32_t squareMaxCount,
                                      MTLPixelFormat renderTargetFormat,
                                      MTLPixelFormat depthStencilFormat,
									  bool isReversedDepth)
{
    RendererImplemented* renderer = new RendererImplemented(squareMaxCount);
    renderer->materialCompiler_ = new ::Effekseer::MaterialCompilerMetal();

    CreateFixedShaderForMetal(&renderer->fixedShader_);

	auto gd = static_cast<EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
    auto g = static_cast<LLGI::GraphicsMetal*>(gd->GetGraphics());
    LLGI::RenderPassPipelineStateKey key;
    key.RenderTargetFormats.resize(1);
    key.RenderTargetFormats.at(0) = LLGI::ConvertFormat(renderTargetFormat);
    key.DepthFormat = LLGI::ConvertFormat(depthStencilFormat);
    key.IsColorCleared = false;
    key.IsDepthCleared = false;
    
    auto pipelineState = g->CreateRenderPassPipelineState(key);

    if (renderer->Initialize(gd, pipelineState, isReversedDepth))
    {
        ES_SAFE_RELEASE(pipelineState);
        return renderer;
    }

    ES_SAFE_RELEASE(pipelineState);

    ES_SAFE_DELETE(renderer);

    return nullptr;
}

::EffekseerRenderer::Renderer* Create(int32_t squareMaxCount,
                                      MTLPixelFormat renderTargetFormat,
                                      MTLPixelFormat depthStencilFormat,
									  bool isReversedDepth)
{
	auto graphicDevice = CreateDevice();

	auto ret = Create(graphicDevice, squareMaxCount, renderTargetFormat, depthStencilFormat, isReversedDepth);

	if (ret != nullptr)
	{
		ES_SAFE_RELEASE(graphicDevice);
		return ret;
	}

	ES_SAFE_RELEASE(graphicDevice);
	return nullptr;

	auto graphics = new LLGI::GraphicsMetal();
    graphics->Initialize(nullptr);
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

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsMetal*>(gd->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateCommandList(r->GetGraphicsDevice(), memoryPool);
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDevice* graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsMetal*>(gd->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
    return CreateSingleFrameMemoryPool(r->GetGraphicsDevice());
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLRenderCommandEncoder> encoder)
{
	assert(commandList != nullptr);

    LLGI::CommandListMetalPlatformRenderPassContext context;
    context.RenderEncoder = encoder;

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->BeginWithPlatform(nullptr);
	c->GetInternal()->BeginRenderPassWithPlatformPtr(&context);
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->EndRenderPassWithPlatformPtr();
	c->GetInternal()->EndWithPlatform();
}

void RendererImplemented::GenerateVertexBuffer()
{
    // Metal doesn't need to update buffer to make sure it has the correct size
    auto sc = std::max(4000, m_squareMaxCount);
    m_vertexBuffer = VertexBuffer::Create(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * sc * 4, true, false);
}

void RendererImplemented::GenerateIndexBuffer()
{
    auto sc = std::max(4000, m_squareMaxCount);

	m_indexBuffer = EffekseerRendererLLGI::IndexBuffer::Create(graphicsDevice_, sc * 6, false, false);
	if (m_indexBuffer == nullptr)
		return;

	m_indexBuffer->Lock();

	for (int i = 0; i < sc; i++)
	{
		uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect(6);
		buf[0] = 3 + 4 * i;
		buf[1] = 1 + 4 * i;
		buf[2] = 0 + 4 * i;
		buf[3] = 3 + 4 * i;
		buf[4] = 0 + 4 * i;
		buf[5] = 2 + 4 * i;
	}

	m_indexBuffer->Unlock();
}

/*
void RendererImplemented::SetExternalCommandBuffer(id<MTLCommandBuffer> extCommandBuffer)
{
    if (commandList_ != nullptr)
    {
        auto clm = static_cast<LLGI::CommandListMetal*>(GetCurrentCommandList());
        [extCommandBuffer retain];
        [clm->GetImpl()->commandBuffer release];
        clm->GetImpl()->commandBuffer = extCommandBuffer;
    }
}
    
void RendererImplemented::SetExternalRenderEncoder(id<MTLRenderCommandEncoder> extRenderEncoder)
{
    if (commandList_ != nullptr)
    {
        auto clm = static_cast<LLGI::CommandListMetal*>(GetCurrentCommandList());
        [extRenderEncoder retain];
        [clm->GetImpl()->renderEncoder release];
        clm->GetImpl()->renderEncoder = extRenderEncoder;
    }
}

bool RendererImplemented::BeginRendering()
{
    assert(GetGraphics() != NULL);

    impl->CalculateCameraProjectionMatrix();

    // initialize states
    m_renderState->GetActiveState().Reset();
    m_renderState->Update(true);

    if (commandList_ != nullptr)
    {
#ifdef __EFFEKSEER_RENDERERMETAL_INTERNAL_COMMAND_BUFFER__
        GetCurrentCommandList()->Begin();
#else
        GetCurrentCommandList()->CommandList::Begin();
#endif
#ifdef __EFFEKSEER_RENDERERMETAL_INTERNAL_RENDER_PASS__
        auto g = static_cast<LLGI::GraphicsMetal*>(GetGraphics());
        GetCurrentCommandList()->BeginRenderPass(g->GetRenderPass());
#else
        GetCurrentCommandList()->CommandList::BeginRenderPass(nullptr);
#endif
    }

    // reset renderer
    m_standardRenderer->ResetAndRenderingIfRequired();

    return true;
}

bool RendererImplemented::EndRendering()
{
    assert(GetGraphics() != NULL);

    // reset renderer
    m_standardRenderer->ResetAndRenderingIfRequired();

    if (commandList_ != nullptr)
    {
#ifdef __EFFEKSEER_RENDERERMETAL_INTERNAL_RENDER_PASS__
        GetCurrentCommandList()->EndRenderPass();
#endif
#ifdef __EFFEKSEER_RENDERERMETAL_INTERNAL_COMMAND_BUFFER__
        GetCurrentCommandList()->End();
        GetGraphics()->Execute(GetCurrentCommandList());
#endif
    }
    return true;
}
*/

/*
::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface) {

    if (materialCompiler_ == nullptr)
        return nullptr;

    return new MaterialLoader(this->GetGraphicsDevice(), fileInterface, platformType_, materialCompiler_);
}
*/

} // namespace EffekseerRendererMetal
