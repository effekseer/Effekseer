

#include "EffekseerRendererLLGI.Renderer.h"
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.IndexBuffer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.VertexBuffer.h"
//#include "EffekseerRendererLLGI.SpriteRenderer.h"
//#include "EffekseerRendererLLGI.RibbonRenderer.h"
//#include "EffekseerRendererLLGI.RingRenderer.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
//#include "EffekseerRendererLLGI.TrackRenderer.h"
#include "EffekseerRendererLLGI.MaterialLoader.h"
#include "EffekseerRendererLLGI.ModelLoader.h"
//#include "EffekseerRendererLLGI.TextureLoader.h"

#include "../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../EffekseerRendererCommon/TextureLoader.h"
#endif

namespace EffekseerRendererLLGI
{

::Effekseer::TextureLoader* CreateTextureLoader(Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new EffekseerRenderer::TextureLoader(graphicsDevice, fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* CreateModelLoader(Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
{
	return new ModelLoader(graphicsDevice, fileInterface);
}

bool PiplineStateKey::operator<(const PiplineStateKey& v) const
{
	if (shader != v.shader)
		return shader < v.shader;
	if (state.AlphaBlend != v.state.AlphaBlend)
		return state.AlphaBlend < v.state.AlphaBlend;
	if (state.CullingType != v.state.CullingType)
		return state.CullingType < v.state.CullingType;
	if (state.DepthTest != v.state.DepthTest)
		return v.state.DepthTest;
	if (state.DepthWrite != v.state.DepthWrite)
		return v.state.DepthWrite;

	for (int i = 0; i < 4; i++)
	{
		if (state.TextureFilterTypes[i] != v.state.TextureFilterTypes[i])
			return state.TextureFilterTypes[i] < v.state.TextureFilterTypes[i];
		if (state.TextureWrapTypes[i] != v.state.TextureWrapTypes[i])
			return state.TextureWrapTypes[i] < v.state.TextureWrapTypes[i];
	}

	if (topologyType != v.topologyType)
		return topologyType < v.topologyType;

	if (renderPassPipelineState != v.renderPassPipelineState)
		return renderPassPipelineState < v.renderPassPipelineState;

	return false;
}

LLGI::CommandList* RendererImplemented::GetCurrentCommandList()
{
	if (commandList_ != nullptr)
	{
		return commandList_->GetInternal();
	}

	assert(0);
	return nullptr;
}

LLGI::PipelineState* RendererImplemented::GetOrCreatePiplineState()
{
	PiplineStateKey key;
	key.state = m_renderState->GetActiveState();
	key.shader = currentShader;
	key.topologyType = currentTopologyType_;
	key.renderPassPipelineState = renderPassPipelineState_;

	auto it = piplineStates_.find(key);
	if (it != piplineStates_.end())
	{
		return it->second;
	}

	auto piplineState = GetGraphics()->CreatePiplineState();

	if (isReversedDepth_)
	{
		piplineState->DepthFunc = LLGI::DepthFuncType::GreaterEqual;
	}
	else
	{
		piplineState->DepthFunc = LLGI::DepthFuncType::LessEqual;
	}

	piplineState->SetShader(LLGI::ShaderStageType::Vertex, currentShader->GetVertexShader());
	piplineState->SetShader(LLGI::ShaderStageType::Pixel, currentShader->GetPixelShader());

	for (auto i = 0; i < currentShader->GetVertexLayouts().size(); i++)
	{
		piplineState->VertexLayouts[i] = currentShader->GetVertexLayouts()[i].Format;
		piplineState->VertexLayoutNames[i] = currentShader->GetVertexLayouts()[i].Name;
		piplineState->VertexLayoutSemantics[i] = currentShader->GetVertexLayouts()[i].Semantic;
	}
	piplineState->VertexLayoutCount = currentShader->GetVertexLayouts().size();

	piplineState->Topology = currentTopologyType_;

	piplineState->IsDepthTestEnabled = key.state.DepthTest;
	piplineState->IsDepthWriteEnabled = key.state.DepthWrite;
	piplineState->Culling = (LLGI::CullingMode)key.state.CullingType;

	piplineState->IsBlendEnabled = true;
	piplineState->BlendSrcFuncAlpha = LLGI::BlendFuncType::One;
	piplineState->BlendDstFuncAlpha = LLGI::BlendFuncType::One;
	piplineState->BlendEquationAlpha = LLGI::BlendEquationType::Max;

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Opacity)
	{
		piplineState->IsBlendEnabled = false;
		piplineState->IsBlendEnabled = true;
		piplineState->BlendDstFunc = LLGI::BlendFuncType::Zero;
		piplineState->BlendSrcFunc = LLGI::BlendFuncType::One;
		piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
	}

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Blend)
	{
		piplineState->BlendDstFunc = LLGI::BlendFuncType::OneMinusSrcAlpha;
		piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
		piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
	}

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Add)
	{
		piplineState->BlendDstFunc = LLGI::BlendFuncType::One;
		piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
		piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
	}

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Sub)
	{
		piplineState->BlendDstFunc = LLGI::BlendFuncType::One;
		piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
		piplineState->BlendEquationRGB = LLGI::BlendEquationType::ReverseSub;
		piplineState->BlendSrcFuncAlpha = LLGI::BlendFuncType::Zero;
		piplineState->BlendDstFuncAlpha = LLGI::BlendFuncType::One;
		piplineState->BlendEquationAlpha = LLGI::BlendEquationType::Add;
	}

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Mul)
	{
		piplineState->BlendDstFunc = LLGI::BlendFuncType::SrcColor;
		piplineState->BlendSrcFunc = LLGI::BlendFuncType::Zero;
		piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
		piplineState->BlendSrcFuncAlpha = LLGI::BlendFuncType::Zero;
		piplineState->BlendDstFuncAlpha = LLGI::BlendFuncType::One;
		piplineState->BlendEquationAlpha = LLGI::BlendEquationType::Add;
	}

	piplineState->SetRenderPassPipelineState(renderPassPipelineState_);

	if (!piplineState->Compile())
	{
		assert(0);
	}

	piplineStates_[key] = piplineState;

	return piplineState;
}

void RendererImplemented::GenerateVertexBuffer()
{
	m_vertexBuffer =
		VertexBuffer::Create(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true, false);
}

void RendererImplemented::GenerateIndexBuffer()
{
	m_indexBuffer = IndexBuffer::Create(graphicsDevice_, m_squareMaxCount * 6, false, false);
	if (m_indexBuffer == nullptr)
		return;

	m_indexBuffer->Lock();

	for (int i = 0; i < m_squareMaxCount; i++)
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

RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: graphicsDevice_(nullptr)
	, m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_renderState(NULL)

	, shader_(nullptr)
	, shader_unlit_(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_background.UserPtr = nullptr;
}

RendererImplemented::~RendererImplemented()
{
	// to prevent objects to be disposed before finish renderings.
	GetGraphics()->WaitFinish();

	for (auto p : piplineStates_)
	{
		p.second->Release();
	}
	piplineStates_.clear();

	ES_SAFE_RELEASE(renderPassPipelineState_);

	ES_SAFE_RELEASE(commandList_);

	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(m_distortingCallback);

	auto p = (LLGI::Texture*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);

	ES_SAFE_DELETE(m_standardRenderer);

	ES_SAFE_DELETE(shader_);
	ES_SAFE_DELETE(shader_lit_);
	ES_SAFE_DELETE(shader_unlit_);

	ES_SAFE_DELETE(shader_ad_unlit_);
	ES_SAFE_DELETE(shader_ad_lit_);
	ES_SAFE_DELETE(shader_ad_distortion_);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	if (materialCompiler_ != nullptr)
	{
		materialCompiler_->Release();
		materialCompiler_ = nullptr;
	}
	LLGI::SafeRelease(graphicsDevice_);
}

void RendererImplemented::OnLostDevice()
{
}

void RendererImplemented::OnResetDevice()
{
}

bool RendererImplemented::Initialize(LLGI::Graphics* graphics, LLGI::RenderPassPipelineState* renderPassPipelineState, bool isReversedDepth)
{

	auto gd = new Backend::GraphicsDevice(graphics);

	auto ret = Initialize(gd, renderPassPipelineState, isReversedDepth);

	ES_SAFE_RELEASE(gd);

	return ret;
}

bool RendererImplemented::Initialize(Backend::GraphicsDevice* graphicsDevice,
									 LLGI::RenderPassPipelineState* renderPassPipelineState,
									 bool isReversedDepth)
{
	graphicsDevice_ = graphicsDevice;
	renderPassPipelineState_ = renderPassPipelineState;
	isReversedDepth_ = isReversedDepth;

	LLGI::SafeAddRef(graphicsDevice_);
	LLGI::SafeAddRef(renderPassPipelineState_);

	// Generate vertex buffer
	{
		GenerateVertexBuffer();
		if (m_vertexBuffer == NULL)
			return false;
	}

	// Generate index buffer
	{
		GenerateIndexBuffer();
		if (m_indexBuffer == NULL)
			return false;
	}

	// Generate index buffer for rendering wireframes
	{
		m_indexBufferForWireframe = IndexBuffer::Create(graphicsDevice_, m_squareMaxCount * 8, false, false);
		if (m_indexBufferForWireframe == NULL)
			return false;

		m_indexBufferForWireframe->Lock();

		for (int i = 0; i < m_squareMaxCount; i++)
		{
			uint16_t* buf = (uint16_t*)m_indexBufferForWireframe->GetBufferDirect(8);
			buf[0] = 0 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 2 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
			buf[6] = 1 + 4 * i;
			buf[7] = 3 + 4 * i;
		}

		m_indexBufferForWireframe->Unlock();
	}

	m_renderState = new RenderState(this);

	// shader
	// pos(3) color(1) uv(2)
	std::vector<VertexLayout> layouts;
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 2});

	std::vector<VertexLayout> layouts_distort;
	layouts_distort.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_distort.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_distort.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 2});
	layouts_distort.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 3});
	layouts_distort.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 4});

	std::vector<VertexLayout> layouts_ad;
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 2});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 3}); // AlphaTextureUV + UVDistortionTextureUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});		 // BlendUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 5}); // BlendAlphaUV + BlendUVDistortionUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 6});			 // FlipbookIndexAndNextRate
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 7});			 // AlphaThreshold

	std::vector<VertexLayout> layouts_distort_ad;
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 2});
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 3});
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 4});
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 5}); // AlphaTextureUV + UVDistortionTextureUV
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 6});		 // BlendUV
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 7}); // BlendAlphaUV + BlendUVDistortionUV
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 8});			 // FlipbookIndexAndNextRate
	layouts_distort_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 9});			 // AlphaThreshold

	shader_ = Shader::Create(graphicsDevice_,
							 fixedShader_.SpriteUnlit_VS.data(),
							 fixedShader_.SpriteUnlit_VS.size(),
							 fixedShader_.SpriteUnlit_PS.data(),
							 fixedShader_.SpriteUnlit_PS.size(),
							 "StandardRenderer",
							 layouts,
							 false);
	if (shader_ == NULL)
		return false;

	shader_unlit_ = Shader::Create(graphicsDevice_,
								   fixedShader_.SpriteDistortion_VS.data(),
								   fixedShader_.SpriteDistortion_VS.size(),
								   fixedShader_.SpriteDistortion_PS.data(),
								   fixedShader_.SpriteDistortion_PS.size(),
								   "StandardRenderer Distortion",
								   layouts_distort,
								   false);
	if (shader_unlit_ == NULL)
		return false;

	shader_ad_unlit_ = Shader::Create(graphicsDevice_,
									  fixedShader_.AdvancedSpriteUnlit_VS.data(),
									  fixedShader_.AdvancedSpriteUnlit_VS.size(),
									  fixedShader_.AdvancedSpriteUnlit_PS.data(),
									  fixedShader_.AdvancedSpriteUnlit_PS.size(),
									  "StandardRenderer",
									  layouts_ad,
									  false);
	if (shader_ad_unlit_ == NULL)
		return false;

	shader_ad_distortion_ = Shader::Create(graphicsDevice_,
										   fixedShader_.AdvancedSpriteDistortion_VS.data(),
										   fixedShader_.AdvancedSpriteDistortion_VS.size(),
										   fixedShader_.AdvancedSpriteDistortion_PS.data(),
										   fixedShader_.AdvancedSpriteDistortion_PS.size(),
										   "StandardRenderer Distortion",
										   layouts_distort_ad,
										   false);
	if (shader_ad_distortion_ == NULL)
		return false;

	std::vector<VertexLayout> layouts_lighting;
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 2});
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 3});
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});
	layouts_lighting.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 5});

	std::vector<VertexLayout> layouts_lighting_ad;
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 2});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 3});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 5});
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 6}); // AlphaTextureUV + UVDistortionTextureUV
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 7});		  // BlendUV
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 8}); // BlendAlphaUV + BlendUVDistortionUV
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 9});		  // FlipbookIndexAndNextRate
	layouts_lighting_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 10});		  // AlphaThreshold

	shader_lit_ = Shader::Create(graphicsDevice_,
								 fixedShader_.SpriteLit_VS.data(),
								 fixedShader_.SpriteLit_VS.size(),
								 fixedShader_.SpriteLit_PS.data(),
								 fixedShader_.SpriteLit_PS.size(),
								 "StandardRenderer Lighting",
								 layouts_lighting,
								 false);

	shader_ad_lit_ = Shader::Create(graphicsDevice_,
									fixedShader_.AdvancedSpriteLit_VS.data(),
									fixedShader_.AdvancedSpriteLit_VS.size(),
									fixedShader_.AdvancedSpriteLit_PS.data(),
									fixedShader_.AdvancedSpriteLit_PS.size(),
									"StandardRenderer Lighting",
									layouts_lighting_ad,
									false);

	shader_lit_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_lit_->SetPixelConstantBufferSize(sizeof(float) * 4 * 9);

	shader_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_->SetPixelConstantBufferSize(sizeof(float) * 4 * 6);

	shader_unlit_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_unlit_->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);

	shader_ad_lit_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_ad_lit_->SetPixelConstantBufferSize(sizeof(float) * 4 * 9);

	shader_ad_unlit_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_ad_unlit_->SetPixelConstantBufferSize(sizeof(float) * 4 * 6);

	shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);

	m_standardRenderer =
		new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GetImpl()->CreateProxyTextures(this);

	return true;
}

void RendererImplemented::Destroy()
{
	Release();
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
}

void RendererImplemented::SetRenderPassPipelineState(LLGI::RenderPassPipelineState* renderPassPipelineState)
{
	ES_SAFE_RELEASE(renderPassPipelineState_);
	renderPassPipelineState_ = renderPassPipelineState;
	LLGI::SafeAddRef(renderPassPipelineState_);
}

bool RendererImplemented::BeginRendering()
{
	assert(graphicsDevice_ != NULL);

	impl->CalculateCameraProjectionMatrix();

	// initialize states
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	if (commandList_ == nullptr)
	{
		GetCurrentCommandList()->Begin();
		// GetCurrentCommandList()->BeginRenderPass(nullptr);
	}

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

bool RendererImplemented::EndRendering()
{
	assert(graphicsDevice_ != NULL);

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	if (commandList_ == nullptr)
	{
		// GetCurrentCommandList()->EndRenderPass();
		GetCurrentCommandList()->End();
		GetGraphics()->Execute(GetCurrentCommandList());
	}
	return true;
}

void RendererImplemented::SetCommandList(EffekseerRenderer::CommandList* commandList)
{

	ES_SAFE_ADDREF(commandList);
	ES_SAFE_RELEASE(commandList_);

	commandList_ = static_cast<CommandList*>(commandList);
}

VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	if (m_renderMode == ::Effekseer::RenderMode::Wireframe)
	{
		return m_indexBufferForWireframe;
	}
	else
	{
		return m_indexBuffer;
	}
}

int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this);
}

::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this);
}

::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this);
}

::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this, &fixedShader_);
}

::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this);
}

::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new EffekseerRenderer::TextureLoader(graphicsDevice_, fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(graphicsDevice_, fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
	return new MaterialLoader(graphicsDevice_, fileInterface, platformType_, materialCompiler_);
}

Effekseer::TextureData* RendererImplemented::GetBackground()
{
	if (m_background.UserPtr == nullptr)
		return nullptr;
	return &m_background;
}

void RendererImplemented::SetBackground(LLGI::Texture* background)
{
	ES_SAFE_ADDREF(background);

	auto p = (LLGI::Texture*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);
	m_background.UserPtr = background;
}

void RendererImplemented::SetBackgroundTexture(Effekseer::TextureData* textuerData)
{
	if (textuerData == nullptr)
	{
		auto back = (LLGI::Texture*)m_background.UserPtr;
		ES_SAFE_RELEASE(back);
		m_background.UserPtr = nullptr;
		return;
	}

	auto texture = static_cast<LLGI::Texture*>(textuerData->UserPtr);
	ES_SAFE_ADDREF(texture);

	auto back = (LLGI::Texture*)m_background.UserPtr;
	ES_SAFE_RELEASE(back);

	m_background.UserPtr = texture;
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return m_distortingCallback;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(m_distortingCallback);
	m_distortingCallback = callback;
}

void RendererImplemented::SetVertexBuffer(LLGI::VertexBuffer* vertexBuffer, int32_t stride)
{
	currentVertexBuffer_ = vertexBuffer;
	currentVertexBufferStride_ = stride;
}

void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t stride)
{
	currentVertexBuffer_ = vertexBuffer->GetVertexBuffer();
	currentVertexBufferStride_ = stride;
}

void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	GetCurrentCommandList()->SetIndexBuffer(indexBuffer->GetIndexBuffer());
}

void RendererImplemented::SetIndexBuffer(LLGI::IndexBuffer* indexBuffer)
{
	GetCurrentCommandList()->SetIndexBuffer(indexBuffer);
}

void RendererImplemented::SetVertexBuffer(Effekseer::Backend::VertexBuffer* vertexBuffer, int32_t stride)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer);

	SetVertexBuffer(vb->GetBuffer(), stride);
}

void RendererImplemented::SetIndexBuffer(Effekseer::Backend::IndexBuffer* indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer);

	SetIndexBuffer(ib->GetBuffer());
}

void RendererImplemented::SetLayout(Shader* shader)
{
	if (m_renderMode == Effekseer::RenderMode::Normal)
	{
		currentTopologyType_ = LLGI::TopologyType::Triangle;
	}
	else
	{
		currentTopologyType_ = LLGI::TopologyType::Line;
	}
}

void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	// constant buffer
	LLGI::ConstantBuffer* constantBufferVS = nullptr;
	LLGI::ConstantBuffer* constantBufferPS = nullptr;

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = commandList_->GetMemoryPooll()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, LLGI::ShaderStageType::Vertex);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = commandList_->GetMemoryPooll()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
		assert(constantBufferPS != nullptr);
		memcpy(constantBufferPS->Lock(), currentShader->GetPixelConstantBuffer(), currentShader->GetPixelConstantBufferSize());
		constantBufferPS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferPS, LLGI::ShaderStageType::Pixel);
	}

	auto piplineState = GetOrCreatePiplineState();
	GetCurrentCommandList()->SetPipelineState(piplineState);

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	if (m_renderMode == Effekseer::RenderMode::Normal)
	{
		GetCurrentCommandList()->SetVertexBuffer(
			currentVertexBuffer_, currentVertexBufferStride_, vertexOffset * currentVertexBufferStride_);
		GetCurrentCommandList()->Draw(spriteCount * 2);
	}
	else
	{
		GetCurrentCommandList()->SetVertexBuffer(
			currentVertexBuffer_, currentVertexBufferStride_, vertexOffset * currentVertexBufferStride_);
		GetCurrentCommandList()->Draw(spriteCount * 4);
	}

	LLGI::SafeRelease(constantBufferVS);
	LLGI::SafeRelease(constantBufferPS);
}

void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	DrawPolygonInstanced(vertexCount, indexCount, 1);
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	// constant buffer
	LLGI::ConstantBuffer* constantBufferVS = nullptr;
	LLGI::ConstantBuffer* constantBufferPS = nullptr;

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = commandList_->GetMemoryPooll()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, LLGI::ShaderStageType::Vertex);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = commandList_->GetMemoryPooll()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
		assert(constantBufferPS != nullptr);
		memcpy(constantBufferPS->Lock(), currentShader->GetPixelConstantBuffer(), currentShader->GetPixelConstantBufferSize());
		constantBufferPS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferPS, LLGI::ShaderStageType::Pixel);
	}

	auto piplineState = GetOrCreatePiplineState();
	GetCurrentCommandList()->SetPipelineState(piplineState);

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

	GetCurrentCommandList()->SetVertexBuffer(currentVertexBuffer_, currentVertexBufferStride_, 0);
	GetCurrentCommandList()->Draw(indexCount / 3, instanceCount);

	LLGI::SafeRelease(constantBufferVS);
	LLGI::SafeRelease(constantBufferPS);
}

Shader* RendererImplemented::GetShader(::EffekseerRenderer::RendererShaderType type) const
{
	if (type == ::EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		return shader_ad_distortion_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedLit)
	{
		return shader_ad_lit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedUnlit)
	{
		return shader_ad_unlit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::BackDistortion)
	{
		return shader_unlit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Lit)
	{
		return shader_lit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Unlit)
	{
		return shader_;
	}

	return nullptr;
}

void RendererImplemented::BeginShader(Shader* shader)
{
	currentShader = shader;
}

void RendererImplemented::EndShader(Shader* shader)
{
	currentShader = nullptr;
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	auto state = GetRenderState()->GetActiveState();
	LLGI::TextureWrapMode ws[2];
	ws[(int)Effekseer::TextureWrapType::Clamp] = LLGI::TextureWrapMode::Clamp;
	ws[(int)Effekseer::TextureWrapType::Repeat] = LLGI::TextureWrapMode::Repeat;

	LLGI::TextureMinMagFilter fs[2];
	fs[(int)Effekseer::TextureFilterType::Linear] = LLGI::TextureMinMagFilter::Linear;
	fs[(int)Effekseer::TextureFilterType::Nearest] = LLGI::TextureMinMagFilter::Nearest;

	for (int32_t i = 0; i < count; i++)
	{
		if (textures[i] == nullptr)
		{
			GetCurrentCommandList()->SetTexture(
				nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Vertex);
			GetCurrentCommandList()->SetTexture(
				nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Pixel);
		}
		else
		{
			if (textures[i]->TexturePtr != nullptr)
			{
				auto texture = static_cast<Backend::Texture*>(textures[i]->TexturePtr);
				auto t = texture->GetTexture().get();
				GetCurrentCommandList()->SetTexture(
					t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Vertex);
				GetCurrentCommandList()->SetTexture(
					t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Pixel);
			}
			else
			{
				auto t = (LLGI::Texture*)(textures[i]->UserPtr);
				GetCurrentCommandList()->SetTexture(
					t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Vertex);
				GetCurrentCommandList()->SetTexture(
					t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Pixel);	
			}
		}
	}
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

Effekseer::TextureData* RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	std::array<uint8_t, 4> buf;

	if (type == EffekseerRenderer::ProxyTextureType::White)
	{
		buf.fill(255);
	}
	else if (type == EffekseerRenderer::ProxyTextureType::Normal)
	{
		buf.fill(127);
		buf[2] = 255;
		buf[3] = 255;
	}
	else
	{
		assert(0);
	}

	LLGI::TextureInitializationParameter texParam;
	texParam.Size = LLGI::Vec2I(16, 16);
	auto texture = GetGraphics()->CreateTexture(texParam);
	auto texbuf = reinterpret_cast<uint8_t*>(texture->Lock());

	for (int32_t i = 0; i < 16 * 16; i++)
	{
		memcpy(texbuf + i * 4, buf.data(), 4);
	}

	texture->Unlock();

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 16;
	textureData->Height = 16;
	return textureData;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
	if (data != nullptr && data->UserPtr != nullptr)
	{
		auto texture = (LLGI::Texture*)data->UserPtr;
		texture->Release();
	}

	if (data != nullptr)
	{
		delete data;
	}
}

} // namespace EffekseerRendererLLGI
