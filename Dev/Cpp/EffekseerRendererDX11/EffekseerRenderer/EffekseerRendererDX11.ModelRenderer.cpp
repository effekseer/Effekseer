
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RenderState.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#include "EffekseerRendererDX11.IndexBuffer.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.VertexBuffer.h"

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
	: m_renderer(renderer)
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

	D3D11_INPUT_ELEMENT_DESC decl[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	Shader* shader_advanced_lit = Shader::Create(renderer.Get(),
												 renderer->GetGraphicsDevice()->CreateShaderFromBinary(
													 ShaderLightingVS_::g_main,
													 sizeof(ShaderLightingVS_::g_main),
													 ShaderLightingPS_::g_main,
													 sizeof(ShaderLightingPS_::g_main)),
												 "ModelRendererLightingTextureNormal",
												 decl,
												 ARRAYSIZE(decl),
												 false);

	Shader* shader_advanced_unlit = Shader::Create(renderer.Get(),
												   renderer->GetGraphicsDevice()->CreateShaderFromBinary(
													   ShaderVS_::g_main,
													   sizeof(ShaderVS_::g_main),
													   ShaderPS_::g_main,
													   sizeof(ShaderPS_::g_main)),
												   "ModelRendererTexture",
												   decl,
												   ARRAYSIZE(decl),
												   false);

	auto shader_advanced_distortion = Shader::Create(renderer.Get(),
													 renderer->GetGraphicsDevice()->CreateShaderFromBinary(
														 ShaderDistortionVS_::g_main,
														 sizeof(ShaderDistortionVS_::g_main),
														 ShaderDistortionPS_::g_main,
														 sizeof(ShaderDistortionPS_::g_main)),
													 "ModelRendererDistortionTexture",
													 decl,
													 ARRAYSIZE(decl),
													 false);

	Shader* shader_lit = Shader::Create(renderer.Get(),
										renderer->GetGraphicsDevice()->CreateShaderFromBinary(
											ShaderLightingVS_15_::g_main,
											sizeof(ShaderLightingVS_15_::g_main),
											ShaderLightingPS_15_::g_main,
											sizeof(ShaderLightingPS_15_::g_main)),
										"ModelRendererLightingTextureNormal",
										decl,
										ARRAYSIZE(decl),
										false);

	Shader* shader_unlit = Shader::Create(renderer.Get(),
										  renderer->GetGraphicsDevice()->CreateShaderFromBinary(
											  ShaderVS_15_::g_main,
											  sizeof(ShaderVS_15_::g_main),
											  ShaderPS_15_::g_main,
											  sizeof(ShaderPS_15_::g_main)),
										  "ModelRendererTexture",
										  decl,
										  ARRAYSIZE(decl),
										  false);

	auto shader_distortion = Shader::Create(renderer.Get(),
											renderer->GetGraphicsDevice()->CreateShaderFromBinary(
												ShaderDistortionVS_15_::g_main,
												sizeof(ShaderDistortionVS_15_::g_main),
												ShaderDistortionPS_15_::g_main,
												sizeof(ShaderDistortionPS_15_::g_main)),
											"ModelRendererDistortionTexture",
											decl,
											ARRAYSIZE(decl),
											false);

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
	BeginRendering_(m_renderer.Get(), parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(m_renderer.Get(), parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (parameter.ModelIndex < 0)
	{
		return;
	}

	Effekseer::ModelRef model = nullptr;

	if (parameter.IsProceduralMode)
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

	if (m_renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
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
		m_renderer.Get(),
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
