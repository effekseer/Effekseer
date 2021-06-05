
#include "efk.ImageRendererDX11.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace efk
{
namespace Standard_VS
{
static
#include "Shader/EffekseerRenderer.Tool_VS.h"
} // namespace Standard_VS

namespace Standard_PS
{
static
#include "Shader/EffekseerRenderer.Tool_PS.h"
} // namespace Standard_PS

namespace StandardNoTexture_PS
{
static
#include "Shader/EffekseerRenderer.ToolNoTexture_PS.h"
} // namespace StandardNoTexture_PS

ImageRendererDX11::ImageRendererDX11(const EffekseerRenderer::RendererRef& renderer)
	: ImageRenderer(renderer)
	, renderer(EffekseerRendererDX11::RendererImplementedRef::FromPinned(renderer.Get()))
{
	spdlog::trace("Begin new ImageRendererDX11");

	// Position(3) Color(1) UV(2)

	D3D11_INPUT_ELEMENT_DESC decl[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	shader = EffekseerRendererDX11::Shader::Create(this->renderer.Get(),
												   renderer->GetGraphicsDevice()->CreateShaderFromBinary(

													   Standard_VS::g_VS,
													   sizeof(Standard_VS::g_VS),
													   Standard_PS::g_PS,
													   sizeof(Standard_PS::g_PS)),
												   "StandardRenderer",
												   decl,
												   3,
												   false);

	shader_no_texture = EffekseerRendererDX11::Shader::Create(this->renderer.Get(),
															  renderer->GetGraphicsDevice()->CreateShaderFromBinary(
																  Standard_VS::g_VS,
																  sizeof(Standard_VS::g_VS),
																  StandardNoTexture_PS::g_PS,
																  sizeof(StandardNoTexture_PS::g_PS)),
															  "StandardRenderer No Texture",
															  decl,
															  3,
															  false);

	if (shader != nullptr)
	{
		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	}
	else
	{
		spdlog::trace("FAIL Create shader");
	}

	if (shader_no_texture != nullptr)
	{
		shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	}
	else
	{
		spdlog::trace("FAIL Create shader_no_texture");
	}

	spdlog::trace("End new ImageRendererDX11");
}

ImageRendererDX11::~ImageRendererDX11()
{
	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(shader_no_texture);
}

void ImageRendererDX11::Draw(const Effekseer::Vector3D positions[],
							 const Effekseer::Vector2D uvs[],
							 const Effekseer::Color colors[],
							 ::Effekseer::TextureRef texturePtr)
{
	Sprite s;

	for (int32_t i = 0; i < 4; i++)
	{
		s.Verteies[i].Pos = positions[i];
		s.Verteies[i].UV[0] = uvs[i].X;
		s.Verteies[i].UV[1] = uvs[i].Y;
		s.Verteies[i].SetColor(colors[i], false);
	}

	s.TexturePtr = texturePtr;

	sprites.push_back(s);
}

void ImageRendererDX11::Render()
{
	if (sprites.size() == 0)
		return;

	for (int32_t i = 0; i < sprites.size(); i++)

		for (auto i = 0; i < sprites.size(); i++)
		{
			renderer->GetVertexBuffer()->Lock();

			auto verteies =
				(EffekseerRendererDX11::Vertex*)renderer->GetVertexBuffer()->GetBufferDirect(sizeof(EffekseerRendererDX11::Vertex) * 4);

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

			EffekseerRendererDX11::Shader* shader_ = nullptr;

			if (sprites[i].TexturePtr != nullptr)
			{
				shader_ = (EffekseerRendererDX11::Shader*)shader;
			}
			else
			{
				shader_ = (EffekseerRendererDX11::Shader*)shader_no_texture;
			}

			renderer->SetRenderMode(Effekseer::RenderMode::Normal);
			renderer->BeginShader(shader_);

			Effekseer::Matrix44 mats[2];
			mats[0].Indentity();
			mats[1].Indentity();

			memcpy(shader_->GetVertexConstantBuffer(), mats, sizeof(Effekseer::Matrix44) * 2);

			shader_->SetConstantBuffer();

			if (sprites[i].TexturePtr != nullptr)
			{
				auto texture = sprites[i].TexturePtr->GetBackend();
				renderer->SetTextures(shader_, &texture, 1);
			}

			renderer->GetRenderState()->Update(true);

			renderer->SetLayout(shader_);

			// ID3D11ShaderResourceView* srv[1];
			// srv[0] = (ID3D11ShaderResourceView*)sprites[i].TexturePtr;
			// renderer->GetContext()->PSSetShaderResources(0, 1, srv);

			{
				ID3D11Buffer* vBuf = renderer->GetVertexBuffer()->GetInterface();
				uint32_t vertexSize = sizeof(EffekseerRendererDX11::Vertex);
				uint32_t offset = 0;
				renderer->GetContext()->IASetVertexBuffers(0, 1, &vBuf, &vertexSize, &offset);
			}

			renderer->GetContext()->IASetIndexBuffer(renderer->GetIndexBuffer()->GetInterface(), DXGI_FORMAT_R16_UINT, 0);

			renderer->GetContext()->DrawIndexed(2 * 3, 0, 0);

			renderer->EndShader(shader_);

			renderer->GetRenderState()->Pop();
		}
}

void ImageRendererDX11::ClearCache()
{
	sprites.clear();
}

void ImageRendererDX11::OnLostDevice()
{
	auto shader_ = (EffekseerRendererDX11::Shader*)shader;
	shader_->OnLostDevice();

	shader_ = (EffekseerRendererDX11::Shader*)shader_no_texture;
	shader_->OnLostDevice();
}

void ImageRendererDX11::OnResetDevice()
{
	auto shader_ = (EffekseerRendererDX11::Shader*)shader;
	shader_->OnResetDevice();

	shader_ = (EffekseerRendererDX11::Shader*)shader_no_texture;
	shader_->OnLostDevice();
}
} // namespace efk