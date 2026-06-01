#include "GroundPlaneRenderer.h"

#include "DepthRendering.h"

namespace Effekseer
{
namespace ToolRuntime
{

bool GroundPlaneRenderer::Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	groundMeshRenderer_ = StaticMeshRenderer::Create(graphicsDevice);

	if (groundMeshRenderer_ == nullptr)
	{
		return false;
	}

	Effekseer::CustomVector<StaticMeshVertex> vbData(4);
	const auto indices = CreateGroundPlaneIndices32();
	Effekseer::CustomVector<int32_t> ibData;
	ibData.resize(static_cast<int32_t>(indices.size()));
	for (int32_t i = 0; i < static_cast<int32_t>(indices.size()); i++)
	{
		ibData[i] = indices[i];
	}

	auto groundMesh = StaticMesh::Create(graphicsDevice, vbData, ibData, true);

	Effekseer::Backend::TextureParameter texParams;
	texParams.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	texParams.Size = {128, 128};

	Effekseer::CustomVector<uint8_t> initialData;
	initialData.resize(texParams.Size[0] * texParams.Size[1] * 4);

	for (size_t i = 0; i < initialData.size() / 4; i++)
	{
		const size_t x = i % texParams.Size[0] * 2 / texParams.Size[0];
		const size_t y = i / texParams.Size[1] * 2 / texParams.Size[1];
		if (x ^ y == 0)
		{
			initialData[i * 4 + 0] = 90;
			initialData[i * 4 + 1] = 90;
			initialData[i * 4 + 2] = 90;
			initialData[i * 4 + 3] = 255;
		}
		else
		{
			initialData[i * 4 + 0] = 60;
			initialData[i * 4 + 1] = 60;
			initialData[i * 4 + 2] = 60;
			initialData[i * 4 + 3] = 255;
		}
	}
	groundMesh->Texture = graphicsDevice->CreateTexture(texParams, initialData);

	groundMeshRenderer_->SetStaticMesh(groundMesh);

	UpdateGround();

	return true;
}

void GroundPlaneRenderer::SetExtent(int32_t extent)
{
	if (GroundExtent == extent)
	{
		return;
	}

	GroundExtent = extent;
	UpdateGround();
}

void GroundPlaneRenderer::UpdateGround()
{
	std::array<StaticMeshVertex, 4> vbData;
	auto parameter = GroundPlaneParameter{};
	parameter.Height = 0.0f;
	parameter.HalfExtent = static_cast<float>(GroundExtent);
	const auto groundVertices = CreateGroundPlaneWorldVertices(parameter);

	for (int32_t i = 0; i < static_cast<int32_t>(vbData.size()); i++)
	{
		vbData[i].Pos = {groundVertices[i].Position.X, groundVertices[i].Position.Y, groundVertices[i].Position.Z};
		vbData[i].VColor = {255, 255, 255, 255};
		vbData[i].UV = groundVertices[i].UV;
		vbData[i].Normal = {0.0f, 1.0f, 0.0f};
	}

	groundMeshRenderer_->GetStaticMesh()->GetVertexBuffer()->UpdateData(
		vbData.data(), static_cast<int32_t>(vbData.size() * sizeof(StaticMeshVertex)), 0);
}

void GroundPlaneRenderer::Render(EffekseerRenderer::RendererRef renderer)
{
	RendererParameter param{};
	param.CameraMatrix = renderer->GetCameraMatrix();
	param.ProjectionMatrix = renderer->GetProjectionMatrix();
	param.WorldMatrix.Translation(0.0f, GroundHeight, 0.0f);
	param.DirectionalLightDirection = renderer->GetLightDirection().ToFloat4();
	param.DirectionalLightColor = renderer->GetLightColor().ToFloat4();
	param.AmbientLightColor = renderer->GetLightAmbientColor().ToFloat4();
	groundMeshRenderer_->Render(param);
}

std::shared_ptr<GroundPlaneRenderer> GroundPlaneRenderer::Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	auto ret = std::make_shared<GroundPlaneRenderer>();
	if (ret->Initialize(graphicsDevice))
	{
		return ret;
	}
	return nullptr;
}

} // namespace ToolRuntime
} // namespace Effekseer
