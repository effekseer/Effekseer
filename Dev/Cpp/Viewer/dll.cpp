
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "dll.h"

#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3dx9.lib" )

#define NONDLL	1
#define MSWIN32 1
#define BGDWIN32 1
#include <gd/gd.h>
#include <gd/gdfontmb.h>

#if _DEBUG
#pragma comment(lib,"Debug/libgd_static.lib")
#else
#pragma comment(lib,"Release/libgd_static.lib")
#endif

#define Z_SOLO
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>
#if _DEBUG
#pragma comment(lib,"Debug/libpng16.lib")
#pragma comment(lib,"Debug/zlib.lib")
#else
#pragma comment(lib,"Release/libpng16.lib")
#pragma comment(lib,"Release/zlib.lib")
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static float								g_RotX = 30.0f;
static float								g_RotY = -30.0f;
static Effekseer::Vector3D					g_lightDirection = Effekseer::Vector3D(1,1,1);
static float								g_Distance = 15.0f;
const float									PI = 3.14159265f;

static bool									g_mouseRotDirectionInvX = false;
static bool									g_mouseRotDirectionInvY = false;

static bool									g_mouseSlideDirectionInvX = false;
static bool									g_mouseSlideDirectionInvY = false;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SavePNGImage(const wchar_t* filepath, int32_t width, int32_t height, void* data, bool rev)
{
	/* 構造体確保 */
#if _WIN32
	FILE *fp = _wfopen(filepath, L"wb");
#else
	FILE *fp = fopen(ToUtf8String(filepath).c_str(), "wb");
#endif

	if (fp == nullptr) return;

	png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop ip = png_create_info_struct(pp);

	/* 書き込み準備 */
	png_init_io(pp, fp);
	png_set_IHDR(pp, ip, width, height,
		8, /* 8bit以外にするなら変える */
		PNG_COLOR_TYPE_RGBA, /* RGBA以外にするなら変える */
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* ピクセル領域確保 */
	std::vector<png_byte>  raw1D(height * png_get_rowbytes(pp, ip));
	std::vector<png_bytep> raw2D(height * sizeof(png_bytep));
	for (int32_t i = 0; i < height; i++)
	{
		raw2D[i] = &raw1D[i*png_get_rowbytes(pp, ip)];
	}

	memcpy((void*) raw1D.data(), data, width * height * 4);

	/* 上下反転 */
	if (rev)
	{
		for (int32_t i = 0; i < height / 2; i++)
		{
			png_bytep swp = raw2D[i];
			raw2D[i] = raw2D[height - i - 1];
			raw2D[height - i - 1] = swp;
		}
	}

	/* 書き込み */
	png_write_info(pp, ip);
	png_write_image(pp, raw2D.data());
	png_write_end(pp, ip);

	/* 解放 */
	png_destroy_write_struct(&pp, &ip);
	fclose(fp);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Combine( const wchar_t* rootPath, const wchar_t* treePath,  wchar_t* dst, int dst_length )
{
	int rootPathLength = 0;
	while( rootPath[ rootPathLength ] != 0 )
	{
		rootPathLength++;
	}

	int treePathLength = 0;
	while( treePath[ treePathLength ] != 0 )
	{
		treePathLength++;
	}

	// 両方ともなし
	if( rootPathLength == 0 && treePathLength == 0 )
	{
		return false;
	}

	// 片方なし
	if( rootPathLength == 0 )
	{
		if( treePathLength < dst_length )
		{
			memcpy( dst, treePath, sizeof(wchar_t) * (treePathLength + 1) );
			return true;
		}
		else
		{
			return false;
		}
	}

	if( treePathLength == 0 )
	{
		if( rootPathLength < dst_length )
		{
			memcpy( dst, rootPath, sizeof(wchar_t) * (rootPathLength + 1) );
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// 両方あり

	// ディレクトリパスまで戻す。
	int PathPosition = rootPathLength;
	while( PathPosition > 0 )
	{
		if( rootPath[ PathPosition - 1 ] == L'/' || rootPath[ PathPosition - 1 ] == L'\\' )
		{
			break;
		}
		PathPosition--;
	}

	// コピーする
	memcpy( dst, rootPath, sizeof(wchar_t) * PathPosition );
	dst[ PathPosition ] = 0;

	// 無理やり繋げる
	if( PathPosition + treePathLength > dst_length )
	{
		return false;
	}

	memcpy( &(dst[ PathPosition ]), treePath, sizeof(wchar_t) * treePathLength );
	PathPosition = PathPosition + treePathLength;
	dst[ PathPosition ] = 0;

	// ../ ..\ の処理
	for( int i = 0; i < PathPosition - 2; i++ )
	{
		if( dst[ i ] == L'.' && dst[ i + 1 ] == L'.' && ( dst[ i + 2 ] == L'/' || dst[ i + 2 ] == L'\\' ) )
		{
			int pos = 0;

			if( i > 1 && dst[ i - 2 ] == L'.' )
			{
				
			}
			else
			{
				for( pos = i - 2; pos > 0; pos-- )
				{
					if( dst[ pos - 1 ] == L'/' || dst[ pos - 1 ] == L'\\' )
					{
						break;
					}
				}

				for( int k = pos; k < PathPosition; k++ )
				{
					dst[ k ] = dst[ k + (i + 3) - pos ];
				}
				PathPosition = PathPosition - ( i + 3 - pos );
				i = pos - 1;
			}
		}
	}
	dst[ PathPosition ] = 0;
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerParamater::ViewerParamater()
	: GuideWidth		( 0 )
	, GuideHeight		( 0 )
	, IsPerspective		( false )
	, IsOrthographic	( false )
	, FocusX			( 0 )
	, FocusY			( 0 )
	, FocusZ			( 0 )
	, AngleX			( 0 )
	, AngleY			( 0 )
	, Distance			( 0 )
	, RendersGuide		( false )

	, IsCullingShown	(false)
	, CullingRadius		( 0 )
	, CullingX			( 0 )
	, CullingY			( 0 )
	, CullingZ			( 0 )

{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerEffectBehavior::ViewerEffectBehavior()
	: CountX	( 1 )
	, CountY	( 1 )
	, CountZ	( 1 )
	, Distance	( 5.0f )
	, RemovedTime	( 0xffff )
	, PositionX	( 0.0f )
	, PositionY	( 0.0f )
	, PositionZ	( 0.0f )
	, RotationX	( 0.0f )
	, RotationY	( 0.0f )
	, RotationZ	( 0.0f )
	, ScaleX	( 1.0f )
	, ScaleY	( 1.0f )
	, ScaleZ	( 1.0f )
	
	, PositionVelocityX	( 0.0f )
	, PositionVelocityY	( 0.0f )
	, PositionVelocityZ	( 0.0f )
	, RotationVelocityX	( 0.0f )
	, RotationVelocityY	( 0.0f )
	, RotationVelocityZ	( 0.0f )
	, ScaleVelocityX	( 0.0f )
	, ScaleVelocityY	( 0.0f )
	, ScaleVelocityZ	( 0.0f )
	
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerTool::Renderer*		g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::EffekseerTool::Sound*			g_sound = NULL;
static std::map<std::wstring,IDirect3DTexture9*> m_textures;
static std::map<std::wstring,EffekseerRendererDX9::Model*> m_models;

static std::vector<::Effekseer::Handle>	g_handles;

static ::Effekseer::Vector3D	g_focus_position;

static ::Effekseer::Client*		g_client = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::TextureLoader(EffekseerRenderer::Renderer* renderer, bool isSRGBMode)
	: m_renderer	( renderer )
	, m_isSRGBMode(isSRGBMode)
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Native::TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	wchar_t dst[260];
	Combine( RootPath.c_str(), (const wchar_t *)path, dst, 260 );

	std::wstring key( dst );
	IDirect3DTexture9* pTexture = NULL;

	if( m_textures.count( key ) > 0 )
	{
		pTexture = m_textures[ key ];
	}
	else
	{
		FILE* fp_texture = _wfopen( dst, L"rb" );
		if( fp_texture != NULL )
		{
			fseek( fp_texture, 0, SEEK_END );
			size_t size_texture = ftell( fp_texture );
			char* data_texture = new char[size_texture];
			fseek( fp_texture, 0, SEEK_SET );
			fread( data_texture, 1, size_texture, fp_texture );
			fclose( fp_texture );

			//D3DXCreateTextureFromFileInMemory( ((EffekseerRendererDX9::RendererImplemented*)m_renderer)->GetDevice(), data_texture, size_texture, &pTexture );

			DWORD usage = 0;
			if (m_isSRGBMode)
			{
				usage = 0;
			}
			
			D3DXCreateTextureFromFileInMemoryEx(
				((EffekseerRendererDX9::RendererImplemented*)m_renderer)->GetDevice(),
				data_texture,
				size_texture,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				usage,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,
				NULL,
				NULL,
				&pTexture);

			delete [] data_texture;

			m_textures[ key ] = pTexture;
		}
	}

	return pTexture;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::TextureLoader::Unload( void* data )
{
	/*
	if( data != NULL )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
	*/
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::SoundLoader::SoundLoader( Effekseer::SoundLoader* loader )
	: m_loader( loader )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::SoundLoader::~SoundLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Native::SoundLoader::Load( const EFK_CHAR* path )
{
	EFK_CHAR dst[260];
	Combine( RootPath.c_str(), (const wchar_t *)path, (wchar_t *)dst, 260 );

	return m_loader->Load( dst );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SoundLoader::Unload( void* handle )
{
	m_loader->Unload( handle );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::ModelLoader::ModelLoader( EffekseerRendererDX9::Renderer* renderer )
	: m_renderer	( renderer )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::ModelLoader::~ModelLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Native::ModelLoader::Load( const EFK_CHAR* path )
{
	wchar_t dst[260];
	Combine( RootPath.c_str(), (const wchar_t *)path, dst, 260 );

	std::wstring key( dst );
	EffekseerRendererDX9::Model* model = NULL;

	if( m_models.count( key ) > 0 )
	{
		model = m_models[ key ];
	}
	else
	{

		FILE* fp_model = _wfopen( (const wchar_t *)dst, L"rb" );

		if( fp_model != NULL )
		{
			HRESULT hr;

			fseek( fp_model, 0, SEEK_END );
			size_t size_model = ftell( fp_model );
			uint8_t* data_model = new uint8_t[size_model];
			fseek( fp_model, 0, SEEK_SET );
			fread( data_model, 1, size_model, fp_model );
			fclose( fp_model );

			model = new EffekseerRendererDX9::Model( data_model, size_model );

			model->ModelCount = Effekseer::Min( Effekseer::Max( model->GetModelCount(), 1 ), 40);

			model->VertexCount = model->GetVertexCount();

			IDirect3DVertexBuffer9* vb = NULL;
			hr = m_renderer->GetDevice()->CreateVertexBuffer(
				sizeof(Effekseer::Model::VertexWithIndex) * model->VertexCount * model->ModelCount,
				D3DUSAGE_WRITEONLY,
				0,
				D3DPOOL_MANAGED,
				&vb,
				NULL );

			if( FAILED( hr ) )
			{
				/* DirectX9ExではD3DPOOL_MANAGED使用不可 */
				hr = m_renderer->GetDevice()->CreateVertexBuffer(
					sizeof(Effekseer::Model::VertexWithIndex) * model->VertexCount * model->ModelCount,
					D3DUSAGE_WRITEONLY,
					0,
					D3DPOOL_DEFAULT,
					&vb,
					NULL );
			}

			if( vb != NULL )
			{
				uint8_t* resource = NULL;
				vb->Lock( 0, 0, (void**)&resource, 0 );

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
						v.Index[0] = m;

						memcpy( resource, &v, sizeof(Effekseer::Model::VertexWithIndex) );
						resource += sizeof(Effekseer::Model::VertexWithIndex);
					}
				}

				vb->Unlock();
			}

			model->VertexBuffer = vb;

			model->FaceCount = model->GetFaceCount();
			model->IndexCount = model->FaceCount * 3;
			IDirect3DIndexBuffer9* ib = NULL;
			hr = m_renderer->GetDevice()->CreateIndexBuffer( 
				sizeof(Effekseer::Model::Face) * model->FaceCount * model->ModelCount,
				D3DUSAGE_WRITEONLY, 
				D3DFMT_INDEX32, 
				D3DPOOL_MANAGED, 
				&ib, 
				NULL );

			if( FAILED( hr ) )
			{
				hr = m_renderer->GetDevice()->CreateIndexBuffer( 
					sizeof(Effekseer::Model::Face) * model->FaceCount * model->ModelCount,
					D3DUSAGE_WRITEONLY, 
					D3DFMT_INDEX32, 
					D3DPOOL_DEFAULT, 
					&ib, 
					NULL );
			}

			if( ib != NULL )
			{
				uint8_t* resource = NULL;
				ib->Lock( 0, 0, (void**)&resource, 0 );
				for(int32_t m = 0; m < model->ModelCount; m++ )
				{
					for( int32_t i = 0; i < model->FaceCount; i++ )
					{
						Effekseer::Model::Face f;
						f.Indexes[0] = model->GetFaces()[i].Indexes[0] + model->GetVertexCount() * m;
						f.Indexes[1] = model->GetFaces()[i].Indexes[1] + model->GetVertexCount() * m;
						f.Indexes[2] = model->GetFaces()[i].Indexes[2] + model->GetVertexCount() * m;

						memcpy( resource, &f, sizeof(Effekseer::Model::Face) );
						resource += sizeof(Effekseer::Model::Face);
					}
				}
				
				ib->Unlock();
			}

			model->IndexBuffer = ib;

			delete [] data_model;

			return model;
		}

	}

	return model;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::ModelLoader::Unload( void* data )
{
	/*
	if( data != NULL )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
	*/
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::Native()
	: m_time			( 0 )
	, m_step			( 1 )
{
	g_client = Effekseer::Client::Create();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::~Native()
{
	ES_SAFE_DELETE( g_client );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::CreateWindow_Effekseer(void* pHandle, int width, int height, bool isSRGBMode)
{
	m_isSRGBMode = isSRGBMode;

	g_renderer = new ::EffekseerTool::Renderer( 20000, isSRGBMode );
	if( g_renderer->Initialize( (HWND)pHandle, width, height ) )
	{
		// 関数追加
		//::Effekseer::ScriptRegister::SetExternalFunction(0, print);

		g_manager = ::Effekseer::Manager::Create( 20000 );

		{
			::Effekseer::SpriteRenderer* sprite_renderer = g_renderer->GetRenderer()->CreateSpriteRenderer();
			::Effekseer::RibbonRenderer* ribbon_renderer = g_renderer->GetRenderer()->CreateRibbonRenderer();
			::Effekseer::RingRenderer* ring_renderer = g_renderer->GetRenderer()->CreateRingRenderer();
			::Effekseer::ModelRenderer* model_renderer = g_renderer->GetRenderer()->CreateModelRenderer();
			::Effekseer::TrackRenderer* track_renderer = g_renderer->GetRenderer()->CreateTrackRenderer();

			if( sprite_renderer == NULL )
			{
				printf("Failed create Renderer\n");
				g_manager->Destroy();
				g_manager = NULL;
				ES_SAFE_DELETE( g_renderer );
				return false;
			}
			g_manager->SetSpriteRenderer( sprite_renderer );
			g_manager->SetRibbonRenderer( ribbon_renderer );
			g_manager->SetRingRenderer( ring_renderer );
			g_manager->SetModelRenderer( model_renderer );
			g_manager->SetTrackRenderer( track_renderer );
			g_manager->SetTextureLoader( new TextureLoader( (EffekseerRendererDX9::Renderer *)g_renderer->GetRenderer(), isSRGBMode ) );
			g_manager->SetModelLoader( new ModelLoader( (EffekseerRendererDX9::Renderer *)g_renderer->GetRenderer() ) );
		}
	}
	else
	{
		ES_SAFE_DELETE( g_renderer );
		return false;
	}

	g_sound = new ::EffekseerTool::Sound();
	if( g_sound->Initialize( 16, 16 ) )
	{
		g_manager->SetSoundPlayer( g_sound->GetSound()->CreateSoundPlayer() );
		g_manager->SetSoundLoader( new SoundLoader( g_sound->GetSound()->CreateSoundLoader() ) );
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::UpdateWindow()
{
	assert( g_manager != NULL );
	assert( g_renderer != NULL );

	::Effekseer::Vector3D position( 0, 0, g_Distance );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );

	if( g_renderer->IsRightHand )
	{
		mat_rot_y.RotationY( -g_RotY / 180.0f * PI );
		::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
		::Effekseer::Vector3D::Transform( position, position, mat );

		position.X += g_focus_position.X;
		position.Y += g_focus_position.Y;
		position.Z += g_focus_position.Z;
	
		::Effekseer::Matrix44 cameraMat;
		g_renderer->GetRenderer()->SetCameraMatrix( 
			::Effekseer::Matrix44().LookAtRH( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	}
	else
	{
		mat_rot_y.RotationY( (g_RotY+180.0f) / 180.0f * PI );
		::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
		::Effekseer::Vector3D::Transform( position, position, mat );
	
		::Effekseer::Vector3D temp_focus = g_focus_position;
		temp_focus.Z = - temp_focus.Z;
		
		position.X += temp_focus.X;
		position.Y += temp_focus.Y;
		position.Z += temp_focus.Z;
	
		::Effekseer::Matrix44 cameraMat;
		g_renderer->GetRenderer()->SetCameraMatrix( 
			::Effekseer::Matrix44().LookAtLH( position, temp_focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	}

	g_sound->SetListener( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) );
	
	g_renderer->BeginRendering();
	g_manager->Draw();
	g_renderer->EndRendering();
	g_renderer->Present();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::ResizeWindow( int width, int height )
{
	g_renderer->Resize( width, height );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::DestroyWindow()
{
	assert( g_renderer != NULL );
	assert( g_manager != NULL );

	for( size_t i = 0; i < g_handles.size(); i++ )
	{
		g_manager->StopEffect( g_handles[i] );
	}
	g_handles.clear();
	
	InvalidateTextureCache();

	ES_SAFE_RELEASE( g_effect );

	g_manager->Destroy();
	ES_SAFE_DELETE( g_renderer );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::LoadEffect( void* pData, int size, const wchar_t* Path )
{
	assert( g_effect == NULL );

	g_handles.clear();

	((TextureLoader*)g_manager->GetTextureLoader())->RootPath = std::wstring( Path );
	((ModelLoader*)g_manager->GetModelLoader())->RootPath = std::wstring( Path );
	
	SoundLoader* soundLoader = (SoundLoader*)g_manager->GetSoundLoader();
	if( soundLoader )
	{
		soundLoader->RootPath = std::wstring( Path );
	}

	g_effect = Effekseer::Effect::Create( g_manager, pData, size );
	assert( g_effect != NULL );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::RemoveEffect()
{
	ES_SAFE_RELEASE( g_effect );
	g_handles.clear();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::PlayEffect()
{
	assert( g_effect != NULL );

	for( int32_t z = 0; z < m_effectBehavior.CountZ; z++ )
	{
	for( int32_t y = 0; y < m_effectBehavior.CountY; y++ )
	{
	for( int32_t x = 0; x < m_effectBehavior.CountX; x++ )
	{
		auto lenX = m_effectBehavior.Distance * (m_effectBehavior.CountX - 1);
		auto lenY = m_effectBehavior.Distance * (m_effectBehavior.CountY - 1);
		auto lenZ = m_effectBehavior.Distance * (m_effectBehavior.CountZ - 1);

		auto posX = m_effectBehavior.Distance * x - lenX / 2.0f;
		auto posY = m_effectBehavior.Distance * y - lenY / 2.0f;
		auto posZ = m_effectBehavior.Distance * z - lenZ / 2.0f;

		posX += m_effectBehavior.PositionX;
		posY += m_effectBehavior.PositionY;
		posZ += m_effectBehavior.PositionZ;

		g_handles.push_back( g_manager->Play( g_effect, x, y, z ) );
	}
	}
	}
	
	
	m_time = 0;
	m_rootLocation.X = m_effectBehavior.PositionX;
	m_rootLocation.Y = m_effectBehavior.PositionY;
	m_rootLocation.Z = m_effectBehavior.PositionZ;
	m_rootRotation.X = m_effectBehavior.RotationX;
	m_rootRotation.Y = m_effectBehavior.RotationY;
	m_rootRotation.Z = m_effectBehavior.RotationZ;
	m_rootScale.X = m_effectBehavior.ScaleX;
	m_rootScale.Y = m_effectBehavior.ScaleY;
	m_rootScale.Z = m_effectBehavior.ScaleZ;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StopEffect()
{
	if( g_handles.size() <= 0 ) return false;

	for( size_t i = 0; i < g_handles.size(); i++ )
	{
		g_manager->StopEffect( g_handles[i] );
	}
	g_handles.clear();

	g_manager->Update();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StepEffect( int frame )
{
	if( frame <= 0 )
	{
	}
	else if( frame == 1 )
	{
		StepEffect();
	}
	else
	{
		bool mute = g_sound->GetMute();
		g_sound->SetMute(true);

		for( size_t i = 0; i < g_handles.size(); i++ )
		{
			g_manager->SetShown( g_handles[i], false );
		}

		for( int i = 0; i < frame - 1; i++ )
		{
			StepEffect();
		}

		for( size_t i = 0; i < g_handles.size(); i++ )
		{
			g_manager->SetShown( g_handles[i], true );
		}

		g_sound->SetMute(mute);
		g_manager->Update();
	}
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StepEffect()
{
	if( m_time % m_step == 0 )
	{
		m_rootLocation.X += m_effectBehavior.PositionVelocityX;
		m_rootLocation.Y += m_effectBehavior.PositionVelocityY;
		m_rootLocation.Z += m_effectBehavior.PositionVelocityZ;

		m_rootRotation.X += m_effectBehavior.RotationVelocityX;
		m_rootRotation.Y += m_effectBehavior.RotationVelocityY;
		m_rootRotation.Z += m_effectBehavior.RotationVelocityZ;
		
		m_rootScale.X += m_effectBehavior.ScaleVelocityX;
		m_rootScale.Y += m_effectBehavior.ScaleVelocityY;
		m_rootScale.Z += m_effectBehavior.ScaleVelocityZ;

		int32_t index = 0;

		for( int32_t z = 0; z < m_effectBehavior.CountZ && index < g_handles.size(); z++ )
		{
		for( int32_t y = 0; y < m_effectBehavior.CountY && index < g_handles.size(); y++ )
		{
		for( int32_t x = 0; x < m_effectBehavior.CountX && index < g_handles.size(); x++ )
		{
			auto lenX = m_effectBehavior.Distance * (m_effectBehavior.CountX - 1);
			auto lenY = m_effectBehavior.Distance * (m_effectBehavior.CountY - 1);
			auto lenZ = m_effectBehavior.Distance * (m_effectBehavior.CountZ - 1);

			auto posX = m_effectBehavior.Distance * x - lenX / 2.0f;
			auto posY = m_effectBehavior.Distance * y - lenY / 2.0f;
			auto posZ = m_effectBehavior.Distance * z - lenZ / 2.0f;

			posX += m_rootLocation.X;
			posY += m_rootLocation.Y;
			posZ += m_rootLocation.Z;

			Effekseer::Matrix43 mat, matTra, matRot, matScale;
			matTra.Translation( posX, posY, posZ );
			matRot.RotationZXY( m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y );
			matScale.Scaling(  m_rootScale.X, m_rootScale.Y,  m_rootScale.Z );

			mat.Indentity();
			Effekseer::Matrix43::Multiple( mat, mat, matScale );
			Effekseer::Matrix43::Multiple( mat, mat, matRot );
			Effekseer::Matrix43::Multiple( mat, mat, matTra );

			g_manager->SetMatrix( g_handles[index], mat );

			g_manager->SetTargetLocation( g_handles[index], 
				m_effectBehavior.TargetPositionX,
				m_effectBehavior.TargetPositionY,
				m_effectBehavior.TargetPositionZ );

			index++;
		}
		}
		}

		if( m_time >= m_effectBehavior.RemovedTime )
		{
			for( size_t i = 0; i < g_handles.size(); i++ )
			{
				g_manager->StopRoot( g_handles[i] );
			}
		}

		g_manager->Update( (float)m_step );
	}

	m_time++;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Rotate( float x, float y )
{
	if( g_mouseRotDirectionInvX )
	{
		x = -x;
	}

	if( g_mouseRotDirectionInvY )
	{
		y = -y;
	}

	g_RotY += x;
	g_RotX += y;

	while( g_RotY >= 180.0f )
	{
		g_RotY -= 180.0f * 2.0f;
	}

	while( g_RotY <= - 180.0f )
	{
		g_RotY += 180.0f * 2.0f;
	}

	if( g_RotX > 180.0f / 2.0f )
	{
		g_RotX = 180.0f / 2.0f;
	}

	if( g_RotX < - 180.0f / 2.0f )
	{
		g_RotX = - 180.0f / 2.0f;
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Slide( float x, float y )
{
	if( ::g_mouseSlideDirectionInvX )
	{
		x = -x;
	}

	if( ::g_mouseSlideDirectionInvY )
	{
		y = -y;
	}

	::Effekseer::Vector3D up( 0, 1, 0 );
	::Effekseer::Vector3D right( 1, 0, 0 );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );
	mat_rot_y.RotationY( - g_RotY / 180.0f * PI );
	::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
	::Effekseer::Vector3D::Transform( up, up, mat );
	::Effekseer::Vector3D::Transform( right, right, mat );
	
	up.X = up.X * y;
	up.Y = up.Y * y;
	up.Z = up.Z * y;
	right.X = right.X * ( - x );
	right.Y = right.Y * ( - x );
	right.Z = right.Z * ( - x );

	::Effekseer::Vector3D v;
	v.X = up.X + right.X;
	v.Y = up.Y + right.Y;
	v.Z = up.Z + right.Z;
	g_focus_position.X += v.X;
	g_focus_position.Y += v.Y;
	g_focus_position.Z += v.Z;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Zoom( float zoom )
{
	g_Distance -= zoom;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetRandomSeed( int seed )
{
	srand( seed );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Record(const wchar_t* pathWithoutExt, const wchar_t* ext, int32_t count, int32_t offsetFrame, int32_t freq, bool isTranslucent)
{
	if (g_effect == NULL) return false;

	g_renderer->IsBackgroundTranslucent = isTranslucent;

	::Effekseer::Vector3D position(0, 0, g_Distance);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);
	mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
	::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
	::Effekseer::Vector3D::Transform(position, position, mat);
	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	g_renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	StopEffect();

	::Effekseer::Handle handle = g_manager->Play(g_effect, 0, 0, 0);

	g_manager->SetTargetLocation(handle,
		m_effectBehavior.TargetPositionX,
		m_effectBehavior.TargetPositionY,
		m_effectBehavior.TargetPositionZ);

	for (int i = 0; i < offsetFrame; i++)
	{
		g_manager->Update();
	}
	
	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) return false;

		g_renderer->BeginRendering();
		g_manager->Draw();
		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels);

		wchar_t path_[260];
		swprintf_s(path_, L"%s.%d%s", pathWithoutExt, i, ext);

		SavePNGImage(path_, g_renderer->GuideWidth, g_renderer->GuideHeight, pixels.data(), false);
	}

	g_manager->StopEffect(handle);
	g_manager->Update();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Record(const wchar_t* path, int32_t count, int32_t xCount, int32_t offsetFrame, int32_t freq, bool isTranslucent)
{
	if( g_effect == NULL ) return false;

	int32_t yCount = count / xCount;
	if (count != xCount * yCount) yCount++;

	std::vector<Effekseer::Color> pixels_out;
	pixels_out.resize((g_renderer->GuideWidth * xCount) * (g_renderer->GuideHeight * yCount));

	g_renderer->IsBackgroundTranslucent = isTranslucent;

	::Effekseer::Vector3D position( 0, 0, g_Distance );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );
	mat_rot_y.RotationY( - g_RotY / 180.0f * PI );
	::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
	::Effekseer::Vector3D::Transform( position, position, mat );
	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	g_renderer->GetRenderer()->SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	StopEffect();
	
	::Effekseer::Handle handle = g_manager->Play( g_effect, 0, 0, 0 );
	
	g_manager->SetTargetLocation(handle,
		m_effectBehavior.TargetPositionX,
		m_effectBehavior.TargetPositionY,
		m_effectBehavior.TargetPositionZ);

	for( int i = 0; i < offsetFrame; i++ )
	{
		g_manager->Update();
	}

	int32_t count_ = 0;
	for( int y = 0; y < yCount; y++ )
	{
		for( int x = 0; x < xCount; x++ )
		{
			if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) return false;

			g_renderer->BeginRendering();
			g_manager->Draw();
			g_renderer->EndRendering();

			for (int j = 0; j < freq; j++)
			{
				g_manager->Update();
			}

			count_++;
			if (count == count_)
			{
				g_manager->StopEffect(handle);
			}

			std::vector<Effekseer::Color> pixels;
			g_renderer->EndRecord(pixels);

			for (int32_t y_ = 0; y_ < g_renderer->GuideHeight; y_++)
			{
				for (int32_t x_ = 0; x_ < g_renderer->GuideWidth; x_++)
				{
					pixels_out[x * g_renderer->GuideWidth + x_ + (g_renderer->GuideWidth * xCount) * (g_renderer->GuideHeight * y + y_)] = pixels[x_ + y_ * g_renderer->GuideWidth];
				}
			}
		}
	}
	
Exit:;

	SavePNGImage(path, g_renderer->GuideWidth * xCount, g_renderer->GuideHeight * yCount, pixels_out.data(), false);
	
	g_manager->Update();

	return true;
}

bool Native::RecordAsGifAnimation(const wchar_t* path, int32_t count, int32_t offsetFrame, int32_t freq, bool isTranslucent)
{
	if (g_effect == NULL) return false;

	g_renderer->IsBackgroundTranslucent = isTranslucent;

	::Effekseer::Vector3D position(0, 0, g_Distance);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);
	mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
	::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
	::Effekseer::Vector3D::Transform(position, position, mat);
	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	g_renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	StopEffect();

	::Effekseer::Handle handle = g_manager->Play(g_effect, 0, 0, 0);

	g_manager->SetTargetLocation(handle,
		m_effectBehavior.TargetPositionX,
		m_effectBehavior.TargetPositionY,
		m_effectBehavior.TargetPositionZ);

	for (int i = 0; i < offsetFrame; i++)
	{
		g_manager->Update();
	}

	FILE*		fp = nullptr;
	gdImagePtr	img = nullptr;

	img = gdImageCreate(g_renderer->GuideWidth, g_renderer->GuideHeight);
	fp =  _wfopen(path, L"wb");
	gdImageGifAnimBegin(img, fp, false, 0);

	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) return false;

		g_renderer->BeginRendering();
		g_manager->Draw();
		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels);

		int delay = (int) round((1.0 / (double) 60.0 * freq) * 100.0);
		gdImagePtr frameImage = gdImageCreateTrueColor(g_renderer->GuideWidth, g_renderer->GuideHeight);

		for (int32_t y = 0; y < g_renderer->GuideHeight; y++)
		{
			for (int32_t x = 0; x < g_renderer->GuideWidth; x++)
			{
				auto c = pixels[x + y * g_renderer->GuideWidth];
				gdImageSetPixel(frameImage, x, y, gdTrueColor(c.R, c.G, c.B));
			}
		}
		gdImageTrueColorToPalette(frameImage, true, gdMaxColors);
		gdImageGifAnimAdd(frameImage, fp, true, 0, 0, delay, gdDisposalNone, NULL);
		gdImageDestroy(frameImage);
	}

	gdImageGifAnimEnd(fp);
	fclose(fp);
	gdImageDestroy(img);

	g_manager->StopEffect(handle);
	g_manager->Update();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerParamater Native::GetViewerParamater()
{
	assert( g_renderer != NULL );

	ViewerParamater paramater;

	paramater.GuideWidth = g_renderer->GuideWidth;
	paramater.GuideHeight = g_renderer->GuideHeight;
	paramater.IsPerspective = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE;
	paramater.IsOrthographic = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC;
	paramater.FocusX = g_focus_position.X;
	paramater.FocusY = g_focus_position.Y;
	paramater.FocusZ = g_focus_position.Z;
	paramater.AngleX = g_RotX;
	paramater.AngleY = g_RotY;
	paramater.Distance = g_Distance;
	paramater.RendersGuide = g_renderer->RendersGuide;
	paramater.RateOfMagnification = g_renderer->RateOfMagnification;

	return paramater;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetViewerParamater( ViewerParamater& paramater )
{
	assert( g_renderer != NULL );

	g_renderer->GuideWidth = paramater.GuideWidth;
	g_renderer->GuideHeight = paramater.GuideHeight;
	
	g_renderer->RateOfMagnification = paramater.RateOfMagnification;
	
	if( paramater.IsPerspective )
	{
		g_renderer->SetProjectionType( ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE );
	}

	if( paramater.IsOrthographic )
	{
		g_renderer->SetProjectionType( ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC );
	}

	g_focus_position.X = paramater.FocusX;
	g_focus_position.Y = paramater.FocusY;
	g_focus_position.Z = paramater.FocusZ;
	g_RotX = paramater.AngleX;
	g_RotY = paramater.AngleY;
	g_Distance = paramater.Distance;
	g_renderer->RendersGuide = paramater.RendersGuide;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerEffectBehavior Native::GetEffectBehavior()
{
	return m_effectBehavior;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetViewerEffectBehavior( ViewerEffectBehavior& behavior )
{
	m_effectBehavior = behavior;
	if( m_effectBehavior.CountX <= 0 ) m_effectBehavior.CountX = 1;
	if( m_effectBehavior.CountY <= 0 ) m_effectBehavior.CountY = 1;
	if( m_effectBehavior.CountZ <= 0 ) m_effectBehavior.CountZ = 1;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetSoundMute( bool mute )
{
	g_sound->SetMute( mute );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetSoundVolume( float volume )
{
	g_sound->SetVolume( volume );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::InvalidateTextureCache()
{
	{
		std::map<std::wstring,IDirect3DTexture9*>::iterator it = m_textures.begin();
		std::map<std::wstring,IDirect3DTexture9*>::iterator it_end = m_textures.end();
		while( it != it_end )
		{
			ES_SAFE_RELEASE( (*it).second );
			++it;
		}
		m_textures.clear();
	}

	{
		std::map<std::wstring,EffekseerRendererDX9::Model*>::iterator it = m_models.begin();
		std::map<std::wstring,EffekseerRendererDX9::Model*>::iterator it_end = m_models.end();
		while( it != it_end )
		{
			ES_SAFE_DELETE( (*it).second );
			++it;
		}
		m_models.clear();
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetIsGridShown( bool value, bool xy, bool xz, bool yz )
{
	g_renderer->IsGridShown = value;
	g_renderer->IsGridXYShown = xy;
	g_renderer->IsGridXZShown = xz;
	g_renderer->IsGridYZShown = yz;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetGridLength( float length )
{
	g_renderer->GridLength = length;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetBackgroundColor( uint8_t r, uint8_t g, uint8_t b )
{
	g_renderer->BackgroundColor.R = r;
	g_renderer->BackgroundColor.G = g;
	g_renderer->BackgroundColor.B = b;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetBackgroundImage( const wchar_t* path )
{
	FILE* fp_texture = _wfopen( path, L"rb" );
	if( fp_texture != NULL )
	{
		fseek( fp_texture, 0, SEEK_END );
		size_t size_texture = ftell( fp_texture );
		char* data_texture = new char[size_texture];
		fseek( fp_texture, 0, SEEK_SET );
		fread( data_texture, 1, size_texture, fp_texture );
		fclose( fp_texture );

		g_renderer->LoadBackgroundImage( data_texture, size_texture );

		delete [] data_texture;
	}
	else
	{
		g_renderer->LoadBackgroundImage( NULL, 0 );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetGridColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	g_renderer->GridColor.R = r;
	g_renderer->GridColor.G = g;
	g_renderer->GridColor.B = b;
	g_renderer->GridColor.A = a;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetMouseInverseFlag( bool rotX, bool rotY, bool slideX, bool slideY )
{
	g_mouseRotDirectionInvX = rotX;
	g_mouseRotDirectionInvY = rotY;
	
	g_mouseSlideDirectionInvX = slideX;
	g_mouseSlideDirectionInvY = slideY;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetStep( int32_t step )
{
	m_step = step;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StartNetwork( const char* host, uint16_t port )
{
	return g_client->Start( (char*)host, port );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::StopNetwork()
{
	g_client->Stop();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::IsConnectingNetwork()
{
	return g_client->IsConnected();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SendDataByNetwork( const wchar_t* key, void* data, int size, const wchar_t* path )
{
	g_client->Reload( (const EFK_CHAR *)key, data, size );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetLightDirection( float x, float y, float z )
{
	g_lightDirection = Effekseer::Vector3D( x, y, z );

	Effekseer::Vector3D temp = g_lightDirection;

	if( !g_renderer->IsRightHand )
	{
		temp.Z = - temp.Z;
	}

	g_renderer->GetRenderer()->SetLightDirection( temp );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetLightColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	g_renderer->GetRenderer()->SetLightColor( Effekseer::Color( r, g, b, a ) );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetLightAmbientColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	g_renderer->GetRenderer()->SetLightAmbientColor( Effekseer::Color( r, g, b, a ) );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetIsRightHand( bool value )
{
	g_renderer->IsRightHand = value;
	if( g_renderer->IsRightHand )
	{
		g_manager->SetCoordinateSystem( Effekseer::CoordinateSystem::RH );
	}
	else
	{
		g_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	}

	g_renderer->RecalcProjection();

	
	{
		Effekseer::Vector3D temp = g_lightDirection;

		if( !g_renderer->IsRightHand )
		{
			temp.Z = - temp.Z;
		}

		g_renderer->GetRenderer()->SetLightDirection( temp );
	}
}

void Native::SetCullingParameter( bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ)
{
	g_renderer->IsCullingShown = isCullingShown;
	g_renderer->CullingRadius = cullingRadius;
	g_renderer->CullingPosition.X = cullingX;
	g_renderer->CullingPosition.Y = cullingY;
	g_renderer->CullingPosition.Z = cullingZ;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------