
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.RenderState.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.TextureLoader.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.VertexBuffer.h"

#include "EffekseerRendererGL.GLExtension.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

#include "ShaderHeader/standard_renderer_PS.h"
#include "ShaderHeader/standard_renderer_VS.h"
#include "ShaderHeader/standard_renderer_distortion_PS.h"
#include "ShaderHeader/standard_renderer_distortion_VS.h"
#include "ShaderHeader/standard_renderer_lighting_PS.h"
#include "ShaderHeader/standard_renderer_lighting_VS.h"

#include "ShaderHeader/sprite_distortion_ps.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_ps.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_ps.h"
#include "ShaderHeader/sprite_unlit_vs.h"

namespace EffekseerRendererGL
{

::EffekseerRenderer::GraphicsDevice* CreateDevice(OpenGLDeviceType deviceType)
{
	return new GraphicsDevice(deviceType);
}

::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface, ::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(fileInterface, colorSpaceType);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface, OpenGLDeviceType deviceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(fileInterface, deviceType);
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* CreateMaterialLoader(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new MaterialLoader(static_cast<GraphicsDevice*>(graphicsDevice), fileInterface);
#else
	return NULL;
#endif
}

Renderer* Renderer::Create(int32_t squareMaxCount, OpenGLDeviceType deviceType)
{
	GLExt::Initialize(deviceType);

	RendererImplemented* renderer = new RendererImplemented(squareMaxCount, deviceType, nullptr);
	if (renderer->Initialize())
	{
		return renderer;
	}
	return NULL;
}

Renderer* Renderer::Create(int32_t squareMaxCount, ::EffekseerRenderer::GraphicsDevice* graphicDevice)
{
	auto g = static_cast<GraphicsDevice*>(graphicDevice);

	GLExt::Initialize(g->GetDeviceType());

	RendererImplemented* renderer = new RendererImplemented(squareMaxCount, g->GetDeviceType(), g);
	if (renderer->Initialize())
	{
		return renderer;
	}
	return NULL;
}

int32_t RendererImplemented::GetIndexSpriteCount() const
{
	int vsSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4;

	size_t size = sizeof(EffekseerRenderer::SimpleVertex);
	size = (std::min)(size, sizeof(VertexDistortion));
	size = (std::min)(size, sizeof(EffekseerRenderer::DynamicVertex));
	size = (std::min)(size, sizeof(EffekseerRenderer::LightingVertex));

	return (vsSize / size / 4 + 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount, OpenGLDeviceType deviceType, GraphicsDevice* graphicsDevice)
	: m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_indexBufferForWireframe(NULL)
	, m_squareMaxCount(squareMaxCount)
	, m_renderState(NULL)
	, m_restorationOfStates(true)
	, m_currentVertexArray(NULL)
	, m_standardRenderer(nullptr)
	, m_vao_wire_frame(nullptr)
	, m_distortingCallback(nullptr)

	, m_deviceType(deviceType)
	, graphicsDevice_(graphicsDevice)
{
	m_background.UserID = 0;
	m_background.HasMipmap = false;

	if (graphicsDevice == nullptr)
	{
		graphicsDevice_ = new GraphicsDevice(deviceType);
	}
	else
	{
		ES_SAFE_ADDREF(graphicsDevice_);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(m_distortingCallback);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(shader_unlit_);
	ES_SAFE_DELETE(shader_distortion_);
	ES_SAFE_DELETE(shader_lit_);

	ES_SAFE_DELETE(shader_ad_unlit_);
	ES_SAFE_DELETE(shader_ad_lit_);
	ES_SAFE_DELETE(shader_ad_distortion_);

	auto isVaoEnabled = vao_unlit_ != nullptr;

	ES_SAFE_DELETE(vao_unlit_);
	ES_SAFE_DELETE(vao_distortion_);
	ES_SAFE_DELETE(vao_lit_);

	ES_SAFE_DELETE(vao_ad_unlit_);
	ES_SAFE_DELETE(vao_ad_lit_);
	ES_SAFE_DELETE(vao_ad_distortion_);

	ES_SAFE_DELETE(m_vao_wire_frame);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	ES_SAFE_RELEASE(graphicsDevice_);

	if (GLExt::IsSupportedVertexArray() && defaultVertexArray_ > 0)
	{
		GLExt::glDeleteVertexArrays(1, &defaultVertexArray_);
		defaultVertexArray_ = 0;
	}
}

void RendererImplemented::OnLostDevice()
{
	if (graphicsDevice_ != nullptr)
		graphicsDevice_->OnLostDevice();
}

void RendererImplemented::OnResetDevice()
{
	if (graphicsDevice_ != nullptr)
		graphicsDevice_->OnResetDevice();

	GenerateIndexData();
}

void RendererImplemented::GenerateIndexData()
{
	if (indexBufferStride_ == 2)
	{
		GenerateIndexDataStride<uint16_t>();
	}
	else if (indexBufferStride_ == 4)
	{
		GenerateIndexDataStride<uint32_t>();
	}
}

template <typename T>
void RendererImplemented::GenerateIndexDataStride()
{
	// generate an index buffer
	if (m_indexBuffer != nullptr)
	{
		m_indexBuffer->Lock();

		for (int i = 0; i < GetIndexSpriteCount(); i++)
		{
			std::array<T, 6> buf;
			buf[0] = (T)(3 + 4 * i);
			buf[1] = (T)(1 + 4 * i);
			buf[2] = (T)(0 + 4 * i);
			buf[3] = (T)(3 + 4 * i);
			buf[4] = (T)(0 + 4 * i);
			buf[5] = (T)(2 + 4 * i);
			memcpy(m_indexBuffer->GetBufferDirect(6), buf.data(), sizeof(T) * 6);
		}

		m_indexBuffer->Unlock();
	}

	// generate an index buffer for a wireframe
	if (m_indexBufferForWireframe != nullptr)
	{
		m_indexBufferForWireframe->Lock();

		for (int i = 0; i < GetIndexSpriteCount(); i++)
		{
			std::array<T, 8> buf;
			buf[0] = (T)(0 + 4 * i);
			buf[1] = (T)(1 + 4 * i);
			buf[2] = (T)(2 + 4 * i);
			buf[3] = (T)(3 + 4 * i);
			buf[4] = (T)(0 + 4 * i);
			buf[5] = (T)(2 + 4 * i);
			buf[6] = (T)(1 + 4 * i);
			buf[7] = (T)(3 + 4 * i);
			memcpy(m_indexBufferForWireframe->GetBufferDirect(8), buf.data(), sizeof(T) * 8);
		}

		m_indexBufferForWireframe->Unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize()
{
	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	}

	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	if (GetIndexSpriteCount() * 4 > 65536)
	{
		indexBufferStride_ = 4;
	}

	SetSquareMaxCount(m_squareMaxCount);

	m_renderState = new RenderState(this);

	ShaderCodeView unlit_ad_vs(get_standard_renderer_VS(GetDeviceType()));
	ShaderCodeView unlit_ad_ps(get_standard_renderer_PS(GetDeviceType()));
	ShaderCodeView distortion_ad_vs(get_standard_renderer_distortion_VS(GetDeviceType()));
	ShaderCodeView distortion_ad_ps(get_standard_renderer_distortion_PS(GetDeviceType()));
	ShaderCodeView lit_ad_vs(get_standard_renderer_lighting_VS(GetDeviceType()));
	ShaderCodeView lit_ad_ps(get_standard_renderer_lighting_PS(GetDeviceType()));

	ShaderCodeView unlit_vs(get_sprite_unlit_vs(GetDeviceType()));
	ShaderCodeView unlit_ps(get_sprite_unlit_ps(GetDeviceType()));
	ShaderCodeView distortion_vs(get_sprite_distortion_vs(GetDeviceType()));
	ShaderCodeView distortion_ps(get_sprite_distortion_ps(GetDeviceType()));
	ShaderCodeView lit_vs(get_sprite_lit_vs(GetDeviceType()));
	ShaderCodeView lit_ps(get_sprite_lit_ps(GetDeviceType()));

	shader_ad_unlit_ = Shader::Create(GetGraphicsDevice(), &unlit_ad_vs, 1, &unlit_ad_ps, 1, "Standard Tex", false, false);
	if (shader_ad_unlit_ == nullptr)
		return false;

	shader_ad_distortion_ = Shader::Create(GetGraphicsDevice(), &distortion_ad_vs, 1, &distortion_ad_ps, 1, "Standard Distortion Tex", false, false);
	if (shader_ad_distortion_ == nullptr)
		return false;

	shader_ad_lit_ = Shader::Create(GetGraphicsDevice(), &lit_ad_vs, 1, &lit_ad_ps, 1, "Standard Lighting Tex", false, false);

	shader_unlit_ = Shader::Create(GetGraphicsDevice(), &unlit_vs, 1, &unlit_ps, 1, "Standard Tex", false, false);
	if (shader_unlit_ == nullptr)
		return false;

	shader_distortion_ = Shader::Create(GetGraphicsDevice(), &distortion_vs, 1, &distortion_ps, 1, "Standard Distortion Tex", false, false);
	if (shader_distortion_ == nullptr)
		return false;

	shader_lit_ = Shader::Create(GetGraphicsDevice(), &lit_vs, 1, &lit_ps, 1, "Standard Lighting Tex", false, false);

	auto applyPSAdvancedRendererParameterTexture = [](Shader* shader, int32_t offset) -> void {
		shader->SetTextureSlot(0 + offset, shader->GetUniformId("Sampler_g_alphaSampler"));
		shader->SetTextureSlot(1 + offset, shader->GetUniformId("Sampler_g_uvDistortionSampler"));
		shader->SetTextureSlot(2 + offset, shader->GetUniformId("Sampler_g_blendSampler"));
		shader->SetTextureSlot(3 + offset, shader->GetUniformId("Sampler_g_blendAlphaSampler"));
		shader->SetTextureSlot(4 + offset, shader->GetUniformId("Sampler_g_blendUVDistortionSampler"));
	};

	// Unlit

	static ShaderAttribInfo sprite_attribs_ad[8] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_UV", GL_FLOAT, 2, 16, false},

		{"Input_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 6, false},
		{"Input_BlendUV", GL_FLOAT, 2, sizeof(float) * 10, false},
		{"Input_Blend_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 12, false},
		{"Input_FlipbookIndex", GL_FLOAT, 1, sizeof(float) * 16, false},
		{"Input_AlphaThreshold", GL_FLOAT, 1, sizeof(float) * 17, false},
	};

	shader_ad_unlit_->GetAttribIdList(8, sprite_attribs_ad);

	static ShaderAttribInfo sprite_attribs[3] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_UV", GL_FLOAT, 2, 16, false},
	};
	shader_unlit_->GetAttribIdList(3, sprite_attribs);

	for (auto& shader : {shader_ad_unlit_, shader_unlit_})
	{
		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		shader->SetPixelConstantBufferSize(sizeof(float) * 4 * 6);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mflipbookParameter"), sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);

		{
			int32_t offset = 0;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.flipbookParameter"), 0 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.uvDistortionParameter"), sizeof(float) * 4 * 1 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.blendTextureParameter"), sizeof(float) * 4 * 2 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.emissiveScaling"), sizeof(float) * 4 * 3 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.edgeColor"), sizeof(float) * 4 * 4 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.edgeParameter"), sizeof(float) * 4 * 5 + offset);
		}

		applyPSAdvancedRendererParameterTexture(shader, 1);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_g_sampler"));
	}

	vao_unlit_ = VertexArray::Create(this, shader_unlit_, GetVertexBuffer(), GetIndexBuffer(), false);
	vao_ad_unlit_ = VertexArray::Create(this, shader_ad_unlit_, GetVertexBuffer(), GetIndexBuffer(), false);

	// Distortion
	static ShaderAttribInfo sprite_attribs_distortion_ad[10] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_UV", GL_FLOAT, 2, 16, false},
		{"Input_Binormal", GL_FLOAT, 3, 24, false},
		{"Input_Tangent", GL_FLOAT, 3, 36, false},

		{"Input_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 12, false},
		{"Input_BlendUV", GL_FLOAT, 2, sizeof(float) * 16, false},
		{"Input_Blend_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 18, false},
		{"Input_FlipbookIndex", GL_FLOAT, 1, sizeof(float) * 22, false},
		{"Input_AlphaThreshold", GL_FLOAT, 1, sizeof(float) * 23, false},
	};

	shader_ad_distortion_->GetAttribIdList(10, sprite_attribs_distortion_ad);

	static ShaderAttribInfo sprite_attribs_distortion[5] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_UV", GL_FLOAT, 2, 16, false},
		{"Input_Binormal", GL_FLOAT, 3, 24, false},
		{"Input_Tangent", GL_FLOAT, 3, 36, false},
	};

	shader_distortion_->GetAttribIdList(5, sprite_attribs_distortion);

	for (auto& shader : {shader_ad_distortion_, shader_distortion_})
	{
		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		shader->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.g_scale"), 0);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.mUVInversedBack"), sizeof(float) * 4);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_g_sampler"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_g_backSampler"));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("CBVS0.mflipbookParameter"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);

		{
			int32_t offset = sizeof(float) * 4 * 2;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.flipbookParameter"), 0 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.uvDistortionParameter"), sizeof(float) * 4 * 1 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.blendTextureParameter"), sizeof(float) * 4 * 2 + offset);
		}

		applyPSAdvancedRendererParameterTexture(shader, 2);
	}

	vao_ad_distortion_ = VertexArray::Create(this, shader_ad_distortion_, GetVertexBuffer(), GetIndexBuffer(), false);

	vao_distortion_ = VertexArray::Create(this, shader_distortion_, GetVertexBuffer(), GetIndexBuffer(), false);

	// Lit

	EffekseerRendererGL::ShaderAttribInfo sprite_attribs_lighting_ad[11] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_Normal", GL_UNSIGNED_BYTE, 4, 16, true},
		{"Input_Tangent", GL_UNSIGNED_BYTE, 4, 20, true},
		{"Input_UV1", GL_FLOAT, 2, 24, false},
		{"Input_UV2", GL_FLOAT, 2, 32, false},

		{"Input_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 10, false},
		{"Input_BlendUV", GL_FLOAT, 2, sizeof(float) * 14, false},
		{"Input_Blend_Alpha_Dist_UV", GL_FLOAT, 4, sizeof(float) * 16, false},
		{"Input_FlipbookIndex", GL_FLOAT, 1, sizeof(float) * 20, false},
		{"Input_AlphaThreshold", GL_FLOAT, 1, sizeof(float) * 21, false},
	};

	shader_ad_lit_->GetAttribIdList(11, sprite_attribs_lighting_ad);

	EffekseerRendererGL::ShaderAttribInfo sprite_attribs_lighting[6] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_Normal", GL_UNSIGNED_BYTE, 4, 16, true},
		{"Input_Tangent", GL_UNSIGNED_BYTE, 4, 20, true},
		{"Input_UV1", GL_FLOAT, 2, 24, false},
		{"Input_UV2", GL_FLOAT, 2, 32, false},
	};

	shader_lit_->GetAttribIdList(6, sprite_attribs_lighting);

	for (auto shader : {shader_ad_lit_, shader_lit_})
	{
		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		shader->SetPixelConstantBufferSize(sizeof(float) * 4 * 9);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightDirection"), sizeof(float[4]) * 0);
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightColor"), sizeof(float[4]) * 1);
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightAmbient"), sizeof(float[4]) * 2);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_g_colorSampler"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_g_normalSampler"));

		{
			int32_t offset = sizeof(float) * 4 * 3;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFlipbookParameter"), 0 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fUVDistortionParameter"), sizeof(float) * 4 * 1 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fBlendTextureParameter"), sizeof(float) * 4 * 2 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEmissiveScaling"), sizeof(float) * 4 * 3 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEdgeColor"), sizeof(float) * 4 * 4 + offset);
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEdgeParameter"), sizeof(float) * 4 * 5 + offset);
		}

		applyPSAdvancedRendererParameterTexture(shader, 2);
	}

	vao_ad_lit_ = VertexArray::Create(this, shader_ad_lit_, GetVertexBuffer(), GetIndexBuffer(), false);
	vao_lit_ = VertexArray::Create(this, shader_lit_, GetVertexBuffer(), GetIndexBuffer(), false);

	m_vao_wire_frame = VertexArray::Create(this, shader_unlit_, GetVertexBuffer(), m_indexBufferForWireframe, false);

	m_standardRenderer =
		new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(currentVAO);
	}

	GetImpl()->CreateProxyTextures(this);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glGenVertexArrays(1, &defaultVertexArray_);
	}

	// Transpiled shader for OpenGL 3.x is transposed
	if (GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		shader_unlit_->SetIsTransposeEnabled(true);
		shader_distortion_->SetIsTransposeEnabled(true);
		shader_lit_->SetIsTransposeEnabled(true);

		shader_ad_unlit_->SetIsTransposeEnabled(true);
		shader_ad_lit_->SetIsTransposeEnabled(true);
		shader_ad_distortion_->SetIsTransposeEnabled(true);
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destroy()
{
	Release();
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	m_restorationOfStates = flag;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	GLCheckError();

	impl->CalculateCameraProjectionMatrix();

	// store state
	if (m_restorationOfStates)
	{
		m_originalState.blend = glIsEnabled(GL_BLEND);
		m_originalState.cullFace = glIsEnabled(GL_CULL_FACE);
		m_originalState.depthTest = glIsEnabled(GL_DEPTH_TEST);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			m_originalState.texture = glIsEnabled(GL_TEXTURE_2D);
		}

		glGetBooleanv(GL_DEPTH_WRITEMASK, &m_originalState.depthWrite);
		glGetIntegerv(GL_DEPTH_FUNC, &m_originalState.depthFunc);
		glGetIntegerv(GL_CULL_FACE_MODE, &m_originalState.cullFaceMode);
		glGetIntegerv(GL_BLEND_SRC_RGB, &m_originalState.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &m_originalState.blendDst);
		glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_originalState.arrayBufferBinding);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_originalState.elementArrayBufferBinding);

		for (size_t i = 0; i < m_originalState.boundTextures.size(); i++)
		{
			GLint bound = 0;
			GLExt::glActiveTexture(GL_TEXTURE0 + i);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
			m_originalState.boundTextures[i] = bound;
		}

		if (GLExt::IsSupportedVertexArray())
		{
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_originalState.vao);
		}
	}

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	currentTextures_.clear();
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	m_renderState->GetActiveState().TextureIDs.fill(0);

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	GLCheckError();

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	// restore states
	if (m_restorationOfStates)
	{
		if (GLExt::IsSupportedVertexArray())
		{
			GLExt::glBindVertexArray(m_originalState.vao);
		}

		for (size_t i = 0; i < m_originalState.boundTextures.size(); i++)
		{
			GLExt::glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_originalState.boundTextures[i]);
		}
		GLExt::glActiveTexture(GL_TEXTURE0);

		if (m_originalState.blend)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		if (m_originalState.cullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (m_originalState.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			if (m_originalState.texture)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		}

		glDepthFunc(m_originalState.depthFunc);
		glDepthMask(m_originalState.depthWrite);
		glCullFace(m_originalState.cullFaceMode);
		glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
		GLExt::glBlendEquation(m_originalState.blendEquation);

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_originalState.arrayBufferBinding);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_originalState.elementArrayBufferBinding);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3)
		{
			for (int32_t i = 0; i < 4; i++)
			{
				GLExt::glBindSampler(i, 0);
			}
		}
	}

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	return m_indexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetSquareMaxCount(int32_t count)
{
	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	m_squareMaxCount = count;

	if (m_vertexBuffer != nullptr)
		AddRef();
	if (m_indexBuffer != nullptr)
		AddRef();
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);

	// generate a vertex buffer
	{
		m_vertexBuffer =
			VertexBuffer::Create(this, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true, false);
		if (m_vertexBuffer == NULL)
			return;
	}

	// generate an index buffer
	{
		m_indexBuffer = IndexBuffer::Create(this, GetIndexSpriteCount() * 6, false, indexBufferStride_, false);
		if (m_indexBuffer == nullptr)
			return;
	}

	// generate an index buffer for a wireframe
	{
		m_indexBufferForWireframe = IndexBuffer::Create(this, GetIndexSpriteCount() * 8, false, indexBufferStride_, false);
		if (m_indexBufferForWireframe == nullptr)
			return;
	}

	// generate index data
	GenerateIndexData();

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(fileInterface);
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(fileInterface, GetDeviceType());
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new MaterialLoader(GetGraphicsDevice(), fileInterface);
#else
	return nullptr;
#endif
}

void RendererImplemented::SetBackground(GLuint background, bool hasMipmap)
{
	m_background.UserID = background;
	m_background.HasMipmap = hasMipmap;
}

void RendererImplemented::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	m_background = *textureData;
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return m_distortingCallback;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(m_distortingCallback);
	m_distortingCallback = callback;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetInterface());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(GLuint vertexBuffer, int32_t size)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetIndexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetInterface());
		indexBufferCurrentStride_ = indexBuffer->GetStride();
	}
	else
	{
		indexBufferCurrentStride_ = m_currentVertexArray->GetIndexBuffer()->GetStride();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(GLuint indexBuffer)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetIndexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		indexBufferCurrentStride_ = 4;
	}
	else
	{
		indexBufferCurrentStride_ = m_currentVertexArray->GetIndexBuffer()->GetStride();
	}
}

void RendererImplemented::SetVertexBuffer(Effekseer::Backend::VertexBuffer* vertexBuffer, int32_t size)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer);
	SetVertexBuffer(vb->GetBuffer(), size);
}

void RendererImplemented::SetIndexBuffer(Effekseer::Backend::IndexBuffer* indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer);
	SetIndexBuffer(ib->GetBuffer());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexArray(VertexArray* vertexArray)
{
	m_currentVertexArray = vertexArray;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	GLCheckError();

	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		shader->EnableAttribs();
		shader->SetVertex();
		GLCheckError();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	GLsizei stride = GL_UNSIGNED_SHORT;
	if (indexBufferCurrentStride_ == 4)
	{
		stride = GL_UNSIGNED_INT;
	}

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		glDrawElements(GL_TRIANGLES, spriteCount * 6, stride, (void*)(vertexOffset / 4 * 6 * indexBufferCurrentStride_));
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		glDrawElements(GL_LINES, spriteCount * 8, stride, (void*)(vertexOffset / 4 * 8 * indexBufferCurrentStride_));
	}

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

	GLCheckError();
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

	GLExt::glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL, instanceCount);

	GLCheckError();
}

Shader* RendererImplemented::GetShader(::EffekseerRenderer::StandardRendererShaderType type) const
{
	if (type == ::EffekseerRenderer::StandardRendererShaderType::AdvancedBackDistortion)
	{
		return shader_ad_distortion_;
	}
	else if (type == ::EffekseerRenderer::StandardRendererShaderType::AdvancedLit)
	{
		return shader_ad_lit_;
	}
	else if (type == ::EffekseerRenderer::StandardRendererShaderType::AdvancedUnlit)
	{
		return shader_ad_unlit_;
	}
	else if (type == ::EffekseerRenderer::StandardRendererShaderType::BackDistortion)
	{
		return shader_distortion_;
	}
	else if (type == ::EffekseerRenderer::StandardRendererShaderType::Lit)
	{
		return shader_lit_;
	}
	else if (type == ::EffekseerRenderer::StandardRendererShaderType::Unlit)
	{
		return shader_unlit_;
	}

	return shader_unlit_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	// change VAO with shader
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		SetVertexArray(m_vao_wire_frame);
	}
	else if (shader == shader_unlit_)
	{
		SetVertexArray(vao_unlit_);
	}
	else if (shader == shader_distortion_)
	{
		SetVertexArray(vao_distortion_);
	}
	else if (shader == shader_lit_)
	{
		SetVertexArray(vao_lit_);
	}
	else if (shader == shader_ad_unlit_)
	{
		SetVertexArray(vao_ad_unlit_);
	}
	else if (shader == shader_ad_distortion_)
	{
		SetVertexArray(vao_ad_distortion_);
	}
	else if (shader == shader_ad_lit_)
	{
		SetVertexArray(vao_ad_lit_);
	}
	else if (m_currentVertexArray != nullptr)
	{
		SetVertexArray(m_currentVertexArray);
	}
	else
	{
		m_currentVertexArray = nullptr;

		if (defaultVertexArray_ > 0)
		{
			GLExt::glBindVertexArray(defaultVertexArray_);
		}
	}

	shader->BeginScene();

	if (m_currentVertexArray)
	{
		GLExt::glBindVertexArray(m_currentVertexArray->GetInterface());
	}

	assert(currentShader == nullptr);
	currentShader = shader;

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	assert(currentShader == shader);
	currentShader = nullptr;

	GLCheckError();

	if (m_currentVertexArray)
	{
		if (m_currentVertexArray->GetVertexBuffer() == nullptr)
		{
			shader->DisableAttribs();
			GLCheckError();

			GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			GLCheckError();

			GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
			GLCheckError();
		}

		GLExt::glBindVertexArray(0);
		GLCheckError();
		m_currentVertexArray = nullptr;
	}
	else
	{
		shader->DisableAttribs();
		GLCheckError();

		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GLCheckError();

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLCheckError();

		if (defaultVertexArray_ > 0)
		{
			GLExt::glBindVertexArray(0);
		}
	}

	shader->EndScene();
	GLCheckError();
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	GLCheckError();

	for (int i = count; i < currentTextures_.size(); i++)
	{
		m_renderState->GetActiveState().TextureIDs[i] = 0;
	}

	currentTextures_.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		GLuint id = 0;
		if (textures[i] != nullptr)
		{
			id = (GLuint)textures[i]->UserID;
		}

		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, id);

		if (textures[i] != nullptr)
		{
			m_renderState->GetActiveState().TextureIDs[i] = textures[i]->UserID;
			currentTextures_[i] = *textures[i];
		}
		else
		{
			currentTextures_[i].UserID = 0;
			currentTextures_[i].UserPtr = nullptr;
			m_renderState->GetActiveState().TextureIDs[i] = 0;
		}

		if (shader->GetTextureSlotEnable(i))
		{
			GLExt::glUniform1i(shader->GetTextureSlot(i), i);
		}
	}
	GLExt::glActiveTexture(GL_TEXTURE0);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

Effekseer::TextureData* RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{

	GLint bound = 0;
	GLExt::glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

	std::array<uint8_t, 4> buf;

	if (type == EffekseerRenderer::ProxyTextureType::White)
	{
		buf.fill(255);
	}
	else if (type == EffekseerRenderer::ProxyTextureType::Normal)
	{
		buf.fill(127);
		buf[2] = 255;
		buf[3] = 255;
	}
	else
	{
		assert(0);
	}

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());

	// Generate mipmap
	GLExt::glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bound);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = nullptr;
	textureData->UserID = texture;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 1;
	textureData->Height = 1;
	return textureData;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
	if (data != nullptr)
	{
		GLuint texture = (GLuint)data->UserID;
		glDeleteTextures(1, &texture);
		delete data;
	}
}

bool RendererImplemented::IsVertexArrayObjectSupported() const
{
	return GLExt::IsSupportedVertexArray();
}

} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
