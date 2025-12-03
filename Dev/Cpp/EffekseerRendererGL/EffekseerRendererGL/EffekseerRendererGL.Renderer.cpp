
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RenderState.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.GpuTimer.h"
#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include "EffekseerRendererGL.GLExtension.h"

#include <EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRendererCommon/ModelLoader.h>
#include <EffekseerRendererCommon/TextureLoader.h>

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

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
													::Effekseer::FileInterfaceRef fileInterface)
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

RendererRef Renderer::Create(int32_t squareMaxCount, OpenGLDeviceType deviceType, bool isExtensionsEnabled, bool isPremultipliedAlphaEnabled)
{
	GLCheckError();
	auto device = CreateGraphicsDevice(deviceType, isExtensionsEnabled);
	if (device == nullptr)
	{
		return nullptr;
	}

	return Create(device, squareMaxCount, isPremultipliedAlphaEnabled);
}

RendererRef Renderer::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount, bool isPremultipliedAlphaEnabled)
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
		renderer->GetImpl()->IsPremultipliedAlphaEnabled = isPremultipliedAlphaEnabled;
		return renderer;
	}
	return nullptr;
}

int32_t RendererImplemented::GetIndexSpriteCount() const
{
	int vsSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * squareMaxCount_ * 4;

	size_t size = sizeof(EffekseerRenderer::SimpleVertex);
	size = (std::min)(size, sizeof(EffekseerRenderer::DynamicVertex));
	size = (std::min)(size, sizeof(EffekseerRenderer::LightingVertex));

	return (int32_t)(vsSize / size / 4 + 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount, Backend::GraphicsDeviceRef graphicsDevice)
	: squareMaxCount_(squareMaxCount)
	, renderState_(nullptr)
	, restorationOfStates_(true)
	, standardRenderer_(nullptr)
	, distortingCallback_(nullptr)
	, deviceType_(graphicsDevice->GetDeviceType())
{
	graphicsDevice_ = graphicsDevice;
	graphicsDevice_->SetIsRestorationOfStatesRequired(false);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(distortingCallback_);
	ES_SAFE_DELETE(standardRenderer_);
	ES_SAFE_DELETE(renderState_);

	// NOTE : It is better to reset on a same context where Rendering method runs
	renderingVAO_.reset();
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

bool RendererImplemented::GenerateIndexData()
{
	if (indexBufferStride_ == Effekseer::Backend::IndexBufferStrideType::Stride2)
	{
		return EffekseerRenderer::GenerateIndexDataStride<int16_t>(graphicsDevice_, GetIndexSpriteCount(), indexBuffer_, indexBufferForWireframe_);
	}
	else if (indexBufferStride_ == Effekseer::Backend::IndexBufferStrideType::Stride4)
	{
		return EffekseerRenderer::GenerateIndexDataStride<int32_t>(graphicsDevice_, GetIndexSpriteCount(), indexBuffer_, indexBufferForWireframe_);
	}

	return false;
}

bool RendererImplemented::Initialize()
{
	GLCheckError();

	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	GLint vaoBinding = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vaoBinding);
	}

	std::unique_ptr<Backend::VertexArrayObject> initVAO;
	if (GLExt::IsSupportedVertexArray())
	{
		initVAO = std::make_unique<Backend::VertexArrayObject>();
		if (!initVAO->IsValid())
		{
			initVAO.reset();
		}

		if (initVAO != nullptr)
		{
			GLExt::glBindVertexArray(initVAO->GetVAO());
		}
	}

	if (GetIndexSpriteCount() * 4 > 65536)
	{
		indexBufferStride_ = Effekseer::Backend::IndexBufferStrideType::Stride4;
	}

	GLCheckError();

	renderState_ = new RenderState(this);

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
	const auto texLocLit = GetTextureLocations(EffekseerRenderer::RendererShaderType::Lit);
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

	auto shader_ad_unlit_ = Shader::Create(GetInternalGraphicsDevice(), shader_unlit_ad, "UnlitAd");
	if (shader_ad_unlit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile UnlitAd");
		return false;
	}
	GetImpl()->ShaderAdUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_unlit_);

	auto uniformLayoutDistAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdDist, uniformLayoutElementsDist);
	auto shader_distortion_ad = graphicsDevice_->CreateShaderFromCodes({distortion_ad_vs}, {distortion_ad_ps}, uniformLayoutDistAd).DownCast<Backend::Shader>();

	auto shader_ad_distortion_ = Shader::Create(GetInternalGraphicsDevice(), shader_distortion_ad, "DistAd");
	if (shader_ad_distortion_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile DistAd");
		return false;
	}
	GetImpl()->ShaderAdDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_distortion_);

	auto uniformLayoutLitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocAdLit, uniformLayoutElementsLitUnlit);
	auto shader_lit_ad = graphicsDevice_->CreateShaderFromCodes({lit_ad_vs}, {lit_ad_ps}, uniformLayoutLitAd).DownCast<Backend::Shader>();

	auto shader_ad_lit_ = Shader::Create(GetInternalGraphicsDevice(), shader_lit_ad, "LitAd");
	if (shader_ad_lit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile LitAd");
		return false;
	}
	GetImpl()->ShaderAdLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_lit_);

	auto uniformLayoutUnlit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsLitUnlit);
	auto shader_unlit = graphicsDevice_->CreateShaderFromCodes({unlit_vs}, {unlit_ps}, uniformLayoutUnlit).DownCast<Backend::Shader>();

	auto shader_unlit_ = Shader::Create(GetInternalGraphicsDevice(), shader_unlit, "Unlit");
	if (shader_unlit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Unlit");
		return false;
	}
	GetImpl()->ShaderUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_unlit_);

	auto uniformLayoutDist = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocDist, uniformLayoutElementsDist);
	auto shader_distortion = graphicsDevice_->CreateShaderFromCodes({distortion_vs}, {distortion_ps}, uniformLayoutDist).DownCast<Backend::Shader>();

	auto shader_distortion_ = Shader::Create(GetInternalGraphicsDevice(), shader_distortion, "Dist");
	if (shader_distortion_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Dist");
		return false;
	}
	GetImpl()->ShaderDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_distortion_);

	auto uniformLayoutLit = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocLit, uniformLayoutElementsLitUnlit);
	auto shader_lit = graphicsDevice_->CreateShaderFromCodes({lit_vs}, {lit_ps}, uniformLayoutLit).DownCast<Backend::Shader>();

	auto shader_lit_ = Shader::Create(GetInternalGraphicsDevice(), shader_lit, "Lit");
	if (shader_lit_ == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile Lit");
		return false;
	}
	GetImpl()->ShaderLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_lit_);

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

	SetSquareMaxCount(squareMaxCount_);

	standardRenderer_ =
		new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GetImpl()->isSoftParticleEnabled = GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3;

	GetImpl()->CreateProxyTextures(this);

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

	if (initVAO != nullptr)
	{
		initVAO.reset();
		GLExt::glBindVertexArray(0);
	}

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(vaoBinding);
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

	GLCheckError();
	return true;
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	restorationOfStates_ = flag;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	GLCheckError();

	impl->CalculateCameraProjectionMatrix();

	// store state
	if (restorationOfStates_)
	{
		originalState_.blend = glIsEnabled(GL_BLEND);
		originalState_.cullFace = glIsEnabled(GL_CULL_FACE);
		originalState_.depthTest = glIsEnabled(GL_DEPTH_TEST);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			originalState_.texture = glIsEnabled(GL_TEXTURE_2D);
		}

		glGetBooleanv(GL_DEPTH_WRITEMASK, &originalState_.depthWrite);
		glGetIntegerv(GL_DEPTH_FUNC, &originalState_.depthFunc);
		glGetIntegerv(GL_CULL_FACE_MODE, &originalState_.cullFaceMode);
		glGetIntegerv(GL_BLEND_SRC_RGB, &originalState_.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &originalState_.blendDst);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &originalState_.blendSrcAlpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &originalState_.blendDstAlpha);
		glGetIntegerv(GL_BLEND_EQUATION, &originalState_.blendEquation);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &originalState_.arrayBufferBinding);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &originalState_.elementArrayBufferBinding);
		glGetIntegerv(GL_CURRENT_PROGRAM, &originalState_.program);

		for (size_t i = 0; i < originalState_.boundTextures.size(); i++)
		{
			GLint bound = 0;
			GLExt::glActiveTexture(GL_TEXTURE0 + (GLenum)i);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
			originalState_.boundTextures[i] = bound;
		}

		if (GLExt::IsSupportedVertexArray())
		{
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &originalState_.vao);
		}
	}

	if (GLExt::IsSupportedVertexArray())
	{
		if (renderingVAO_ == nullptr)
		{
			renderingVAO_ = std::make_unique<EffekseerRendererGL::Backend::VertexArrayObject>();
		}
		GLExt::glBindVertexArray(renderingVAO_->GetVAO());
	}

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	currentTextures_.clear();
	renderState_->GetActiveState().Reset();
	renderState_->Update(true);

	renderState_->GetActiveState().TextureIDs.fill(0);

	// reset renderer
	standardRenderer_->ResetAndRenderingIfRequired();

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
	standardRenderer_->ResetAndRenderingIfRequired();

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(0);
	}

	// restore states
	if (restorationOfStates_)
	{
		if (GLExt::IsSupportedVertexArray())
		{
			GLExt::glBindVertexArray(originalState_.vao);
		}

		for (size_t i = 0; i < originalState_.boundTextures.size(); i++)
		{
			GLExt::glActiveTexture(GL_TEXTURE0 + (GLenum)i);
			glBindTexture(GL_TEXTURE_2D, originalState_.boundTextures[i]);
		}
		GLExt::glActiveTexture(GL_TEXTURE0);

		if (originalState_.blend)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		if (originalState_.cullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (originalState_.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			if (originalState_.texture)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		}

		glDepthFunc(originalState_.depthFunc);
		glDepthMask(originalState_.depthWrite);
		glCullFace(originalState_.cullFaceMode);
		GLExt::glBlendFuncSeparate(originalState_.blendSrc, originalState_.blendDst, originalState_.blendSrcAlpha, originalState_.blendDstAlpha);
		GLExt::glBlendEquation(originalState_.blendEquation);

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, originalState_.arrayBufferBinding);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, originalState_.elementArrayBufferBinding);
		GLExt::glUseProgram(originalState_.program);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3)
		{
			for (int32_t i = 0; i < (int32_t)GetCurrentTextures().size(); i++)
			{
				GLExt::glBindSampler(i, 0);
			}
		}
	}

	currentndexBuffer_ = nullptr;

	GLCheckError();

	return true;
}

Effekseer::Backend::IndexBufferRef RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return indexBufferForWireframe_;
	}
	return indexBuffer_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t RendererImplemented::GetSquareMaxCount() const
{
	return squareMaxCount_;
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

	squareMaxCount_ = count;

	int vertexBufferSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * squareMaxCount_ * 4;

	bool isSupportedBufferRange = GLExt::IsSupportedBufferRange();
#ifdef __ANDROID__
	isSupportedBufferRange = false;
#endif

	if (isSupportedBufferRange)
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(graphicsDevice_, vertexBufferSize, 3);
	}
	else
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferMultiSize>(graphicsDevice_, vertexBufferSize);
	}

	if (!GetImpl()->InternalVertexBuffer->GetIsValid())
	{
		GetImpl()->InternalVertexBuffer = nullptr;
		return;
	}

	// generate index data
	if (!GenerateIndexData())
	{
		return;
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return renderState_;
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
::Effekseer::GpuTimerRef RendererImplemented::CreateGpuTimer()
{
	return ::Effekseer::MakeRefPtr<GpuTimer>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::EffekseerRenderer::CreateTextureLoader(graphicsDevice_, fileInterface, ::Effekseer::ColorSpaceType::Gamma);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
}

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(GetInternalGraphicsDevice(), fileInterface);
}

void RendererImplemented::SetBackground(GLuint background, bool hasMipmap)
{
	if (backgroundGL_ == nullptr)
	{
		backgroundGL_ = graphicsDevice_->CreateTexture(background, hasMipmap, nullptr);
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(backgroundGL_.Get());
		texture->Init(background, hasMipmap, nullptr);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? backgroundGL_ : nullptr);
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return distortingCallback_;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(distortingCallback_);
	distortingCallback_ = callback;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(GLuint vertexBuffer, int32_t size)
{
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());
	SetVertexBuffer(vb->GetBuffer(), size);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
	currentndexBuffer_ = indexBuffer;
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->GetBuffer());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	GLCheckError();

	shader->EnableAttribs();
	GLCheckError();
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
	int32_t strideSize = 2;

	if (currentndexBuffer_->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride4)
	{
		stride = GL_UNSIGNED_INT;
		strideSize = 4;
	}

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		glDrawElements(GL_TRIANGLES, spriteCount * 6, stride, (void*)((size_t)vertexOffset / 4 * 6 * strideSize));
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		glDrawElements(GL_LINES, spriteCount * 8, stride, (void*)((size_t)vertexOffset / 4 * 8 * strideSize));
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

void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	shader->BeginScene();

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

	shader->DisableAttribs();
	GLCheckError();

	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLCheckError();

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLCheckError();

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
		renderState_->GetActiveState().TextureIDs[i] = 0;
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
			renderState_->GetActiveState().TextureIDs[i] = id;
			currentTextures_[i] = textures[i];
		}
		else
		{
			renderState_->GetActiveState().TextureIDs[i] = 0;
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
	renderState_->GetActiveState().Reset();
	renderState_->Update(true);
}

bool RendererImplemented::IsVertexArrayObjectSupported() const
{
	return GLExt::IsSupportedVertexArray();
}

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int vsOffset = 0;

	auto storeVector = [&](const char* name)
	{
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, 1, vsOffset});
		vsOffset += sizeof(float[4]);
	};

	auto storeMatrix = [&](const char* name)
	{
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, 1, vsOffset});
		vsOffset += sizeof(Effekseer::Matrix44);
	};

	storeMatrix("CBVS0.mCamera");
	storeMatrix("CBVS0.mCameraProj");
	storeVector("CBVS0.mUVInversed");
	storeVector("CBVS0.flipbookParameter1");
	storeVector("CBVS0.flipbookParameter2");
}

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int psOffset = 0;

	auto storeVector = [&](const char* name)
	{
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
	storeVector("CBPS0.miscFlags");
}

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	int psOffset = 0;

	auto storeVector = [&](const char* name)
	{
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
	storeVector("CBPS0.miscFlags");
}

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type)
{
	Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

	auto pushColor = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
	{
		texLoc.emplace_back("Sampler_sampler_colorTex");
	};

	auto pushDepth = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
	{
		texLoc.emplace_back("Sampler_sampler_depthTex");
	};

	auto pushBack = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
	{
		texLoc.emplace_back("Sampler_sampler_backTex");
	};

	auto pushNormal = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc)
	{
		texLoc.emplace_back("Sampler_sampler_normalTex");
	};

	auto pushAdvancedRendererParameterLoc = [](Effekseer::CustomVector<Effekseer::CustomString<char>>& texLoc) -> void
	{
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
