
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
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortionTexture_PS.h"
} // namespace ShaderDistortionTexture_

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
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<20>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<20>) / (sizeof(float) * 4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) / (sizeof(float) * 4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<20>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<20>) /
														(sizeof(float) * 4));
#ifdef __EFFEKSEER_BUILD_VERSION16__
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(4);
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

#ifdef __EFFEKSEER_BUILD_VERSION16__
	Shader* shader_lighting_texture_normal = Shader::Create(renderer,
															ShaderLightingTextureNormal_::g_vs30_VS,
															sizeof(ShaderLightingTextureNormal_::g_vs30_VS),
															ShaderLightingTextureNormal_::g_ps30_PS,
															sizeof(ShaderLightingTextureNormal_::g_ps30_PS),
															"ModelRendererLightingTextureNormal",
															decl);

	Shader* shader_texture = Shader::Create(renderer,
											ShaderTexture_::g_vs30_VS,
											sizeof(ShaderTexture_::g_vs30_VS),
											ShaderTexture_::g_ps30_PS,
											sizeof(ShaderTexture_::g_ps30_PS),
											"ModelRendererTexture",
											decl);

	auto shader_distortion_texture = Shader::Create(renderer,
													ShaderDistortionTexture_::g_vs30_VS,
													sizeof(ShaderDistortionTexture_::g_vs30_VS),
													ShaderDistortionTexture_::g_ps30_PS,
													sizeof(ShaderDistortionTexture_::g_ps30_PS),
													"ModelRendererDistortionTexture",
													decl);
#else
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
#endif

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
	EndRendering_<RendererImplemented, Shader, Model, true, 20>(
		m_renderer, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
