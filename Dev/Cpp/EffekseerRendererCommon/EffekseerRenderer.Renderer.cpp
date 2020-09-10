
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.Renderer_Impl.h"
#include <Effekseer.h>
#include <assert.h>

namespace EffekseerRenderer
{

Renderer::Renderer()
{
	impl = new Impl();
}

Renderer::~Renderer()
{
	ES_SAFE_DELETE(impl);
}

Renderer::Impl* Renderer::GetImpl()
{
	return impl;
}

::Effekseer::Vector3D Renderer::GetLightDirection() const
{
	return impl->GetLightDirection();
}

void Renderer::SetLightDirection(const ::Effekseer::Vector3D& direction)
{
	impl->SetLightDirection(direction);
}

const ::Effekseer::Color& Renderer::GetLightColor() const
{
	return impl->GetLightColor();
}

void Renderer::SetLightColor(const ::Effekseer::Color& color)
{
	impl->SetLightColor(color);
}

const ::Effekseer::Color& Renderer::GetLightAmbientColor() const
{
	return impl->GetLightAmbientColor();
}

void Renderer::SetLightAmbientColor(const ::Effekseer::Color& color)
{
	impl->SetLightAmbientColor(color);
}

::Effekseer::Matrix44 Renderer::GetProjectionMatrix() const
{
	return impl->GetProjectionMatrix();
}

void Renderer::SetProjectionMatrix(const ::Effekseer::Matrix44& mat)
{
	impl->SetProjectionMatrix(mat);
}

::Effekseer::Matrix44 Renderer::GetCameraMatrix() const
{
	return impl->GetCameraMatrix();
}

void Renderer::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	impl->SetCameraMatrix(mat);
}

::Effekseer::Matrix44 Renderer::GetCameraProjectionMatrix() const
{
	return impl->GetCameraProjectionMatrix();
}

::Effekseer::Vector3D Renderer::GetCameraFrontDirection() const
{
	return impl->GetCameraFrontDirection();
}

::Effekseer::Vector3D Renderer::GetCameraPosition() const
{
	return impl->GetCameraPosition();
}

void Renderer::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	impl->SetCameraParameter(front, position);
}

int32_t Renderer::GetDrawCallCount() const
{
	return impl->GetDrawCallCount();
}

int32_t Renderer::GetDrawVertexCount() const
{
	return impl->GetDrawVertexCount();
}

void Renderer::ResetDrawCallCount()
{
	impl->ResetDrawCallCount();
}

void Renderer::ResetDrawVertexCount()
{
	impl->ResetDrawVertexCount();
}

Effekseer::RenderMode Renderer::GetRenderMode() const
{
	return impl->GetRenderMode();
}

void Renderer::SetRenderMode(Effekseer::RenderMode renderMode)
{
	impl->SetRenderMode(renderMode);
}

UVStyle Renderer::GetTextureUVStyle() const
{
	return impl->GetTextureUVStyle();
}

void Renderer::SetTextureUVStyle(UVStyle style)
{
	impl->SetTextureUVStyle(style);
}

UVStyle Renderer::GetBackgroundTextureUVStyle() const
{
	return impl->GetBackgroundTextureUVStyle();
}

void Renderer::SetBackgroundTextureUVStyle(UVStyle style)
{
	impl->SetBackgroundTextureUVStyle(style);
}

float Renderer::GetTime() const
{
	return impl->GetTime();
}

void Renderer::SetTime(float time)
{
	impl->SetTime(time);
}

void Renderer::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	// not implemented
	assert(0);
}

Model::InternalModel::InternalModel()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
	VertexCount = 0;
	IndexCount = 0;
	FaceCount = 0;
}

Model::InternalModel::~InternalModel()
{
	ES_SAFE_RELEASE(VertexBuffer);
	ES_SAFE_RELEASE(IndexBuffer);
}

Model::Model(uint8_t* data, int32_t size, int maximumModelCount, Effekseer::Backend::GraphicsDevice* graphicsDevice)
	: Effekseer::Model(data, size)
	, graphicsDevice_(graphicsDevice)
	, InternalModels(nullptr)
	, ModelCount(0)
{
	ES_SAFE_ADDREF(graphicsDevice_);

	ModelCount = Effekseer::Min(Effekseer::Max(GetModelCount(), 1), maximumModelCount);
}

Model::~Model()
{
	ES_SAFE_DELETE_ARRAY(InternalModels);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Model::LoadToGPU()
{
	if (IsLoadedOnGPU)
	{
		return false;
	}

	InternalModels = new Model::InternalModel[GetFrameCount()];

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		InternalModels[f].VertexCount = GetVertexCount(f);

		{
			ES_SAFE_RELEASE(InternalModels[f].VertexBuffer);

			std::vector<Effekseer::Model::Vertex> vs;
			for (int32_t m = 0; m < ModelCount; m++)
			{
				for (int32_t i = 0; i < GetVertexCount(f); i++)
				{
					Effekseer::Model::Vertex v;
					v.Position = GetVertexes(f)[i].Position;
					v.Normal = GetVertexes(f)[i].Normal;
					v.Binormal = GetVertexes(f)[i].Binormal;
					v.Tangent = GetVertexes(f)[i].Tangent;
					v.UV = GetVertexes(f)[i].UV;
					v.VColor = GetVertexes(f)[i].VColor;

					vs.push_back(v);
				}
			}

			InternalModels[f].VertexBuffer = graphicsDevice_->CreateVertexBuffer(sizeof(Effekseer::Model::Vertex) * GetVertexCount(f) * ModelCount, vs.data(), false);
			if (InternalModels[f].VertexBuffer == nullptr)
			{
				return false;
			}
		}

		InternalModels[f].FaceCount = GetFaceCount(f);
		InternalModels[f].IndexCount = InternalModels[f].FaceCount * 3;

		{
			ES_SAFE_RELEASE(InternalModels[f].IndexBuffer);

			std::vector<Effekseer::Model::Face> fs;
			for (int32_t m = 0; m < ModelCount; m++)
			{
				for (int32_t i = 0; i < InternalModels[f].FaceCount; i++)
				{
					Effekseer::Model::Face face;
					face.Indexes[0] = GetFaces(f)[i].Indexes[0] + GetVertexCount(f) * m;
					face.Indexes[1] = GetFaces(f)[i].Indexes[1] + GetVertexCount(f) * m;
					face.Indexes[2] = GetFaces(f)[i].Indexes[2] + GetVertexCount(f) * m;
					fs.push_back(face);
				}
			}

			InternalModels[f].IndexBuffer = graphicsDevice_->CreateIndexBuffer(3 * InternalModels[f].FaceCount * ModelCount, fs.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
			if (InternalModels[f].IndexBuffer == nullptr)
			{
				return false;
			}
		}
	}

	IsLoadedOnGPU = true;

	return true;
}

} // namespace EffekseerRenderer