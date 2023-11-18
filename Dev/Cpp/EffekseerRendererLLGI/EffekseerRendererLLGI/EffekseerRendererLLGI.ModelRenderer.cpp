
#include "EffekseerRendererLLGI.RenderState.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"

#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{
const int LLGI_InstanceCount = 40;

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
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<LLGI_InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<LLGI_InstanceCount>));
		shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	{
		std::array<Shader*, 2> shaders;
		shaders[0] = m_shader_lighting_texture_normal;
		shaders[1] = m_shader_texture;

		for (size_t i = 0; i < shaders.size(); i++)
		{
			shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<LLGI_InstanceCount>));
			shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
		}

		m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<LLGI_InstanceCount>));
		m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));
	}

	VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;

	graphicsDevice_ = m_renderer->GetGraphicsDeviceInternal().Get();
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

	auto graphicsDevice = renderer->GetGraphicsDevice();
	auto vl = EffekseerRenderer::GetModelRendererVertexLayout(renderer->GetGraphicsDevice()).DownCast<Backend::VertexLayout>();
	vl->MakeGenerated();

	auto lit_vs_shader_data = Backend::Serialize(fixedShader->ModelLit_VS);
	auto lit_ps_shader_data = Backend::Serialize(fixedShader->ModelLit_PS);

	Shader* shader_lighting_texture_normal = Shader::Create(graphicsDevice,
															graphicsDevice->CreateShaderFromBinary(
																lit_vs_shader_data.data(),
																(int32_t)lit_vs_shader_data.size(),
																lit_ps_shader_data.data(),
																(int32_t)lit_ps_shader_data.size()),
															vl,
															"ModelRendererLightingTextureNormal");

	auto unlit_vs_shader_data = Backend::Serialize(fixedShader->ModelUnlit_VS);
	auto unlit_ps_shader_data = Backend::Serialize(fixedShader->ModelUnlit_PS);

	Shader* shader_texture = Shader::Create(graphicsDevice,
											graphicsDevice->CreateShaderFromBinary(
												unlit_vs_shader_data.data(),
												(int32_t)unlit_vs_shader_data.size(),
												unlit_ps_shader_data.data(),
												(int32_t)unlit_ps_shader_data.size()),
											vl,
											"ModelRendererTexture");

	auto dist_vs_shader_data = Backend::Serialize(fixedShader->ModelDistortion_VS);
	auto dist_ps_shader_data = Backend::Serialize(fixedShader->ModelDistortion_PS);

	auto shader_distortion_texture = Shader::Create(graphicsDevice,
													graphicsDevice->CreateShaderFromBinary(
														dist_vs_shader_data.data(),
														(int32_t)dist_vs_shader_data.size(),
														dist_ps_shader_data.data(),
														(int32_t)dist_ps_shader_data.size()),
													vl,
													"ModelRendererDistortionTexture");

	auto ad_lit_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelLit_VS);
	auto ad_lit_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelLit_PS);

	Shader* shader_ad_lit = Shader::Create(graphicsDevice,
										   graphicsDevice->CreateShaderFromBinary(
											   ad_lit_vs_shader_data.data(),
											   (int32_t)ad_lit_vs_shader_data.size(),
											   ad_lit_ps_shader_data.data(),
											   (int32_t)ad_lit_ps_shader_data.size()),
										   vl,
										   "ModelRendererLightingTextureNormal");

	auto ad_unlit_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelUnlit_VS);
	auto ad_unlit_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelUnlit_PS);

	Shader* shader_ad_unlit = Shader::Create(graphicsDevice,
											 graphicsDevice->CreateShaderFromBinary(
												 ad_unlit_vs_shader_data.data(),
												 (int32_t)ad_unlit_vs_shader_data.size(),
												 ad_unlit_ps_shader_data.data(),
												 (int32_t)ad_unlit_ps_shader_data.size()),
											 vl,
											 "ModelRendererTexture");

	auto ad_dist_vs_shader_data = Backend::Serialize(fixedShader->AdvancedModelDistortion_VS);
	auto ad_dist_ps_shader_data = Backend::Serialize(fixedShader->AdvancedModelDistortion_PS);

	auto shader_ad_distortion = Shader::Create(graphicsDevice,
											   graphicsDevice->CreateShaderFromBinary(
												   ad_dist_vs_shader_data.data(),
												   (int32_t)ad_dist_vs_shader_data.size(),
												   ad_dist_ps_shader_data.data(),
												   (int32_t)ad_dist_ps_shader_data.size()),
											   vl,
											   "ModelRendererDistortionTexture");

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

	model->StoreBufferToGPU(graphicsDevice_);
	if (!model->GetIsBufferStoredOnGPU())
	{
		return;
	}

	if (m_renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
	{
		model->GenerateWireIndexBuffer(graphicsDevice_);
		if (!model->GetIsWireIndexBufferGenerated())
		{
			return;
		}
	}

	EndRendering_<RendererImplemented, Shader, Effekseer::Model, true, LLGI_InstanceCount>(
		m_renderer, shader_ad_lit_, shader_ad_unlit_, shader_ad_distortion_, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter, userData);
}

} // namespace EffekseerRendererLLGI
