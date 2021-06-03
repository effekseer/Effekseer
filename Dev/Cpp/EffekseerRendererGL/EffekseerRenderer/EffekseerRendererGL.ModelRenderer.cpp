
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

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"

#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/model_distortion_vs.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_lit_vs.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/model_unlit_vs.h"

namespace EffekseerRendererGL
{

static const int InstanceCount = 10;

void AddModelVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, bool isAd, bool isInstancing, int N)
{
	using namespace Effekseer::Backend;

	int vsOffset = 0;

	auto storeVector = [&](const char* name, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, count, vsOffset});
		vsOffset += sizeof(float[4]) * count;
	};

	auto storeMatrix = [&](const char* name, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, count, vsOffset});
		vsOffset += sizeof(Effekseer::Matrix44) * count;
	};

	storeMatrix("CBVS0.mCameraProj");

	if (isInstancing)
	{
		storeMatrix("CBVS0.mModel_Inst", N);
	}
	else
	{
		storeMatrix("CBVS0.mModel", N);
	}

	storeVector("CBVS0.fUV", N);

	if (isAd)
	{
		storeVector("CBVS0.fAlphaUV", N);
		storeVector("CBVS0.fUVDistortionUV", N);
		storeVector("CBVS0.fBlendUV", N);
		storeVector("CBVS0.fBlendAlphaUV", N);
		storeVector("CBVS0.fBlendUVDistortionUV", N);
		storeVector("CBVS0.fFlipbookParameter");
		storeVector("CBVS0.fFlipbookIndexAndNextRate", N);
		storeVector("CBVS0.fModelAlphaThreshold", N);
	}

	storeVector("CBVS0.fModelColor", N);
	storeVector("CBVS0.fLightDirection");
	storeVector("CBVS0.fLightColor");
	storeVector("CBVS0.fLightAmbient");
	storeVector("CBVS0.mUVInversed");
}

template <int N>
void ModelRenderer::InitRenderer()
{
	auto applyPSAdvancedRendererParameterTexture = [](Shader* shader, int32_t offset) -> void {
		shader->SetTextureSlot(0 + offset, shader->GetUniformId("Sampler_sampler_alphaTex"));
		shader->SetTextureSlot(1 + offset, shader->GetUniformId("Sampler_sampler_uvDistortionTex"));
		shader->SetTextureSlot(2 + offset, shader->GetUniformId("Sampler_sampler_blendTex"));
		shader->SetTextureSlot(3 + offset, shader->GetUniformId("Sampler_sampler_blendAlphaTex"));
		shader->SetTextureSlot(4 + offset, shader->GetUniformId("Sampler_sampler_blendUVDistortionTex"));
	};

	for (size_t i = 0; i < 6; i++)
	{
		m_va[i] = nullptr;
	}

	const Effekseer::Backend::VertexLayoutElement vlElem[6] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Normal", "NORMAL", 1},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Binormal", "NORMAL", 1},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Tangent", "NORMAL", 2},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 3},
	};

	auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 6).DownCast<Backend::VertexLayout>();

	shader_ad_lit_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<N>));
	shader_ad_unlit_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<N>));
	shader_ad_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<N>));
	shader_ad_lit_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
	shader_ad_unlit_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
	shader_ad_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));

	shader_lit_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<N>));
	shader_unlit_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<N>));
	shader_distortion_->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<N>));
	shader_lit_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
	shader_unlit_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBuffer));
	shader_distortion_->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::PixelConstantBufferDistortion));

	for (auto& shader : {shader_ad_lit_, shader_lit_})
	{
		shader->SetVertexLayout(vl);
		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_sampler_normalTex"));
	}
	applyPSAdvancedRendererParameterTexture(shader_ad_lit_, 2);
	shader_lit_->SetTextureSlot(2, shader_lit_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_lit_->SetTextureSlot(7, shader_ad_lit_->GetUniformId("Sampler_sampler_depthTex"));

	for (auto& shader : {shader_ad_unlit_, shader_unlit_})
	{
		shader->SetVertexLayout(vl);
		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));
	}
	applyPSAdvancedRendererParameterTexture(shader_ad_unlit_, 1);
	shader_unlit_->SetTextureSlot(1, shader_unlit_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_unlit_->SetTextureSlot(6, shader_ad_unlit_->GetUniformId("Sampler_sampler_depthTex"));

	for (auto& shader : {shader_ad_distortion_, shader_distortion_})
	{
		shader->SetVertexLayout(vl);
		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_sampler_backTex"));
	}
	applyPSAdvancedRendererParameterTexture(shader_ad_distortion_, 2);
	shader_distortion_->SetTextureSlot(2, shader_distortion_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_distortion_->SetTextureSlot(7, shader_ad_distortion_->GetUniformId("Sampler_sampler_depthTex"));

	Shader* shaders[4];
	shaders[0] = shader_ad_lit_;
	shaders[1] = shader_ad_unlit_;
	shaders[2] = shader_lit_;
	shaders[3] = shader_unlit_;

	for (int32_t i = 0; i < 4; i++)
	{
		auto isAd = i < 2;

		int vsOffset = 0;
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("CBVS0.mCameraProj"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		if (VertexType == EffekseerRenderer::ModelRendererVertexType::Instancing)
		{
			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("CBVS0.mModel_Inst"), vsOffset, N);
		}
		else
		{
			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("CBVS0.mModel"), vsOffset, N);
		}

		vsOffset += sizeof(Effekseer::Matrix44) * N;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fUV"), vsOffset, N);

		vsOffset += sizeof(float[4]) * N;

		if (isAd)
		{
			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fAlphaUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fUVDistortionUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendAlphaUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fBlendUVDistortionUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fFlipbookParameter"), vsOffset);

			vsOffset += sizeof(float[4]) * 1;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fFlipbookIndexAndNextRate"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fModelAlphaThreshold"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;
		}

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fModelColor"), vsOffset, N);

		vsOffset += sizeof(float[4]) * N;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.fLightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("CBVS0.mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		AssignPixelConstantBuffer(shaders[i]);
	}

	Shader* shaders_d[2];
	shaders_d[0] = shader_ad_distortion_;
	shaders_d[1] = shader_distortion_;

	for (int32_t i = 0; i < 2; i++)
	{
		auto isAd = i < 1;

		int vsOffset = 0;
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("CBVS0.mCameraProj"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		if (VertexType == EffekseerRenderer::ModelRendererVertexType::Instancing)
		{
			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("CBVS0.mModel_Inst"), vsOffset, N);
		}
		else
		{
			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("CBVS0.mModel"), vsOffset, N);
		}

		vsOffset += sizeof(Effekseer::Matrix44) * N;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fUV"), vsOffset, N);

		vsOffset += sizeof(float[4]) * N;

		if (isAd)
		{
			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fAlphaUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fUVDistortionUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendAlphaUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fBlendUVDistortionUV"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fFlipbookParameter"), vsOffset);

			vsOffset += sizeof(float[4]) * 1;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fFlipbookIndexAndNextRate"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;

			shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fModelAlphaThreshold"), vsOffset, N);

			vsOffset += sizeof(float[4]) * N;
		}

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fModelColor"), vsOffset, N);

		vsOffset += sizeof(float[4]) * N;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.fLightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("CBVS0.mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		AssignDistortionPixelConstantBuffer(shaders_d[i]);
	}
}

ModelRenderer::ModelRenderer(RendererImplemented* renderer,
							 Shader* shader_ad_lit,
							 Shader* shader_ad_unlit,
							 Shader* shader_ad_distortion,
							 Shader* shader_lit,
							 Shader* shader_unlit,
							 Shader* shader_distortion)
	: m_renderer(renderer)
	, shader_ad_lit_(shader_ad_lit)
	, shader_ad_unlit_(shader_ad_unlit)
	, shader_ad_distortion_(shader_ad_distortion)
	, shader_lit_(shader_lit)
	, shader_unlit_(shader_unlit)
	, shader_distortion_(shader_distortion)
{
	graphicsDevice_ = renderer->GetGraphicsDevice().DownCast<Backend::GraphicsDevice>();
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		VertexType = EffekseerRenderer::ModelRendererVertexType::Instancing;
		InitRenderer<InstanceCount>();
	}
	else
	{
		InitRenderer<1>();
	}

	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	}

	m_va[0] = VertexArray::Create(graphicsDevice_, shader_ad_lit_, nullptr, nullptr);
	m_va[1] = VertexArray::Create(graphicsDevice_, shader_ad_unlit_, nullptr, nullptr);
	m_va[2] = VertexArray::Create(graphicsDevice_, shader_ad_distortion_, nullptr, nullptr);
	m_va[3] = VertexArray::Create(graphicsDevice_, shader_lit_, nullptr, nullptr);
	m_va[4] = VertexArray::Create(graphicsDevice_, shader_unlit_, nullptr, nullptr);
	m_va[5] = VertexArray::Create(graphicsDevice_, shader_distortion_, nullptr, nullptr);

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
	for (size_t i = 0; i < 6; i++)
	{
		ES_SAFE_DELETE(m_va[i]);
	}

	ES_SAFE_DELETE(shader_unlit_);
	ES_SAFE_DELETE(shader_lit_);
	ES_SAFE_DELETE(shader_distortion_);

	ES_SAFE_DELETE(shader_ad_unlit_);
	ES_SAFE_DELETE(shader_ad_lit_);
	ES_SAFE_DELETE(shader_ad_distortion_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRendererRef ModelRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != nullptr);

	auto graphicsDevice = renderer->GetGraphicsDevice();

	int instanceCount = 1;
	bool instaincing = false;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		instanceCount = InstanceCount;
		instaincing = true;
	}
	else
	{
		instaincing = false;
	}

	Shader* shader_ad_lit = nullptr;
	Shader* shader_ad_unlit = nullptr;
	Shader* shader_ad_distortion = nullptr;
	Shader* shader_lit = nullptr;
	Shader* shader_unlit = nullptr;
	Shader* shader_distortion = nullptr;

	ShaderCodeView ad_lit_vs(get_ad_model_lit_vs(renderer->GetDeviceType()));
	ShaderCodeView ad_lit_ps(get_ad_model_lit_ps(renderer->GetDeviceType()));
	ShaderCodeView ad_unlit_vs(get_ad_model_unlit_vs(renderer->GetDeviceType()));
	ShaderCodeView ad_unlit_ps(get_ad_model_unlit_ps(renderer->GetDeviceType()));
	ShaderCodeView ad_dist_vs(get_ad_model_distortion_vs(renderer->GetDeviceType()));
	ShaderCodeView ad_dist_ps(get_ad_model_distortion_ps(renderer->GetDeviceType()));
	ShaderCodeView lit_vs(get_model_lit_vs(renderer->GetDeviceType()));
	ShaderCodeView lit_ps(get_model_lit_ps(renderer->GetDeviceType()));
	ShaderCodeView unlit_vs(get_model_unlit_vs(renderer->GetDeviceType()));
	ShaderCodeView unlit_ps(get_model_unlit_ps(renderer->GetDeviceType()));
	ShaderCodeView dist_vs(get_model_distortion_vs(renderer->GetDeviceType()));
	ShaderCodeView dist_ps(get_model_distortion_ps(renderer->GetDeviceType()));

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
	AddModelVertexUniformLayout(uniformLayoutElementsLitUnlit, false, instaincing, instanceCount);
	AddPixelUniformLayout(uniformLayoutElementsLitUnlit);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsDist;
	AddModelVertexUniformLayout(uniformLayoutElementsDist, false, instaincing, instanceCount);
	AddDistortionPixelUniformLayout(uniformLayoutElementsDist);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlitAd;
	AddModelVertexUniformLayout(uniformLayoutElementsLitUnlitAd, true, instaincing, instanceCount);
	AddPixelUniformLayout(uniformLayoutElementsLitUnlitAd);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsDistAd;
	AddModelVertexUniformLayout(uniformLayoutElementsDistAd, true, instaincing, instanceCount);
	AddDistortionPixelUniformLayout(uniformLayoutElementsDistAd);

	auto uniformLayoutLitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsLitUnlitAd);
	auto shader_lit_ad_in = graphicsDevice->CreateShaderFromCodes({ad_lit_vs}, {ad_lit_ps}, uniformLayoutLitAd).DownCast<Backend::Shader>();

	auto uniformLayoutUnlitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsLitUnlitAd);
	auto shader_unlit_ad_in = graphicsDevice->CreateShaderFromCodes({ad_unlit_vs}, {ad_unlit_ps}, uniformLayoutUnlitAd).DownCast<Backend::Shader>();

	auto uniformLayoutDistAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsDistAd);
	auto shader_dist_ad_in = graphicsDevice->CreateShaderFromCodes({ad_dist_vs}, {ad_dist_ps}, uniformLayoutDistAd).DownCast<Backend::Shader>();

	auto uniformLayoutLit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsLitUnlit);
	auto shader_lit_in = graphicsDevice->CreateShaderFromCodes({lit_vs}, {lit_ps}, uniformLayoutLit).DownCast<Backend::Shader>();

	auto uniformLayoutUnlit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsLitUnlit);
	auto shader_unlit_in = graphicsDevice->CreateShaderFromCodes({unlit_vs}, {unlit_ps}, uniformLayoutUnlit).DownCast<Backend::Shader>();

	auto uniformLayoutDist = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElementsDist);
	auto shader_dist_in = graphicsDevice->CreateShaderFromCodes({dist_vs}, {dist_ps}, uniformLayoutDist).DownCast<Backend::Shader>();

	shader_ad_lit = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_lit_ad_in, "ModelRendererLitAd");
	if (shader_ad_lit == nullptr)
		goto End;

	shader_ad_unlit = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_unlit_ad_in, "ModelRendererUnlitAd");
	if (shader_ad_unlit == nullptr)
		goto End;

	shader_ad_distortion = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_dist_ad_in, "ModelRendererDistAd");
	if (shader_ad_distortion == nullptr)
		goto End;

	shader_lit = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_lit_in, "ModelRendererLit");
	if (shader_lit == nullptr)
		goto End;

	shader_unlit = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_unlit_in, "ModelRendererUnlit");
	if (shader_unlit == nullptr)
		goto End;

	shader_distortion = Shader::Create(renderer->GetInternalGraphicsDevice(), shader_dist_in, "ModelRendererDit");
	if (shader_distortion == nullptr)
		goto End;

	// Transpiled shader is transposed
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		shader_ad_lit->SetIsTransposeEnabled(true);
		shader_ad_unlit->SetIsTransposeEnabled(true);
		shader_ad_distortion->SetIsTransposeEnabled(true);
		shader_lit->SetIsTransposeEnabled(true);
		shader_unlit->SetIsTransposeEnabled(true);
		shader_distortion->SetIsTransposeEnabled(true);
	}

	return ModelRendererRef(new ModelRenderer(renderer, shader_ad_lit, shader_ad_unlit, shader_ad_distortion, shader_lit, shader_unlit, shader_distortion));
End:;

	ES_SAFE_DELETE(shader_ad_lit);
	ES_SAFE_DELETE(shader_ad_unlit);
	ES_SAFE_DELETE(shader_ad_distortion);
	ES_SAFE_DELETE(shader_lit);
	ES_SAFE_DELETE(shader_unlit);
	ES_SAFE_DELETE(shader_distortion);
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
	if (collector_.DoRequireAdvancedRenderer())
	{
		if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			m_renderer->SetVertexArray(m_va[2]);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			m_renderer->SetVertexArray(m_va[0]);
		}
		else
		{
			m_renderer->SetVertexArray(m_va[1]);
		}
	}
	else
	{
		if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			m_renderer->SetVertexArray(m_va[5]);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			m_renderer->SetVertexArray(m_va[3]);
		}
		else
		{
			m_renderer->SetVertexArray(m_va[4]);
		}
	}

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

	if (VertexType == EffekseerRenderer::ModelRendererVertexType::Instancing)
	{
		EndRendering_<RendererImplemented, Shader, Effekseer::Model, true, InstanceCount>(
			m_renderer, shader_ad_lit_, shader_ad_unlit_, shader_ad_distortion_, shader_lit_, shader_unlit_, shader_distortion_, parameter, userData);
	}
	else
	{
		EndRendering_<RendererImplemented, Shader, Effekseer::Model, false, 1>(
			m_renderer, shader_ad_lit_, shader_ad_unlit_, shader_ad_distortion_, shader_lit_, shader_unlit_, shader_distortion_, parameter, userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
