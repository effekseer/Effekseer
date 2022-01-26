
#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

class ImageRenderer
{
	struct RendererParameter
	{
		Effekseer::Matrix44 ProjectionMatrix;
		Effekseer::Matrix44 CameraMatrix;
	};

	struct Vertex
	{
		Effekseer::Vector3D Pos;
		Effekseer::Vector2D UV;
		Effekseer::Color VColor;
	};

	struct Sprite
	{
		std::array<Vertex, 4> Verteies;
		::Effekseer::TextureRef TexturePtr = nullptr;
	};

	std::vector<Sprite> sprites_;

	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::UniformLayoutRef uniformLayout_;
	Backend::ShaderRef shader_;
	Backend::PipelineStateRef pip_;
	Backend::VertexLayoutRef vertexLayout_;
	Backend::TextureRef dummyTexture_;

	Backend::VertexBufferRef vb_;
	Backend::IndexBufferRef ib_;

public:
	static std::shared_ptr<ImageRenderer> Create(RefPtr<Backend::GraphicsDevice> graphicsDevice);

	void Draw(const Effekseer::Vector3D positions[],
			  const Effekseer::Vector2D uvs[],
			  const Effekseer::Color colors[],
			  ::Effekseer::TextureRef texturePtr);

	void Render();

	void ClearCache();
};

} // namespace Effekseer::Tool

/*

namespace efk
{
class ImageRendererDX11 : public ImageRenderer
{
private:
	struct Sprite
	{
		std::array<EffekseerRendererDX11::Vertex, 4> Verteies;
		::Effekseer::TextureRef TexturePtr = nullptr;
	};

	EffekseerRendererDX11::RendererImplementedRef renderer;
	EffekseerRenderer::ShaderBase* shader = nullptr;
	EffekseerRenderer::ShaderBase* shader_no_texture = nullptr;

	std::vector<Sprite> sprites;

public:
	ImageRendererDX11(const EffekseerRenderer::RendererRef& renderer);

	virtual ~ImageRendererDX11();

	void Draw(const Effekseer::Vector3D positions[],
			  const Effekseer::Vector2D uvs[],
			  const Effekseer::Color colors[],
			  ::Effekseer::TextureRef texturePtr) override;

	void Render() override;

	void ClearCache() override;

	void OnLostDevice() override;

	void OnResetDevice() override;
};
} // namespace efk
*/