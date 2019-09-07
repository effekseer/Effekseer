
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
							 Shader* shader_lighting_normal,
							 Shader* shader_lighting_texture,
							 Shader* shader_lighting,
							 Shader* shader_texture,
							 Shader* shader,
							 Shader* shader_distortion_texture,
							 Shader* shader_distortion)
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_lighting_normal(shader_lighting_normal)
	, m_shader_lighting_texture(shader_lighting_texture)
	, m_shader_lighting(shader_lighting)
	, m_shader_texture(shader_texture)
	, m_shader(shader)
	, m_shader_distortion_texture(shader_distortion_texture)
	, m_shader_distortion(shader_distortion)
{
	Shader* shaders[6];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_lighting_normal;
	shaders[2] = m_shader_lighting_texture;
	shaders[3] = m_shader_lighting;
	shaders[4] = m_shader_texture;
	shaders[5] = m_shader;

	for (int32_t i = 0; i < 6; i++)
	{
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>) / (sizeof(float) * 4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) / (sizeof(float) * 4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>) / (sizeof(float) * 4));
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1 + 1);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
	m_shader_distortion->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>) / (sizeof(float) * 4));
	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1 + 1);
}

ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(m_shader_lighting_texture_normal);
	ES_SAFE_DELETE(m_shader_lighting_normal);
	ES_SAFE_DELETE(m_shader_lighting_texture);
	ES_SAFE_DELETE(m_shader_lighting);
	ES_SAFE_DELETE(m_shader_texture);
	ES_SAFE_DELETE(m_shader);

	ES_SAFE_DELETE(m_shader_distortion_texture);
	ES_SAFE_DELETE(m_shader_distortion);
}

ModelRenderer* ModelRenderer::Create(RendererImplemented* renderer, FixedShader* fixedShader)
{

	assert(renderer != NULL);
	assert(renderer->GetGraphics() != NULL);

	std::vector<LLGI::VertexLayoutFormat> layouts;
	layouts.push_back(LLGI::VertexLayoutFormat::R32G32B32_FLOAT);
	layouts.push_back(LLGI::VertexLayoutFormat::R32G32B32_FLOAT);
	layouts.push_back(LLGI::VertexLayoutFormat::R32G32B32_FLOAT);
	layouts.push_back(LLGI::VertexLayoutFormat::R32G32B32_FLOAT);
	layouts.push_back(LLGI::VertexLayoutFormat::R32G32_FLOAT);
	layouts.push_back(LLGI::VertexLayoutFormat::R8G8B8A8_UNORM);
	layouts.push_back(LLGI::VertexLayoutFormat::R8G8B8A8_UNORM);

	Shader* shader_lighting_texture_normal = Shader::Create(renderer,
															fixedShader->ModelShaderLightingTextureNormal_VS.data(),
															fixedShader->ModelShaderLightingTextureNormal_VS.size(),
															fixedShader->ModelShaderLightingTextureNormal_PS.data(),
															fixedShader->ModelShaderLightingTextureNormal_PS.size(),
															"ModelRendererLightingTextureNormal",
															layouts);

	Shader* shader_lighting_normal = Shader::Create(renderer,
													fixedShader->ModelShaderLightingNormal_VS.data(),
													fixedShader->ModelShaderLightingNormal_VS.size(),
													fixedShader->ModelShaderLightingNormal_PS.data(),
													fixedShader->ModelShaderLightingNormal_PS.size(),
													"ModelRendererLightingNormal",
													layouts);

	Shader* shader_lighting_texture = Shader::Create(renderer,
													 fixedShader->ModelShaderLightingTexture_VS.data(),
													 fixedShader->ModelShaderLightingTexture_VS.size(),
													 fixedShader->ModelShaderLightingTexture_PS.data(),
													 fixedShader->ModelShaderLightingTexture_PS.size(),
													 "ModelRendererLightingTexture",
													 layouts);

	Shader* shader_lighting = Shader::Create(renderer,
											 fixedShader->ModelShaderLighting_VS.data(),
											 fixedShader->ModelShaderLighting_VS.size(),
											 fixedShader->ModelShaderLighting_PS.data(),
											 fixedShader->ModelShaderLighting_PS.size(),
											 "ModelRendererLighting",
											 layouts);

	Shader* shader_texture = Shader::Create(renderer,
											fixedShader->ModelShaderTexture_VS.data(),
											fixedShader->ModelShaderTexture_VS.size(),
											fixedShader->ModelShaderTexture_PS.data(),
											fixedShader->ModelShaderTexture_PS.size(),
											"ModelRendererTexture",
											layouts);

	Shader* shader = Shader::Create(renderer,
									fixedShader->ModelShader_VS.data(),
									fixedShader->ModelShader_VS.size(),
									fixedShader->ModelShader_PS.data(),
									fixedShader->ModelShader_PS.size(),
									"ModelRenderer",
									layouts);

	auto shader_distortion_texture = Shader::Create(renderer,
													fixedShader->ModelShaderDistortionTexture_VS.data(),
													fixedShader->ModelShaderDistortionTexture_VS.size(),
													fixedShader->ModelShaderDistortionTexture_PS.data(),
													fixedShader->ModelShaderDistortionTexture_PS.size(),
													"ModelRendererDistortionTexture",
													layouts);

	auto shader_distortion = Shader::Create(renderer,
											fixedShader->ModelShaderDistortion_VS.data(),
											fixedShader->ModelShaderDistortion_VS.size(),
											fixedShader->ModelShaderDistortion_PS.data(),
											fixedShader->ModelShaderDistortion_PS.size(),
											"ModelRendererDistortion",
											layouts);

	if (shader_lighting_texture_normal == NULL || shader_lighting_normal == NULL || shader_lighting_texture == NULL ||
		shader_lighting == NULL || shader_texture == NULL || shader == NULL || shader_distortion_texture == NULL ||
		shader_distortion == NULL)
	{
		ES_SAFE_DELETE(shader_lighting_texture_normal);
		ES_SAFE_DELETE(shader_lighting_normal);
		ES_SAFE_DELETE(shader_lighting_texture);
		ES_SAFE_DELETE(shader_lighting);
		ES_SAFE_DELETE(shader_texture);
		ES_SAFE_DELETE(shader);
		ES_SAFE_DELETE(shader_distortion_texture);
		ES_SAFE_DELETE(shader_distortion);
	}

	return new ModelRenderer(renderer,
							 shader_lighting_texture_normal,
							 shader_lighting_normal,
							 shader_lighting_texture,
							 shader_lighting,
							 shader_texture,
							 shader,
							 shader_distortion_texture,
							 shader_distortion);
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
	EndRendering_<RendererImplemented, Shader, Model, false, 1>(m_renderer,
																m_shader_lighting_texture_normal,
																m_shader_lighting_normal,
																m_shader_lighting_texture,
																m_shader_lighting,
																m_shader_texture,
																m_shader,
																m_shader_distortion_texture,
																m_shader_distortion,
																parameter);
}

} // namespace EffekseerRendererLLGI
