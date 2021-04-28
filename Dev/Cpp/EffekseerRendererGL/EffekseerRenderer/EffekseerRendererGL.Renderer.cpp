
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RenderState.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.VertexBuffer.h"

#include "EffekseerRendererGL.GLExtension.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#include "../../EffekseerRendererCommon/ModelLoader.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/TextureLoader.h"
#endif

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"

#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_vs.h"

#include "GraphicsDevice.h"

namespace EffekseerRendererGL
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(OpenGLDeviceType deviceType, bool isExtensionsEnabled)
{
	return Effekseer::MakeRefPtr<Backend::GraphicsDevice>(deviceType, isExtensionsEnabled);
}

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface, ::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	auto gd = new Backend::GraphicsDevice(OpenGLDeviceType::OpenGL2);
	auto ret = ::Effekseer::TextureLoaderRef(new EffekseerRenderer::TextureLoader(gd, fileInterface));
	ES_SAFE_RELEASE(gd);
	return ret;
#else
	return nullptr;
#endif
}

::Effekseer::TextureLoaderRef CreateTextureLoader(
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
	::Effekseer::FileInterface* fileInterface,
	::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::TextureLoader>(graphicsDevice.Get(), fileInterface, colorSpaceType);
#else
	return nullptr;
#endif
}

::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface, OpenGLDeviceType deviceType)
{
	auto gd = ::Effekseer::MakeRefPtr<Backend::GraphicsDevice>(OpenGLDeviceType::OpenGL2);
	auto ret = ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(gd, fileInterface);
	return ret;
}

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
													::Effekseer::FileInterface* fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice.DownCast<Backend::GraphicsDevice>(), fileInterface);
}

Effekseer::Backend::TextureRef CreateTexture(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, GLuint buffer, bool hasMipmap, const std::function<void()>& onDisposed)
{
	auto gd = graphicsDevice.DownCast<Backend::GraphicsDevice>();
	return gd->CreateTexture(buffer, hasMipmap, onDisposed);
}

TextureProperty GetTextureProperty(::Effekseer::Backend::TextureRef texture)
{
	if (texture != nullptr)
	{
		auto t = texture.DownCast<Backend::Texture>();
		return TextureProperty{t->GetBuffer()};
	}
	else
	{
		return TextureProperty{};
	}
}

RendererRef Renderer::Create(int32_t squareMaxCount, OpenGLDeviceType deviceType, bool isExtensionsEnabled)
{
	return Create(CreateGraphicsDevice(deviceType, isExtensionsEnabled), squareMaxCount);
}

RendererRef Renderer::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount)
{
	auto g = graphicsDevice.DownCast<Backend::GraphicsDevice>();

	auto renderer = ::Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount, g);
	if (renderer->Initialize())
	{
		return renderer;
	}
	return nullptr;
}

int32_t RendererImplemented::GetIndexSpriteCount() const
{
	int vsSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4;

	size_t size = sizeof(EffekseerRenderer::SimpleVertex);
	size = (std::min)(size, sizeof(EffekseerRenderer::DynamicVertex));
	size = (std::min)(size, sizeof(EffekseerRenderer::LightingVertex));

	return (int32_t)(vsSize / size / 4 + 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount, Backend::GraphicsDeviceRef graphicsDevice)
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_indexBufferForWireframe(nullptr)
	, m_squareMaxCount(squareMaxCount)
	, m_renderState(nullptr)
	, m_restorationOfStates(true)
	, m_currentVertexArray(nullptr)
	, m_standardRenderer(nullptr)
	, vao_unlit_wire_(nullptr)
	, m_distortingCallback(nullptr)
	, m_deviceType(graphicsDevice->GetDeviceType())
{
	graphicsDevice_ = graphicsDevice;
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

	ES_SAFE_DELETE(vao_unlit_);
	ES_SAFE_DELETE(vao_distortion_);
	ES_SAFE_DELETE(vao_lit_);

	ES_SAFE_DELETE(vao_ad_unlit_);
	ES_SAFE_DELETE(vao_ad_lit_);
	ES_SAFE_DELETE(vao_ad_distortion_);

	ES_SAFE_DELETE(vao_unlit_wire_);
	ES_SAFE_DELETE(vao_lit_wire_);
	ES_SAFE_DELETE(vao_distortion_wire_);
	ES_SAFE_DELETE(vao_ad_unlit_wire_);
	ES_SAFE_DELETE(vao_ad_lit_wire_);
	ES_SAFE_DELETE(vao_ad_distortion_wire_);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	if (GLExt::IsSupportedVertexArray() && defaultVertexArray_ > 0)
	{
		GLExt::glDeleteVertexArrays(1, &defaultVertexArray_);
		defaultVertexArray_ = 0;
	}
}

void RendererImplemented::OnLostDevice()
{
	if (graphicsDevice_ != nullptr)
		graphicsDevice_->LostDevice();
}

void RendererImplemented::OnResetDevice()
{
	if (graphicsDevice_ != nullptr)
		graphicsDevice_->ResetDevice();

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

	ShaderCodeView unlit_ad_vs(get_ad_sprite_unlit_vs(GetDeviceType()));
	ShaderCodeView unlit_ad_ps(get_ad_model_unlit_ps(GetDeviceType()));
	ShaderCodeView distortion_ad_vs(get_ad_sprite_distortion_vs(GetDeviceType()));
	ShaderCodeView distortion_ad_ps(get_ad_model_distortion_ps(GetDeviceType()));
	ShaderCodeView lit_ad_vs(get_ad_sprite_lit_vs(GetDeviceType()));
	ShaderCodeView lit_ad_ps(get_ad_model_lit_ps(GetDeviceType()));

	ShaderCodeView unlit_vs(get_sprite_unlit_vs(GetDeviceType()));
	ShaderCodeView unlit_ps(get_model_unlit_ps(GetDeviceType()));
	ShaderCodeView distortion_vs(get_sprite_distortion_vs(GetDeviceType()));
	ShaderCodeView distortion_ps(get_model_distortion_ps(GetDeviceType()));
	ShaderCodeView lit_vs(get_sprite_lit_vs(GetDeviceType()));
	ShaderCodeView lit_ps(get_model_lit_ps(GetDeviceType()));

	shader_ad_unlit_ = Shader::Create(GetIntetnalGraphicsDevice(), &unlit_ad_vs, 1, &unlit_ad_ps, 1, "Standard Tex", false, false);
	if (shader_ad_unlit_ == nullptr)
		return false;

	shader_ad_distortion_ = Shader::Create(GetIntetnalGraphicsDevice(), &distortion_ad_vs, 1, &distortion_ad_ps, 1, "Standard Distortion Tex", false, false);
	if (shader_ad_distortion_ == nullptr)
		return false;

	shader_ad_lit_ = Shader::Create(GetIntetnalGraphicsDevice(), &lit_ad_vs, 1, &lit_ad_ps, 1, "Standard Lighting Tex", false, false);

	shader_unlit_ = Shader::Create(GetIntetnalGraphicsDevice(), &unlit_vs, 1, &unlit_ps, 1, "Standard Tex", false, false);
	if (shader_unlit_ == nullptr)
		return false;

	shader_distortion_ = Shader::Create(GetIntetnalGraphicsDevice(), &distortion_vs, 1, &distortion_ps, 1, "Standard Distortion Tex", false, false);
	if (shader_distortion_ == nullptr)
		return false;

	shader_lit_ = Shader::Create(GetIntetnalGraphicsDevice(), &lit_vs, 1, &lit_ps, 1, "Standard Lighting Tex", false, false);

	auto applyPSAdvancedRendererParameterTexture = [](Shader* shader, int32_t offset) -> void {
		shader->SetTextureSlot(0 + offset, shader->GetUniformId("Sampler_sampler_alphaTex"));
		shader->SetTextureSlot(1 + offset, shader->GetUniformId("Sampler_sampler_uvDistortionTex"));
		shader->SetTextureSlot(2 + offset, shader->GetUniformId("Sampler_sampler_blendTex"));
		shader->SetTextureSlot(3 + offset, shader->GetUniformId("Sampler_sampler_blendAlphaTex"));
		shader->SetTextureSlot(4 + offset, shader->GetUniformId("Sampler_sampler_blendUVDistortionTex"));
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
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.fFlipbookParameter"), sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCameraProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));

		AssignPixelConstantBuffer(shader);
	}

	applyPSAdvancedRendererParameterTexture(shader_ad_unlit_, 1);
	shader_unlit_->SetTextureSlot(1, shader_unlit_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_unlit_->SetTextureSlot(6, shader_ad_unlit_->GetUniformId("Sampler_sampler_depthTex"));

	vao_unlit_ = VertexArray::Create(graphicsDevice_, shader_unlit_, GetVertexBuffer(), GetIndexBuffer());
	vao_ad_unlit_ = VertexArray::Create(graphicsDevice_, shader_ad_unlit_, GetVertexBuffer(), GetIndexBuffer());

	// Distortion
	EffekseerRendererGL::ShaderAttribInfo sprite_attribs_normal_ad[11] = {
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

	EffekseerRendererGL::ShaderAttribInfo sprite_attribs_normal[6] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false},
		{"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true},
		{"Input_Normal", GL_UNSIGNED_BYTE, 4, 16, true},
		{"Input_Tangent", GL_UNSIGNED_BYTE, 4, 20, true},
		{"Input_UV1", GL_FLOAT, 2, 24, false},
		{"Input_UV2", GL_FLOAT, 2, 32, false},
	};

	shader_ad_distortion_->GetAttribIdList(11, sprite_attribs_normal_ad);

	shader_distortion_->GetAttribIdList(6, sprite_attribs_normal);

	for (auto& shader : {shader_ad_distortion_, shader_distortion_})
	{
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCameraProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_sampler_backTex"));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("CBVS0.fFlipbookParameter"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);

		AssignDistortionPixelConstantBuffer(shader);
	}

	applyPSAdvancedRendererParameterTexture(shader_ad_distortion_, 2);
	shader_distortion_->SetTextureSlot(2, shader_distortion_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_distortion_->SetTextureSlot(7, shader_ad_distortion_->GetUniformId("Sampler_sampler_depthTex"));

	vao_ad_distortion_ = VertexArray::Create(graphicsDevice_, shader_ad_distortion_, GetVertexBuffer(), GetIndexBuffer());

	vao_distortion_ = VertexArray::Create(graphicsDevice_, shader_distortion_, GetVertexBuffer(), GetIndexBuffer());

	// Lit

	shader_ad_lit_->GetAttribIdList(11, sprite_attribs_normal_ad);

	shader_lit_->GetAttribIdList(6, sprite_attribs_normal);

	for (auto shader : {shader_ad_lit_, shader_lit_})
	{
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCamera"), 0);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("CBVS0.mCameraProj"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBVS0.mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->SetTextureSlot(0, shader->GetUniformId("Sampler_sampler_colorTex"));
		shader->SetTextureSlot(1, shader->GetUniformId("Sampler_sampler_normalTex"));

		AssignPixelConstantBuffer(shader);
	}

	applyPSAdvancedRendererParameterTexture(shader_ad_lit_, 2);
	shader_lit_->SetTextureSlot(2, shader_lit_->GetUniformId("Sampler_sampler_depthTex"));
	shader_ad_lit_->SetTextureSlot(7, shader_ad_lit_->GetUniformId("Sampler_sampler_depthTex"));

	vao_ad_lit_ = VertexArray::Create(graphicsDevice_, shader_ad_lit_, GetVertexBuffer(), GetIndexBuffer());
	vao_lit_ = VertexArray::Create(graphicsDevice_, shader_lit_, GetVertexBuffer(), GetIndexBuffer());

	vao_unlit_wire_ = VertexArray::Create(graphicsDevice_, shader_unlit_, GetVertexBuffer(), m_indexBufferForWireframe);
	vao_lit_wire_ = VertexArray::Create(graphicsDevice_, shader_lit_, GetVertexBuffer(), m_indexBufferForWireframe);
	vao_distortion_wire_ = VertexArray::Create(graphicsDevice_, shader_distortion_, GetVertexBuffer(), m_indexBufferForWireframe);
	vao_ad_unlit_wire_ = VertexArray::Create(graphicsDevice_, shader_ad_unlit_, GetVertexBuffer(), m_indexBufferForWireframe);
	vao_ad_lit_wire_ = VertexArray::Create(graphicsDevice_, shader_ad_lit_, GetVertexBuffer(), m_indexBufferForWireframe);
	vao_ad_distortion_wire_ = VertexArray::Create(graphicsDevice_, shader_ad_distortion_, GetVertexBuffer(), m_indexBufferForWireframe);

	m_standardRenderer =
		new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
	GetImpl()->isSoftParticleEnabled = GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3;

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
			GLExt::glActiveTexture(GL_TEXTURE0 + (GLenum)i);
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
			GLExt::glActiveTexture(GL_TEXTURE0 + (GLenum)i);
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
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return m_indexBufferForWireframe;
	}
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
			VertexBuffer::Create(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true);
		if (m_vertexBuffer == nullptr)
			return;
	}

	// generate an index buffer
	{
		m_indexBuffer = IndexBuffer::Create(graphicsDevice_, GetIndexSpriteCount() * 6, false, indexBufferStride_);
		if (m_indexBuffer == nullptr)
			return;
	}

	// generate an index buffer for a wireframe
	{
		m_indexBufferForWireframe = IndexBuffer::Create(graphicsDevice_, GetIndexSpriteCount() * 8, false, indexBufferStride_);
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
::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::TextureLoader>(graphicsDevice_.Get(), fileInterface);
#else
	return nullptr;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
}

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(GetIntetnalGraphicsDevice(), fileInterface);
}

void RendererImplemented::SetBackground(GLuint background, bool hasMipmap)
{
	if (m_backgroundGL == nullptr)
	{
		m_backgroundGL = graphicsDevice_->CreateTexture(background, hasMipmap, nullptr);
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(m_backgroundGL.Get());
		texture->Init(background, hasMipmap, nullptr);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? m_backgroundGL : nullptr);
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

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());
	SetVertexBuffer(vb->GetBuffer(), size);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
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
		glDrawElements(GL_TRIANGLES, spriteCount * 6, stride, (void*)((size_t)vertexOffset / 4 * 6 * indexBufferCurrentStride_));
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		glDrawElements(GL_LINES, spriteCount * 8, stride, (void*)((size_t)vertexOffset / 4 * 8 * indexBufferCurrentStride_));
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

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	GLCheckError();
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		GLExt::glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		GLExt::glDrawElementsInstanced(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}
	GLCheckError();
}

Shader* RendererImplemented::GetShader(::EffekseerRenderer::RendererShaderType type) const
{
	if (type == ::EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		return shader_ad_distortion_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedLit)
	{
		return shader_ad_lit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedUnlit)
	{
		return shader_ad_unlit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::BackDistortion)
	{
		return shader_distortion_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Lit)
	{
		return shader_lit_;
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Unlit)
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
	if (m_currentVertexArray != nullptr)
	{
		SetVertexArray(m_currentVertexArray);
	}
	else if (shader == shader_unlit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_unlit_wire_);
		}
		else
		{
			SetVertexArray(vao_unlit_);
		}
	}
	else if (shader == shader_distortion_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_distortion_wire_);
		}
		else
		{
			SetVertexArray(vao_distortion_);
		}
	}
	else if (shader == shader_lit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_lit_wire_);
		}
		else
		{
			SetVertexArray(vao_lit_);
		}
	}
	else if (shader == shader_ad_unlit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_ad_unlit_wire_);
		}
		else
		{
			SetVertexArray(vao_ad_unlit_);
		}
	}
	else if (shader == shader_ad_distortion_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_ad_distortion_wire_);
		}
		else
		{
			SetVertexArray(vao_ad_distortion_);
		}
	}
	else if (shader == shader_ad_lit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(vao_ad_lit_wire_);
		}
		else
		{
			SetVertexArray(vao_ad_lit_);
		}
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

void RendererImplemented::SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count)
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
			auto texture = static_cast<Backend::Texture*>(textures[i].Get());
			id = texture->GetBuffer();
		}

		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, id);

		if (textures[i] != nullptr)
		{
			m_renderState->GetActiveState().TextureIDs[i] = id;
			currentTextures_[i] = textures[i];
		}
		else
		{
			m_renderState->GetActiveState().TextureIDs[i] = 0;
			currentTextures_[i].Reset();
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

bool RendererImplemented::IsVertexArrayObjectSupported() const
{
	return GLExt::IsSupportedVertexArray();
}

void AssignPixelConstantBuffer(Shader* shader)
{
	int psOffset = 0;
	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightDirection"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightColor"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fLightAmbient"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFlipbookParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fUVDistortionParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fBlendTextureParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fCameraFrontDirection"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFalloffParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFalloffBeginColor"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFalloffEndColor"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEmissiveScaling"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEdgeColor"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fEdgeParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.softParticleParam"), psOffset);
	psOffset += sizeof(float[4]) * 1;
	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.reconstructionParam1"), psOffset);
	psOffset += sizeof(float[4]) * 1;
	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.reconstructionParam2"), psOffset);
	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.mUVInversedBack"), psOffset);
	psOffset += sizeof(float[4]) * 1;
}

void AssignDistortionPixelConstantBuffer(Shader* shader)
{
	int psOffset = 0;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.g_scale"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.mUVInversedBack"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fFlipbookParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fUVDistortionParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.fBlendTextureParameter"), psOffset);

	psOffset += sizeof(float[4]) * 1;

	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.softParticleParam"), psOffset);
	psOffset += sizeof(float[4]) * 1;
	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.reconstructionParam1"), psOffset);
	psOffset += sizeof(float[4]) * 1;
	shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("CBPS0.reconstructionParam2"), psOffset);
	psOffset += sizeof(float[4]) * 1;
}

} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
