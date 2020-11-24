﻿
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.IndexBuffer.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.VertexBuffer.h"

#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{
const int InstanceCount = 10;

ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_ad_lit,
							 Shader* shader_ad_unlit,
							 Shader* shader_ad_distortion,
							 Shader* shader_lighting_texture_normal,
							 Shader* shader_texture,
							 Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, shader_ad_lit_(shader_ad_lit)
	, shader_ad_unlit_(shader_ad_unlit)
	, shader_ad_distortion_(shader_ad_distortion)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	// Ad
	{
		std::array<Shader*, 2> shaders;
		shaders[0] = shader_ad_lit_;
		shaders[1] = shader_ad_unlit_;

		for (size_t i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedPixelConstantBuffer));
		}

		shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<InstanceCount>));
		shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererDistortionPixelConstantBuffer));
	}

	{
		std::array<Shader*, 2> shaders;
		shaders[0] = m_shader_lighting_texture_normal;
		shaders[1] = m_shader_texture;

		for (size_t i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		}

		m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<InstanceCount>));
		m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererDistortionPixelConstantBuffer));
	}

	VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;

	graphicsDevice_ = m_renderer->GetGraphicsDevice().Get();
	LLGI::SafeAddRef(graphicsDevice_);
}

ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE(m_shader_lighting_texture_normal);
	ES_SAFE_DELETE(m_shader_texture);
	ES_SAFE_DELETE(m_shader_distortion_texture);

	ES_SAFE_DELETE(shader_ad_unlit_);
	ES_SAFE_DELETE(shader_ad_lit_);
	ES_SAFE_DELETE(shader_ad_distortion_);

	LLGI::SafeRelease(graphicsDevice_);
}

ModelRendererRef ModelRenderer::Create(RendererImplemented* renderer, FixedShader* fixedShader)
{

	assert(renderer != nullptr);
	assert(renderer->GetGraphics() != nullptr);

	std::vector<VertexLayout> layouts;
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 0});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 1});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 2});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "TEXCOORD", 3});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 4});
	layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "TEXCOORD", 5});

	Shader* shader_lighting_texture_normal = Shader::Create(renderer->GetGraphicsDevice().Get(),
															fixedShader->ModelLit_VS.data(),
															(int32_t)fixedShader->ModelLit_VS.size(),
															fixedShader->ModelLit_PS.data(),
															(int32_t)fixedShader->ModelLit_PS.size(),
															"ModelRendererLightingTextureNormal",
															layouts,
															true);

	Shader* shader_texture = Shader::Create(renderer->GetGraphicsDevice().Get(),
											fixedShader->ModelUnlit_VS.data(),
											(int32_t)fixedShader->ModelUnlit_VS.size(),
											fixedShader->ModelUnlit_PS.data(),
											(int32_t)fixedShader->ModelUnlit_PS.size(),
											"ModelRendererTexture",
											layouts,
											true);

	auto shader_distortion_texture = Shader::Create(renderer->GetGraphicsDevice().Get(),
													fixedShader->ModelDistortion_VS.data(),
													(int32_t)fixedShader->ModelDistortion_VS.size(),
													fixedShader->ModelDistortion_PS.data(),
													(int32_t)fixedShader->ModelDistortion_PS.size(),
													"ModelRendererDistortionTexture",
													layouts,
													true);

	Shader* shader_ad_lit = Shader::Create(renderer->GetGraphicsDevice().Get(),
										   fixedShader->AdvancedModelLit_VS.data(),
										   (int32_t)fixedShader->AdvancedModelLit_VS.size(),
										   fixedShader->AdvancedModelLit_PS.data(),
										   (int32_t)fixedShader->AdvancedModelLit_PS.size(),
										   "ModelRendererLightingTextureNormal",
										   layouts,
										   true);

	Shader* shader_ad_unlit = Shader::Create(renderer->GetGraphicsDevice().Get(),
											 fixedShader->AdvancedModelUnlit_VS.data(),
											 (int32_t)fixedShader->AdvancedModelUnlit_VS.size(),
											 fixedShader->AdvancedModelUnlit_PS.data(),
											 (int32_t)fixedShader->AdvancedModelUnlit_PS.size(),
											 "ModelRendererTexture",
											 layouts,
											 true);

	auto shader_ad_distortion = Shader::Create(renderer->GetGraphicsDevice().Get(),
											   fixedShader->AdvancedModelDistortion_VS.data(),
											   (int32_t)fixedShader->AdvancedModelDistortion_VS.size(),
											   fixedShader->AdvancedModelDistortion_PS.data(),
											   (int32_t)fixedShader->AdvancedModelDistortion_PS.size(),
											   "ModelRendererDistortionTexture",
											   layouts,
											   true);

	if (shader_lighting_texture_normal == nullptr || shader_texture == nullptr || shader_distortion_texture == nullptr ||
		shader_ad_lit == nullptr || shader_ad_unlit == nullptr || shader_ad_distortion == nullptr)
	{
		ES_SAFE_DELETE(shader_lighting_texture_normal);
		ES_SAFE_DELETE(shader_texture);
		ES_SAFE_DELETE(shader_distortion_texture);
		ES_SAFE_DELETE(shader_ad_unlit);
		ES_SAFE_DELETE(shader_ad_lit);
		ES_SAFE_DELETE(shader_ad_distortion);
	}

	return ModelRendererRef(new ModelRenderer(renderer, shader_ad_lit, shader_ad_unlit, shader_ad_distortion, shader_lighting_texture_normal, shader_texture, shader_distortion_texture));
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

	Effekseer::Model* model = nullptr;

	if (parameter.IsProcedualMode)
	{
		model = parameter.EffectPointer->GetProcedualModel(parameter.ModelIndex);
	}
	else
	{
		model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	}

	if (model == nullptr)
	{
		return;
	}

	model->StoreBufferToGPU(graphicsDevice_);
	if (!model->GetIsBufferStoredOnGPU())
	{
		return;
	}

	EndRendering_<RendererImplemented, Shader, Effekseer::Model, true, InstanceCount>(
		m_renderer, shader_ad_lit_, shader_ad_unlit_, shader_ad_distortion_, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter);
}

} // namespace EffekseerRendererLLGI
