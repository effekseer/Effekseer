
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

#ifdef __APPLE__
#import <TargetConditionals.h>
#endif

namespace EffekseerRendererGL
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(OpenGLDeviceType deviceType, bool isExtensionsEnabled)
{
	GLCheckError();
	auto g = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(deviceType, isExtensionsEnabled);
	if (!g->GetIsValid())
	{
		GLCheckError();
		return nullptr;
	}

	GLCheckError();
	return g;
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
	GLCheckError();
	auto device = CreateGraphicsDevice(deviceType, isExtensionsEnabled);
	if (device == nullptr)
	{
		return nullptr;
	}

	return Create(device, squareMaxCount);
}

RendererRef Renderer::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount)
{
	GLCheckError();
	if (graphicsDevice == nullptr)
	{
		return nullptr;
	}

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

void VertexArrayGroup::Create(
	Backend::GraphicsDeviceRef graphicsDevice,
	VertexBuffer* vertexBuffer,
	IndexBuffer* indexBuffer,
	IndexBuffer* indexBufferForWireframe,
	Shader* shader_unlit,
	Shader* shader_distortion,
	Shader* shader_lit,
	Shader* shader_ad_unlit,
	Shader* shader_ad_lit,
	Shader* shader_ad_distortion)
{
	vao_ad_distortion = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_distortion, vertexBuffer, indexBuffer));
	vao_distortion = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_distortion, vertexBuffer, indexBuffer));
	vao_ad_lit = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_lit, vertexBuffer, indexBuffer));
	vao_lit = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_lit, vertexBuffer, indexBuffer));
	vao_unlit = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_unlit, vertexBuffer, indexBuffer));
	vao_ad_unlit = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_unlit, vertexBuffer, indexBuffer));

	vao_unlit_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_unlit, vertexBuffer, indexBufferForWireframe));
	vao_lit_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_lit, vertexBuffer, indexBufferForWireframe));
	vao_distortion_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_distortion, vertexBuffer, indexBufferForWireframe));
	vao_ad_unlit_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_unlit, vertexBuffer, indexBufferForWireframe));
	vao_ad_lit_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_lit, vertexBuffer, indexBufferForWireframe));
	vao_ad_distortion_wire = std::unique_ptr<VertexArray>(VertexArray::Create(graphicsDevice, shader_ad_distortion, vertexBuffer, indexBufferForWireframe));
}

RendererImplemented::PlatformSetting RendererImplemented::GetPlatformSetting()
{
#if defined(EMSCRIPTEN) || defined(__ANDROID__) || (defined(__APPLE__) && (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR))
	return PlatformSetting{false, 1};
#endif
	return PlatformSetting{true, 3};
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount, Backend::GraphicsDeviceRef graphicsDevice)
	: m_indexBuffer(nullptr)
	, m_indexBufferForWireframe(nullptr)
	, m_squareMaxCount(squareMaxCount)
	, m_renderState(nullptr)
	, m_restorationOfStates(true)
	, m_currentVertexArray(nullptr)
	, m_standardRenderer(nullptr)
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

	ES_SAFE_DELETE(m_renderState);
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
	GLCheckError();
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

	GLCheckError();

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

	const auto texLocUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Unlit);
	const auto texLocLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
	const auto texLocDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::BackDistortion);
	const auto texLocAdUnlit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedUnlit);
	const auto texLocAdLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedLit);
	const auto texLocAdDist = GetTextureLocations(EffekseerRenderer::RendererShaderType::AdvancedBackDistortion);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
	AddVertexUniformLayout(uniformLayoutElementsLitUnlit);
	AddPixelUniformLayout(uniformLayoutElementsLitUnlit);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsDist;
	AddVertexUniformLayout(uniformLayoutElementsDist);
	AddDistortionPixelUniformLayout(uniformLayoutElementsDist);

	auto uniformLayoutUnlitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdUnlit, uniformLayoutElementsLitUnlit);
	auto shader_unlit_ad = graphicsDevice_->CreateShaderFromCodes({unlit_ad_vs}, {unlit_ad_ps}, uniformLayoutUnlitAd).DownCast<Backend::Shader>();

	shader_ad_unlit_ = Shader::Create(GetInternalGraphicsDevice(), shader_unlit_ad, "UnlitAd");
	if (shader_ad_unlit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile UnlitAd");
		return false;
	}

	auto uniformLayoutDistAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdDist, uniformLayoutElementsDist);
	auto shader_distortion_ad = graphicsDevice_->CreateShaderFromCodes({distortion_ad_vs}, {distortion_ad_ps}, uniformLayoutDistAd).DownCast<Backend::Shader>();

	shader_ad_distortion_ = Shader::Create(GetInternalGraphicsDevice(), shader_distortion_ad, "DistAd");
	if (shader_ad_distortion_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile DistAd");
		return false;
	}

	auto uniformLayoutLitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdLit, uniformLayoutElementsLitUnlit);
	auto shader_lit_ad = graphicsDevice_->CreateShaderFromCodes({lit_ad_vs}, {lit_ad_ps}, uniformLayoutLitAd).DownCast<Backend::Shader>();

	shader_ad_lit_ = Shader::Create(GetInternalGraphicsDevice(), shader_lit_ad, "LitAd");
	if (shader_ad_lit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile LitAd");
		return false;
	}

	auto uniformLayoutUnlit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsLitUnlit);
	auto shader_unlit = graphicsDevice_->CreateShaderFromCodes({unlit_vs}, {unlit_ps}, uniformLayoutUnlit).DownCast<Backend::Shader>();

	shader_unlit_ = Shader::Create(GetInternalGraphicsDevice(), shader_unlit, "Unlit");
	if (shader_unlit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Unlit");
		return false;
	}

	auto uniformLayoutDist = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocDist, uniformLayoutElementsDist);
	auto shader_distortion = graphicsDevice_->CreateShaderFromCodes({distortion_vs}, {distortion_ps}, uniformLayoutDist).DownCast<Backend::Shader>();

	shader_distortion_ = Shader::Create(GetInternalGraphicsDevice(), shader_distortion, "Dist");
	if (shader_distortion_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Dist");
		return false;
	}

	auto uniformLayoutLit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocLit, uniformLayoutElementsLitUnlit);
	auto shader_lit = graphicsDevice_->CreateShaderFromCodes({lit_vs}, {lit_ps}, uniformLayoutLit).DownCast<Backend::Shader>();

	shader_lit_ = Shader::Create(GetInternalGraphicsDevice(), shader_lit, "Lit");
	if (shader_lit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Lit");
		return false;
	}

	// Unlit
	auto vlUnlitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedUnlit).DownCast<Backend::VertexLayout>();
	shader_ad_unlit_->SetVertexLayout(vlUnlitAd);

	auto vlUnlit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Unlit).DownCast<Backend::VertexLayout>();
	shader_unlit_->SetVertexLayout(vlUnlit);

	for (auto& shader : {shader_ad_unlit_, shader_unlit_})
	{
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));
	}

	// Distortion
	auto vlLitDistAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedLit).DownCast<Backend::VertexLayout>();
	auto vlLitDist = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Lit).DownCast<Backend::VertexLayout>();

	shader_ad_distortion_->SetVertexLayout(vlLitDistAd);
	shader_distortion_->SetVertexLayout(vlLitDist);

	for (auto& shader : {shader_ad_distortion_, shader_distortion_})
	{
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));
	}

	// Lit
	shader_ad_lit_->SetVertexLayout(vlLitDistAd);
	shader_lit_->SetVertexLayout(vlLitDist);

	for (auto shader : {shader_ad_lit_, shader_lit_})
	{
		shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));
	}

	SetSquareMaxCount(m_squareMaxCount);

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

	GLCheckError();
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
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_originalState.blendSrcAlpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &m_originalState.blendDstAlpha);
		glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_originalState.arrayBufferBinding);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_originalState.elementArrayBufferBinding);
		glGetIntegerv(GL_CURRENT_PROGRAM, &m_originalState.program);

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
		GLExt::glBlendFuncSeparate(m_originalState.blendSrc, m_originalState.blendDst, m_originalState.blendSrcAlpha, m_originalState.blendDstAlpha);
		GLExt::glBlendEquation(m_originalState.blendEquation);

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_originalState.arrayBufferBinding);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_originalState.elementArrayBufferBinding);
		GLExt::glUseProgram(m_originalState.program);

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
	return ringVs_[GetImpl()->CurrentRingBufferIndex]->vertexBuffer.get();
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

	const auto setting = GetPlatformSetting();

	ES_SAFE_DELETE(m_indexBuffer);
	ringVs_.clear();
	GetImpl()->CurrentRingBufferIndex = 0;
	GetImpl()->RingBufferCount = setting.ringBufferCount;

	int vertexBufferSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4;

	auto storage = std::make_shared<SharedVertexTempStorage>();
	storage->buffer.resize(vertexBufferSize);

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

	for (int i = 0; i < GetImpl()->RingBufferCount; i++)
	{
		auto rv = std::make_shared<RingVertex>();
		rv->vertexBuffer = std::unique_ptr<VertexBuffer>(VertexBuffer::Create(
			graphicsDevice_,
			setting.isRingBufferEnabled,
			vertexBufferSize,
			true,
			storage));
		if (rv->vertexBuffer == nullptr)
		{
			return;
		}

		rv->vao = std::unique_ptr<VertexArrayGroup>(new VertexArrayGroup());
		rv->vao->Create(
			graphicsDevice_,
			rv->vertexBuffer.get(),
			m_indexBuffer,
			m_indexBufferForWireframe,
			shader_unlit_,
			shader_distortion_,
			shader_lit_,
			shader_ad_unlit_,
			shader_ad_lit_,
			shader_ad_distortion_);

		ringVs_.emplace_back(rv);
	}

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
	return ::Effekseer::MakeRefPtr<MaterialLoader>(GetInternalGraphicsDevice(), fileInterface);
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
		if (GetExternalShaderSettings() == nullptr)
		{
			shader_unlit_->OverrideShader(nullptr);
		}
		else
		{
			shader_unlit_->OverrideShader(GetExternalShaderSettings()->StandardShader);
		}

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

	auto& ringv = ringVs_[GetImpl()->CurrentRingBufferIndex];

	// change VAO with shader
	if (m_currentVertexArray != nullptr)
	{
		SetVertexArray(m_currentVertexArray);
	}
	else if (shader == shader_unlit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_unlit_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_unlit.get());
		}
	}
	else if (shader == shader_distortion_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_distortion_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_distortion.get());
		}
	}
	else if (shader == shader_lit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_lit_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_lit.get());
		}
	}
	else if (shader == shader_ad_unlit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_ad_unlit_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_ad_unlit.get());
		}
	}
	else if (shader == shader_ad_distortion_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_ad_distortion_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_ad_distortion.get());
		}
	}
	else if (shader == shader_ad_lit_)
	{
		if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			SetVertexArray(ringv->vao->vao_ad_lit_wire.get());
		}
		else
		{
			SetVertexArray(ringv->vao->vao_ad_lit.get());
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

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int vsOffset = 0;

	auto storeVector = [&](const char* name) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, 1, vsOffset});
		vsOffset += sizeof(float[4]);
	};

	auto storeMatrix = [&](const char* name) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, 1, vsOffset});
		vsOffset += sizeof(Effekseer::Matrix44);
	};

	storeMatrix("CBVS0.mCamera");
	storeMatrix("CBVS0.mCameraProj");
	storeVector("CBVS0.mUVInversed");
	storeVector("CBVS0.fFlipbookParameter");
}

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int psOffset = 0;

	auto storeVector = [&](const char* name) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Vector4, 1, psOffset});
		psOffset += sizeof(float[4]);
	};

	storeVector("CBPS0.fLightDirection");
	storeVector("CBPS0.fLightColor");
	storeVector("CBPS0.fLightAmbient");
	storeVector("CBPS0.fFlipbookParameter");
	storeVector("CBPS0.fUVDistortionParameter");
	storeVector("CBPS0.fBlendTextureParameter");
	storeVector("CBPS0.fCameraFrontDirection");
	storeVector("CBPS0.fFalloffParameter");
	storeVector("CBPS0.fFalloffBeginColor");
	storeVector("CBPS0.fFalloffEndColor");
	storeVector("CBPS0.fEmissiveScaling");
	storeVector("CBPS0.fEdgeColor");
	storeVector("CBPS0.fEdgeParameter");
	storeVector("CBPS0.softParticleParam");
	storeVector("CBPS0.reconstructionParam1");
	storeVector("CBPS0.reconstructionParam2");
	storeVector("CBPS0.mUVInversedBack");
}

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int psOffset = 0;

	auto storeVector = [&](const char* name) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Vector4, 1, psOffset});
		psOffset += sizeof(float[4]);
	};

	storeVector("CBPS0.g_scale");
	storeVector("CBPS0.mUVInversedBack");
	storeVector("CBPS0.fFlipbookParameter");
	storeVector("CBPS0.fUVDistortionParameter");
	storeVector("CBPS0.fBlendTextureParameter");
	storeVector("CBPS0.softParticleParam");
	storeVector("CBPS0.reconstructionParam1");
	storeVector("CBPS0.reconstructionParam2");
}

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type)
{
	Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

	auto pushColor = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) {
		texLoc.emplace_back("Sampler_sampler_colorTex");
	};

	auto pushDepth = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) {
		texLoc.emplace_back("Sampler_sampler_depthTex");
	};

	auto pushBack = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) {
		texLoc.emplace_back("Sampler_sampler_backTex");
	};

	auto pushNormal = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) {
		texLoc.emplace_back("Sampler_sampler_normalTex");
	};

	auto pushAdvancedRendererParameterLoc = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) -> void {
		texLoc.emplace_back("Sampler_sampler_alphaTex");
		texLoc.emplace_back("Sampler_sampler_uvDistortionTex");
		texLoc.emplace_back("Sampler_sampler_blendTex");
		texLoc.emplace_back("Sampler_sampler_blendAlphaTex");
		texLoc.emplace_back("Sampler_sampler_blendUVDistortionTex");
	};

	pushColor(texLoc);

	if (type == EffekseerRenderer::RendererShaderType::Lit)
	{
		pushNormal(texLoc);
	}
	else if (type == EffekseerRenderer::RendererShaderType::BackDistortion)
	{
		pushBack(texLoc);
	}
	else if (type == EffekseerRenderer::RendererShaderType::AdvancedUnlit)
	{
		pushAdvancedRendererParameterLoc(texLoc);
	}
	else if (type == EffekseerRenderer::RendererShaderType::AdvancedLit)
	{
		pushNormal(texLoc);
		pushAdvancedRendererParameterLoc(texLoc);
	}
	else if (type == EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		pushBack(texLoc);
		pushAdvancedRendererParameterLoc(texLoc);
	}
	pushDepth(texLoc);

	return texLoc;
}

} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
