

#include "EffekseerRendererLLGI.Renderer.h"
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.MaterialLoader.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.GpuParticles.h"
#include "EffekseerRendererLLGI.GpuTimer.h"

#include <EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRendererCommon/ModelLoader.h>
#include <EffekseerRendererCommon/TextureLoader.h>

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
	key.state = renderState_->GetActiveState();
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

	for (size_t i = 0; i < currentShader->GetVertexLayouts()->GetElements().size(); i++)
	{
		piplineState->VertexLayouts[i] = currentShader->GetVertexLayouts()->GetElements()[i].Format;
		piplineState->VertexLayoutNames[i] = currentShader->GetVertexLayouts()->GetElements()[i].Name;
		piplineState->VertexLayoutSemantics[i] = currentShader->GetVertexLayouts()->GetElements()[i].Semantic;
	}
	piplineState->VertexLayoutCount = static_cast<int32_t>(currentShader->GetVertexLayouts()->GetElements().size());

	piplineState->Topology = currentTopologyType_;

	piplineState->IsDepthTestEnabled = key.state.DepthTest;
	piplineState->IsDepthWriteEnabled = key.state.DepthWrite;

	if (isReversedDepth_)
	{
		if (key.state.CullingType == ::Effekseer::CullingType::Back)
		{
			piplineState->Culling = LLGI::CullingMode::Clockwise;
		}
		else if (key.state.CullingType == ::Effekseer::CullingType::Front)
		{
			piplineState->Culling = LLGI::CullingMode::CounterClockwise;
		}
		else if (key.state.CullingType == ::Effekseer::CullingType::Double)
		{
			piplineState->Culling = LLGI::CullingMode::DoubleSide;
		}
	}
	else
	{
		piplineState->Culling = (LLGI::CullingMode)key.state.CullingType;
	}

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
		if (GetImpl()->IsPremultipliedAlphaEnabled)
		{
			piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
			piplineState->BlendEquationAlpha = LLGI::BlendEquationType::Add;
			piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
			piplineState->BlendDstFunc = LLGI::BlendFuncType::OneMinusSrcAlpha;
			piplineState->BlendSrcFuncAlpha = LLGI::BlendFuncType::One;
			piplineState->BlendDstFuncAlpha = LLGI::BlendFuncType::OneMinusSrcAlpha;
		}
		else
		{
			piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
			piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
			piplineState->BlendDstFunc = LLGI::BlendFuncType::OneMinusSrcAlpha;
		}
	}

	if (key.state.AlphaBlend == Effekseer::AlphaBlendType::Add)
	{
		if (GetImpl()->IsPremultipliedAlphaEnabled)
		{
			piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
			piplineState->BlendEquationAlpha = LLGI::BlendEquationType::Add;
			piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
			piplineState->BlendDstFunc = LLGI::BlendFuncType::One;
			piplineState->BlendSrcFuncAlpha = LLGI::BlendFuncType::Zero;
			piplineState->BlendDstFuncAlpha = LLGI::BlendFuncType::One;
		}
		else
		{
			piplineState->BlendEquationRGB = LLGI::BlendEquationType::Add;
			piplineState->BlendSrcFunc = LLGI::BlendFuncType::SrcAlpha;
			piplineState->BlendDstFunc = LLGI::BlendFuncType::One;
		}
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

RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: graphicsDevice_(nullptr)
	, squareMaxCount_(squareMaxCount)
	, coordinateSystem_(::Effekseer::CoordinateSystem::RH)
	, renderState_(nullptr)
	, standardRenderer_(nullptr)
	, distortingCallback_(nullptr)
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

	ES_SAFE_DELETE(distortingCallback_);
	ES_SAFE_DELETE(standardRenderer_);
	ES_SAFE_DELETE(renderState_);

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

	LLGI::SetLogger([](LLGI::LogType type, const std::string& message)
					{ std::cout << message << std::endl; });

	// Generate vertex buffer
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * squareMaxCount_ * 4, 3);
		if (!GetImpl()->InternalVertexBuffer->GetIsValid())
		{
			GetImpl()->InternalVertexBuffer = nullptr;
			return false;
		}
	}

	if (!EffekseerRenderer::GenerateIndexDataStride<int16_t>(graphicsDevice_, squareMaxCount_, indexBuffer_, indexBufferForWireframe_))
	{
		return false;
	}

	renderState_ = new RenderState(this);

	auto vlUnlit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Unlit).DownCast<Backend::VertexLayout>();
	auto vlLit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Lit).DownCast<Backend::VertexLayout>();
	auto vlDist = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::BackDistortion).DownCast<Backend::VertexLayout>();
	auto vlUnlitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedUnlit).DownCast<Backend::VertexLayout>();
	auto vlLitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedLit).DownCast<Backend::VertexLayout>();
	auto vlDistAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedBackDistortion).DownCast<Backend::VertexLayout>();

	vlUnlit->MakeGenerated();
	vlLit->MakeGenerated();
	vlDist->MakeGenerated();
	vlUnlitAd->MakeGenerated();
	vlLitAd->MakeGenerated();
	vlDistAd->MakeGenerated();

	auto unlit_vs_shader_data = Backend::Serialize(fixedShader_.SpriteUnlit_VS);
	auto unlit_ps_shader_data = Backend::Serialize(fixedShader_.ModelUnlit_PS);

	auto shader_unlit = Shader::Create(graphicsDevice_,
									   graphicsDevice_->CreateShaderFromBinary(
										   unlit_vs_shader_data.data(),
										   static_cast<int32_t>(unlit_vs_shader_data.size()),
										   unlit_ps_shader_data.data(),
										   static_cast<int32_t>(unlit_ps_shader_data.size())),
									   vlUnlit,
									   "StandardRenderer");
	if (shader_unlit == nullptr)
		return false;
	GetImpl()->ShaderUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_unlit);

	auto distortion_vs_shader_data = Backend::Serialize(fixedShader_.SpriteDistortion_VS);
	auto distortion_ps_shader_data = Backend::Serialize(fixedShader_.ModelDistortion_PS);

	auto shader_distortion = Shader::Create(graphicsDevice_,
											graphicsDevice_->CreateShaderFromBinary(
												distortion_vs_shader_data.data(),
												static_cast<int32_t>(distortion_vs_shader_data.size()),
												distortion_ps_shader_data.data(),
												static_cast<int32_t>(distortion_ps_shader_data.size())),
											vlDist,
											"StandardRenderer Distortion");
	if (shader_distortion == nullptr)
		return false;
	GetImpl()->ShaderDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_distortion);

	auto ad_unlit_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteUnlit_VS);
	auto ad_unlit_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelUnlit_PS);

	auto shader_ad_unlit = Shader::Create(graphicsDevice_,
										  graphicsDevice_->CreateShaderFromBinary(
											  ad_unlit_vs_shader_data.data(),
											  static_cast<int32_t>(ad_unlit_vs_shader_data.size()),
											  ad_unlit_ps_shader_data.data(),
											  static_cast<int32_t>(ad_unlit_ps_shader_data.size())),
										  vlUnlitAd,
										  "StandardRenderer");
	if (shader_ad_unlit == nullptr)
		return false;
	GetImpl()->ShaderAdUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_unlit);

	auto ad_dist_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteDistortion_VS);
	auto ad_dist_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelDistortion_PS);

	auto shader_ad_distortion = Shader::Create(graphicsDevice_,
											   graphicsDevice_->CreateShaderFromBinary(
												   ad_dist_vs_shader_data.data(),
												   static_cast<int32_t>(ad_dist_vs_shader_data.size()),
												   ad_dist_ps_shader_data.data(),
												   static_cast<int32_t>(ad_dist_ps_shader_data.size())),
											   vlDistAd,
											   "StandardRenderer Distortion");
	if (shader_ad_distortion == nullptr)
		return false;
	GetImpl()->ShaderAdDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_distortion);

	auto lit_vs_shader_data = Backend::Serialize(fixedShader_.SpriteLit_VS);
	auto lit_ps_shader_data = Backend::Serialize(fixedShader_.ModelLit_PS);

	auto shader_lit = Shader::Create(graphicsDevice_,
									 graphicsDevice_->CreateShaderFromBinary(
										 lit_vs_shader_data.data(),
										 static_cast<int32_t>(lit_vs_shader_data.size()),
										 lit_ps_shader_data.data(),
										 static_cast<int32_t>(lit_ps_shader_data.size())),
									 vlLit,
									 "StandardRenderer Lighting");
	GetImpl()->ShaderLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_lit);

	auto ad_lit_vs_shader_data = Backend::Serialize(fixedShader_.AdvancedSpriteLit_VS);
	auto ad_lit_ps_shader_data = Backend::Serialize(fixedShader_.AdvancedModelLit_PS);

	auto shader_ad_lit = Shader::Create(graphicsDevice_,
										graphicsDevice_->CreateShaderFromBinary(
											ad_lit_vs_shader_data.data(),
											static_cast<int32_t>(ad_lit_vs_shader_data.size()),
											ad_lit_ps_shader_data.data(),
											static_cast<int32_t>(ad_lit_ps_shader_data.size())),
										vlLitAd,
										"StandardRenderer Lighting");
	GetImpl()->ShaderAdLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_lit);

	shader_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_ad_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	standardRenderer_ = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

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
	auto gd = graphicsDevice_.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	auto it = renderpassPipelineStates_.find(key);
	if (it != renderpassPipelineStates_.end())
	{
		currentRenderPassPipelineState_ = it->second;
		gd->SetRenderPassPipelineState(currentRenderPassPipelineState_.get());
	}
	else
	{
		auto pipelineState = LLGI::CreateSharedPtr(gd->GetGraphics()->CreateRenderPassPipelineState(key));
		if (pipelineState != nullptr)
		{
			renderpassPipelineStates_[key] = pipelineState;
		}
		currentRenderPassPipelineState_ = pipelineState;
		gd->SetRenderPassPipelineState(currentRenderPassPipelineState_.get());
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
	renderState_->GetActiveState().Reset();
	renderState_->Update(true);

	// reset renderer
	standardRenderer_->ResetAndRenderingIfRequired();

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
	standardRenderer_->ResetAndRenderingIfRequired();

	currentndexBuffer_ = nullptr;

	return true;
}

void RendererImplemented::SetCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	commandList_ = commandList;

	auto device = GetGraphicsDevice().DownCast<Backend::GraphicsDevice>();
	auto cl = commandList_.DownCast<CommandList>();
	device->SetCommandList((cl) ? cl->GetInternal() : nullptr);
}

Effekseer::Backend::IndexBufferRef RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return indexBufferForWireframe_;
	}
	return indexBuffer_;
}

int32_t RendererImplemented::GetSquareMaxCount() const
{
	return squareMaxCount_;
}

::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return renderState_;
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

::Effekseer::GpuTimerRef RendererImplemented::CreateGpuTimer()
{
	return ::Effekseer::GpuTimerRef(new ::EffekseerRendererLLGI::GpuTimer(this, false));
}

::Effekseer::GpuParticleSystemRef RendererImplemented::CreateGpuParticleSystem(const Effekseer::GpuParticleSystem::Settings& settings)
{
	auto gpuParticleSystem = ::Effekseer::GpuParticleSystemRef(new ::EffekseerRendererLLGI::GpuParticleSystem(this));
	if (!gpuParticleSystem->InitSystem(settings))
	{
		return nullptr;
	}
	return gpuParticleSystem;
}

::Effekseer::GpuParticleFactoryRef RendererImplemented::CreateGpuParticleFactory()
{
	return ::Effekseer::GpuParticleFactoryRef(new ::EffekseerRenderer::GpuParticleFactory(GetGraphicsDevice()));
}

::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::EffekseerRenderer::CreateTextureLoader(graphicsDevice_, fileInterface, ::Effekseer::ColorSpaceType::Gamma);
}

::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
}

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice_, fileInterface, platformType_, materialCompiler_);
}

void RendererImplemented::SetBackgroundInternal(LLGI::Texture* background)
{
	if (backgroundLLGI_ == nullptr)
	{
		backgroundLLGI_ = graphicsDevice_->CreateTexture(background);
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(backgroundLLGI_.Get());
		texture->Init(background);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? backgroundLLGI_ : nullptr);
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return distortingCallback_;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(distortingCallback_);
	distortingCallback_ = callback;
}

void RendererImplemented::SetVertexBuffer(LLGI::Buffer* vertexBuffer, int32_t stride)
{
	currentVertexBuffer_ = vertexBuffer;
	currentVertexBufferStride_ = stride;
}

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());

	SetVertexBuffer(vb->GetBuffer(), stride);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
	currentndexBuffer_ = indexBuffer;
	GetCurrentCommandList()->SetIndexBuffer(ib->GetBuffer(), ib->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride2 ? 2 : 4);
}

void RendererImplemented::SetLayout(Shader* shader)
{
	if (renderMode_ == Effekseer::RenderMode::Normal)
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
	LLGI::Buffer* constantBufferVS = nullptr;
	LLGI::Buffer* constantBufferPS = nullptr;

	auto cl = commandList_.DownCast<CommandList>();

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, 0);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
		assert(constantBufferPS != nullptr);
		memcpy(constantBufferPS->Lock(), currentShader->GetPixelConstantBuffer(), currentShader->GetPixelConstantBufferSize());
		constantBufferPS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferPS, 1);
	}

	auto piplineState = GetOrCreatePiplineState();
	GetCurrentCommandList()->SetPipelineState(piplineState);

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	if (renderMode_ == Effekseer::RenderMode::Normal)
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
	LLGI::Buffer* constantBufferVS = nullptr;
	LLGI::Buffer* constantBufferPS = nullptr;

	auto cl = commandList_.DownCast<CommandList>();

	if (currentShader->GetVertexConstantBufferSize() > 0)
	{
		constantBufferVS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetVertexConstantBufferSize());
		assert(constantBufferVS != nullptr);
		memcpy(constantBufferVS->Lock(), currentShader->GetVertexConstantBuffer(), currentShader->GetVertexConstantBufferSize());
		constantBufferVS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferVS, 0);
	}

	if (currentShader->GetPixelConstantBufferSize() > 0)
	{
		constantBufferPS = cl->GetMemoryPool()->CreateConstantBuffer(currentShader->GetPixelConstantBufferSize());
		assert(constantBufferPS != nullptr);
		memcpy(constantBufferPS->Lock(), currentShader->GetPixelConstantBuffer(), currentShader->GetPixelConstantBufferSize());
		constantBufferPS->Unlock();
		GetCurrentCommandList()->SetConstantBuffer(constantBufferPS, 1);
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
	std::array<LLGI::TextureWrapMode, 3> ws;
	ws[(int)Effekseer::TextureWrapType::Clamp] = LLGI::TextureWrapMode::Clamp;
	ws[(int)Effekseer::TextureWrapType::Repeat] = LLGI::TextureWrapMode::Repeat;
	ws[(int)Effekseer::TextureWrapType::Mirror] = LLGI::TextureWrapMode::Mirror;

	LLGI::TextureMinMagFilter fs[2];
	fs[(int)Effekseer::TextureFilterType::Linear] = LLGI::TextureMinMagFilter::Linear;
	fs[(int)Effekseer::TextureFilterType::Nearest] = LLGI::TextureMinMagFilter::Nearest;

	for (int32_t i = 0; i < count; i++)
	{
		if (textures[i] == nullptr)
		{
			GetCurrentCommandList()->SetTexture(nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
			GetCurrentCommandList()->SetTexture(nullptr, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
		}
		else
		{
			auto texture = static_cast<Backend::Texture*>(textures[i].Get());
			auto t = texture->GetTexture().get();
			GetCurrentCommandList()->SetTexture(t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
			GetCurrentCommandList()->SetTexture(t, ws[(int)state.TextureWrapTypes[i]], fs[(int)state.TextureFilterTypes[i]], i);
		}
	}
}

void RendererImplemented::ResetRenderState()
{
	renderState_->GetActiveState().Reset();
	renderState_->Update(true);
}

void RendererImplemented::ResetQuery(LLGI::Query* query)
{
	GetCurrentCommandList()->ResetQuery(query);
}

void RendererImplemented::BeginQuery(LLGI::Query* query, uint32_t queryIndex)
{
	GetCurrentCommandList()->BeginQuery(query, queryIndex);
}

void RendererImplemented::EndQuery(LLGI::Query* query, uint32_t queryIndex)
{
	GetCurrentCommandList()->EndQuery(query, queryIndex);
}

void RendererImplemented::RecordTimestamp(LLGI::Query* query, uint32_t queryIndex)
{
	GetCurrentCommandList()->RecordTimestamp(query, queryIndex);
}

} // namespace EffekseerRendererLLGI
