
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RenderState.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"

namespace EffekseerRendererDX11
{

namespace ShaderLightingVS_
{
static
#include "ShaderHeader/ad_model_lit_vs.h"
}; // namespace ShaderLightingVS_

namespace ShaderLightingPS_
{
static
#include "ShaderHeader/ad_model_lit_ps.h"

} // namespace ShaderLightingPS_

namespace ShaderVS_
{
static
#include "ShaderHeader/ad_model_unlit_vs.h"
} // namespace ShaderVS_

namespace ShaderPS_
{
static
#include "ShaderHeader/ad_model_unlit_ps.h"
} // namespace ShaderPS_

namespace ShaderDistortionVS_
{
static
#include "ShaderHeader/ad_model_distortion_vs.h"
} // namespace ShaderDistortionVS_

namespace ShaderDistortionPS_
{
static
#include "ShaderHeader/ad_model_distortion_ps.h"
} // namespace ShaderDistortionPS_

namespace ShaderLightingVS_15_
{
static
#include "ShaderHeader/model_lit_vs.h"
}; // namespace ShaderLightingVS_15_

namespace ShaderLightingPS_15_
{
static
#include "ShaderHeader/model_lit_ps.h"

} // namespace ShaderLightingPS_15_

namespace ShaderVS_15_
{
static
#include "ShaderHeader/model_unlit_vs.h"
} // namespace ShaderVS_15_

namespace ShaderPS_15_
{
static
#include "ShaderHeader/model_unlit_ps.h"
} // namespace ShaderPS_15_

namespace ShaderDistortionVS_15_
{
static
#include "ShaderHeader/model_distortion_vs.h"
} // namespace ShaderDistortionVS_15_

namespace ShaderDistortionPS_15_
{
static
#include "ShaderHeader/model_distortion_ps.h"
} // namespace ShaderDistortionPS_15_

ModelRenderer::ModelRenderer(const RendererImplementedRef& renderer,
							 Shader* shader_advanced_lit,
							 Shader* shader_advanced_unlit,
							 Shader* shader_advanced_distortion,
							 Shader* shader_lit,
							 Shader* shader_unlit,
							 Shader* shader_distortion)
	: renderer_(renderer)
	, shader_advanced_lit_(shader_advanced_lit)
	, shader_advanced_unlit_(shader_advanced_unlit)
	, shader_advanced_distortion_(shader_advanced_distortion)
	, shader_lit_(shader_lit)
	, shader_unlit_(shader_unlit)
	, shader_distortion_(shader_distortion)
{
	{
		Shader* shaders[2];
		shaders[0] = shader_advanced_lit_;
		shaders[1] = shader_advanced_unlit_;

		for (int32_t i = 0; i < 2; i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<40>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_advanced_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<40>));
		shader_advanced_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	{
		Shader* shaders[2];
		shaders[0] = shader_lit_;
		shaders[1] = shader_unlit_;

		for (int32_t i = 0; i < 2; i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
		shader_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;

	graphicsDevice_ = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(renderer->GetDevice(), nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(shader_advanced_lit_);
	ES_SAFE_DELETE(shader_advanced_unlit_);
	ES_SAFE_DELETE(shader_advanced_distortion_);
	ES_SAFE_DELETE(shader_lit_);
	ES_SAFE_DELETE(shader_unlit_);
	ES_SAFE_DELETE(shader_distortion_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRendererRef ModelRenderer::Create(const RendererImplementedRef& renderer)
{
	assert(renderer != nullptr);
	assert(renderer->GetDevice() != nullptr);

	auto vl = EffekseerRenderer::GetModelRendererVertexLayout(renderer->GetGraphicsDevice()).DownCast<Backend::VertexLayout>();

	Shader* shader_advanced_lit = Shader::Create(renderer->GetGraphicsDevice(),
												 renderer->GetGraphicsDevice()->CreateShaderFromBinary(
													 ShaderLightingVS_::g_main,
													 sizeof(ShaderLightingVS_::g_main),
													 ShaderLightingPS_::g_main,
													 sizeof(ShaderLightingPS_::g_main)),
												 vl,
												 "ModelRendererLightingTextureNormal");

	Shader* shader_advanced_unlit = Shader::Create(renderer->GetGraphicsDevice(),
												   renderer->GetGraphicsDevice()->CreateShaderFromBinary(
													   ShaderVS_::g_main,
													   sizeof(ShaderVS_::g_main),
													   ShaderPS_::g_main,
													   sizeof(ShaderPS_::g_main)),
												   vl,
												   "ModelRendererTexture");

	auto shader_advanced_distortion = Shader::Create(renderer->GetGraphicsDevice(),
													 renderer->GetGraphicsDevice()->CreateShaderFromBinary(
														 ShaderDistortionVS_::g_main,
														 sizeof(ShaderDistortionVS_::g_main),
														 ShaderDistortionPS_::g_main,
														 sizeof(ShaderDistortionPS_::g_main)),
													 vl,
													 "ModelRendererDistortionTexture");

	Shader* shader_lit = Shader::Create(renderer->GetGraphicsDevice(),
										renderer->GetGraphicsDevice()->CreateShaderFromBinary(
											ShaderLightingVS_15_::g_main,
											sizeof(ShaderLightingVS_15_::g_main),
											ShaderLightingPS_15_::g_main,
											sizeof(ShaderLightingPS_15_::g_main)),
										vl,
										"ModelRendererLightingTextureNormal");

	Shader* shader_unlit = Shader::Create(renderer->GetGraphicsDevice(),
										  renderer->GetGraphicsDevice()->CreateShaderFromBinary(
											  ShaderVS_15_::g_main,
											  sizeof(ShaderVS_15_::g_main),
											  ShaderPS_15_::g_main,
											  sizeof(ShaderPS_15_::g_main)),
										  vl,
										  "ModelRendererTexture");

	auto shader_distortion = Shader::Create(renderer->GetGraphicsDevice(),
											renderer->GetGraphicsDevice()->CreateShaderFromBinary(
												ShaderDistortionVS_15_::g_main,
												sizeof(ShaderDistortionVS_15_::g_main),
												ShaderDistortionPS_15_::g_main,
												sizeof(ShaderDistortionPS_15_::g_main)),
											vl,
											"ModelRendererDistortionTexture");

	if (shader_advanced_lit == nullptr || shader_advanced_unlit == nullptr || shader_advanced_distortion == nullptr || shader_lit == nullptr || shader_unlit == nullptr || shader_distortion == nullptr)
	{
		ES_SAFE_DELETE(shader_advanced_lit);
		ES_SAFE_DELETE(shader_advanced_unlit);
		ES_SAFE_DELETE(shader_advanced_distortion);
		ES_SAFE_DELETE(shader_lit);
		ES_SAFE_DELETE(shader_unlit);
		ES_SAFE_DELETE(shader_distortion);
	}

	return ModelRendererRef(new ModelRenderer(renderer, shader_advanced_lit, shader_advanced_unlit, shader_advanced_distortion, shader_lit, shader_unlit, shader_distortion));
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(renderer_.Get(), parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(renderer_.Get(), parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (!parameter.IsExternalMode && parameter.ModelIndex < 0)
	{
		return;
	}

	Effekseer::ModelRef model = nullptr;

	if (parameter.IsExternalMode)
	{
		model = parameter.ExternalModel;
	}
	else if (parameter.IsProceduralMode)
	{
		model = parameter.EffectPointer->GetProceduralModel(parameter.ModelIndex);
	}
	else
	{
		model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	}

	if (model == nullptr)
	{
		return;
	}

	model->StoreBufferToGPU(graphicsDevice_.Get());
	if (!model->GetIsBufferStoredOnGPU())
	{
		return;
	}

	if (renderer_->GetRenderMode() == Effekseer::RenderMode::Wireframe)
	{
		model->GenerateWireIndexBuffer(graphicsDevice_.Get());
		if (!model->GetIsWireIndexBufferGenerated())
		{
			return;
		}
	}

	EndRendering_<
		RendererImplemented,
		Shader,
		Effekseer::Model,
		true,
		40>(
		renderer_.Get(),
		shader_advanced_lit_,
		shader_advanced_unlit_,
		shader_advanced_distortion_,
		shader_lit_,
		shader_unlit_,
		shader_distortion_,
		parameter,
		userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
