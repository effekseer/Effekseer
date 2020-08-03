
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RenderState.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.VertexBuffer.h"
#include <string>

#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "ShaderHeader/model_renderer_distortion_PS.h"
#include "ShaderHeader/model_renderer_distortion_VS.h"
#include "ShaderHeader/model_renderer_lighting_texture_normal_PS.h"
#include "ShaderHeader/model_renderer_lighting_texture_normal_VS.h"
#include "ShaderHeader/model_renderer_texture_PS.h"
#include "ShaderHeader/model_renderer_texture_VS.h"
#else
#include "ShaderHeader_15/model_renderer_distortion_PS.h"
#include "ShaderHeader_15/model_renderer_distortion_VS.h"
#include "ShaderHeader_15/model_renderer_lighting_texture_normal_PS.h"
#include "ShaderHeader_15/model_renderer_lighting_texture_normal_VS.h"
#include "ShaderHeader_15/model_renderer_texture_PS.h"
#include "ShaderHeader_15/model_renderer_texture_VS.h"
#endif

namespace EffekseerRendererGL
{

static std::string Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

#ifdef __EFFEKSEER_BUILD_VERSION16__
static const int NumAttribs_Model = 6;

static ShaderAttribInfo g_model_attribs[NumAttribs_Model] = {
	{"Input_Pos", GL_FLOAT, 3, 0, false},
	{"Input_Normal", GL_FLOAT, 3, 12, false},
	{"Input_Binormal", GL_FLOAT, 3, 24, false},
	{"Input_Tangent", GL_FLOAT, 3, 36, false},
	{"Input_UV", GL_FLOAT, 2, 48, false},
	{"Input_Color", GL_UNSIGNED_BYTE, 4, 56, true},
};

#else
static const int NumAttribs_Model = 6;

static ShaderAttribInfo g_model_attribs[NumAttribs_Model] = {
	{"Input_Pos", GL_FLOAT, 3, 0, false},
	{"Input_Normal", GL_FLOAT, 3, 12, false},
	{"Input_Binormal", GL_FLOAT, 3, 24, false},
	{"Input_Tangent", GL_FLOAT, 3, 36, false},
	{"Input_UV", GL_FLOAT, 2, 48, false},
	{"Input_Color", GL_UNSIGNED_BYTE, 4, 56, true},
#if defined(MODEL_SOFTWARE_INSTANCING)
	{"a_InstanceID", GL_FLOAT, 1, 0, false},
	{"a_UVOffset", GL_FLOAT, 4, 0, false},
	{"a_ModelColor", GL_FLOAT, 4, 0, false},
#endif
};

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_lighting_texture_normal,
							 Shader* shader_texture,
							 Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	auto applyPSAdvancedRendererParameterTexture = [](Shader* shader, int32_t offset) -> void {
#ifdef __EFFEKSEER_BUILD_VERSION16__
		shader->SetTextureSlot(0 + offset, shader->GetUniformId("Sampler_g_alphaSampler"));
		shader->SetTextureSlot(1 + offset, shader->GetUniformId("Sampler_g_uvDistortionSampler"));
		shader->SetTextureSlot(2 + offset, shader->GetUniformId("Sampler_g_blendSampler"));
		shader->SetTextureSlot(3 + offset, shader->GetUniformId("Sampler_g_blendAlphaSampler"));
		shader->SetTextureSlot(4 + offset, shader->GetUniformId("Sampler_g_blendUVDistortionSampler"));
#endif
	};

	for (size_t i = 0; i < 8; i++)
	{
		m_va[i] = nullptr;
	}

	shader_lighting_texture_normal->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_lighting_texture_normal->SetTextureSlot(0, shader_lighting_texture_normal->GetUniformId("Sampler_g_colorSampler"));
	shader_lighting_texture_normal->SetTextureSlot(1, shader_lighting_texture_normal->GetUniformId("Sampler_g_normalSampler"));
	applyPSAdvancedRendererParameterTexture(shader_lighting_texture_normal, 2);

	shader_texture->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_texture->SetTextureSlot(0, shader_texture->GetUniformId("Sampler_g_colorSampler"));
	applyPSAdvancedRendererParameterTexture(shader_texture, 1);

	shader_distortion_texture->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_distortion_texture->SetTextureSlot(0, shader_distortion_texture->GetUniformId("Sampler_g_sampler"));
	shader_distortion_texture->SetTextureSlot(1, shader_distortion_texture->GetUniformId("Sampler_g_backSampler"));
	applyPSAdvancedRendererParameterTexture(shader_distortion_texture, 2);

	Shader* shaders[2];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_texture;

	for (int32_t i = 0; i < 2; i++)
	{
		shaders[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));

		int vsOffset = 0;
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("CBVS0.mCameraProj"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("CBVS0.mModel"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fUVDistortionUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendUVDistortionUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fFlipbookParameter"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fFlipbookIndexAndNextRate"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fModelAlphaThreshold"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;
#endif
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fModelColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		int psOffset = 0;
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fLightDirection"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fLightColor"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fLightAmbient"), psOffset);

		psOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fFlipbookParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fUVDistortionParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fBlendTextureParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fCameraFrontDirection"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fFalloffParam.Param"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fFalloffParam.BeginColor"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fFalloffParam.EndColor"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fEmissiveScaling"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fEdgeColor"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBPS0.fEdgeParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;
#endif
	}

	Shader* shaders_d[1];
	shaders_d[0] = shader_distortion_texture;

	for (int32_t i = 0; i < 1; i++)
	{
		shaders_d[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders_d[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));

		int vsOffset = 0;
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("CBVS0.mCameraProj"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("CBVS0.mModel"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fUVDistortionUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendUVDistortionUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fFlipbookParameter"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fFlipbookIndexAndNextRate"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fModelAlphaThreshold"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;
#endif
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fModelColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);
#else
		shaders_d[i]->SetPixelConstantBufferSize(sizeof(float) * 4 * 2);
#endif
		int psOffset = 0;

		shaders_d[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBPS0.g_scale"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBPS0.mUVInversedBack"), psOffset);

		psOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBPS0.fFlipbookParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBPS0.fUVDistortionParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBPS0.fBlendTextureParameter"), psOffset);

		psOffset += sizeof(float[4]) * 1;
#endif
	}

	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	}

	m_va[0] = VertexArray::Create(renderer, m_shader_lighting_texture_normal, nullptr, nullptr, true);
	m_va[4] = VertexArray::Create(renderer, m_shader_texture, nullptr, nullptr, true);
	m_va[6] = VertexArray::Create(renderer, m_shader_distortion_texture, nullptr, nullptr, true);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(currentVAO);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
	for (size_t i = 0; i < 8; i++)
	{
		ES_SAFE_DELETE(m_va[i]);
	}

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

	Shader* shader_lighting_texture_normal = NULL;
	Shader* shader_texture = NULL;
	Shader* shader_distortion_texture = NULL;

	ShaderCodeView ltnVS(get_model_renderer_lighting_texture_normal_VS(renderer->GetDeviceType()));
	ShaderCodeView ltnPS(get_model_renderer_lighting_texture_normal_PS(renderer->GetDeviceType()));
	ShaderCodeView tVS(get_model_renderer_texture_VS(renderer->GetDeviceType()));
	ShaderCodeView tPS(get_model_renderer_texture_PS(renderer->GetDeviceType()));
	ShaderCodeView dVS(get_model_renderer_distortion_VS(renderer->GetDeviceType()));
	ShaderCodeView dPS(get_model_renderer_distortion_PS(renderer->GetDeviceType()));

	shader_lighting_texture_normal = Shader::Create(renderer->GetGraphicsDevice(), &ltnVS, 1, &ltnPS, 1, "ModelRenderer1", true, false);
	if (shader_lighting_texture_normal == NULL)
		goto End;

	shader_texture = Shader::Create(renderer->GetGraphicsDevice(), &tVS, 1, &tPS, 1, "ModelRenderer5", true, false);
	if (shader_texture == NULL)
		goto End;

	shader_distortion_texture = Shader::Create(renderer->GetGraphicsDevice(), &dVS, 1, &dPS, 1, "ModelRenderer7", true, false);
	if (shader_distortion_texture == NULL)
		goto End;

	// Transpiled shader is transposed
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		shader_lighting_texture_normal->SetIsTransposeEnabled(true);
		shader_texture->SetIsTransposeEnabled(true);
		shader_distortion_texture->SetIsTransposeEnabled(true);
	}

	return new ModelRenderer(renderer, shader_lighting_texture_normal, shader_texture, shader_distortion_texture);
End:;

	ES_SAFE_DELETE(shader_lighting_texture_normal);
	ES_SAFE_DELETE(shader_texture);
	ES_SAFE_DELETE(shader_distortion_texture);
	return nullptr;
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
	if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
	{
		m_renderer->SetVertexArray(m_va[6]);
	}
	else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
	{
		m_renderer->SetVertexArray(m_va[0]);
	}
	else
	{
		m_renderer->SetVertexArray(m_va[4]);
	}

	if (parameter.ModelIndex < 0)
	{
		return;
	}

	auto model = (Model*)parameter.EffectPointer->GetModel(parameter.ModelIndex);
	if (model == nullptr)
	{
		return;
	}

	for (auto i = 0; i < model->GetFrameCount(); i++)
	{
		model->InternalModels[i].TryDelayLoad();
	}

	m_shader_lighting_texture_normal->SetVertexSize(model->GetVertexSize());
	m_shader_texture->SetVertexSize(model->GetVertexSize());
	m_shader_distortion_texture->SetVertexSize(model->GetVertexSize());

#if defined(MODEL_SOFTWARE_INSTANCING)
	EndRendering_<RendererImplemented, Shader, GLuint, Model, true, 20>(
		m_renderer, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter);
#else
	EndRendering_<RendererImplemented, Shader, Model, false, 1>(
		m_renderer, m_shader_lighting_texture_normal, m_shader_texture, m_shader_distortion_texture, parameter);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
