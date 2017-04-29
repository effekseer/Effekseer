
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Renderer.h"
#include "EffekseerRendererDX11.ModelLoader.h"
#include <memory>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface )
	: device			( device )
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
	std::auto_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	if( reader.get() == NULL ) return false;

	if( reader.get() != NULL )
	{
		HRESULT hr;

		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read( data_model, size_model );

		Model* model = new Model( data_model, size_model );

		model->ModelCount = Effekseer::Min( Effekseer::Max( model->GetModelCount(), 1 ), 40);

		model->VertexCount = model->GetVertexCount();

		if( model->VertexCount == 0 ) return NULL;

		{
			std::vector<Effekseer::Model::VertexWithIndex> vs;
			for(int32_t m = 0; m < model->ModelCount; m++ )
			{
				for( int32_t i = 0; i < model->GetVertexCount(); i++ )
				{
					Effekseer::Model::VertexWithIndex v;
					v.Position = model->GetVertexes()[i].Position;
					v.Normal = model->GetVertexes()[i].Normal;
					v.Binormal = model->GetVertexes()[i].Binormal;
					v.Tangent = model->GetVertexes()[i].Tangent;
					v.UV = model->GetVertexes()[i].UV;
					v.VColor = model->GetVertexes()[i].VColor;
					v.Index[0] = m;

					vs.push_back( v );
				}
			}

			ID3D11Buffer* vb = NULL;

			D3D11_BUFFER_DESC hBufferDesc;
			hBufferDesc.ByteWidth = sizeof(Effekseer::Model::VertexWithIndex) * model->GetVertexCount() * model->ModelCount;
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			D3D11_SUBRESOURCE_DATA hSubResourceData;
			hSubResourceData.pSysMem = &(vs[0]);
			hSubResourceData.SysMemPitch = 0;
			hSubResourceData.SysMemSlicePitch = 0;

			if( FAILED( device->CreateBuffer(&hBufferDesc, &hSubResourceData, &vb) ) )
			{
				return NULL;
			}

			model->VertexBuffer = vb;
		}

		model->FaceCount = model->GetFaceCount();

		/* 0.50より追加(0.50以前から移行する時は追記する必要あり) */
		model->IndexCount = model->FaceCount * 3;

		{
			std::vector<Effekseer::Model::Face> fs;
			for(int32_t m = 0; m < model->ModelCount; m++ )
			{
				for( int32_t i = 0; i < model->FaceCount; i++ )
				{
					Effekseer::Model::Face f;
					f.Indexes[0] = model->GetFaces()[i].Indexes[0] + model->GetVertexCount() * m;
					f.Indexes[1] = model->GetFaces()[i].Indexes[1] + model->GetVertexCount() * m;
					f.Indexes[2] = model->GetFaces()[i].Indexes[2] + model->GetVertexCount() * m;
					fs.push_back(f);
				}
			}

			ID3D11Buffer* ib = NULL;
			D3D11_BUFFER_DESC hBufferDesc;
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.ByteWidth = sizeof(int32_t) * 3 * model->FaceCount * model->ModelCount;
			hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(int32_t);

			D3D11_SUBRESOURCE_DATA hSubResourceData;
			hSubResourceData.pSysMem = &(fs[0]);
			hSubResourceData.SysMemPitch = 0;
			hSubResourceData.SysMemSlicePitch = 0;

			if( FAILED( device->CreateBuffer(&hBufferDesc, &hSubResourceData, &ib) ) )
			{
				return NULL;
			}

			model->IndexBuffer = ib;
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
