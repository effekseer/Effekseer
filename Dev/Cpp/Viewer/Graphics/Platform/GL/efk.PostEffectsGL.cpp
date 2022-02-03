#include <algorithm>
#include <iostream>

#include "efk.PostEffectsGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>

namespace efk
{

namespace
{
#include "../../../Shaders/GLSL_GL_Header/postfx_basic_vs.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_blend_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_blur_h_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_blur_v_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_copy_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_downsample_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_extract_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_linear_to_srgb_ps.h"
#include "../../../Shaders/GLSL_GL_Header/postfx_tonemap_ps.h"
} // namespace

const Effekseer::Backend::VertexLayoutElement vlElem[2] = {
	{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_Pos", "POSITION", 0},
	{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
};

BlitterGL::BlitterGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: graphics(graphics)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto gd = this->renderer_->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	// Generate vertex data
	vertexBuffer.reset(VertexBuffer::Create(gd, true, sizeof(Vertex) * 4, true));

	vertexBuffer->Lock();
	{
		Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
		verteces[0] = Vertex{-1.0f, 1.0f, 0.0f, 0.0f};
		verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 1.0f};
		verteces[2] = Vertex{1.0f, 1.0f, 1.0f, 0.0f};
		verteces[3] = Vertex{1.0f, -1.0f, 1.0f, 1.0f};
	}
	vertexBuffer->Unlock();
}

BlitterGL::~BlitterGL()
{
}

std::unique_ptr<EffekseerRendererGL::VertexArray> BlitterGL::CreateVAO(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, EffekseerRendererGL::Shader* shader)
{
	using namespace EffekseerRendererGL;
	auto gd = graphicsDevice.DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	return std::unique_ptr<VertexArray>(VertexArray::Create(gd, shader, vertexBuffer.get(), renderer_->GetIndexBuffer()));
}

void BlitterGL::Blit(EffekseerRendererGL::Shader* shader,
					 EffekseerRendererGL::VertexArray* vao,
					 const std::vector<Effekseer::Backend::TextureRef>& textures,
					 const void* constantData,
					 size_t constantDataSize,
					 Effekseer::Backend::TextureRef dest,
					 bool isCleared)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	// Set GLStates
	renderer_->SetVertexArray(vao);
	renderer_->BeginShader(shader);

	// Set shader parameters
	if (constantData != nullptr)
	{
		memcpy(shader->GetPixelConstantBuffer(), constantData, constantDataSize);
		shader->SetConstantBuffer();
	}

	// Set destination texture
	graphics->SetRenderTarget({dest}, nullptr);

	if (isCleared)
	{
		graphics->Clear(Effekseer::Color(0, 0, 0, 0));
	}

	// Set source textures
	for (size_t slot = 0; slot < textures.size(); slot++)
	{
		auto texture = textures[slot];
		auto t = texture.DownCast<EffekseerRendererGL::Backend::Texture>();
		GLExt::glBindSampler(slot, 0);
		GLExt::glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, t->GetBuffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GLExt::glUniform1i(shader->GetTextureSlot(slot), slot);

		GLCheckError();
	}

	GLsizei stride = GL_UNSIGNED_SHORT;
	if (renderer_->GetIndexBuffer()->GetStride() == 4)
	{
		stride = GL_UNSIGNED_INT;
	}

	glDrawElements(GL_TRIANGLES, 6, stride, nullptr);
	GLCheckError();

	renderer_->EndShader(shader);
}

BloomEffectGL::BloomEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: BloomEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();

	EffekseerRendererGL::ShaderCodeView basicVS(gl_postfx_basic_vs);

	// Extract shader
	EffekseerRendererGL::ShaderCodeView extractPS(gl_postfx_extract_ps);

	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "CBPS0.g_filterParams", UniformBufferLayoutElementType::Vector4, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "CBPS0.g_intensity", UniformBufferLayoutElementType::Vector4, 1, sizeof(float) * 4});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, uniformLayoutElm);

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElmSingleImage;
	uniformLayoutElmSingleImage.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "CBPS0.g_size", UniformBufferLayoutElementType::Vector4, 1, 0});

	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
		Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"},
		uniformLayoutElmSingleImage);

	auto uniformLayoutImages = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
		Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler", "Sampler_g_sampler_1", "Sampler_g_sampler_2", "Sampler_g_sampler_3"},
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	shaderExtract.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {extractPS}, uniformLayout, "Bloom extract"));

	shaderExtract->SetVertexLayout(vl);
	shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);

	// Downsample shader

	EffekseerRendererGL::ShaderCodeView downSamplePS(gl_postfx_downsample_ps);

	shaderDownsample.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {downSamplePS}, uniformLayoutSingleImage, "Bloom downsample"));
	shaderDownsample->SetVertexLayout(vl);

	// Blend shader
	EffekseerRendererGL::ShaderCodeView blendPS(gl_postfx_blend_ps);

	shaderBlend.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {blendPS}, uniformLayoutImages, "Bloom blend"));
	shaderBlend->SetVertexLayout(vl);

	// Blur(horizontal) shader
	EffekseerRendererGL::ShaderCodeView blend_h_PS(gl_postfx_blur_h_ps);

	shaderBlurH.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {blend_h_PS}, uniformLayoutSingleImage, "Bloom blurH"));
	shaderBlurH->SetVertexLayout(vl);

	// Blur(vertical) shader
	EffekseerRendererGL::ShaderCodeView blend_v_PS(gl_postfx_blur_v_ps);

	shaderBlurV.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {blend_v_PS}, uniformLayoutSingleImage, "Bloom blurV"));
	shaderBlurV->SetVertexLayout(vl);

	// Setup VAOs
	vaoExtract = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderExtract.get());
	vaoDownsample = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderDownsample.get());
	vaoBlend = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlend.get());
	vaoBlurH = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlurH.get());
	vaoBlurV = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlurV.get());

	shaderDownsample->SetPixelConstantBufferSize(sizeof(float) * 4);
	shaderBlurH->SetPixelConstantBufferSize(sizeof(float) * 4);
	shaderBlurV->SetPixelConstantBufferSize(sizeof(float) * 4);
}

BloomEffectGL::~BloomEffectGL()
{
}

void BloomEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	if (!enabled)
	{
		return;
	}

	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	if (renderTextureWidth != src->GetParameter().Size[0] || renderTextureHeight != src->GetParameter().Size[1])
	{
		SetupBuffers(src->GetParameter().Size[0], src->GetParameter().Size[1]);
	}

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	state.TextureFilterTypes[0] = TextureFilterType::Linear;
	state.TextureWrapTypes[0] = TextureWrapType::Clamp;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

	// Extract pass
	{
		const float knee = threshold * softKnee;
		const float constantData[8] = {
			threshold,
			threshold - knee,
			knee * 2.0f,
			0.25f / (knee + 0.00001f),
			intensity,
		};
		blitter.Blit(shaderExtract.get(), vaoExtract.get(), std::vector<Effekseer::Backend::TextureRef>{src}, constantData, sizeof(constantData), extractBuffer);
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = (i == 0) ? extractBuffer
								: lowresBuffers[0][i - 1];

		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		blitter.Blit(shaderDownsample.get(), vaoDownsample.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[0][i]);
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[0][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		blitter.Blit(shaderBlurH.get(), vaoBlurH.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[1][i]);
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[1][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		blitter.Blit(shaderBlurV.get(), vaoBlurV.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[0][i]);
	}

	// Blending pass
	state.AlphaBlend = AlphaBlendType::Add;
	renderer_->GetRenderState()->Update(false);
	{
		const std::vector<Effekseer::Backend::TextureRef> textures{lowresBuffers[0][0],
																   lowresBuffers[0][1],
																   lowresBuffers[0][2],
																   lowresBuffers[0][3]};
		blitter.Blit(shaderBlend.get(), vaoBlend.get(), textures, nullptr, 0, dest, false);
	}

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
	GLCheckError();
}

void BloomEffectGL::OnLostDevice()
{
	ReleaseBuffers();
}

void BloomEffectGL::OnResetDevice()
{
}

void BloomEffectGL::SetupBuffers(int32_t width, int32_t height)
{
	const auto createRenderTexture = [&](Effekseer::Tool::Vector2I size, Effekseer::Backend::TextureFormatType format)
	{
		Effekseer::Backend::TextureParameter param;
		param.Format = format;
		param.Size[0] = size.X;
		param.Size[1] = size.Y;
		param.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;
		return graphics->GetGraphicsDevice()->CreateTexture(param);
	};

	ReleaseBuffers();

	renderTextureWidth = width;
	renderTextureHeight = height;

	// Create high brightness extraction buffer
	{
		auto bufferSize = Effekseer::Tool::Vector2I(width, height);
		extractBuffer = createRenderTexture(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
	}

	// Create low-resolution buffers
	for (int i = 0; i < BlurBuffers; i++)
	{
		auto bufferSize = Effekseer::Tool::Vector2I(width, height);
		for (int j = 0; j < BlurIterations; j++)
		{
			bufferSize.X = std::max(1, (bufferSize.X + 1) / 2);
			bufferSize.Y = std::max(1, (bufferSize.Y + 1) / 2);
			lowresBuffers[i][j] = createRenderTexture(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
		}
	}
}

void BloomEffectGL::ReleaseBuffers()
{
	renderTextureWidth = 0;
	renderTextureHeight = 0;

	extractBuffer.Reset();
	for (int i = 0; i < BlurBuffers; i++)
	{
		for (int j = 0; j < BlurIterations; j++)
		{
			lowresBuffers[i][j].Reset();
		}
	}
}

TonemapEffectGL::TonemapEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: TonemapEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	EffekseerRendererGL::ShaderCodeView basicVS(gl_postfx_basic_vs);

	// Copy shader
	EffekseerRendererGL::ShaderCodeView copyPS(gl_postfx_copy_ps);

	shaderCopy.reset(Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {copyPS}, uniformLayoutSingleImage, "Tonemap copy"));
	shaderCopy->SetVertexLayout(vl);

	// Reinhard shader
	EffekseerRendererGL::ShaderCodeView tonemapPS(gl_postfx_tonemap_ps);

	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "CBPS0.g_toneparams", UniformBufferLayoutElementType::Vector4, 1, 0});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, uniformLayoutElm);

	shaderReinhard.reset(EffekseerRendererGL::Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {tonemapPS}, uniformLayout, "Tonemap Reinhard"));
	shaderReinhard->SetVertexLayout(vl);
	shaderReinhard->SetPixelConstantBufferSize(sizeof(float) * 4);

	// Setup VAOs
	vaoCopy = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderCopy.get());
	vaoReinhard = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderReinhard.get());
}

TonemapEffectGL::~TonemapEffectGL()
{
}

void TonemapEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

	const std::vector<Effekseer::Backend::TextureRef> textures{src};

	if (algorithm == Algorithm::Off)
	{
		blitter.Blit(shaderCopy.get(), vaoCopy.get(), textures, nullptr, 0, dest);
	}
	else if (algorithm == Algorithm::Reinhard)
	{
		const float constantData[4] = {exposure, 16.0f * 16.0f};
		blitter.Blit(shaderReinhard.get(), vaoReinhard.get(), textures, constantData, sizeof(constantData), dest);
	}

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
	GLCheckError();
}

LinearToSRGBEffectGL::LinearToSRGBEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: LinearToSRGBEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	EffekseerRendererGL::ShaderCodeView basicVS(gl_postfx_basic_vs);
	EffekseerRendererGL::ShaderCodeView linierToSrgbPS(gl_postfx_linear_to_srgb_ps);

	// Copy shader
	shader_.reset(Shader::Create(renderer_->GetInternalGraphicsDevice(), {basicVS}, {linierToSrgbPS}, uniformLayoutSingleImage, "LinearToSRGB"));
	shader_->SetVertexLayout(vl);

	// Setup VAOs
	vao_ = blitter.CreateVAO(renderer->GetGraphicsDevice(), shader_.get());
}

LinearToSRGBEffectGL::~LinearToSRGBEffectGL()
{
}

void LinearToSRGBEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

	const std::vector<Effekseer::Backend::TextureRef> textures{src};

	blitter.Blit(shader_.get(), vao_.get(), textures, nullptr, 0, dest);

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
	GLCheckError();
}
} // namespace efk
