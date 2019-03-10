
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#include "Graphics/efk.PNGHelper.h"
#include "Graphics/efk.AVIExporter.h"
#include "Graphics/efk.GifHelper.h"

#ifdef _WIN32
#include "3rdParty/imgui_platform/imgui_impl_dx9.h"
#include "3rdParty/imgui_platform/imgui_impl_dx11.h"
#endif

#include "dll.h"

#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "d3dcompiler.lib")

#define NONDLL	1

#ifdef _WIN32
#define MSWIN32 1
#define BGDWIN32 1
#endif
#include <gd/gd.h>
#include <gd/gdfontmb.h>

#ifdef _WIN32
#if _DEBUG
#pragma comment(lib,"x86/Debug/libgd_static.lib")
#else
#pragma comment(lib,"x86/Release/libgd_static.lib")
#endif
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

static int		g_lastViewWidth = 0;
static int		g_lastViewHeight = 0;

bool Combine( const char16_t* rootPath, const char16_t* treePath,  char16_t* dst, int dst_length )
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

	// both pathes are none
	if( rootPathLength == 0 && treePathLength == 0 )
	{
		return false;
	}

	// either path is none
	if( rootPathLength == 0 )
	{
		if( treePathLength < dst_length )
		{
			memcpy( dst, treePath, sizeof(char16_t) * (treePathLength + 1) );
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
			memcpy( dst, rootPath, sizeof(char16_t) * (rootPathLength + 1) );
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// both exists

	// back to a directory separator
	int PathPosition = rootPathLength;
	while( PathPosition > 0 )
	{
		if( rootPath[ PathPosition - 1 ] == u'/' || rootPath[ PathPosition - 1 ] == u'\\' )
		{
			break;
		}
		PathPosition--;
	}

	// copy
	memcpy( dst, rootPath, sizeof(char16_t) * PathPosition );
	dst[ PathPosition ] = 0;

	// connect forcely
	if( PathPosition + treePathLength > dst_length )
	{
		return false;
	}

	memcpy( &(dst[ PathPosition ]), treePath, sizeof(char16_t) * treePathLength );
	PathPosition = PathPosition + treePathLength;
	dst[ PathPosition ] = 0;

	// execute ..\ or ../
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

	, Distortion		( DistortionType::Current )
	, RenderingMode		( RenderMode::Normal )

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

struct HandleHolder
{
	::Effekseer::Handle		Handle = 0;
	int32_t					Time = 0;
	bool					IsRootStopped = false;

	HandleHolder()
		: Handle(0)
		, Time(0)
	{
	}

	HandleHolder(::Effekseer::Handle handle, int32_t time = 0)
		: Handle(handle)
		, Time(time)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerTool::Renderer*		g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::EffekseerTool::Sound*			g_sound = NULL;
static std::map<std::u16string, Effekseer::TextureData*> m_textures;
static std::map<std::u16string,Effekseer::Model*> m_models;
static std::map<std::u16string, std::shared_ptr<efk::ImageResource>>	g_imageResources;

static std::vector<HandleHolder>	g_handles;

static ::Effekseer::Vector3D	g_focus_position;

static ::Effekseer::Client*		g_client = NULL;

static efk::DeviceType			g_deviceType = efk::DeviceType::OpenGL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::TextureLoader(EffekseerRenderer::Renderer* renderer)
	: m_renderer	( renderer )
{
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		auto r = (EffekseerRendererGL::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererGL::CreateTextureLoader();
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto r = (EffekseerRendererDX11::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
	}
	else
	{
		auto r = (EffekseerRendererDX9::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
	}
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::~TextureLoader()
{
	ES_SAFE_DELETE(m_originalTextureLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effekseer::TextureData* Native::TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	char16_t dst[260];
	Combine( RootPath.c_str(), (const char16_t *)path, dst, 260 );

	std::u16string key( dst );

	if( m_textures.count( key ) > 0 )
	{
		return m_textures[key];
	}
	else
	{
		auto t = m_originalTextureLoader->Load((EFK_CHAR*)dst, textureType);
		
		if (t != nullptr)
		{
			m_textures[key] = t;
		}

		return t;
	}

	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::TextureLoader::Unload(Effekseer::TextureData* data )
{
	//m_originalTextureLoader->Unload(data);
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
	Combine( RootPath.c_str(), (const char16_t *)path, (char16_t *)dst, 260 );

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
Native::ModelLoader::ModelLoader( EffekseerRenderer::Renderer* renderer )
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
	char16_t dst[260];
	Combine( RootPath.c_str(), (const char16_t *)path, dst, 260 );

	std::u16string key( dst );
	Effekseer::Model* model = NULL;

	if( m_models.count( key ) > 0 )
	{
		return m_models[ key ];
	}
	else
	{
		if (g_deviceType == efk::DeviceType::OpenGL)
		{
			auto r = (EffekseerRendererGL::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererGL::CreateModelLoader();
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
#ifdef _WIN32
		else if (g_deviceType == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererDX11::CreateModelLoader(r->GetDevice());
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
		else
		{
			auto r = (EffekseerRendererDX9::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererDX9::CreateModelLoader(r->GetDevice());
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
#endif
	}
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

::Effekseer::Effect* Native::GetEffect()
{
	return g_effect;
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
bool Native::CreateWindow_Effekseer(void* pHandle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	m_isSRGBMode = isSRGBMode;
	g_deviceType = deviceType;
	
	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	g_renderer = new ::EffekseerTool::Renderer(spriteCount, isSRGBMode, g_deviceType );
	if( g_renderer->Initialize( pHandle, width, height ) )
	{
		// 関数追加
		//::Effekseer::ScriptRegister::SetExternalFunction(0, print);

		g_manager = ::Effekseer::Manager::Create(spriteCount);

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

			m_textureLoader = new TextureLoader((EffekseerRenderer::Renderer *)g_renderer->GetRenderer());
			g_manager->SetTextureLoader(m_textureLoader);
			g_manager->SetModelLoader(new ModelLoader((EffekseerRenderer::Renderer *)g_renderer->GetRenderer()));
			
		}

		// Assign device lost events.
		g_renderer->LostedDevice = [this]() -> void
		{

			this->InvalidateTextureCache();
			auto e = this->GetEffect();
			if (e != nullptr)
			{
				e->UnloadResources();
			}

			{
				Effekseer::TextureLoader* loader = nullptr;
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
					auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererGL::CreateTextureLoader();
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
					auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
				}
				else
				{
					auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
				}
#endif
				for (auto& resource : g_imageResources)
				{
					loader->Unload(resource.second->GetTextureData());
					resource.second->GetTextureData() = nullptr;
				}

				delete loader;
			}

			{
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{

				}
				else
				{
					ImGui_ImplDX9_InvalidateDeviceObjects();
				}
#endif
			}
		};

		g_renderer->ResettedDevice = [this]() -> void
		{
			auto e = this->GetEffect();
			if (e != nullptr)
			{
				e->ReloadResources();
			}

			{
				Effekseer::TextureLoader* loader = nullptr;
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
					auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererGL::CreateTextureLoader();
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
					auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
				}
				else
				{
					auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
				}
#endif
				for (auto& resource : g_imageResources)
				{
					resource.second->GetTextureData() = loader->Load(resource.second->GetPath(), Effekseer::TextureType::Color);
				}

				delete loader;
			}

			{
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
				}
				else
				{
					ImGui_ImplDX9_CreateDeviceObjects();
				}
#endif
			}
		};
	}
	else
	{
		ES_SAFE_DELETE( g_renderer );
		return false;
	}

	g_sound = new ::EffekseerTool::Sound();
	if( g_sound->Initialize() )
	{
		g_manager->SetSoundPlayer( g_sound->GetSound()->CreateSoundPlayer() );
		g_manager->SetSoundLoader( new SoundLoader( g_sound->GetSound()->CreateSoundLoader() ) );
	}

	return true;
}

void Native::ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	g_renderer->GetGraphics()->Clear(Effekseer::Color(r, g, b, a));
}

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
	g_sound->Update();

	return true;
}

void Native::RenderWindow()
{
	g_renderer->BeginRendering();

	if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
	{
		g_manager->DrawBack();

		// HACK
		g_renderer->GetRenderer()->EndRendering();

		g_renderer->CopyToBackground();

		// HACK
		g_renderer->GetRenderer()->BeginRendering();
		g_manager->DrawFront();
	}
	else
	{
		g_manager->Draw();
	}

	g_renderer->EndRendering();
}

void Native::Present()
{
	g_renderer->Present();
}

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
		g_manager->StopEffect( g_handles[i].Handle );
	}
	g_handles.clear();
	
	InvalidateTextureCache();

	{
		Effekseer::TextureLoader* loader = nullptr;
		if (g_deviceType == efk::DeviceType::OpenGL)
		{
			auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererGL::CreateTextureLoader();
		}
#ifdef _WIN32
		else if (g_deviceType == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
		}
		else
		{
			auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
		}
#endif
		for (auto& resource : g_imageResources)
		{
			loader->Unload(resource.second->GetTextureData());
		}
		
		delete loader;
	}

	ES_SAFE_RELEASE( g_effect );

	g_manager->Destroy();
	ES_SAFE_DELETE( g_renderer );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::LoadEffect( void* pData, int size, const char16_t* Path )
{
	assert( g_effect == NULL );

	g_handles.clear();

	((TextureLoader*)g_manager->GetTextureLoader())->RootPath = std::u16string( Path );
	((ModelLoader*)g_manager->GetModelLoader())->RootPath = std::u16string( Path );
	
	SoundLoader* soundLoader = (SoundLoader*)g_manager->GetSoundLoader();
	if( soundLoader )
	{
		soundLoader->RootPath = std::u16string( Path );
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

		HandleHolder handleHolder(g_manager->Play(g_effect, posX, posY, posZ));

		Effekseer::Matrix43 mat, matTra, matRot, matScale;
		matTra.Translation(posX, posY, posZ);
		matRot.RotationZXY(m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y);
		matScale.Scaling(m_rootScale.X, m_rootScale.Y, m_rootScale.Z);

		mat.Indentity();
		Effekseer::Matrix43::Multiple(mat, mat, matScale);
		Effekseer::Matrix43::Multiple(mat, mat, matRot);
		Effekseer::Matrix43::Multiple(mat, mat, matTra);

		g_manager->SetMatrix(handleHolder.Handle, mat);

		g_handles.push_back(handleHolder);

		if (m_effectBehavior.AllColorR != 255 ||
			m_effectBehavior.AllColorG != 255 ||
			m_effectBehavior.AllColorB != 255 ||
			m_effectBehavior.AllColorA != 255)
		{
			g_manager->SetAllColor(
				handleHolder.Handle,
				Effekseer::Color(
				m_effectBehavior.AllColorR,
				m_effectBehavior.AllColorG,
				m_effectBehavior.AllColorB,
				m_effectBehavior.AllColorA));
		}
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
		g_manager->StopEffect( g_handles[i].Handle );
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
			g_manager->SetShown( g_handles[i].Handle, false );
		}

		for( int i = 0; i < frame - 1; i++ )
		{
			StepEffect();
		}

		for( size_t i = 0; i < g_handles.size(); i++ )
		{
			g_manager->SetShown( g_handles[i].Handle, true );
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
	if (m_effectBehavior.TimeSpan > 0 && m_time > 0 && m_time % m_effectBehavior.TimeSpan == 0)
	{
		PlayEffect();
	}

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

			g_manager->SetMatrix( g_handles[index].Handle, mat );

			g_manager->SetTargetLocation(
				g_handles[index].Handle, 
				m_effectBehavior.TargetPositionX,
				m_effectBehavior.TargetPositionY,
				m_effectBehavior.TargetPositionZ );

			index++;
		}
		}
		}

		
		for( size_t i = 0; i < g_handles.size(); i++ )
		{
			if (g_handles[i].Time >= m_effectBehavior.RemovedTime)
			{
				g_manager->StopRoot(g_handles[i].Handle);
				g_handles[i].IsRootStopped = true;
			}
		}
		

		g_manager->Update( (float)m_step );

		for (size_t i = 0; i < g_handles.size(); i++)
		{
			g_handles[i].Time++;
		}
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

void* Native::RenderView(int32_t width, int32_t height)
{
	g_lastViewWidth = width;
	g_lastViewHeight = height;

	g_renderer->BeginRenderToView(width, height);
	RenderWindow();
	g_renderer->EndRenderToView();
	return (void*)g_renderer->GetViewID();
}

bool Native::Record(const char16_t* pathWithoutExt, const char16_t* ext, int32_t count, int32_t offsetFrame, int32_t freq, TransparenceType transparenceType)
{
	if (g_effect == NULL) return false;

	g_renderer->IsBackgroundTranslucent = transparenceType == TransparenceType::Original;

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
	
	g_renderer->BeginRenderToView(g_lastViewWidth, g_lastViewHeight);

	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight))  goto Exit;

		g_renderer->BeginRendering();
		
		if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
		{
			g_manager->DrawBack();

			// HACK
			g_renderer->GetRenderer()->EndRendering();

			g_renderer->CopyToBackground();

			// HACK
			g_renderer->GetRenderer()->BeginRendering();
			g_manager->DrawFront();
		}
		else
		{
			g_manager->Draw();
		}

		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels, transparenceType == TransparenceType::Generate, transparenceType == TransparenceType::None);

		char16_t path_[260];
		
#ifdef _WIN32
		auto p_ = (wchar_t*)path_;
		swprintf_s(p_, 260, L"%s.%d%s", pathWithoutExt, i, ext);
#else
        
        char pathWOE[256];
        char ext_[256];
        char path8_dst[256];
        Effekseer::ConvertUtf16ToUtf8( (int8_t*)pathWOE, 256, (const int16_t*)pathWithoutExt );
        Effekseer::ConvertUtf16ToUtf8( (int8_t*)ext_, 256, (const int16_t*)ext );
        sprintf(path8_dst, "%s.%d%s", pathWOE, i, ext_);
        Effekseer::ConvertUtf8ToUtf16( (int16_t*)path_, 260, (const int8_t*)path8_dst );
#endif

		efk::PNGHelper pngHelper;
		pngHelper.Save((char16_t*)path_, g_renderer->GuideWidth, g_renderer->GuideHeight, pixels.data());
	}

Exit:;

	g_manager->StopEffect(handle);
	g_manager->Update();

	g_renderer->EndRenderToView();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Record(const char16_t* path, int32_t count, int32_t xCount, int32_t offsetFrame, int32_t freq, TransparenceType transparenceType)
{
	if( g_effect == NULL ) return false;

	int32_t yCount = count / xCount;
	if (count != xCount * yCount) yCount++;

	std::vector<Effekseer::Color> pixels_out;
	pixels_out.resize((g_renderer->GuideWidth * xCount) * (g_renderer->GuideHeight * yCount));

	g_renderer->IsBackgroundTranslucent = transparenceType == TransparenceType::Original;

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

	g_renderer->BeginRenderToView(g_lastViewWidth, g_lastViewHeight);

	int32_t count_ = 0;
	for( int y = 0; y < yCount; y++ )
	{
		for( int x = 0; x < xCount; x++ )
		{
			if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) goto Exit;

			g_renderer->BeginRendering();

			if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
			{
				g_manager->DrawBack();

				// HACK
				g_renderer->GetRenderer()->EndRendering();

				g_renderer->CopyToBackground();

				// HACK
				g_renderer->GetRenderer()->BeginRendering();
				g_manager->DrawFront();
			}
			else
			{
				g_manager->Draw();
			}

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
			g_renderer->EndRecord(pixels, transparenceType == TransparenceType::Generate, transparenceType == TransparenceType::None);

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

	efk::PNGHelper pngHelper;
	pngHelper.Save((char16_t*)path, g_renderer->GuideWidth * xCount, g_renderer->GuideHeight * yCount, pixels_out.data());

	g_manager->Update();

	g_renderer->EndRenderToView();

	return true;
}

bool Native::RecordAsGifAnimation(const char16_t* path, int32_t count, int32_t offsetFrame, int32_t freq, TransparenceType transparenceType)
{
	if (g_effect == NULL) return false;

	g_renderer->IsBackgroundTranslucent = transparenceType == TransparenceType::Original;

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

	g_renderer->BeginRenderToView(g_lastViewWidth, g_lastViewHeight);

	efk::GifHelper helper;
	helper.Initialize(path, g_renderer->GuideWidth, g_renderer->GuideHeight, freq);

	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) goto End;

		g_renderer->BeginRendering();

		if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
		{
			g_manager->DrawBack();

			// HACK
			g_renderer->GetRenderer()->EndRendering();

			g_renderer->CopyToBackground();

			// HACK
			g_renderer->GetRenderer()->BeginRendering();
			g_manager->DrawFront();
		}
		else
		{
			g_manager->Draw();
		}

		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels, transparenceType == TransparenceType::Generate, transparenceType == TransparenceType::None);

		helper.AddImage(pixels);

		/*
		int delay = (int)round((1.0 / (double) 60.0 * freq) * 100.0);
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
		*/
	}


	/*
	FILE*		fp = nullptr;
	gdImagePtr	img = nullptr;

	img = gdImageCreate(g_renderer->GuideWidth, g_renderer->GuideHeight);
	
#ifdef _WIN32
	_wfopen_s(&fp, (const wchar_t*)path, L"rb");
#else
	int8_t path8[256];
	ConvertUtf16ToUtf8(path8, 256, (const int16_t*)path);
	fp = fopen((const char*)path8, "rb");
#endif

	gdImageGifAnimBegin(img, fp, false, 0);

	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) return false;

		g_renderer->BeginRendering();
	
		if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
		{
			g_manager->DrawBack();

			// HACK
			g_renderer->GetRenderer()->EndRendering();

			g_renderer->CopyToBackground();

			// HACK
			g_renderer->GetRenderer()->BeginRendering();
			g_manager->DrawFront();
		}
		else
		{
			g_manager->Draw();
		}

		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels, transparenceType == TransparenceType::Generate, transparenceType == TransparenceType::None);

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
	*/

End:;

	g_manager->StopEffect(handle);
	g_manager->Update();

	g_renderer->EndRenderToView();

	return true;
}

bool Native::RecordAsAVI(const char16_t* path, int32_t count, int32_t offsetFrame, int32_t freq, TransparenceType transparenceType)
{
	if (g_effect == NULL) return false;

	g_renderer->IsBackgroundTranslucent = transparenceType == TransparenceType::Original;

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

	FILE* fp = nullptr;
#ifdef _WIN32
    _wfopen_s( &fp, (const wchar_t*)path, L"wb" );
#else
    int8_t path8[256];
    Effekseer::ConvertUtf16ToUtf8( path8, 256, (const int16_t*)path );
    fp = fopen( (const char*)path8, "wb" );
#endif

    if (fp == nullptr) return false;

    
	efk::AVIExporter exporter;
	exporter.Initialize(g_renderer->GuideWidth, g_renderer->GuideHeight, (int32_t)(60.0f / (float)freq), count);

	std::vector<uint8_t> d;
	exporter.BeginToExportAVI(d);
	fwrite(d.data(), 1, d.size(), fp);

	g_renderer->BeginRenderToView(g_lastViewWidth, g_lastViewHeight);

	for (int32_t i = 0; i < count; i++)
	{
		if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight)) goto End;
        
		g_renderer->BeginRendering();
	
		if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
		{
			g_manager->DrawBack();

			// HACK
			g_renderer->GetRenderer()->EndRendering();

			g_renderer->CopyToBackground();

			// HACK
			g_renderer->GetRenderer()->BeginRendering();
			g_manager->DrawFront();
		}
		else
		{
			g_manager->Draw();
		}

		g_renderer->EndRendering();

		for (int j = 0; j < freq; j++)
		{
			g_manager->Update();
		}

		std::vector<Effekseer::Color> pixels;
		g_renderer->EndRecord(pixels, transparenceType == TransparenceType::Generate, transparenceType == TransparenceType::None);

		exporter.ExportFrame(d, pixels);
		fwrite(d.data(), 1, d.size(), fp);
	}

	exporter.FinishToExportAVI(d);
	fwrite(d.data(), 1, d.size(), fp);

End:;

	fclose(fp);

	g_manager->StopEffect(handle);
	g_manager->Update();

	g_renderer->EndRenderToView();

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
	
	paramater.Distortion = (DistortionType)g_renderer->Distortion;
	paramater.RenderingMode = (RenderMode)g_renderer->RenderingMode;

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

	g_renderer->Distortion = (::EffekseerTool::eDistortionType)paramater.Distortion;
	g_renderer->RenderingMode = (::Effekseer::RenderMode)paramater.RenderingMode;
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
		auto it = m_textures.begin();
		auto it_end = m_textures.end();
		while( it != it_end )
		{
			m_textureLoader->GetOriginalTextureLoader()->Unload( (*it).second );
			++it;
		}
		m_textures.clear();
	}

	{
		auto it = m_models.begin();
		auto it_end = m_models.end();
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
void Native::SetBackgroundImage( const char16_t* path )
{
	g_renderer->LoadBackgroundImage(path);
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
void Native::SendDataByNetwork( const char16_t* key, void* data, int size, const char16_t* path )
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


efk::ImageResource* Native::LoadImageResource(const char16_t* path)
{
	Effekseer::TextureLoader* loader = nullptr;
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererGL::CreateTextureLoader();
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
	}
	else
	{
		auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
	}
#endif

	auto resource = std::make_shared<efk::ImageResource>();

	resource->GetTextureData() = loader->Load(path, Effekseer::TextureType::Color);
	resource->SetPath(path);

	if (g_imageResources[path] != nullptr)
	{
		loader->Unload(g_imageResources[path]->GetTextureData());
	}
	
	g_imageResources[path] = resource;

	delete loader;

	return resource.get();
}

int32_t Native::GetAndResetDrawCall()
{
	auto call = g_renderer->GetRenderer()->GetDrawCallCount();
	g_renderer->GetRenderer()->ResetDrawCallCount();
	return call;
}

int32_t Native::GetAndResetVertexCount()
{
	auto call = g_renderer->GetRenderer()->GetDrawVertexCount();
	g_renderer->GetRenderer()->ResetDrawVertexCount();
	return call;
}

int32_t Native::GetInstanceCount()
{
	if (m_time == 0) return 0;

	int32_t sum = 0;
	for (int i = 0; i < g_handles.size(); i++)
	{
		auto count = g_manager->GetInstanceCount(g_handles[i].Handle);
		
		// Root
		if (!g_handles[i].IsRootStopped) count--;

		if (!g_manager->Exists(g_handles[i].Handle)) count = 0;

		sum += count;
	}

	return sum;
}

float Native::GetFPS()
{
	return 60.0;
}

bool Native::IsDebugMode()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

EffekseerRenderer::Renderer* Native::GetRenderer()
{
	return g_renderer->GetRenderer();
}
