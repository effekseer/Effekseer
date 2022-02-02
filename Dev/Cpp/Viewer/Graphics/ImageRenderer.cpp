#include "ImageRenderer.h"

namespace
{
#ifdef _WIN32

using BYTE = uint8_t;

namespace VS
{
#include "../Shaders/HLSL_DX11_Header/image_vs.h"
}

namespace PS
{
#include "../Shaders/HLSL_DX11_Header/image_ps.h"
}
#endif

#include "../Shaders/GLSL_GL_Header/image_ps.h"
#include "../Shaders/GLSL_GL_Header/image_vs.h"

} // namespace

namespace Effekseer::Tool
{

std::shared_ptr<ImageRenderer> ImageRenderer::Create(RefPtr<Backend::GraphicsDevice> graphicsDevice)
{
	auto vb = graphicsDevice->CreateVertexBuffer(static_cast<int32_t>(sizeof(Vertex) * 4 * 128), nullptr, true);

	std::array<int32_t, 6 * 128> indexes;

	for (int32_t i = 0; i < 128; i++)
	{
		indexes[0 + i * 6] = 0 + i * 4;
		indexes[1 + i * 6] = 1 + i * 4;
		indexes[2 + i * 6] = 2 + i * 4;
		indexes[3 + i * 6] = 1 + i * 4;
		indexes[4 + i * 6] = 2 + i * 4;
		indexes[5 + i * 6] = 3 + i * 4;
	}

	auto ib = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(6 * 128), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	if (vb == nullptr || ib == nullptr)
	{
		return nullptr;
	}

	// shader
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, std::move(uniformLayoutElements));

	Effekseer::Backend::ShaderRef shader;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shader = graphicsDevice->CreateShaderFromBinary(VS::g_main, sizeof(VS::g_main), PS::g_main, sizeof(PS::g_main));
#endif
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes({{gl_image_vs}}, {{gl_image_ps}}, uniformLayout);
	}

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "Input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "Input_UV";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "TEXCOORD";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[2].Name = "Input_Color";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "NORMAL";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsDepthWriteEnabled = false;
	pipParam.IsBlendEnabled = true;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	auto ret = std::make_shared<ImageRenderer>();

	ret->vb_ = vb;
	ret->ib_ = ib;
	ret->graphicsDevice_ = graphicsDevice;
	ret->pip_ = pip;
	ret->shader_ = shader;
	ret->vertexLayout_ = vertexLayout;

	Backend::TextureParameter texParam;
	const int32_t textureSize = 16;
	texParam.Format = Backend::TextureFormatType::R8G8B8A8_UNORM;
	texParam.Size = {textureSize, textureSize};

	CustomVector<uint8_t> initialTexData;
	initialTexData.resize(textureSize * textureSize * 4);
	for (auto& v : initialTexData)
	{
		v = 255;
	}

	ret->dummyTexture_ = graphicsDevice->CreateTexture(texParam, initialTexData);

	return ret;
}

void ImageRenderer::Draw(const Effekseer::Vector3D positions[],
						 const Effekseer::Vector2D uvs[],
						 const Effekseer::Color colors[],
						 ::Effekseer::TextureRef texturePtr)
{
	Sprite s;

	for (int32_t i = 0; i < 4; i++)
	{
		s.Verteies[i].Pos = positions[i];
		s.Verteies[i].UV = uvs[i];
		s.Verteies[i].VColor = colors[i];
	}

	s.TexturePtr = texturePtr;

	sprites_.emplace_back(s);
}

void ImageRenderer::Render()
{
	if (sprites_.empty())
	{
		return;
	}

	for (size_t i = 0; i < sprites_.size(); i++)
	{
		const auto& sprite = sprites_[i];
		vb_->UpdateData(sprite.Verteies.data(), sizeof(Vertex) * 4, sizeof(Vertex) * 4 * i);

		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = 1;
		drawParam.TexturePtrs[0] = sprite.TexturePtr != nullptr ? sprite.TexturePtr->GetBackend() : dummyTexture_;
		drawParam.TextureSamplingTypes[0] = Backend::TextureSamplingType::Linear;
		drawParam.TextureWrapTypes[0] = Backend::TextureWrapType::Repeat;

		drawParam.VertexBufferPtr = vb_;
		drawParam.IndexBufferPtr = ib_;
		drawParam.PipelineStatePtr = pip_;
		drawParam.PrimitiveCount = 2;
		drawParam.InstanceCount = 1;
		drawParam.IndexOffset = i * 6;

		graphicsDevice_->Draw(drawParam);
	}
}

void ImageRenderer::ClearCache()
{
	sprites_.clear();
}

} // namespace Effekseer::Tool
