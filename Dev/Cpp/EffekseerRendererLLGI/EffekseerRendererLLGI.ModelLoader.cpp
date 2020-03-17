
#include "EffekseerRendererLLGI.ModelLoader.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include <memory>

namespace EffekseerRendererLLGI
{

ModelLoader::ModelLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
	: graphicsDevice_(graphicsDevice), m_fileInterface(fileInterface)
{
	LLGI::SafeAddRef(graphicsDevice_);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

ModelLoader::~ModelLoader() { LLGI::SafeRelease(graphicsDevice_); }

void* ModelLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == NULL)
		return nullptr;

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		Model* model = new Model(data_model, size_model);

		model->ModelCount = Effekseer::Min(Effekseer::Max(model->GetModelCount(), 1), 40);

		model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

		for (int32_t f = 0; f < model->GetFrameCount(); f++)
		{
			model->InternalModels[f].VertexCount = model->GetVertexCount(f);

			{
				std::vector<Effekseer::Model::VertexWithIndex> vs;
				for (int32_t m = 0; m < model->ModelCount; m++)
				{
					for (int32_t i = 0; i < model->GetVertexCount(f); i++)
					{
						Effekseer::Model::VertexWithIndex v;
						v.Position = model->GetVertexes(f)[i].Position;
						v.Normal = model->GetVertexes(f)[i].Normal;
						v.Binormal = model->GetVertexes(f)[i].Binormal;
						v.Tangent = model->GetVertexes(f)[i].Tangent;
						v.UV = model->GetVertexes(f)[i].UV;
						v.VColor = model->GetVertexes(f)[i].VColor;
						v.Index[0] = m;

						vs.push_back(v);
					}
				}

				auto vb_size = sizeof(Effekseer::Model::VertexWithIndex) * model->GetVertexCount(f) * model->ModelCount;

				model->InternalModels[f].VertexBuffer = graphicsDevice_->GetGraphics()->CreateVertexBuffer(vb_size);

				auto p_ = model->InternalModels[f].VertexBuffer->Lock();
				memcpy(p_, vs.data(), sizeof(Effekseer::Model::VertexWithIndex) * vs.size());
				model->InternalModels[f].VertexBuffer->Unlock();
			}

			model->InternalModels[f].FaceCount = model->GetFaceCount(f);
			model->InternalModels[f].IndexCount = model->InternalModels[f].FaceCount * 3;

			{
				std::vector<Effekseer::Model::Face> fs;
				for (int32_t m = 0; m < model->ModelCount; m++)
				{
					for (int32_t i = 0; i < model->InternalModels[f].FaceCount; i++)
					{
						Effekseer::Model::Face face;
						face.Indexes[0] = model->GetFaces(f)[i].Indexes[0] + model->GetVertexCount(f) * m;
						face.Indexes[1] = model->GetFaces(f)[i].Indexes[1] + model->GetVertexCount(f) * m;
						face.Indexes[2] = model->GetFaces(f)[i].Indexes[2] + model->GetVertexCount(f) * m;
						fs.push_back(face);
					}
				}

				model->InternalModels[f].IndexBuffer = graphicsDevice_->GetGraphics()->CreateIndexBuffer(
					4, sizeof(int32_t) * 3 * model->InternalModels[f].FaceCount * model->ModelCount);

				auto p_ = model->InternalModels[f].IndexBuffer->Lock();
				memcpy(p_, fs.data(), sizeof(Effekseer::Model::Face) * fs.size());
				model->InternalModels[f].IndexBuffer->Unlock();
			}
		}

		delete[] data_model;

		return (void*)model;
	}

	return NULL;
}

void ModelLoader::Unload(void* data)
{
	if (data != NULL)
	{
		Model* model = (Model*)data;
		delete model;
	}
}

} // namespace EffekseerRendererLLGI

