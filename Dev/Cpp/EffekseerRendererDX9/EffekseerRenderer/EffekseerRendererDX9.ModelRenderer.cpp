
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RenderState.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
#include "EffekseerRendererDX9.Shader.h"
#include "EffekseerRendererDX9.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

#ifdef __EFFEKSEER_BUILD_VERSION16__

namespace ShaderLightingTextureNormal_VS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h"
} // namespace ShaderLightingTextureNormal_VS

namespace ShaderLightingTextureNormal_PS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h"

} // namespace ShaderLightingTextureNormal_PS

namespace ShaderTexture_VS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h"
} // namespace ShaderTexture_VS

namespace ShaderTexture_PS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h"
} // namespace ShaderTexture_PS

namespace ShaderDistortionTexture_VS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"
} // namespace ShaderDistortionTexture_VS

namespace ShaderDistortionTexture_PS
{
static
#include "ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderDistortion_PS.h"
} // namespace ShaderDistortionTexture_PS


#else

namespace ShaderLightingTextureNormal_VS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h"
} // namespace ShaderLightingTextureNormal_VS

namespace ShaderLightingTextureNormal_PS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h"

} // namespace ShaderLightingTextureNormal_PS

namespace ShaderTexture_VS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h"
} // namespace ShaderTexture_VS

namespace ShaderTexture_PS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h"
} // namespace ShaderTexture_PS

namespace ShaderDistortionTexture_VS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"
} // namespace ShaderDistortionTexture_VS

namespace ShaderDistortionTexture_PS
{
static
#include "ShaderHeader_15/EffekseerRenderer.ModelRenderer.ShaderDistortion_PS.h"
} // namespace ShaderDistortionTexture_PS

#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
const int32_t ModelRendererInstanceCount = 10;
#else
const int32_t ModelRendererInstanceCount = 20;
#endif

ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_lighting_texture_normal,
							 Shader* shader_texture,
							 Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	Shader* shaders[2];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_texture;

	for (int32_t i = 0; i < 2; i++)
	{
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>) / (sizeof(float) * 4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) / (sizeof(float) * 4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<ModelRendererInstanceCount>) /
														(sizeof(float) * 4));
#ifdef __EFFEKSEER_BUILD_VERSION16__
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);
	m_shader_distortion_texture->SetPixelRegisterCount(5);
#else
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1 + 1);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(m_shader_lighting_texture_normal);
	ES_SAFE_DELETE(m_shader_texture);

	ES_SAFE_DELETE(m_shader_distortion_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ModelRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);
	assert(renderer->GetDevice() != NULL);

	// 座標(3) 法線(3)*3 UV(2)
	D3DVERTEXELEMENT9 decl[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
								{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
								{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
								{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
								{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
								{0, 56, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
								{0, 60, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6},
								D3DDECL_END()};

	Shader* shader_lighting_texture_normal = Shader::Create(renderer,
															ShaderLightingTextureNormal_VS::g_vs30_main,
															sizeof(ShaderLightingTextureNormal_VS::g_vs30_main),
															ShaderLightingTextureNormal_PS::g_ps30_main,
															sizeof(ShaderLightingTextureNormal_PS::g_ps30_main),
															"ModelRendererLightingTextureNormal",
															decl);

	Shader* shader_texture = Shader::Create(renderer,
											ShaderTexture_VS::g_vs30_main,
											sizeof(ShaderTexture_VS::g_vs30_main),
											ShaderTexture_PS::g_ps30_main,
											sizeof(ShaderTexture_PS::g_ps30_main),
											"ModelRendererTexture",
											decl);

	auto shader_distortion_texture = Shader::Create(renderer,
													ShaderDistortionTexture_VS::g_vs30_main,
													sizeof(ShaderDistortionTexture_VS::g_vs30_main),
													ShaderDistortionTexture_PS::g_ps30_main,
													sizeof(ShaderDistortionTexture_PS::g_ps30_main),
													"ModelRendererDistortionTexture",
													decl);

	if (shader_lighting_texture_normal == NULL || shader_texture == NULL || shader_distortion_texture == NULL)
	{
		ES_SAFE_DELETE(shader_lighting_texture_normal);
		ES_SAFE_DELETE(shader_texture);
		ES_SAFE_DELETE(shader_distortion_texture);
	}

	return new ModelRenderer(renderer, shader_lighting_texture_normal, shader_texture, shader_distortion_texture);
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(m_renderer, parameter, instanceParameter, userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	auto model = (Model*)parameter.EffectPointer->GetModel(parameter.ModelIndex);
	if (model == nullptr)
	{
		return;
	}

	model->LoadToGPU();
	if (!model->IsLoadedOnGPU)
	{
		return;
	}

	EndRendering_<
		RendererImplemented,
		Shader,
		Model,
		true,
		ModelRendererInstanceCount>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_texture,
		m_shader_distortion_texture,
		parameter );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
