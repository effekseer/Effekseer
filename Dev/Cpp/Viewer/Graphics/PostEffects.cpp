#include "PostEffects.h"
#include "../Math/Vector2I.h"

namespace
{

#ifdef _WIN32

using BYTE = uint8_t;

namespace PostFX_Basic_VS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_basic_vs.h"
} // namespace PostFX_Basic_VS

namespace PostFX_LinearToSRGB_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_linear_to_srgb_ps.h"
} // namespace PostFX_LinearToSRGB_PS

namespace PostFX_Copy_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_copy_ps.h"
} // namespace PostFX_Copy_PS

namespace PostFX_Tonemap_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_tonemap_ps.h"
} // namespace PostFX_Tonemap_PS

namespace PostFX_Extract_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_extract_ps.h"
} // namespace PostFX_Extract_PS

namespace PostFX_Downsample_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_downsample_ps.h"
} // namespace PostFX_Downsample_PS

namespace PostFX_Blend_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_blend_ps.h"
} // namespace PostFX_Blend_PS

namespace PostFX_BlurH_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_blur_h_ps.h"
} // namespace PostFX_BlurH_PS

namespace PostFX_BlurV_PS
{
static
#include "../Shaders/HLSL_DX11_Header/postfx_blur_v_ps.h"
} // namespace PostFX_BlurV_PS

#endif

#include "../Shaders/GLSL_GL_Header/postfx_basic_vs.h"
#include "../Shaders/GLSL_GL_Header/postfx_blend_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_blur_h_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_blur_v_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_copy_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_downsample_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_extract_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_linear_to_srgb_ps.h"
#include "../Shaders/GLSL_GL_Header/postfx_tonemap_ps.h"

} // namespace

namespace Effekseer::Tool
{

BloomPostEffect::BloomPostEffect(Backend::GraphicsDeviceRef graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
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

	Effekseer::Backend::ShaderRef shaderExtract;
	Effekseer::Backend::ShaderRef shaderDownsample;
	Effekseer::Backend::ShaderRef shaderBlend;
	Effekseer::Backend::ShaderRef shaderBlurH;
	Effekseer::Backend::ShaderRef shaderBlurV;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shaderExtract = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_Extract_PS::g_main,
			sizeof(PostFX_Extract_PS::g_main));

		shaderDownsample = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_Downsample_PS::g_main,
			sizeof(PostFX_Downsample_PS::g_main));

		shaderBlend = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_Blend_PS::g_main,
			sizeof(PostFX_Blend_PS::g_main));

		// Blur(horizontal) shader
		shaderBlurH = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_BlurH_PS::g_main,
			sizeof(PostFX_BlurH_PS::g_main));

		// Blur(vertical) shader
		shaderBlurV = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_BlurV_PS::g_main,
			sizeof(PostFX_BlurV_PS::g_main));
#endif
	}
	else
	{
		shaderExtract = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_extract_ps},
			uniformLayout);

		shaderDownsample = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_downsample_ps},
			uniformLayoutSingleImage);

		shaderBlend = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_blend_ps},
			uniformLayoutImages);

		shaderBlurH = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_blur_h_ps},
			uniformLayoutSingleImage);

		shaderBlurV = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_blur_v_ps},
			uniformLayoutSingleImage);
	}

	if (shaderExtract != nullptr)
	{
		extract_ = std::make_unique<PostProcess>(graphicsDevice, shaderExtract, 0, sizeof(float) * 8);
	}

	downsample_ = std::make_unique<PostProcess>(graphicsDevice, shaderDownsample, 0, sizeof(float) * 4);
	blend_ = std::make_unique<PostProcess>(graphicsDevice, shaderBlend, 0, 0, PostProcessBlendType::Add);
	blurH_ = std::make_unique<PostProcess>(graphicsDevice, shaderBlurH, 0, sizeof(float) * 4);
	blurV_ = std::make_unique<PostProcess>(graphicsDevice, shaderBlurV, 0, sizeof(float) * 4);
}

void BloomPostEffect::Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src)
{
	if (!enabled_)
	{
		return;
	}

	if (renderPass_ == nullptr || currentRenderTarget_ != dst)
	{
		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures;
		textures.resize(1);
		textures.at(0) = dst;
		Effekseer::Backend::TextureRef depthTexture;
		renderPass_ = graphicsDevice_->CreateRenderPass(textures, depthTexture);
		currentRenderTarget_ = dst;
	}

	if (renderTextureWidth != src->GetParameter().Size[0] || renderTextureHeight != src->GetParameter().Size[1])
	{
		SetupBuffers(src->GetParameter().Size[0], src->GetParameter().Size[1]);
	}

	// Extract pass
	{
		const float knee = threshold_ * softKnee_;
		const std::array<float, 8> constantData = {
			threshold_,
			threshold_ - knee,
			knee * 2.0f,
			0.25f / (knee + 0.00001f),
			intensity_,
		};

		graphicsDevice_->BeginRenderPass(renderPassExtract_, true, false, {0, 0, 0, 0});

		extract_->GetDrawParameter().TextureCount = 1;
		extract_->GetDrawParameter().TexturePtrs[0] = src;

		auto uniformBuffer = extract_->GetUniformBufferPS();
		graphicsDevice_->UpdateUniformBuffer(uniformBuffer, sizeof(float) * 8, 0, constantData.data());

		extract_->Render();

		graphicsDevice_->EndRenderPass();
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = (i == 0) ? extractBuffer
								: lowresBuffers[0][i - 1];

		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		graphicsDevice_->BeginRenderPass(renderPassDownscales_[i], true, false, {0, 0, 0, 0});

		downsample_->GetDrawParameter().TextureCount = 1;
		downsample_->GetDrawParameter().TexturePtrs[0] = texture;

		auto uniformBuffer = downsample_->GetUniformBufferPS();
		graphicsDevice_->UpdateUniformBuffer(uniformBuffer, sizeof(float) * 4, 0, uniforms.data());

		downsample_->Render();

		graphicsDevice_->EndRenderPass();
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[0][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		graphicsDevice_->BeginRenderPass(renderPassBlurs_[1][i], true, false, {0, 0, 0, 0});

		blurH_->GetDrawParameter().TextureCount = 1;
		blurH_->GetDrawParameter().TexturePtrs[0] = texture;

		auto uniformBuffer = blurH_->GetUniformBufferPS();
		graphicsDevice_->UpdateUniformBuffer(uniformBuffer, sizeof(float) * 4, 0, uniforms.data());

		blurH_->Render();

		graphicsDevice_->EndRenderPass();
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[1][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];

		graphicsDevice_->BeginRenderPass(renderPassBlurs_[0][i], true, false, {0, 0, 0, 0});

		blurV_->GetDrawParameter().TextureCount = 1;
		blurV_->GetDrawParameter().TexturePtrs[0] = texture;

		auto uniformBuffer = blurV_->GetUniformBufferPS();
		graphicsDevice_->UpdateUniformBuffer(uniformBuffer, sizeof(float) * 4, 0, uniforms.data());

		blurV_->Render();

		graphicsDevice_->EndRenderPass();
	}

	// Blending pass
	{
		graphicsDevice_->BeginRenderPass(renderPass_, false, false, {0, 0, 0, 0});

		blend_->GetDrawParameter().TextureCount = 4;
		blend_->GetDrawParameter().TexturePtrs[0] = lowresBuffers[0][0];
		blend_->GetDrawParameter().TexturePtrs[1] = lowresBuffers[0][1];
		blend_->GetDrawParameter().TexturePtrs[2] = lowresBuffers[0][2];
		blend_->GetDrawParameter().TexturePtrs[3] = lowresBuffers[0][3];
		blend_->Render();

		graphicsDevice_->EndRenderPass();
	}
}

void BloomPostEffect::SetupBuffers(int32_t width, int32_t height)
{
	const auto createRenderTexture = [&](Effekseer::Tool::Vector2I size, Effekseer::Backend::TextureFormatType format) {
		Effekseer::Backend::TextureParameter param;
		param.Format = format;
		param.Size[0] = size.X;
		param.Size[1] = size.Y;
		param.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;
		return graphicsDevice_->CreateTexture(param);
	};

	const auto createRenderPass = [&](Effekseer::Backend::TextureRef texture) {
		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures;
		textures.resize(1);
		textures.at(0) = texture;
		Effekseer::Backend::TextureRef depthTexture;
		return graphicsDevice_->CreateRenderPass(textures, depthTexture);
	};

	ReleaseBuffers();

	renderTextureWidth = width;
	renderTextureHeight = height;

	// Create high brightness extraction buffer
	{
		auto bufferSize = Effekseer::Tool::Vector2I(width, height);
		extractBuffer = createRenderTexture(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
		renderPassExtract_ = createRenderPass(extractBuffer);
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

	for (int j = 0; j < BlurIterations; j++)
	{
		renderPassDownscales_[j] = createRenderPass(lowresBuffers[0][j]);

		renderPassBlurs_[0][j] = createRenderPass(lowresBuffers[0][j]);
		renderPassBlurs_[1][j] = createRenderPass(lowresBuffers[1][j]);
	}
}

void BloomPostEffect::ReleaseBuffers()
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

bool BloomPostEffect::GetIsValid() const
{
	return extract_ != nullptr && blend_ != nullptr && downsample_ != nullptr && blurH_ != nullptr && blurV_ != nullptr;
}

TonemapPostEffect::TonemapPostEffect(Backend::GraphicsDeviceRef graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	auto uniformLayoutCopy = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
		Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"},
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(Effekseer::Backend::UniformLayoutElement{Effekseer::Backend::ShaderStageType::Pixel, "CBPS0.g_toneparams", Effekseer::Backend::UniformBufferLayoutElementType::Vector4, 1, 0});
	auto uniformLayoutTone = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, uniformLayoutElm);

	Effekseer::Backend::ShaderRef shader_copy;
	Effekseer::Backend::ShaderRef shader_tone;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shader_copy = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_Copy_PS::g_main,
			sizeof(PostFX_Copy_PS::g_main));

		shader_tone = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_Tonemap_PS::g_main,
			sizeof(PostFX_Tonemap_PS::g_main));

#endif
	}
	else
	{
		shader_copy = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_copy_ps},
			uniformLayoutCopy);

		shader_tone = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_tonemap_ps},
			uniformLayoutTone);
	}

	if (shader_copy != nullptr)
	{
		postProcessCopy_ = std::make_unique<PostProcess>(graphicsDevice, shader_copy, 0, 0);
	}

	if (shader_tone != nullptr)
	{
		postProcessTone_ = std::make_unique<PostProcess>(graphicsDevice, shader_tone, 0, sizeof(float) * 4);
	}
}

void TonemapPostEffect::Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src)
{
	if (renderPass_ == nullptr || currentRenderTarget_ != dst)
	{
		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures;
		textures.resize(1);
		textures.at(0) = dst;
		Effekseer::Backend::TextureRef depthTexture;
		renderPass_ = graphicsDevice_->CreateRenderPass(textures, depthTexture);
		currentRenderTarget_ = dst;
	}

	graphicsDevice_->BeginRenderPass(renderPass_, true, false, {0, 0, 0, 0});

	if (algorithm_ == Algorithm::Off)
	{
		auto& drawParam = postProcessCopy_->GetDrawParameter();
		drawParam.TextureCount = 1;
		drawParam.TexturePtrs[0] = src;
		drawParam.TextureWrapTypes[0] = Backend::TextureWrapType::Clamp;

		postProcessCopy_->Render();
	}
	else if (algorithm_ == Algorithm::Reinhard)
	{
		auto& drawParam = postProcessTone_->GetDrawParameter();
		drawParam.TextureCount = 1;
		drawParam.TexturePtrs[0] = src;
		drawParam.TextureWrapTypes[0] = Backend::TextureWrapType::Clamp;

		const std::array<float, 4> constantData = {exposure_, 16.0f * 16.0f};

		auto uniformBuffer = postProcessTone_->GetUniformBufferPS();
		graphicsDevice_->UpdateUniformBuffer(uniformBuffer, sizeof(float) * 4, 0, constantData.data());

		postProcessTone_->Render();
	}

	graphicsDevice_->EndRenderPass();
}

bool TonemapPostEffect::GetIsValid() const
{
	return postProcessCopy_ != nullptr;
}

LinearToSRGBPostEffect::LinearToSRGBPostEffect(Backend::GraphicsDeviceRef graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
		Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"},
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	Effekseer::Backend::ShaderRef shader;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shader = graphicsDevice->CreateShaderFromBinary(
			PostFX_Basic_VS::g_main,
			sizeof(PostFX_Basic_VS::g_main),
			PostFX_LinearToSRGB_PS::g_main,
			sizeof(PostFX_LinearToSRGB_PS::g_main));
#endif
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes(
			{gl_postfx_basic_vs},
			{gl_postfx_linear_to_srgb_ps},
			uniformLayoutSingleImage);
	}

	if (shader != nullptr)
	{
		postProcess_ = std::make_unique<PostProcess>(graphicsDevice, shader, 0, 0);
	}
}

void LinearToSRGBPostEffect::Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src)
{
	if (renderPass_ == nullptr || currentRenderTarget_ != dst)
	{
		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures;
		textures.resize(1);
		textures.at(0) = dst;
		Effekseer::Backend::TextureRef depthTexture;
		renderPass_ = graphicsDevice_->CreateRenderPass(textures, depthTexture);
		currentRenderTarget_ = dst;
	}

	graphicsDevice_->BeginRenderPass(renderPass_, true, false, {0, 0, 0, 0});

	auto& drawParam = postProcess_->GetDrawParameter();

	drawParam.TextureCount = 1;
	drawParam.TexturePtrs[0] = src;
	drawParam.TextureWrapTypes[0] = Backend::TextureWrapType::Clamp;

	postProcess_->Render();

	graphicsDevice_->EndRenderPass();
}

bool LinearToSRGBPostEffect::GetIsValid() const
{
	return postProcess_ != nullptr;
}

} // namespace Effekseer::Tool
