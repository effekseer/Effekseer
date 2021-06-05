#if _WIN32
#define NOMINMAX
#endif

#include "efk.PostEffectsDX11.h"
#include <algorithm>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace efk
{
namespace PostFX_Basic_VS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Basic_VS.h"
} // namespace PostFX_Basic_VS

namespace PostFX_Copy_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Copy_PS.h"
} // namespace PostFX_Copy_PS

namespace PostFX_Extract_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Extract_PS.h"
} // namespace PostFX_Extract_PS

namespace PostFX_Downsample_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Downsample_PS.h"
} // namespace PostFX_Downsample_PS

namespace PostFX_Blend_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Blend_PS.h"
} // namespace PostFX_Blend_PS

namespace PostFX_BlurH_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_BlurH_PS.h"
} // namespace PostFX_BlurH_PS

namespace PostFX_BlurV_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_BlurV_PS.h"
} // namespace PostFX_BlurV_PS

namespace PostFX_Tonemap_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_Tonemap_PS.h"
} // namespace PostFX_Tonemap_PS

namespace PostFX_LinearToSRGB_PS
{
static
#include "Shader/efk.GraphicsDX11.PostFX_LinearToSRGB_PS.h"
} // namespace PostFX_LinearToSRGB_PS

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
										   PostFX_Basic_VS::g_VS,
										   sizeof(PostFX_Basic_VS::g_VS),
										   PostFX_Extract_PS::g_PS,
										   sizeof(PostFX_Extract_PS::g_PS)),
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
											  PostFX_Basic_VS::g_VS,
											  sizeof(PostFX_Basic_VS::g_VS),
											  PostFX_Downsample_PS::g_PS,
											  sizeof(PostFX_Downsample_PS::g_PS)),
										  "Bloom downsample",
										  PostFx_ShaderDecl,
										  2,
										  false));

	// Blend shader
	shaderBlend.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										 PostFX_Basic_VS::g_VS,
										 sizeof(PostFX_Basic_VS::g_VS),
										 PostFX_Blend_PS::g_PS,
										 sizeof(PostFX_Blend_PS::g_PS)),
									 "Bloom blend",
									 PostFx_ShaderDecl,
									 2,
									 false));

	// Blur(horizontal) shader
	shaderBlurH.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										 PostFX_Basic_VS::g_VS,
										 sizeof(PostFX_Basic_VS::g_VS),
										 PostFX_BlurH_PS::g_PS,
										 sizeof(PostFX_BlurH_PS::g_PS)),
									 "Bloom blurH",
									 PostFx_ShaderDecl,
									 2,
									 false));

	// Blur(vertical) shader
	shaderBlurV.reset(Shader::Create(renderer_.Get(),
									 graphics->GetGraphicsDevice()
										 ->CreateShaderFromBinary(
											 PostFX_Basic_VS::g_VS,
											 sizeof(PostFX_Basic_VS::g_VS),
											 PostFX_BlurV_PS::g_PS,
											 sizeof(PostFX_BlurV_PS::g_PS)),
									 "Bloom blurV",
									 PostFx_ShaderDecl,
									 2,
									 false));
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

	if (renderTextureSize_ != src->GetSize())
	{
		SetupBuffers(src->GetSize());
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

		blitter.Blit(shaderExtract.get(), {src}, constantData, sizeof(constantData), extractBuffer->GetAsBackend());
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		auto textures = (i == 0) ? extractBuffer->GetAsBackend()
								 : lowresBuffers[0][i - 1]->GetAsBackend();

		blitter.Blit(shaderDownsample.get(), {textures}, nullptr, 0, lowresBuffers[0][i]->GetAsBackend());
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		blitter.Blit(shaderBlurH.get(), {lowresBuffers[0][i]->GetAsBackend()}, nullptr, 0, lowresBuffers[1][i]->GetAsBackend());
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		blitter.Blit(shaderBlurV.get(), {lowresBuffers[1][i]->GetAsBackend()}, nullptr, 0, lowresBuffers[0][i]->GetAsBackend());
	}

	// Blending pass
	{
		auto textures = {lowresBuffers[0][0]->GetAsBackend(),
						 lowresBuffers[0][1]->GetAsBackend(),
						 lowresBuffers[0][2]->GetAsBackend(),
						 lowresBuffers[0][3]->GetAsBackend()};
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
	ReleaseBuffers();

	renderTextureSize_ = size;

	// Create high brightness extraction buffer
	{
		extractBuffer.reset(RenderTexture::Create(graphics));

		if (extractBuffer != nullptr)
		{
			extractBuffer->Initialize(Effekseer::Tool::Vector2DI(size[0], size[1]), Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
		}
		else
		{
			spdlog::trace("FAIL Create extractBuffer");
			isValid_ = false;
			return;
		}
	}

	// Create low-resolution buffers
	for (int i = 0; i < BlurBuffers; i++)
	{
		auto bufferSize = Effekseer::Tool::Vector2DI(size[0], size[1]);
		for (int j = 0; j < BlurIterations; j++)
		{
			bufferSize.X = std::max(1, (bufferSize.X + 1) / 2);
			bufferSize.Y = std::max(1, (bufferSize.Y + 1) / 2);
			lowresBuffers[i][j].reset(RenderTexture::Create(graphics));

			if (lowresBuffers[i][j] != nullptr)
			{
				lowresBuffers[i][j]->Initialize(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
			}
			else
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

	extractBuffer.reset();
	for (int i = 0; i < BlurBuffers; i++)
	{
		for (int j = 0; j < BlurIterations; j++)
		{
			lowresBuffers[i][j].reset();
		}
	}
}

TonemapEffectDX11::TonemapEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: TonemapEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererDX11::RendererImplementedRef::FromPinned(renderer.Get()))

{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Copy shader
	shaderCopy.reset(Shader::Create(renderer_.Get(),
									graphics->GetGraphicsDevice()->CreateShaderFromBinary(
										PostFX_Basic_VS::g_VS,
										sizeof(PostFX_Basic_VS::g_VS),
										PostFX_Copy_PS::g_PS,
										sizeof(PostFX_Copy_PS::g_PS)),
									"Tonemap Copy",
									PostFx_ShaderDecl,
									2,
									false));

	// Reinhard shader
	shaderReinhard.reset(Shader::Create(renderer_.Get(),
										graphics->GetGraphicsDevice()->CreateShaderFromBinary(
											PostFX_Basic_VS::g_VS,
											sizeof(PostFX_Basic_VS::g_VS),
											PostFX_Tonemap_PS::g_PS,
											sizeof(PostFX_Tonemap_PS::g_PS)),
										"Tonemap Reinhard",
										PostFx_ShaderDecl,
										2,
										false));

	if (shaderReinhard != nullptr)
	{
		shaderReinhard->SetPixelConstantBufferSize(sizeof(float) * 4);
	}
	else
	{
		spdlog::trace("FAIL Create shaderReinhard");
		isValid_ = false;
		return;
	}
}

TonemapEffectDX11::~TonemapEffectDX11()
{
}

void TonemapEffectDX11::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Tonemap pass
	const auto textures = {src};

	if (algorithm == Algorithm::Off)
	{
		blitter.Blit(shaderCopy.get(), textures, nullptr, 0, dest);
	}
	else if (algorithm == Algorithm::Reinhard)
	{
		const float constantData[4] = {exposure, 16.0f * 16.0f};
		blitter.Blit(shaderReinhard.get(), textures, constantData, sizeof(constantData), dest);
	}
}

LinearToSRGBEffectDX11::LinearToSRGBEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: LinearToSRGBEffect(graphics)
	, blitter_(graphics, renderer)
	, renderer_(EffekseerRendererDX11::RendererImplementedRef::FromPinned(renderer.Get()))

{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Copy shader
	shader_.reset(Shader::Create(renderer_.Get(),
								 graphics->GetGraphicsDevice()->CreateShaderFromBinary(
									 PostFX_Basic_VS::g_VS,
									 sizeof(PostFX_Basic_VS::g_VS),
									 PostFX_LinearToSRGB_PS::g_PS,
									 sizeof(PostFX_LinearToSRGB_PS::g_PS)),
								 "LinearToSRGB",
								 PostFx_ShaderDecl,
								 2,
								 false));

	if (shader_ != nullptr)
	{
	}
	else
	{
		spdlog::trace("FAIL Create shaderLinearToSRGB");
		isValid_ = false;
		return;
	}
}

LinearToSRGBEffectDX11::~LinearToSRGBEffectDX11()
{
}

void LinearToSRGBEffectDX11::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// LinearToSRGB pass
	const auto textures = {src};

	blitter_.Blit(shader_.get(), textures, nullptr, 0, dest);
}
} // namespace efk
