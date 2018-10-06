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

	BloomEffectDX11::BloomEffectDX11(Graphics* graphics)
		: BloomEffect(graphics)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Position(2) UV(2)
		const D3D11_INPUT_ELEMENT_DESC decl[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Extract shader
		shaderExtract.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Extract_PS::g_PS, sizeof(PostFX_Extract_PS::g_PS),
			"Bloom extract", decl, 2));
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
		shaderExtract->SetPixelRegisterCount(1);

		// Copy shader
		shaderCopy.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Copy_PS::g_PS, sizeof(PostFX_Copy_PS::g_PS),
			"Bloom copy", decl, 2));
		//shaderCopy->SetVertexRegisterCount(8);

		// Blend shader
		shaderBlend.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_Blend_PS::g_PS, sizeof(PostFX_Blend_PS::g_PS),
			"Bloom blend", decl, 2));
		//shaderBlend->SetVertexRegisterCount(8);

		// Blur(horizontal) shader
		shaderBlurH.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_BlurH_PS::g_PS, sizeof(PostFX_BlurH_PS::g_PS),
			"Bloom blurH", decl, 2));
		//shaderBlurH->SetVertexRegisterCount(8);

		// Blur(vertical) shader
		shaderBlurV.reset(Shader::Create(renderer,
			PostFX_Basic_VS::g_VS, sizeof(PostFX_Basic_VS::g_VS),
			PostFX_BlurV_PS::g_PS, sizeof(PostFX_BlurV_PS::g_PS),
			"Bloom blurV", decl, 2));
		//shaderBlurV->SetVertexRegisterCount(8);

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

	BloomEffectDX11::~BloomEffectDX11()
	{
		ES_SAFE_RELEASE( sampler );
	}

	void BloomEffectDX11::Render()
	{
		if( !enabled )
		{
			return;
		}

		using namespace Effekseer;
		using namespace EffekseerRendererDX11;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();
		auto renderTexture = graphics->GetRenderTexture();
		auto depthTexture = graphics->GetDepthTexture();

		if (renderTextureWidth  != renderTexture->GetWidth() || 
			renderTextureHeight != renderTexture->GetHeight())
		{
			SetupBuffers(renderTexture->GetWidth(), renderTexture->GetHeight());
		}

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = AlphaBlendType::Opacity;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		renderer->SetVertexBuffer(vertexBuffer.get(), sizeof(Vertex));
		renderer->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
		renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		renderer->GetContext()->PSSetSamplers(0, 1, &sampler);

		// Extract pass
		renderer->BeginShader(shaderExtract.get());
		renderer->GetContext()->IASetInputLayout(shaderExtract->GetLayoutInterface());
		{
			const float knee = threshold * (1.0f - softKnee);
			const float constantData[8] = {
				threshold, 
				threshold - knee, 
				knee * 2.0f, 
				0.25f / (knee + 0.00001f),
				intensity,
			};
			memcpy(shaderExtract->GetPixelConstantBuffer(), constantData, sizeof(constantData));
			shaderExtract->SetConstantBuffer();
			graphics->SetRenderTarget(extractBuffer.get(), nullptr);
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)renderTexture->GetViewID()};
			renderer->GetContext()->PSSetShaderResources(0, 1, textures);
			renderer->GetContext()->Draw(4, 0);
		}
		renderer->EndShader(shaderExtract.get());

		// Shrink pass
		renderer->BeginShader(shaderCopy.get());
		renderer->GetContext()->IASetInputLayout(shaderCopy->GetLayoutInterface());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[0][i].get(), nullptr);
			ID3D11ShaderResourceView* textures[1];
			textures[0] = (i == 0) ?
				(ID3D11ShaderResourceView*)extractBuffer->GetViewID() : 
				(ID3D11ShaderResourceView*)lowresBuffers[0][i - i]->GetViewID();
			renderer->GetContext()->PSSetShaderResources(0, 1, textures);
			renderer->GetContext()->Draw(4, 0);
		}
		renderer->EndShader(shaderCopy.get());

		// Horizontal gaussian blur pass
		renderer->BeginShader(shaderBlurH.get());
		renderer->GetContext()->IASetInputLayout(shaderBlurH->GetLayoutInterface());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[1][i].get(), nullptr);
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)lowresBuffers[0][i]->GetViewID()};
			renderer->GetContext()->PSSetShaderResources(0, 1, textures);
			renderer->GetContext()->Draw(4, 0);
		}
		renderer->EndShader(shaderBlurH.get());

		// Vertical gaussian blur pass
		renderer->BeginShader(shaderBlurV.get());
		renderer->GetContext()->IASetInputLayout(shaderBlurV->GetLayoutInterface());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[0][i].get(), nullptr);
			ID3D11ShaderResourceView* textures[1] = {
				(ID3D11ShaderResourceView*)lowresBuffers[1][i]->GetViewID()};
			renderer->GetContext()->PSSetShaderResources(0, 1, textures);
			renderer->GetContext()->Draw(4, 0);
		}
		renderer->EndShader(shaderBlurV.get());

		// Blending pass
		state.AlphaBlend = AlphaBlendType::Add;
		//state.AlphaBlend = AlphaBlendType::Opacity;
		renderer->GetRenderState()->Update(false);
		renderer->BeginShader(shaderBlend.get());
		renderer->GetContext()->IASetInputLayout(shaderBlend->GetLayoutInterface());
		{
			graphics->SetRenderTarget(renderTexture, depthTexture);
			ID3D11ShaderResourceView* textures[4] = {
				(ID3D11ShaderResourceView*)lowresBuffers[0][0]->GetViewID(),
				(ID3D11ShaderResourceView*)lowresBuffers[0][1]->GetViewID(),
				(ID3D11ShaderResourceView*)lowresBuffers[0][2]->GetViewID(),
				(ID3D11ShaderResourceView*)lowresBuffers[0][3]->GetViewID()};
			renderer->GetContext()->PSSetShaderResources(0, 4, textures);
			renderer->GetContext()->Draw(4, 0);
		}
		renderer->EndShader(shaderBlend.get());

		// Reset texture/sampler state
		{
			ID3D11ShaderResourceView* textures[4] = {};
			renderer->GetContext()->PSSetShaderResources(0, 4, textures);
			ID3D11SamplerState* samplers[1] = {};
			renderer->GetContext()->PSSetSamplers(0, 1, samplers);
		}

		renderer->GetRenderState()->Update(true);
		renderer->GetRenderState()->Pop();
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
}
