
#pragma once

#include <Effekseer.h>

namespace Effekseer::ToolRuntime
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
		Backend::TextureRef TexturePtr = nullptr;
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

	void DrawBackendTexture(const Effekseer::Vector3D positions[],
							const Effekseer::Vector2D uvs[],
							const Effekseer::Color colors[],
							Backend::TextureRef texturePtr);

	void Render();

	void ClearCache();

	bool GetIsValid() const;
};

} // namespace Effekseer::ToolRuntime
