
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

ModelRenderer::ModelRenderer(
	RendererImplemented* renderer,
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
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<20>) / (sizeof(float) * 4));
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1 + 1);
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
	D3DVERTEXELEMENT9 decl[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
			{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1},
			{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2},
			{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			{0, 56, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 3},
			{0, 60, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
			D3DDECL_END()};

	Shader* shader_lighting_texture_normal = Shader::Create(
		renderer,
		ShaderLightingTextureNormal_::g_vs20_VS,
		sizeof(ShaderLightingTextureNormal_::g_vs20_VS),
		ShaderLightingTextureNormal_::g_ps20_PS,
		sizeof(ShaderLightingTextureNormal_::g_ps20_PS),
		"ModelRendererLightingTextureNormal",
		decl);

	Shader* shader_texture = Shader::Create(
		renderer,
		ShaderTexture_::g_vs20_VS,
		sizeof(ShaderTexture_::g_vs20_VS),
		ShaderTexture_::g_ps20_PS,
		sizeof(ShaderTexture_::g_ps20_PS),
		"ModelRendererTexture",
		decl);

	auto shader_distortion_texture = Shader::Create(
		renderer,
		ShaderDistortionTexture_::g_vs20_VS,
		sizeof(ShaderDistortionTexture_::g_vs20_VS),
		ShaderDistortionTexture_::g_ps20_PS,
		sizeof(ShaderDistortionTexture_::g_ps20_PS),
		"ModelRendererDistortionTexture",
		decl);

	if (shader_lighting_texture_normal == NULL ||
		shader_texture == NULL ||
		shader_distortion_texture == NULL)
	{
		ES_SAFE_DELETE(shader_lighting_texture_normal);
		ES_SAFE_DELETE(shader_texture);
		ES_SAFE_DELETE(shader_distortion_texture);
	}

	return new ModelRenderer(renderer,
							 shader_lighting_texture_normal,
							 shader_texture,
							 shader_distortion_texture);
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<
		RendererImplemented>(
		m_renderer,
		parameter,
		instanceParameter,
		userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (parameter.ModelIndex < 0)
	{
		return;
	}

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
		20>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_texture,
		m_shader_distortion_texture,
		parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
