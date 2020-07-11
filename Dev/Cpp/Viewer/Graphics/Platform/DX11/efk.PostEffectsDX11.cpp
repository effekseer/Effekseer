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

BlitterDX11::BlitterDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer)
	: graphics(graphics)
	, renderer_(static_cast<EffekseerRendererDX11::RendererImplemented*>(renderer))
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Generate vertex data
	vertexBuffer.reset(VertexBuffer::Create(renderer_, sizeof(Vertex) * 4, true));
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
					   int32_t numTextures,
					   ID3D11ShaderResourceView* const* textures,
					   const void* constantData,
					   size_t constantDataSize,
					   RenderTexture* dest,
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

	graphics->SetRenderTarget(dest, nullptr);
	renderer_->GetContext()->PSSetShaderResources(0, numTextures, textures);

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

BloomEffectDX11::BloomEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer)
	: BloomEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(static_cast<EffekseerRendererDX11::RendererImplemented*>(renderer))
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Extract shader
	shaderExtract.reset(Shader::Create(renderer_,
									   PostFX_Basic_VS::g_VS,
									   sizeof(PostFX_Basic_VS::g_VS),
									   PostFX_Extract_PS::g_PS,
									   sizeof(PostFX_Extract_PS::g_PS),
									   "Bloom extract",
									   PostFx_ShaderDecl,
									   2));

	if (shaderExtract != nullptr)
	{
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
		shaderExtract->SetPixelRegisterCount(2);
	}
	else
	{
		spdlog::trace("FAIL Create shaderExtract");
		isValid_ = false;
		return;
	}

	// Downsample shader
	shaderDownsample.reset(Shader::Create(renderer_,
										  PostFX_Basic_VS::g_VS,
										  sizeof(PostFX_Basic_VS::g_VS),
										  PostFX_Downsample_PS::g_PS,
										  sizeof(PostFX_Downsample_PS::g_PS),
										  "Bloom downsample",
										  PostFx_ShaderDecl,
										  2));

	// Blend shader
	shaderBlend.reset(Shader::Create(renderer_,
									 PostFX_Basic_VS::g_VS,
									 sizeof(PostFX_Basic_VS::g_VS),
									 PostFX_Blend_PS::g_PS,
									 sizeof(PostFX_Blend_PS::g_PS),
									 "Bloom blend",
									 PostFx_ShaderDecl,
									 2));

	// Blur(horizontal) shader
	shaderBlurH.reset(Shader::Create(renderer_,
									 PostFX_Basic_VS::g_VS,
									 sizeof(PostFX_Basic_VS::g_VS),
									 PostFX_BlurH_PS::g_PS,
									 sizeof(PostFX_BlurH_PS::g_PS),
									 "Bloom blurH",
									 PostFx_ShaderDecl,
									 2));

	// Blur(vertical) shader
	shaderBlurV.reset(Shader::Create(renderer_,
									 PostFX_Basic_VS::g_VS,
									 sizeof(PostFX_Basic_VS::g_VS),
									 PostFX_BlurV_PS::g_PS,
									 sizeof(PostFX_BlurV_PS::g_PS),
									 "Bloom blurV",
									 PostFx_ShaderDecl,
									 2));
}

BloomEffectDX11::~BloomEffectDX11()
{
}

void BloomEffectDX11::Render(RenderTexture* src, RenderTexture* dest)
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
		ID3D11ShaderResourceView* textures[1] = {(ID3D11ShaderResourceView*)src->GetViewID()};
		blitter.Blit(shaderExtract.get(), 1, textures, constantData, sizeof(constantData), extractBuffer.get());
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		ID3D11ShaderResourceView* textures[1];
		textures[0] = (i == 0) ? (ID3D11ShaderResourceView*)extractBuffer->GetViewID()
							   : (ID3D11ShaderResourceView*)lowresBuffers[0][i - 1]->GetViewID();
		blitter.Blit(shaderDownsample.get(), 1, textures, nullptr, 0, lowresBuffers[0][i].get());
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		ID3D11ShaderResourceView* textures[1] = {(ID3D11ShaderResourceView*)lowresBuffers[0][i]->GetViewID()};
		blitter.Blit(shaderBlurH.get(), 1, textures, nullptr, 0, lowresBuffers[1][i].get());
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		ID3D11ShaderResourceView* textures[1] = {(ID3D11ShaderResourceView*)lowresBuffers[1][i]->GetViewID()};
		blitter.Blit(shaderBlurV.get(), 1, textures, nullptr, 0, lowresBuffers[0][i].get());
	}

	// Blending pass
	{
		ID3D11ShaderResourceView* textures[4] = {(ID3D11ShaderResourceView*)lowresBuffers[0][0]->GetViewID(),
												 (ID3D11ShaderResourceView*)lowresBuffers[0][1]->GetViewID(),
												 (ID3D11ShaderResourceView*)lowresBuffers[0][2]->GetViewID(),
												 (ID3D11ShaderResourceView*)lowresBuffers[0][3]->GetViewID()};
		blitter.Blit(shaderBlend.get(), 4, textures, nullptr, 0, dest, AlphaBlendType::Add);
	}
}

void BloomEffectDX11::OnLostDevice()
{
	ReleaseBuffers();
}

void BloomEffectDX11::OnResetDevice()
{
}

void BloomEffectDX11::SetupBuffers(Effekseer::Tool::Vector2DI size)
{
	ReleaseBuffers();

	renderTextureSize_ = size;

	// Create high brightness extraction buffer
	{
		extractBuffer.reset(RenderTexture::Create(graphics));

		if (extractBuffer != nullptr)
		{
			extractBuffer->Initialize(size, TextureFormat::RGBA16F);
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
		auto bufferSize = size;
		for (int j = 0; j < BlurIterations; j++)
		{
			bufferSize.X = std::max(1, (bufferSize.X + 1) / 2);
			bufferSize.Y = std::max(1, (bufferSize.Y + 1) / 2);
			lowresBuffers[i][j].reset(RenderTexture::Create(graphics));

			if (lowresBuffers[i][j] != nullptr)
			{
				lowresBuffers[i][j]->Initialize(bufferSize, TextureFormat::RGBA16F);
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
	renderTextureSize_ = Effekseer::Tool::Vector2DI();

	extractBuffer.reset();
	for (int i = 0; i < BlurBuffers; i++)
	{
		for (int j = 0; j < BlurIterations; j++)
		{
			lowresBuffers[i][j].reset();
		}
	}
}

TonemapEffectDX11::TonemapEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer)
	: TonemapEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(static_cast<EffekseerRendererDX11::RendererImplemented*>(renderer))

{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Copy shader
	shaderCopy.reset(Shader::Create(renderer_,
									PostFX_Basic_VS::g_VS,
									sizeof(PostFX_Basic_VS::g_VS),
									PostFX_Copy_PS::g_PS,
									sizeof(PostFX_Copy_PS::g_PS),
									"Tonemap Copy",
									PostFx_ShaderDecl,
									2));

	// Reinhard shader
	shaderReinhard.reset(Shader::Create(renderer_,
										PostFX_Basic_VS::g_VS,
										sizeof(PostFX_Basic_VS::g_VS),
										PostFX_Tonemap_PS::g_PS,
										sizeof(PostFX_Tonemap_PS::g_PS),
										"Tonemap Reinhard",
										PostFx_ShaderDecl,
										2));

	if (shaderReinhard != nullptr)
	{
		shaderReinhard->SetPixelConstantBufferSize(sizeof(float) * 4);
		shaderReinhard->SetPixelRegisterCount(1);
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

void TonemapEffectDX11::Render(RenderTexture* src, RenderTexture* dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Tonemap pass
	ID3D11ShaderResourceView* textures[1] = {(ID3D11ShaderResourceView*)src->GetViewID()};

	if (algorithm == Algorithm::Off)
	{
		blitter.Blit(shaderCopy.get(), 1, textures, nullptr, 0, dest);
	}
	else if (algorithm == Algorithm::Reinhard)
	{
		const float constantData[4] = {exposure, 16.0f * 16.0f};
		blitter.Blit(shaderReinhard.get(), 1, textures, constantData, sizeof(constantData), dest);
	}
}

LinearToSRGBEffectDX11::LinearToSRGBEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer)
	: LinearToSRGBEffect(graphics)
	, blitter_(graphics, renderer)
	, renderer_(static_cast<EffekseerRendererDX11::RendererImplemented*>(renderer))

{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// Copy shader
	shader_.reset(Shader::Create(renderer_,
								 PostFX_Basic_VS::g_VS,
								 sizeof(PostFX_Basic_VS::g_VS),
								 PostFX_LinearToSRGB_PS::g_PS,
								 sizeof(PostFX_LinearToSRGB_PS::g_PS),
								 "LinearToSRGB",
								 PostFx_ShaderDecl,
								 2));

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

void LinearToSRGBEffectDX11::Render(RenderTexture* src, RenderTexture* dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;

	// LinearToSRGB pass
	ID3D11ShaderResourceView* textures[1] = {(ID3D11ShaderResourceView*)src->GetViewID()};

	blitter_.Blit(shader_.get(), 1, textures, nullptr, 0, dest);
}
} // namespace efk
