
#pragma once

#include <Effekseer.h>

namespace Effekseer
{
namespace Tool
{

struct RendererParameter
{
	Effekseer::Matrix44 ProjectionMatrix;
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 WorldMatrix;
	std::array<float, 4> DirectionalLightDirection;
	std::array<float, 4> DirectionalLightColor;
	std::array<float, 4> AmbientLightColor;
};

struct StaticMeshVertex
{
	std::array<float, 3> Pos;
	std::array<float, 2> UV;
	Effekseer::Color VColor;
	std::array<float, 3> Normal;
};

class StaticMesh
{
private:
	int32_t vertexCount_;
	int32_t indexCount_;
	Backend::VertexBufferRef vb_;
	Backend::IndexBufferRef ib_;

public:
	Backend::VertexBufferRef& GetVertexBuffer()
	{
		return vb_;
	}

	Backend::IndexBufferRef& GetIndexBuffer()
	{
		return ib_;
	}

	int32_t GetIndexCount() const
	{
		return indexCount_;
	}

	Backend::TextureRef Texture;
	bool IsLit = true;

	static std::shared_ptr<StaticMesh> Create(Effekseer::RefPtr<Backend::GraphicsDevice> graphicsDevice, Effekseer::CustomVector<StaticMeshVertex> vertexes, Effekseer::CustomVector<int32_t> indexes, bool isDyanmic = false);
};

class StaticMeshRenderer
{
private:
	struct UniformBufferVS
	{
		Effekseer::Matrix44 projectionMatrix;
		Effekseer::Matrix44 cameraMatrix;
		Effekseer::Matrix44 worldMatrix;
	};

	struct UniformBufferPS
	{
		std::array<float, 4> isLit;
		std::array<float, 4> directionalLightDirection;
		std::array<float, 4> directionalLightColor;
		std::array<float, 4> ambientLightColor;
	};

	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::UniformBufferRef uniformBufferVS_;
	Backend::UniformBufferRef uniformBufferPS_;
	Backend::UniformLayoutRef uniformLayout_;
	Backend::ShaderRef shader_;
	Backend::PipelineStateRef pip_;
	Backend::VertexLayoutRef vertexLayout_;
	Backend::TextureRef dummyTexture_;

	std::shared_ptr<StaticMesh> staticMesh_;

public:
	static std::shared_ptr<StaticMeshRenderer> Create(RefPtr<Backend::GraphicsDevice> graphicsDevice);
	StaticMeshRenderer();
	void Render(const RendererParameter& rendererParameter);

	void SetStaticMesh(const std::shared_ptr<StaticMesh>& mesh);

	std::shared_ptr<StaticMesh>& GetStaticMesh();
};

} // namespace Tool
} // namespace Effekseer