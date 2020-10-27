
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

namespace ShaderLightingTextureNormal_
{
static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h"

	static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h"

} // namespace ShaderLightingTextureNormal_

namespace ShaderTexture_
{
static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h"

	static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h"
} // namespace ShaderTexture_

namespace ShaderDistortionTexture_
{
static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"

	static
#include "Shader_15/EffekseerRenderer.ModelRenderer.ShaderDistortionTexture_PS.h"
} // namespace ShaderDistortionTexture_

#endif

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
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>) / (sizeof(float) * 4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) / (sizeof(float) * 4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>) / (sizeof(float) * 4));
#ifdef __EFFEKSEER_BUILD_VERSION16__
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1 + 1 + 1);
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
	D3D11_INPUT_ELEMENT_DESC decl[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, sizeof(float) * 15, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	Shader* shader_lighting_texture_normal = Shader::Create(
		renderer,
		ShaderLightingTextureNormal_::g_VS,
		sizeof(ShaderLightingTextureNormal_::g_VS),
		ShaderLightingTextureNormal_::g_PS,
		sizeof(ShaderLightingTextureNormal_::g_PS),
		"ModelRendererLightingTextureNormal",
		decl,
		ARRAYSIZE(decl));

	Shader* shader_texture = Shader::Create(
		renderer,
		ShaderTexture_::g_VS,
		sizeof(ShaderTexture_::g_VS),
		ShaderTexture_::g_PS,
		sizeof(ShaderTexture_::g_PS),
		"ModelRendererTexture",
		decl,
		ARRAYSIZE(decl));

	auto shader_distortion_texture = Shader::Create(
		renderer,
		ShaderDistortionTexture_::g_VS,
		sizeof(ShaderDistortionTexture_::g_VS),
		ShaderDistortionTexture_::g_PS,
		sizeof(ShaderDistortionTexture_::g_PS),
		"ModelRendererDistortionTexture",
		decl,
		ARRAYSIZE(decl));

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
		40>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_texture,
		m_shader_distortion_texture,
		parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
