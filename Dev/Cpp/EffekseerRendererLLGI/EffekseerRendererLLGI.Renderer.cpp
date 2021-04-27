

#include "EffekseerRendererLLGI.Renderer.h"
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.IndexBuffer.h"
#include "EffekseerRendererLLGI.MaterialLoader.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.VertexBuffer.h"

#include "../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#include "../EffekseerRendererCommon/ModelLoader.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../EffekseerRendererCommon/TextureLoader.h"
#endif

#include <iostream>

namespace EffekseerRendererLLGI
{

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

LLGI::TextureFormatType ConvertTextureFormat(Effekseer::Backend::TextureFormatType format)
{
	switch (format)
	{
	case Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM:
		return LLGI::TextureFormatType::R8G8B8A8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM:
		return LLGI::TextureFormatType::B8G8R8A8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::R8_UNORM:
		return LLGI::TextureFormatType::R8_UNORM;
		break;
	case Effekseer::Backend::TextureFormatType::R16G16_FLOAT:
		return LLGI::TextureFormatType::R16G16_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT:
		return LLGI::TextureFormatType::R16G16B16A16_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT:
		return LLGI::TextureFormatType::R32G32B32A32_FLOAT;
		break;
	case Effekseer::Backend::TextureFormatType::BC1:
		return LLGI::TextureFormatType::BC1;
		break;
	case Effekseer::Backend::TextureFormatType::BC2:
		return LLGI::TextureFormatType::BC2;
		break;
	case Effekseer::Backend::TextureFormatType::BC3:
		return LLGI::TextureFormatType::BC3;
		break;
	case Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB:
		return LLGI::TextureFormatType::R8G8B8A8_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB:
		return LLGI::TextureFormatType::B8G8R8A8_UNORM_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC1_SRGB:
		return LLGI::TextureFormatType::BC1_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC2_SRGB:
		return LLGI::TextureFormatType::BC2_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::BC3_SRGB:
		return LLGI::TextureFormatType::BC3_SRGB;
		break;
	case Effekseer::Backend::TextureFormatType::D32:
		return LLGI::TextureFormatType::D32;
		break;
	case Effekseer::Backend::TextureFormatType::D24S8:
		return LLGI::TextureFormatType::D24S8;
		break;
	case Effekseer::Backend::TextureFormatType::D32S8:
		return LLGI::TextureFormatType::D32S8;
		break;
	case Effekseer::Backend::TextureFormatType::Unknown:
		break;
	default:
		break;
	}

	return LLGI::TextureFormatType::Unknown;
}

LLGI::CommandList* RendererImplemented::GetCurrentCommandList()
{
	if (commandList_ != nullptr)
	{
		auto cl = commandList_.DownCast<CommandList>();
		return cl->GetInternal();
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
	key.renderPassPipelineState = currentRenderPassPipelineState_.get();

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
	piplineState->VertexLayoutCount = (int32_t)currentShader->GetVertexLayouts().size();

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

	piplineState->SetRenderPassPipelineState(currentRenderPassPipelineState_.get());

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
		VertexBuffer::Create(graphicsDevice_.Get(), EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true, false);
}

void RendererImplemented::GenerateIndexBuffer()
{
	m_indexBuffer = IndexBuffer::Create(graphicsDevice_.Get(), m_squareMaxCount * 6, false, false);
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
	, m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_renderState(nullptr)

	, shader_unlit_(nullptr)
	, shader_distortion_(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
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

	commandList_.Reset();
	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(m_distortingCallback);
	ES_SAFE_DELETE(m_standardRenderer);

	ES_SAFE_DELETE(shader_unlit_);
	ES_SAFE_DELETE(shader_lit_);
	ES_SAFE_DELETE(shader_distortion_);

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
}

void RendererImplemented::OnLostDevice()
{
}

void RendererImplemented::OnResetDevice()
{
}

bool RendererImplemented::Initialize(LLGI::Graphics* graphics, LLGI::RenderPassPipelineStateKey key, bool isReversedDepth)
{

	auto gd = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(graphics);

	auto ret = Initialize(gd, key, isReversedDepth);

	return ret;
}

bool RendererImplemented::Initialize(Backend::GraphicsDeviceRef graphicsDevice,
									 LLGI::RenderPassPipelineStateKey key,
									 bool isReversedDepth)
{
	graphicsDevice_ = graphicsDevice;
	ChangeRenderPassPipelineState(key);
	isReversedDepth_ = isReversedDepth;

	LLGI::SetLogger([](LLGI::LogType type, const std::string& message) { std::cout << message << std::endl; });

	// Generate vertex buffer
	{
		GenerateVertexBuffer();
		if (m_vertexBuffer == nullptr)
			return false;
	}

	// Generate index buffer
	{
		GenerateIndexBuffer();
		if (m_indexBuffer == nullptr)
			return false;
	}

	// Generate index buffer for rendering wireframes
	{
		m_indexBufferForWireframe = IndexBuffer::Create(graphicsDevice_.Get(), m_squareMaxCount * 8, false, false);
		if (m_indexBufferForWireframe == nullptr)
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

	std::vector<VertexLayout> layouts_ad;
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 2});
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 3}); // AlphaTextureUV + UVDistortionTextureUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});		 // BlendUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 5}); // BlendAlphaUV + BlendUVDistortionUV
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 6});			 // FlipbookIndexAndNextRate
	layouts_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 7});			 // AlphaThreshold

	std::vector<VertexLayout> layouts_normal;
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 2});
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 3});
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});
	layouts_normal.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 5});

	std::vector<VertexLayout> layouts_normal_ad;
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 1});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 2});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 3});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 5});
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 6}); // AlphaTextureUV + UVDistortionTextureUV
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 7});		// BlendUV
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT, "TEXCOORD", 8}); // BlendAlphaUV + BlendUVDistortionUV
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 9});			// FlipbookIndexAndNextRate
	layouts_normal_ad.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32_FLOAT, "TEXCOORD", 10});			// AlphaThreshold

	shader_unlit_ = Shader::Create(graphicsDevice_.Get(),
								   fixedShader_.SpriteUnlit_VS.data(),
								   (int32_t)fixedShader_.SpriteUnlit_VS.size(),
								   fixedShader_.ModelUnlit_PS.data(),
								   (int32_t)fixedShader_.ModelUnlit_PS.size(),
								   "StandardRenderer",
								   layouts,
								   false);
	if (shader_unlit_ == nullptr)
		return false;

	shader_distortion_ = Shader::Create(graphicsDevice_.Get(),
										fixedShader_.SpriteDistortion_VS.data(),
										(int32_t)fixedShader_.SpriteDistortion_VS.size(),
										fixedShader_.ModelDistortion_PS.data(),
										(int32_t)fixedShader_.ModelDistortion_PS.size(),
										"StandardRenderer Distortion",
										layouts_normal,
										false);
	if (shader_distortion_ == nullptr)
		return false;

	shader_ad_unlit_ = Shader::Create(graphicsDevice_.Get(),
									  fixedShader_.AdvancedSpriteUnlit_VS.data(),
									  (int32_t)fixedShader_.AdvancedSpriteUnlit_VS.size(),
									  fixedShader_.AdvancedModelUnlit_PS.data(),
									  (int32_t)fixedShader_.AdvancedModelUnlit_PS.size(),
									  "StandardRenderer",
									  layouts_ad,
									  false);
	if (shader_ad_unlit_ == nullptr)
		return false;

	shader_ad_distortion_ = Shader::Create(graphicsDevice_.Get(),
										   fixedShader_.AdvancedSpriteDistortion_VS.data(),
										   (int32_t)fixedShader_.AdvancedSpriteDistortion_VS.size(),
										   fixedShader_.AdvancedModelDistortion_PS.data(),
										   (int32_t)fixedShader_.AdvancedModelDistortion_PS.size(),
										   "StandardRenderer Distortion",
										   layouts_normal_ad,
										   false);
	if (shader_ad_distortion_ == nullptr)
		return false;

	shader_lit_ = Shader::Create(graphicsDevice_.Get(),
								 fixedShader_.SpriteLit_VS.data(),
								 (int32_t)fixedShader_.SpriteLit_VS.size(),
								 fixedShader_.ModelLit_PS.data(),
								 (int32_t)fixedShader_.ModelLit_PS.size(),
								 "StandardRenderer Lighting",
								 layouts_normal,
								 false);

	shader_ad_lit_ = Shader::Create(graphicsDevice_.Get(),
									fixedShader_.AdvancedSpriteLit_VS.data(),
									(int32_t)fixedShader_.AdvancedSpriteLit_VS.size(),
									fixedShader_.AdvancedModelLit_PS.data(),
									(int32_t)fixedShader_.AdvancedModelLit_PS.size(),
									"StandardRenderer Lighting",
									layouts_normal_ad,
									false);

	shader_unlit_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_unlit_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_distortion_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_distortion_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_ad_unlit_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_unlit_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_lit_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_lit_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_lit_->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_lit_->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GetImpl()->CreateProxyTextures(this);
	GetImpl()->isSoftParticleEnabled = true;

	GetImpl()->isDepthReversed = isReversedDepth;

	return true;
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
}

void RendererImplemented::ChangeRenderPassPipelineState(LLGI::RenderPassPipelineStateKey key)
{
	auto it = renderpassPipelineStates_.find(key);
	if (it != renderpassPipelineStates_.end())
	{
		currentRenderPassPipelineState_ = it->second;
	}
	else
	{
		auto gd = graphicsDevice_.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
		auto pipelineState = LLGI::CreateSharedPtr(gd->GetGraphics()->CreateRenderPassPipelineState(key));
		if (pipelineState != nullptr)
		{
			renderpassPipelineStates_[key] = pipelineState;
		}
		currentRenderPassPipelineState_ = pipelineState;
	}
}

bool RendererImplemented::BeginRendering()
{
	assert(graphicsDevice_ != nullptr);

	if (commandList_ == nullptr)
	{
		return false;
	}

	impl->CalculateCameraProjectionMatrix();

	// initialize states
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

bool RendererImplemented::EndRendering()
{
	assert(graphicsDevice_ != nullptr);

	if (commandList_ == nullptr)
	{
		return false;
	}

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

void RendererImplemented::SetCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	commandList_ = commandList;
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

::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this));
}

::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this));
}

::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this));
}

::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this, &fixedShader_);
}

::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this));
}

::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::TextureLoader>(graphicsDevice_.Get(), fileInterface);
#else
	return nullptr;
#endif
}

::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
}

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice_.Get(), fileInterface, platformType_, materialCompiler_);
}

void RendererImplemented::SetBackgroundInternal(LLGI::Texture* background)
{
	if (m_backgroundLLGI == nullptr)
	{
		m_backgroundLLGI = graphicsDevice_->CreateTexture(background);
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(m_backgroundLLGI.Get());
		texture->Init(background);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? m_backgroundLLGI : nullptr);
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

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());

	SetVertexBuffer(vb->GetBuffer(), stride);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());

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

	auto cl = commandList_.DownCast<CommandList>();

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, LLGI::ShaderStageType::Vertex);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
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

	auto cl = commandList_.DownCast<CommandList>();

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, LLGI::ShaderStageType::Vertex);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
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
		return shader_distortion_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Lit)
	{
		return shader_lit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Unlit)
	{
		return shader_unlit_;
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

void RendererImplemented::SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count)
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
			auto texture = static_cast<Backend::Texture*>(textures[i].Get());
			auto t = texture->GetTexture().get();
			GetCurrentCommandList()->SetTexture(
				t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Vertex);
			GetCurrentCommandList()->SetTexture(
				t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i, LLGI::ShaderStageType::Pixel);
		}
	}
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

} // namespace EffekseerRendererLLGI
