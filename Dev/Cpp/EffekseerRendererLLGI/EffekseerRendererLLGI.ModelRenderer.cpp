
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.IndexBuffer.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.VertexBuffer.h"

namespace EffekseerRendererLLGI
{

ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_lighting_texture_normal,
							 Shader* shader_texture,
							 Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	std::array<Shader*, 2> shaders;
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_texture;

	for (size_t i = 0; i < shaders.size(); i++)
	{
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>) / (sizeof(float) * 4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) / (sizeof(float) * 4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>) /
														(sizeof(float) * 4));
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1 + 1);
}

ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(m_shader_lighting_texture_normal);
	ES_SAFE_DELETE(m_shader_texture);
	ES_SAFE_DELETE(m_shader_distortion_texture);
}

ModelRenderer* ModelRenderer::Create(RendererImplemented* renderer, FixedShader* fixedShader)
{

	assert(renderer != NULL);
	assert(renderer->GetGraphics() != NULL);

	std::vector<VertexLayout> layouts;
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "POSITION", 0});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 0});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 1});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 2});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 0});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 3});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UINT, "BLENDINDICES", 0});

	Shader* shader_lighting_texture_normal = Shader::Create(renderer->GetGraphicsDevice(),
															fixedShader->ModelShaderLightingTextureNormal_VS.data(),
															fixedShader->ModelShaderLightingTextureNormal_VS.size(),
															fixedShader->ModelShaderLightingTextureNormal_PS.data(),
															fixedShader->ModelShaderLightingTextureNormal_PS.size(),
															"ModelRendererLightingTextureNormal",
															layouts,
															true);

	Shader* shader_texture = Shader::Create(renderer->GetGraphicsDevice(),
											fixedShader->ModelShaderTexture_VS.data(),
											fixedShader->ModelShaderTexture_VS.size(),
											fixedShader->ModelShaderTexture_PS.data(),
											fixedShader->ModelShaderTexture_PS.size(),
											"ModelRendererTexture",
											layouts,
											true);

	auto shader_distortion_texture = Shader::Create(renderer->GetGraphicsDevice(),
													fixedShader->ModelShaderDistortionTexture_VS.data(),
													fixedShader->ModelShaderDistortionTexture_VS.size(),
													fixedShader->ModelShaderDistortionTexture_PS.data(),
													fixedShader->ModelShaderDistortionTexture_PS.size(),
													"ModelRendererDistortionTexture",
													layouts,
													true);

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

	EndRendering_<RendererImplemented, Shader, Model, false, 1>(
		m_renderer, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter);
}

} // namespace EffekseerRendererLLGI
