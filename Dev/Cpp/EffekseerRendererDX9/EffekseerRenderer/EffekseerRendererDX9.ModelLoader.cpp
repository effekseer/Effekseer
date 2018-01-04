
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include "EffekseerRendererDX9.Renderer.h"
#include "EffekseerRendererDX9.ModelLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface )
	: device			(device)
	, m_fileInterface	( fileInterface )
{
	ES_SAFE_ADDREF(device);

	if( m_fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
	ES_SAFE_RELEASE(device);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* ModelLoader::Load( const EFK_CHAR* path )
{
	std::unique_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	if( reader.get() == NULL ) return false;

	if( reader.get() != NULL )
	{
		HRESULT hr;

		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read( data_model, size_model );

		auto model = new Model( data_model, size_model );

		model->ModelCount = Effekseer::Min( Effekseer::Max( model->GetModelCount(), 1 ), 40);

		model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

		for (int32_t f = 0; f < model->GetFrameCount(); f++)
		{
			model->InternalModels[f].VertexCount = model->GetVertexCount(f);

			IDirect3DVertexBuffer9* vb = NULL;
			hr = device->CreateVertexBuffer(
				sizeof(Effekseer::Model::VertexWithIndex) * model->InternalModels[f].VertexCount * model->ModelCount,
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_MANAGED,
				&vb,
				NULL);

			if (FAILED(hr))
			{
				/* DirectX9ExではD3DPOOL_MANAGED使用不可 */
				hr = device->CreateVertexBuffer(
					sizeof(Effekseer::Model::VertexWithIndex) * model->InternalModels[f].VertexCount * model->ModelCount,
					D3DUSAGE_WRITEONLY,
					0,
					D3DPOOL_DEFAULT,
					&vb,
					NULL);
			}

			if (vb != NULL)
			{
				uint8_t* resource = NULL;
				vb->Lock(0, 0, (void**) &resource, 0);

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

						std::swap(v.VColor.R, v.VColor.B);

						v.Index[0] = m;

						memcpy(resource, &v, sizeof(Effekseer::Model::VertexWithIndex));
						resource += sizeof(Effekseer::Model::VertexWithIndex);
					}
				}

				vb->Unlock();
			}

			model->InternalModels[f].VertexBuffer = vb;

			model->InternalModels[f].FaceCount = model->GetFaceCount(f);
			model->InternalModels[f].IndexCount = model->InternalModels[f].FaceCount * 3;

			IDirect3DIndexBuffer9* ib = NULL;
			hr = device->CreateIndexBuffer(
				sizeof(Effekseer::Model::Face) * model->InternalModels[f].FaceCount * model->ModelCount,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX32,
				D3DPOOL_MANAGED,
				&ib,
				NULL);

			if (FAILED(hr))
			{
				hr = device->CreateIndexBuffer(
					sizeof(Effekseer::Model::Face) *model->InternalModels[f].FaceCount * model->ModelCount,
					D3DUSAGE_WRITEONLY,
					D3DFMT_INDEX32,
					D3DPOOL_DEFAULT,
					&ib,
					NULL);
			}

			if (ib != NULL)
			{
				uint8_t* resource = NULL;
				ib->Lock(0, 0, (void**) &resource, 0);
				for (int32_t m = 0; m < model->ModelCount; m++)
				{
					for (int32_t i = 0; i < model->InternalModels[f].FaceCount; i++)
					{
						Effekseer::Model::Face face;
						face.Indexes[0] = model->GetFaces(f)[i].Indexes[0] + model->GetVertexCount(f) * m;
						face.Indexes[1] = model->GetFaces(f)[i].Indexes[1] + model->GetVertexCount(f) * m;
						face.Indexes[2] = model->GetFaces(f)[i].Indexes[2] + model->GetVertexCount(f) * m;

						memcpy(resource, &face, sizeof(Effekseer::Model::Face));
						resource += sizeof(Effekseer::Model::Face);
					}
				}

				ib->Unlock();
			}

			model->InternalModels[f].IndexBuffer = ib;
		}

		delete [] data_model;

		return (void*)model;
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelLoader::Unload( void* data )
{
	if( data != NULL )
	{
		Model* model = (Model*)data;
		delete model;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__