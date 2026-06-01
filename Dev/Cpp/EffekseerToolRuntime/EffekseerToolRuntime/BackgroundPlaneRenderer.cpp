#include "BackgroundPlaneRenderer.h"

namespace Effekseer
{
namespace ToolRuntime
{

void CreateBackgroundPlaneMeshData(
	const BackgroundPlaneParameter& parameter,
	Effekseer::CustomVector<StaticMeshVertex>& vertexes,
	Effekseer::CustomVector<int32_t>& indexes)
{
	vertexes.resize(4);
	vertexes[0].Pos = {-1.0f, 1.0f, parameter.ClipDepth};
	vertexes[1].Pos = {1.0f, 1.0f, parameter.ClipDepth};
	vertexes[2].Pos = {1.0f, -1.0f, parameter.ClipDepth};
	vertexes[3].Pos = {-1.0f, -1.0f, parameter.ClipDepth};

	for (auto& vertex : vertexes)
	{
		vertex.UV[0] = (vertex.Pos[0] + 1.0f) / 2.0f;
		vertex.UV[1] = 1.0f - (vertex.Pos[1] + 1.0f) / 2.0f;
		vertex.Normal = {0.0f, 1.0f, 0.0f};
		vertex.VColor = parameter.Color;
	}

	indexes.resize(6);
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 0;
	indexes[4] = 2;
	indexes[5] = 3;
}

std::shared_ptr<StaticMesh> CreateBackgroundPlaneMesh(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	const BackgroundPlaneParameter& parameter)
{
	Effekseer::CustomVector<StaticMeshVertex> vertexes;
	Effekseer::CustomVector<int32_t> indexes;
	CreateBackgroundPlaneMeshData(parameter, vertexes, indexes);

	auto mesh = StaticMesh::Create(graphicsDevice, vertexes, indexes);
	if (mesh == nullptr)
	{
		return nullptr;
	}

	mesh->IsLit = false;
	return mesh;
}

bool BackgroundPlaneRenderer::Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	graphicsDevice_ = graphicsDevice;
	meshRenderer_ = StaticMeshRenderer::Create(graphicsDevice_);
	return meshRenderer_ != nullptr;
}

std::shared_ptr<BackgroundPlaneRenderer> BackgroundPlaneRenderer::Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	auto ret = std::make_shared<BackgroundPlaneRenderer>();
	if (ret->Initialize(graphicsDevice))
	{
		return ret;
	}
	return nullptr;
}

bool BackgroundPlaneRenderer::UpdateMesh(const Effekseer::Color& color)
{
	if (mesh_ != nullptr && color == meshColor_)
	{
		return true;
	}

	BackgroundPlaneParameter parameter;
	parameter.Color = color;
	mesh_ = CreateBackgroundPlaneMesh(graphicsDevice_, parameter);
	if (mesh_ == nullptr)
	{
		return false;
	}

	meshColor_ = color;
	mesh_->Texture = texture_;
	meshRenderer_->SetStaticMesh(mesh_);
	return true;
}

void BackgroundPlaneRenderer::SetTexture(Effekseer::Backend::TextureRef texture)
{
	texture_ = texture;
	if (mesh_ != nullptr)
	{
		mesh_->Texture = texture_;
	}
}

void BackgroundPlaneRenderer::Render()
{
	if (meshRenderer_ == nullptr || mesh_ == nullptr)
	{
		return;
	}

	RendererParameter parameter{};
	parameter.CameraMatrix.Indentity();
	parameter.ProjectionMatrix.Indentity();
	parameter.WorldMatrix.Indentity();
	meshRenderer_->Render(parameter);
}

} // namespace ToolRuntime
} // namespace Effekseer
