
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RenderState.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

#include "EffekseerRendererDX9.ModelRenderer.h"
#include "EffekseerRendererDX9.Shader.h"
#include "GraphicsDevice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

namespace ShaderLightingTextureNormal_VS_Ad
{
static
#include "ShaderHeader/ad_model_lit_vs.h"
} // namespace ShaderLightingTextureNormal_VS_Ad

namespace ShaderLightingTextureNormal_PS_Ad
{
static
#include "ShaderHeader/ad_model_lit_ps.h"

} // namespace ShaderLightingTextureNormal_PS_Ad

namespace ShaderTexture_VS_Ad
{
static
#include "ShaderHeader/ad_model_unlit_vs.h"
} // namespace ShaderTexture_VS_Ad

namespace ShaderTexture_PS_Ad
{
static
#include "ShaderHeader/ad_model_unlit_ps.h"
} // namespace ShaderTexture_PS_Ad

namespace ShaderDistortionTexture_VS_Ad
{
static
#include "ShaderHeader/ad_model_distortion_vs.h"
} // namespace ShaderDistortionTexture_VS_Ad

namespace ShaderDistortionTexture_PS_Ad
{
static
#include "ShaderHeader/ad_model_distortion_ps.h"
} // namespace ShaderDistortionTexture_PS_Ad

namespace ShaderLightingTextureNormal_VS
{
static
#include "ShaderHeader/model_lit_vs.h"
} // namespace ShaderLightingTextureNormal_VS

namespace ShaderLightingTextureNormal_PS
{
static
#include "ShaderHeader/model_lit_ps.h"

} // namespace ShaderLightingTextureNormal_PS

namespace ShaderTexture_VS
{
static
#include "ShaderHeader/model_unlit_vs.h"
} // namespace ShaderTexture_VS

namespace ShaderTexture_PS
{
static
#include "ShaderHeader/model_unlit_ps.h"
} // namespace ShaderTexture_PS

namespace ShaderDistortionTexture_VS
{
static
#include "ShaderHeader/model_distortion_vs.h"
} // namespace ShaderDistortionTexture_VS

namespace ShaderDistortionTexture_PS
{
static
#include "ShaderHeader/model_distortion_ps.h"
} // namespace ShaderDistortionTexture_PS

const int32_t ModelRendererInstanceCount = 10;

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
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<ModelRendererInstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_advanced_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<ModelRendererInstanceCount>));
		shader_advanced_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	{
		Shader* shaders[2];
		shaders[0] = shader_lit_;
		shaders[1] = shader_unlit_;

		for (int32_t i = 0; i < 2; i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>));
		shader_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;

	graphicsDevice_ = renderer->GetGraphicsDevice().DownCast<Backend::GraphicsDevice>();
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
	auto graphicsDevice = renderer->GetGraphicsDevice();

	auto vl = EffekseerRenderer::GetModelRendererVertexLayout(graphicsDevice).DownCast<Backend::VertexLayout>();
	vl->MakeGenerated();

	Shader* shader_ad_lit = Shader::Create(graphicsDevice,
										   graphicsDevice->CreateShaderFromBinary(

											   ShaderLightingTextureNormal_VS_Ad::g_vs30_main,
											   sizeof(ShaderLightingTextureNormal_VS_Ad::g_vs30_main),
											   ShaderLightingTextureNormal_PS_Ad::g_ps30_main,
											   sizeof(ShaderLightingTextureNormal_PS_Ad::g_ps30_main)),
										   vl);

	Shader* shader_ad_unlit = Shader::Create(graphicsDevice,
											 graphicsDevice->CreateShaderFromBinary(

												 ShaderTexture_VS_Ad::g_vs30_main,
												 sizeof(ShaderTexture_VS_Ad::g_vs30_main),
												 ShaderTexture_PS_Ad::g_ps30_main,
												 sizeof(ShaderTexture_PS_Ad::g_ps30_main)),
											 vl);

	auto shader_ad_distortion = Shader::Create(graphicsDevice,
											   graphicsDevice->CreateShaderFromBinary(

												   ShaderDistortionTexture_VS_Ad::g_vs30_main,
												   sizeof(ShaderDistortionTexture_VS_Ad::g_vs30_main),
												   ShaderDistortionTexture_PS_Ad::g_ps30_main,
												   sizeof(ShaderDistortionTexture_PS_Ad::g_ps30_main)),
											   vl);

	Shader* shader_lit = Shader::Create(graphicsDevice,
										graphicsDevice->CreateShaderFromBinary(

											ShaderLightingTextureNormal_VS::g_vs30_main,
											sizeof(ShaderLightingTextureNormal_VS::g_vs30_main),
											ShaderLightingTextureNormal_PS::g_ps30_main,
											sizeof(ShaderLightingTextureNormal_PS::g_ps30_main)),
										vl);

	Shader* shader_unlit = Shader::Create(graphicsDevice,
										  graphicsDevice->CreateShaderFromBinary(

											  ShaderTexture_VS::g_vs30_main,
											  sizeof(ShaderTexture_VS::g_vs30_main),
											  ShaderTexture_PS::g_ps30_main,
											  sizeof(ShaderTexture_PS::g_ps30_main)),
										  vl);

	auto shader_distortion = Shader::Create(graphicsDevice,
											graphicsDevice->CreateShaderFromBinary(

												ShaderDistortionTexture_VS::g_vs30_main,
												sizeof(ShaderDistortionTexture_VS::g_vs30_main),
												ShaderDistortionTexture_PS::g_ps30_main,
												sizeof(ShaderDistortionTexture_PS::g_ps30_main)),
											vl);

	if (shader_ad_lit == nullptr || shader_ad_unlit == nullptr || shader_ad_distortion == nullptr ||
		shader_lit == nullptr || shader_unlit == nullptr || shader_distortion == nullptr)
	{
		ES_SAFE_DELETE(shader_ad_lit);
		ES_SAFE_DELETE(shader_ad_unlit);
		ES_SAFE_DELETE(shader_ad_distortion);
		ES_SAFE_DELETE(shader_lit);
		ES_SAFE_DELETE(shader_unlit);
		ES_SAFE_DELETE(shader_distortion);
	}

	return ModelRendererRef(new ModelRenderer(renderer, shader_ad_lit, shader_ad_unlit, shader_ad_distortion, shader_lit, shader_unlit, shader_distortion));
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(renderer_.Get(), parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(renderer_.Get(), parameter, instanceParameter, userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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

	model->StoreBufferToGPU(graphicsDevice_.Get(), true);
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
		ModelRendererInstanceCount>(
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
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
