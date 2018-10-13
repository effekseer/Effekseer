#if _WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include "efk.PostEffectsDX11.h"

namespace efk
{
	namespace PostFX_Basic_VS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_Basic_VS.h"
	}

	namespace PostFX_Copy_PS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_Copy_PS.h"
	}

	namespace PostFX_Extract_PS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_Extract_PS.h"
	}

	namespace PostFX_Blend_PS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_Blend_PS.h"
	}

	namespace PostFX_BlurH_PS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_BlurH_PS.h"
	}

	namespace PostFX_BlurV_PS
	{
		static
#include "Shader/efk.GraphicsDX11.PostFX_BlurV_PS.h"
	}

	// Position(2) UV(2)
	static const D3D11_INPUT_ELEMENT_DESC PostFx_ShaderDecl[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	BlitterDX11::BlitterDX11(Graphics* graphics)
		: graphics(graphics)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Generate vertex data
		vertexBuffer.reset(VertexBuffer::Create(renderer, 
			sizeof(Vertex) * 4, true));
		vertexBuffer->Lock(); {
			Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
			verteces[0] = Vertex{-1.0f,  1.0f, 0.0f, 0.0f};
			verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 1.0f};
			verteces[2] = Vertex{ 1.0f,  1.0f, 1.0f, 0.0f};
			verteces[3] = Vertex{ 1.0f, -1.0f, 1.0f, 1.0f};
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
				{ 0.0f, 0.0f, 0.0f, 0.0f },
				0.0f,
				D3D11_FLOAT32_MAX, };

			renderer->GetDevice()->CreateSamplerState( &SamlerDesc, &sampler );
		}
	}

	BlitterDX11::~BlitterDX11()
	{
		ES_SAFE_RELEASE( sampler );
	}

	void BlitterDX11::Blit(EffekseerRendererDX11::Shader* shader, 
		int32_t numTextures, ID3D11ShaderResourceView* const* textures, 
		const void* constantData, size_t constantDataSize, RenderTexture* dest, 
		Effekseer::AlphaBlendType blendType)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = blendType;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		renderer->SetVertexBuffer(vertexBuffer.get(), sizeof(Vertex));
		renderer->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
		renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		renderer->GetContext()->PSSetSamplers(0, 1, &sampler);

		renderer->BeginShader(shader);
		renderer->GetContext()->IASetInputLayout(shader->GetLayoutInterface());
		
		if (constantData)
		{
			memcpy(shader->GetPixelConstantBuffer(), constantData, constantDataSize);
			shader->SetConstantBuffer();
		}

		graphics->SetRenderTarget(dest, nullptr);
		renderer->GetContext()->PSSetShaderResources(0, numTextures, textures);
		
		renderer->GetContext()->Draw(4, 0);
		renderer->EndShader(shader);

		// Reset texture/sampler state
		{
			ID3D11ShaderResourceView* srv[4] = {};
			renderer->GetContext()->PSSetShaderResources(0, 4, srv);
			ID3D11SamplerState* samplers[1] = {};
			renderer->GetContext()->PSSetSamplers(0, 1, samplers);
		}

		renderer->GetRenderState()->Update(true);
		renderer->GetRenderState()->Pop();
	}

	BloomEffectDX11::BloomEffectDX11(Graphics* graphics)
		: BloomEffect(graphics), blitter(graphics)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Extract shader
		shaderExtract.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Extract_PS::g_PS, sizeof(PostFX_Extract_PS::g_PS),
			"Bloom extract", PostFx_ShaderDecl, 2));
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
		shaderExtract->SetPixelRegisterCount(1);

		// Copy shader
		shaderCopy.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Copy_PS::g_PS, sizeof(PostFX_Copy_PS::g_PS),
			"Bloom copy", PostFx_ShaderDecl, 2));

		// Blend shader
		shaderBlend.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Blend_PS::g_PS, sizeof(PostFX_Blend_PS::g_PS),
			"Bloom blend", PostFx_ShaderDecl, 2));

		// Blur(horizontal) shader
		shaderBlurH.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_BlurH_PS::g_PS, sizeof(PostFX_BlurH_PS::g_PS),
			"Bloom blurH", PostFx_ShaderDecl, 2));

		// Blur(vertical) shader
		shaderBlurV.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_BlurV_PS::g_PS, sizeof(PostFX_BlurV_PS::g_PS),
			"Bloom blurV", PostFx_ShaderDecl, 2));
	}

	BloomEffectDX11::~BloomEffectDX11()
	{
	}

	void BloomEffectDX11::Render(RenderTexture* src, RenderTexture* dest)
	{
		if( !enabled )
		{
			return;
		}

		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		if (renderTextureWidth  != src->GetWidth() || 
			renderTextureHeight != src->GetHeight())
		{
			SetupBuffers(src->GetWidth(), src->GetHeight());
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
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)src->GetViewID()};
			blitter.Blit(shaderExtract.get(), 1, textures, constantData, sizeof(constantData), extractBuffer.get());
		}

		// Shrink pass
		for (int i = 0; i < BlurIterations; i++)
		{
			ID3D11ShaderResourceView* textures[1];
			textures[0] = (i == 0) ?
				(ID3D11ShaderResourceView*)extractBuffer->GetViewID() : 
				(ID3D11ShaderResourceView*)lowresBuffers[0][i - i]->GetViewID();
			blitter.Blit(shaderCopy.get(), 1, textures, nullptr, 0, lowresBuffers[0][i].get());
		}

		// Horizontal gaussian blur pass
		for (int i = 0; i < BlurIterations; i++)
		{
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)lowresBuffers[0][i]->GetViewID()};
			blitter.Blit(shaderBlurH.get(), 1, textures, nullptr, 0, lowresBuffers[1][i].get());
		}

		// Vertical gaussian blur pass
		for (int i = 0; i < BlurIterations; i++)
		{
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)lowresBuffers[1][i]->GetViewID()};
			blitter.Blit(shaderBlurV.get(), 1, textures, nullptr, 0, lowresBuffers[0][i].get());
		}
		
		// Blending pass
		{
			ID3D11ShaderResourceView* textures[4] = {
				(ID3D11ShaderResourceView*)lowresBuffers[0][0]->GetViewID(),
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

	void BloomEffectDX11::SetupBuffers(int32_t width, int32_t height)
	{
		ReleaseBuffers();

		renderTextureWidth = width;
		renderTextureHeight = height;

		const int32_t divides[BlurIterations] = {4, 8, 16, 32};
		
		// Create high brightness extraction buffer
		{
			int32_t bufferWidth  = std::max(1, width  / 2);
			int32_t bufferHeight = std::max(1, height / 2);
			extractBuffer.reset(RenderTexture::Create(graphics));
			extractBuffer->Initialize(bufferWidth, bufferHeight, TextureFormat::RGBA16F);
		}

		// Create low-resolution buffers
		for (int i = 0; i < BlurBuffers; i++) {
			for (int j = 0; j < BlurIterations; j++) {
				int32_t bufferWidth  = std::max(1, width  / divides[j]);
				int32_t bufferHeight = std::max(1, height / divides[j]);
				lowresBuffers[i][j].reset(RenderTexture::Create(graphics));
				lowresBuffers[i][j]->Initialize(bufferWidth, bufferHeight, TextureFormat::RGBA16F);
			}
		}
	}

	void BloomEffectDX11::ReleaseBuffers()
	{
		renderTextureWidth = 0;
		renderTextureHeight = 0;

		extractBuffer.reset();
		for (int i = 0; i < BlurBuffers; i++) {
			for (int j = 0; j < BlurIterations; j++) {
				lowresBuffers[i][j].reset();
			}
		}
	}

	TonemapEffectDX11::TonemapEffectDX11(Graphics* graphics)
		: TonemapEffect(graphics), blitter(graphics)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Copy shader
		shaderCopy.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Copy_PS::g_PS, sizeof(PostFX_Copy_PS::g_PS),
			"Tonemap copy", PostFx_ShaderDecl, 2));
	}

	TonemapEffectDX11::~TonemapEffectDX11()
	{
	}

	void TonemapEffectDX11::Render(RenderTexture* src, RenderTexture* dest)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Tonemap pass
		{
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)src->GetViewID()};
			blitter.Blit(shaderCopy.get(), 1, textures, nullptr, 0, dest);
		}
	}
}
