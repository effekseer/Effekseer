
#include "efk.ImageRendererDX9.h"

namespace efk
{
	namespace Standard_VS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.Tool_VS.h>
	}

	namespace Standard_PS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.Tool_PS.h>
	}

	namespace StandardNoTexture_PS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.ToolNoTexture_PS.h>
	}

	ImageRendererDX9::ImageRendererDX9(EffekseerRenderer::Renderer* renderer)
		: ImageRenderer(renderer)
	{
		this->renderer = (EffekseerRendererDX9::RendererImplemented*)renderer;

		// Position(3) Color(1) UV(2)
		D3DVERTEXELEMENT9 decl[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		shader = EffekseerRendererDX9::Shader::Create(
			this->renderer,
			Standard_VS::g_vs20_VS,
			sizeof(Standard_VS::g_vs20_VS),
			Standard_PS::g_ps20_PS,
			sizeof(Standard_PS::g_ps20_PS),
			"StandardRenderer", decl);

		shader_no_texture = EffekseerRendererDX9::Shader::Create(
			this->renderer,
			Standard_VS::g_vs20_VS,
			sizeof(Standard_VS::g_vs20_VS),
			StandardNoTexture_PS::g_ps20_PS,
			sizeof(StandardNoTexture_PS::g_ps20_PS),
			"StandardRenderer No Texture",
			decl);

		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
		((EffekseerRendererDX9::Shader*)shader)->SetVertexRegisterCount(8);
		shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
		((EffekseerRendererDX9::Shader*)shader_no_texture)->SetVertexRegisterCount(8);

	}

	ImageRendererDX9::~ImageRendererDX9()
	{
		ES_SAFE_DELETE(shader);
		ES_SAFE_DELETE(shader_no_texture);
	}

	void ImageRendererDX9::Draw(const Effekseer::Vector3D positions[], const Effekseer::Vector2D uvs[], const Effekseer::Color colors[], void* texturePtr)
	{
		Sprite s;

		for (int32_t i = 0; i < 4; i++)
		{
			s.Verteies[i].Pos = positions[i];
			s.Verteies[i].UV[0] = uvs[i].X;
			s.Verteies[i].UV[1] = uvs[i].Y;
			s.Verteies[i].Col = D3DCOLOR_ARGB(colors[i].A, colors[i].R, colors[i].G, colors[i].B);
		}

		s.TexturePtr = texturePtr;

		sprites.push_back(s);
	}

	void ImageRendererDX9::Render()
	{
		if (sprites.size() == 0) return;

		for(int32_t i = 0; i < sprites.size(); i++)

		for (auto i = 0; i < sprites.size(); i++)
		{
			renderer->GetVertexBuffer()->Lock();

			auto verteies = (EffekseerRendererDX9::Vertex*)renderer->GetVertexBuffer()->GetBufferDirect(sizeof(EffekseerRendererDX9::Vertex) * 4);

			verteies[0] = sprites[i].Verteies[0];
			verteies[1] = sprites[i].Verteies[1];
			verteies[2] = sprites[i].Verteies[2];
			verteies[3] = sprites[i].Verteies[3];

			renderer->GetVertexBuffer()->Unlock();

			auto& state = renderer->GetRenderState()->Push();
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
			state.DepthWrite = false;
			state.DepthTest = false;
			state.CullingType = Effekseer::CullingType::Double;
			state.TextureFilterTypes[0] = Effekseer::TextureFilterType::Nearest;
			state.TextureWrapTypes[0] = Effekseer::TextureWrapType::Clamp;

			EffekseerRendererDX9::Shader* shader_ = nullptr;

			if (sprites[i].TexturePtr != nullptr)
			{
				shader_ = (EffekseerRendererDX9::Shader*)shader;
			}
			else
			{
				shader_ = (EffekseerRendererDX9::Shader*)shader_no_texture;
			}

			renderer->SetRenderMode(Effekseer::RenderMode::Normal);
			renderer->BeginShader(shader_);

			Effekseer::Matrix44 mats[2];
			mats[0].Indentity();
			mats[1].Indentity();

			memcpy(shader_->GetVertexConstantBuffer(), mats, sizeof(Effekseer::Matrix44) * 2);

			shader_->SetConstantBuffer();

			renderer->GetRenderState()->Update(true);

			renderer->SetLayout(shader_);
			renderer->GetDevice()->SetTexture(0, (IDirect3DTexture9*)sprites[i].TexturePtr);
			renderer->GetDevice()->SetStreamSource(0, renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(EffekseerRendererDX9::Vertex));
			renderer->GetDevice()->SetIndices(renderer->GetIndexBuffer()->GetInterface());
			renderer->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			renderer->EndShader(shader_);

			renderer->GetRenderState()->Pop();
		}
	}

	void ImageRendererDX9::ClearCache()
	{
		sprites.clear();
	}

	void ImageRendererDX9::OnLostDevice()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		shader_->OnLostDevice();

		shader_ = (EffekseerRendererDX9::Shader*)shader_no_texture;
		shader_->OnLostDevice();
	}

	void ImageRendererDX9::OnResetDevice()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		shader_->OnResetDevice();

		shader_ = (EffekseerRendererDX9::Shader*)shader_no_texture;
		shader_->OnLostDevice();
	}
}