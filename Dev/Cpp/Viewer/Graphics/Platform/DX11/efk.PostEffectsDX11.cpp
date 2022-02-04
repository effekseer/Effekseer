#include "efk.PostEffectsDX11.h"
#include <algorithm>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace efk
{
namespace PostFX_Basic_VS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_basic_vs.h"
} // namespace PostFX_Basic_VS

namespace PostFX_Copy_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_copy_ps.h"
} // namespace PostFX_Copy_PS

namespace PostFX_Extract_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_extract_ps.h"
} // namespace PostFX_Extract_PS

namespace PostFX_Downsample_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_downsample_ps.h"
} // namespace PostFX_Downsample_PS

namespace PostFX_Blend_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_blend_ps.h"
} // namespace PostFX_Blend_PS

namespace PostFX_BlurH_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_blur_h_ps.h"
} // namespace PostFX_BlurH_PS

namespace PostFX_BlurV_PS
{
static
#include "../../../Shaders/HLSL_DX11_Header/postfx_blur_v_ps.h"
} // namespace PostFX_BlurV_PS

// Position(2) UV(2)
static const D3D11_INPUT_ELEMENT_DESC PostFx_ShaderDecl[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

BlitterDX11::BlitterDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: graphics(graphics)
	, renderer_(EffekseerRendererDX11::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Generate vertex data
	vertexBuffer.reset(VertexBuffer::Create(renderer_.Get(), sizeof(Vertex) * 4, true, false));
	vertexBuffer->Lock();
	{
		Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
		verteces[0] = Vertex{-1.0f, 1.0f, 0.0f, 0.0f};
		verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 1.0f};
		verteces[2] = Vertex{1.0f, 1.0f, 1.0f, 0.0f};
		verteces[3] = Vertex{1.0f, -1.0f, 1.0f, 1.0f};
	}
	vertexBuffer->Unlock();

	// Generate Sampler State
	{
		const D3D11_SAMPLER_DESC SamlerDesc = {
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			0.0f,
			0,
			D3D11_COMPARISON_ALWAYS,
			{0.0f, 0.0f, 0.0f, 0.0f},
			0.0f,
			D3D11_FLOAT32_MAX,
		};

		renderer_->GetDevice()->CreateSamplerState(&SamlerDesc, &sampler);
	}
}

BlitterDX11::~BlitterDX11()
{
	ES_SAFE_RELEASE(sampler);
}

void BlitterDX11::Blit(EffekseerRendererDX11::Shader* shader,
					   const std::vector<Effekseer::Backend::TextureRef>& textures,
					   const void* constantData,
					   size_t constantDataSize,
					   Effekseer::Backend::TextureRef dest,
					   Effekseer::AlphaBlendType blendType)
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = blendType;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	state.TextureFilterTypes[0] = TextureFilterType::Linear;
	state.TextureWrapTypes[0] = TextureWrapType::Clamp;
	renderer_->GetRenderState()->Update(true);
	renderer_->SetRenderMode(RenderMode::Normal);

	renderer_->SetVertexBuffer(vertexBuffer.get(), sizeof(Vertex));
	renderer_->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	renderer_->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer_->GetContext()->PSSetSamplers(0, 1, &sampler);

	renderer_->BeginShader(shader);
	renderer_->GetContext()->IASetInputLayout(shader->GetLayoutInterface());

	if (constantData)
	{
		memcpy(shader->GetPixelConstantBuffer(), constantData, constantDataSize);
		shader->SetConstantBuffer();
	}

	graphics->SetRenderTarget({dest}, nullptr);

	for (size_t slot = 0; slot < textures.size(); slot++)
	{
		auto texture = textures[slot];
		auto t = texture.DownCast<EffekseerRendererDX11::Backend::Texture>();
		auto srv = t->GetSRV();
		renderer_->GetContext()->PSSetShaderResources(slot, 1, &srv);
	}

	renderer_->GetContext()->Draw(4, 0);
	renderer_->EndShader(shader);

	// Reset texture/sampler state
	{
		ID3D11ShaderResourceView* srv[4] = {};
		renderer_->GetContext()->PSSetShaderResources(0, 4, srv);
		ID3D11SamplerState* samplers[1] = {};
		renderer_->GetContext()->PSSetSamplers(0, 1, samplers);
	}

	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
}

BloomEffectDX11::BloomEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: BloomEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererDX11::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Extract shader
	shaderExtract.reset(Shader::Create(renderer_.Get(),
									   graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										   PostFX_Basic_VS::g_main,
										   sizeof(PostFX_Basic_VS::g_main),
										   PostFX_Extract_PS::g_main,
										   sizeof(PostFX_Extract_PS::g_main)),
									   "Bloom extract",
									   PostFx_ShaderDecl,
									   2,
									   false));

	if (shaderExtract != nullptr)
	{
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
	}
	else
	{
		spdlog::trace("FAIL Create shaderExtract");
		isValid_ = false;
		return;
	}

	// Downsample shader
	shaderDownsample.reset(Shader::Create(renderer_.Get(),
										  graphics->GetGraphicsDevice()->CreateShaderFromBinary(
											  PostFX_Basic_VS::g_main,
											  sizeof(PostFX_Basic_VS::g_main),
											  PostFX_Downsample_PS::g_main,
											  sizeof(PostFX_Downsample_PS::g_main)),
										  "Bloom downsample",
										  PostFx_ShaderDecl,
										  2,
										  false));

	// Blend shader
	shaderBlend.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										 PostFX_Basic_VS::g_main,
										 sizeof(PostFX_Basic_VS::g_main),
										 PostFX_Blend_PS::g_main,
										 sizeof(PostFX_Blend_PS::g_main)),
									 "Bloom blend",
									 PostFx_ShaderDecl,
									 2,
									 false));

	// Blur(horizontal) shader
	shaderBlurH.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										 PostFX_Basic_VS::g_main,
										 sizeof(PostFX_Basic_VS::g_main),
										 PostFX_BlurH_PS::g_main,
										 sizeof(PostFX_BlurH_PS::g_main)),
									 "Bloom blurH",
									 PostFx_ShaderDecl,
									 2,
									 false));

	// Blur(vertical) shader
	shaderBlurV.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()
										 ->CreateShaderFromBinary(
											 PostFX_Basic_VS::g_main,
											 sizeof(PostFX_Basic_VS::g_main),
											 PostFX_BlurV_PS::g_main,
											 sizeof(PostFX_BlurV_PS::g_main)),
									 "Bloom blurV",
									 PostFx_ShaderDecl,
									 2,
									 false));

	shaderDownsample->SetPixelConstantBufferSize(sizeof(float) * 4);
	shaderBlurH->SetPixelConstantBufferSize(sizeof(float) * 4);
	shaderBlurV->SetPixelConstantBufferSize(sizeof(float) * 4);
}

BloomEffectDX11::~BloomEffectDX11()
{
}

void BloomEffectDX11::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	if (!enabled)
	{
		return;
	}

	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	const auto size = std::array<int, 2>{src->GetParameter().Size[0], src->GetParameter().Size[1]};

	if (renderTextureSize_ != size)
	{
		SetupBuffers(size);
	}

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

		blitter.Blit(shaderExtract.get(), {src}, constantData, sizeof(constantData), extractBuffer);
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = (i == 0) ? extractBuffer
								: lowresBuffers[0][i - 1];

		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];
		blitter.Blit(shaderDownsample.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[0][i]);
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[0][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];
		blitter.Blit(shaderBlurH.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[1][i]);
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto texture = lowresBuffers[1][i];
		std::array<float, 4> uniforms;
		uniforms[0] = texture->GetParameter().Size[0];
		uniforms[1] = texture->GetParameter().Size[1];
		blitter.Blit(shaderBlurV.get(), {texture}, uniforms.data(), sizeof(float) * 4, lowresBuffers[0][i]);
	}

	// Blending pass
	{
		auto textures = {lowresBuffers[0][0],
						 lowresBuffers[0][1],
						 lowresBuffers[0][2],
						 lowresBuffers[0][3]};
		blitter.Blit(shaderBlend.get(), textures, nullptr, 0, dest, AlphaBlendType::Add);
	}
}

void BloomEffectDX11::OnLostDevice()
{
	ReleaseBuffers();
}

void BloomEffectDX11::OnResetDevice()
{
}

void BloomEffectDX11::SetupBuffers(std::array<int32_t, 2> size)
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

	renderTextureSize_ = size;

	// Create high brightness extraction buffer
	{
		extractBuffer = createRenderTexture(Effekseer::Tool::Vector2I(size[0], size[1]), Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);

		if (extractBuffer == nullptr)
		{
			spdlog::trace("FAIL Create extractBuffer");
			isValid_ = false;
			return;
		}
	}

	// Create low-resolution buffers
	for (int i = 0; i < BlurBuffers; i++)
	{
		auto bufferSize = Effekseer::Tool::Vector2I(size[0], size[1]);
		for (int j = 0; j < BlurIterations; j++)
		{
			bufferSize.X = std::max(1, (bufferSize.X + 1) / 2);
			bufferSize.Y = std::max(1, (bufferSize.Y + 1) / 2);
			lowresBuffers[i][j] = createRenderTexture(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);

			if (lowresBuffers[i][j] == nullptr)
			{
				spdlog::trace("FAIL Create lowresBuffers[i][j]");
				isValid_ = false;
				return;
			}
		}
	}
}

void BloomEffectDX11::ReleaseBuffers()
{
	renderTextureSize_.fill(0);

	extractBuffer.Reset();
	for (int i = 0; i < BlurBuffers; i++)
	{
		for (int j = 0; j < BlurIterations; j++)
		{
			lowresBuffers[i][j].Reset();
		}
	}
}

} // namespace efk
