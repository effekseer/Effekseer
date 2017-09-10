

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.ModelLoader.h"
#include "Effekseer.DefaultEffectLoader.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

static void PathCombine(EFK_CHAR* dst, const EFK_CHAR* src1, const EFK_CHAR* src2)
{
	int len1 = 0, len2 = 0;
	if( src1 != NULL )
	{
		for( len1 = 0; src1[len1] != L'\0'; len1++ ) {}
		memcpy( dst, src1, len1 * sizeof(EFK_CHAR) );
		if( len1 > 0 && src1[len1 - 1] != L'/' && src1[len1 - 1] != L'\\' )
		{
			dst[len1++] = L'/';
		}
	}
	if( src2 != NULL)
	{
		for( len2 = 0; src2[len2] != L'\0'; len2++ ) {}
		memcpy( &dst[len1], src2, len2 * sizeof(EFK_CHAR) );
	}
	dst[len1 + len2] = L'\0';
}

static void GetParentDir(EFK_CHAR* dst, const EFK_CHAR* src)
{
	int i, last = -1;
	for( i = 0; src[i] != L'\0'; i++ )
	{
		if( src[i] == L'/' || src[i] == L'\\' )
			last = i;
	}
	if( last >= 0 )
	{
		memcpy( dst, src, last * sizeof(EFK_CHAR) );
		dst[last] = L'\0';
	}
	else
	{
		dst[0] = L'\0';
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create( Manager* manager, void* data, int32_t size, float magnification, const EFK_CHAR* materialPath )
{
	return EffectImplemented::Create( manager, data, size, magnification, materialPath );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create(Manager* manager, const EFK_CHAR* path, float magnification, const EFK_CHAR* materialPath)
{
	Setting* setting = manager->GetSetting();

	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == NULL) return NULL;

	void* data = NULL;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size)) return NULL;

	EFK_CHAR parentDir[512];
	if (materialPath == NULL)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	Effect* effect = EffectImplemented::Create(manager, data, size, magnification, materialPath);

	eLoader->Unload(data, size);

	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectImplemented::Create( Manager* pManager, void* pData, int size, float magnification, const EFK_CHAR* materialPath )
{
	if( pData == NULL || size == 0 ) return NULL;

	EffectImplemented* effect = new EffectImplemented( pManager, pData, size );
	if ( !effect->Load( pData, size, magnification, materialPath ) )
	{
		effect->Release();
		effect = NULL;
	}
	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create( Setting* setting, void* data, int32_t size, float magnification, const EFK_CHAR* materialPath )
{
	return EffectImplemented::Create(setting, data, size, magnification, materialPath );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create( Setting* setting, const EFK_CHAR* path, float magnification, const EFK_CHAR* materialPath)
{
	if(setting == NULL) return NULL;
	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == NULL) return NULL;

	void* data = NULL;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size)) return NULL;

	EFK_CHAR parentDir[512];
	if (materialPath == NULL)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	Effect* effect = EffectImplemented::Create(setting, data, size, magnification, materialPath);

	eLoader->Unload(data, size);

	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectImplemented::Create( Setting* setting, void* pData, int size, float magnification, const EFK_CHAR* materialPath )
{
	if( pData == NULL || size == 0 ) return NULL;

	EffectImplemented* effect = new EffectImplemented( setting, pData, size );
	if ( !effect->Load( pData, size, magnification, materialPath ) )
	{
		effect->Release();
		effect = NULL;
	}
	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::EffectLoader* Effect::CreateEffectLoader(::Effekseer::FileInterface* fileInterface)
{
	return new ::Effekseer::DefaultEffectLoader(fileInterface);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::EffectImplemented( Manager* pManager, void* pData, int size )
	: m_pManager		( (ManagerImplemented*)pManager )
	, m_setting			(NULL)
	, m_reference		( 1 )
	, m_version			( 0 )
	, m_ImageCount		( 0 )
	, m_ImagePaths		( NULL )
	, m_pImages			( NULL )
	, m_WaveCount		( 0 )
	, m_WavePaths		( NULL )
	, m_pWaves			( NULL )
	, m_modelCount		( 0 )
	, m_modelPaths		( NULL )
	, m_pModels			( NULL )
	, m_maginification	( 1.0f )
	, m_maginificationExternal	( 1.0f )
	, m_defaultRandomSeed	(-1)
	, m_pRoot			( NULL )

	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)

	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)

{
	ES_SAFE_ADDREF( m_pManager );

	Culling.Shape = CullingShape::NoneShape;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::EffectImplemented( Setting* setting, void* pData, int size )
	: m_pManager		( NULL )
	, m_setting			(setting)
	, m_reference		( 1 )
	, m_version			( 0 )
	, m_ImageCount		( 0 )
	, m_ImagePaths		( NULL )
	, m_pImages			( NULL )
	, m_WaveCount		( 0 )
	, m_WavePaths		( NULL )
	, m_pWaves			( NULL )
	, m_modelCount		( 0 )
	, m_modelPaths		( NULL )
	, m_pModels			( NULL )
	, m_maginification	( 1.0f )
	, m_maginificationExternal	( 1.0f )
	, m_pRoot			( NULL )

	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)

	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)
{
	ES_SAFE_ADDREF( m_setting );
	
	Culling.Shape = CullingShape::NoneShape;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::~EffectImplemented()
{
	Reset();

	ES_SAFE_RELEASE( m_setting );
	ES_SAFE_RELEASE( m_pManager );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectImplemented::GetRoot() const
{
	return m_pRoot;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float EffectImplemented::GetMaginification() const
{
	return m_maginification;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Load( void* pData, int size, float mag, const EFK_CHAR* materialPath )
{
	EffekseerPrintDebug("** Create : Effect\n");

	uint8_t* pos = (uint8_t*)pData;

	// EFKS
	int head = 0;
	memcpy( &head, pos, sizeof(int) );
	if( memcmp( &head, "SKFE", 4 ) != 0 ) return false;
	pos += sizeof( int );

	memcpy( &m_version, pos, sizeof(int) );
	pos += sizeof(int);

	// 画像
	memcpy( &m_ImageCount, pos, sizeof(int) );
	pos += sizeof(int);

	if( m_ImageCount > 0 )
	{
		m_ImagePaths = new EFK_CHAR*[ m_ImageCount ];
		m_pImages = new TextureData*[ m_ImageCount ];

		for( int i = 0; i < m_ImageCount; i++ )
		{
			int length = 0;
			memcpy( &length, pos, sizeof(int) );
			pos += sizeof(int);

			m_ImagePaths[i] = new EFK_CHAR[ length ];
			memcpy( m_ImagePaths[i], pos, length * sizeof(EFK_CHAR) );
			pos += length * sizeof(EFK_CHAR);

			m_pImages[i] = NULL;
		}
	}

	if (m_version >= 9)
	{
		// 画像
		memcpy(&m_normalImageCount, pos, sizeof(int));
		pos += sizeof(int);

		if (m_normalImageCount > 0)
		{
			m_normalImagePaths = new EFK_CHAR*[m_normalImageCount];
			m_normalImages = new TextureData*[m_normalImageCount];

			for (int i = 0; i < m_normalImageCount; i++)
			{
				int length = 0;
				memcpy(&length, pos, sizeof(int));
				pos += sizeof(int);

				m_normalImagePaths[i] = new EFK_CHAR[length];
				memcpy(m_normalImagePaths[i], pos, length * sizeof(EFK_CHAR));
				pos += length * sizeof(EFK_CHAR);

				m_normalImages[i] = NULL;
			}
		}

		// 画像
		memcpy(&m_distortionImageCount, pos, sizeof(int));
		pos += sizeof(int);

		if (m_distortionImageCount > 0)
		{
			m_distortionImagePaths = new EFK_CHAR*[m_distortionImageCount];
			m_distortionImages = new TextureData*[m_distortionImageCount];

			for (int i = 0; i < m_distortionImageCount; i++)
			{
				int length = 0;
				memcpy(&length, pos, sizeof(int));
				pos += sizeof(int);

				m_distortionImagePaths[i] = new EFK_CHAR[length];
				memcpy(m_distortionImagePaths[i], pos, length * sizeof(EFK_CHAR));
				pos += length * sizeof(EFK_CHAR);

				m_distortionImages[i] = NULL;
			}
		}
	}

	if( m_version >= 1 )
	{
		// ウェーブ
		memcpy( &m_WaveCount, pos, sizeof(int) );
		pos += sizeof(int);

		if( m_WaveCount > 0 )
		{
			m_WavePaths = new EFK_CHAR*[ m_WaveCount ];
			m_pWaves = new void*[ m_WaveCount ];

			for( int i = 0; i < m_WaveCount; i++ )
			{
				int length = 0;
				memcpy( &length, pos, sizeof(int) );
				pos += sizeof(int);

				m_WavePaths[i] = new EFK_CHAR[ length ];
				memcpy( m_WavePaths[i], pos, length * sizeof(EFK_CHAR) );
				pos += length * sizeof(EFK_CHAR);

				m_pWaves[i] = NULL;
			}
		}
	}

	if( m_version >= 6 )
	{
		/* モデル */
		memcpy( &m_modelCount, pos, sizeof(int) );
		pos += sizeof(int);

		if( m_modelCount > 0 )
		{
			m_modelPaths = new EFK_CHAR*[ m_modelCount ];
			m_pModels = new void*[ m_modelCount ];

			for( int i = 0; i < m_modelCount; i++ )
			{
				int length = 0;
				memcpy( &length, pos, sizeof(int) );
				pos += sizeof(int);

				m_modelPaths[i] = new EFK_CHAR[ length ];
				memcpy( m_modelPaths[i], pos, length * sizeof(EFK_CHAR) );
				pos += length * sizeof(EFK_CHAR);

				m_pModels[i] = NULL;
			}
		}
	}

	// 拡大率
	if( m_version >= 2 )
	{
		memcpy( &m_maginification, pos, sizeof(float) );
		pos += sizeof(float);
	}

	m_maginification *= mag;
	m_maginificationExternal = mag;

	if (m_version >= 11)
	{
		memcpy(&m_defaultRandomSeed, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}
	else
	{
		m_defaultRandomSeed = -1;
	}

	// カリング
	if( m_version >= 9 )
	{
		memcpy( &(Culling.Shape), pos, sizeof(int32_t) );
		pos += sizeof(int32_t);
		if(Culling.Shape ==	CullingShape::Sphere)
		{
			memcpy( &(Culling.Sphere.Radius), pos, sizeof(float) );
			pos += sizeof(float);
		
			memcpy( &(Culling.Location.X), pos, sizeof(float) );
			pos += sizeof(float);
			memcpy( &(Culling.Location.Y), pos, sizeof(float) );
			pos += sizeof(float);
			memcpy( &(Culling.Location.Z), pos, sizeof(float) );
			pos += sizeof(float);
		}
	}

	// ノード
	m_pRoot = EffectNodeImplemented::Create( this, NULL, pos );

	// リロード用にmaterialPathを記録しておく
    if (materialPath) m_materialPath = materialPath;

	ReloadResources( materialPath );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::Reset()
{
	UnloadResources();

	Setting* loader = GetSetting();

	TextureLoader* textureLoader = loader->GetTextureLoader();

	for( int i = 0; i < m_ImageCount; i++ )
	{
		if( m_ImagePaths[i] != NULL ) delete [] m_ImagePaths[i];
	}

	m_ImageCount = 0;

	ES_SAFE_DELETE_ARRAY( m_ImagePaths );
	ES_SAFE_DELETE_ARRAY(m_pImages);

	{
		for (int i = 0; i < m_normalImageCount; i++)
		{
			if (m_normalImagePaths[i] != NULL) delete [] m_normalImagePaths[i];
		}

		m_normalImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_normalImagePaths);
		ES_SAFE_DELETE_ARRAY(m_normalImages);
	}

	{
		for (int i = 0; i < m_distortionImageCount; i++)
		{
			if (m_distortionImagePaths[i] != NULL) delete [] m_distortionImagePaths[i];
		}

		m_distortionImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_distortionImagePaths);
		ES_SAFE_DELETE_ARRAY(m_distortionImages);
	}

	for( int i = 0; i < m_WaveCount; i++ )
	{
		if( m_WavePaths[i] != NULL ) delete [] m_WavePaths[i];
	}
	m_WaveCount = 0;

	ES_SAFE_DELETE_ARRAY( m_WavePaths );
	ES_SAFE_DELETE_ARRAY( m_pWaves );

	for( int i = 0; i < m_modelCount; i++ )
	{
		if( m_modelPaths[i] != NULL ) delete [] m_modelPaths[i];
	}
	m_modelCount = 0;

	ES_SAFE_DELETE_ARRAY( m_modelPaths );
	ES_SAFE_DELETE_ARRAY( m_pModels );

	ES_SAFE_DELETE( m_pRoot );
}

bool EffectImplemented::IsDyanamicMagnificationValid() const
{
	return GetVersion() >= 8 || GetVersion() < 2;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Manager* EffectImplemented::GetManager() const
{
	return m_pManager;	
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Setting* EffectImplemented::GetSetting() const
{
	if(m_setting != NULL) return m_setting;
	return m_pManager->GetSetting();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectImplemented::GetVersion() const
{
	return m_version;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureData* EffectImplemented::GetColorImage( int n ) const
{
	return m_pImages[ n ];
}

int32_t EffectImplemented::GetColorImageCount() const
{
	return m_ImageCount;
}

TextureData* EffectImplemented::GetNormalImage(int n) const
{
	/* 強制的に互換をとる */
	if (this->m_version <= 8)
	{
		return m_pImages[n];
	}

	return m_normalImages[n];
}

int32_t EffectImplemented::GetNormalImageCount() const
{
	return m_normalImageCount;
}

TextureData* EffectImplemented::GetDistortionImage(int n) const
{
	/* 強制的に互換をとる */
	if (this->m_version <= 8)
	{
		return m_pImages[n];
	}

	return m_distortionImages[n];
}

int32_t EffectImplemented::GetDistortionImageCount() const
{
	return m_distortionImageCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* EffectImplemented::GetWave( int n ) const
{
	return m_pWaves[ n ];
}

int32_t EffectImplemented::GetWaveCount() const
{
	return m_WaveCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* EffectImplemented::GetModel( int n ) const
{
	return m_pModels[ n ];
}

int32_t EffectImplemented::GetModelCount() const
{
	return m_modelCount;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload( void* data, int32_t size, const EFK_CHAR* materialPath )
{
	if(m_pManager == NULL ) return false;

	return Reload( m_pManager, 1, data, size, materialPath );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath )
{
	if(m_pManager == NULL ) return false;

	return Reload( m_pManager, 1, path, materialPath );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload( Manager* managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath )
{
	if(m_pManager == NULL ) return false;

	const EFK_CHAR* matPath = materialPath != NULL ? materialPath : m_materialPath.c_str();
	
	for( int32_t i = 0; i < managersCount; i++)
	{
		((ManagerImplemented*)&(managers[i]))->BeginReloadEffect( this );
	}

	Reset();
	Load( data, size, m_maginificationExternal, matPath );

	for( int32_t i = 0; i < managersCount; i++)
	{
		((ManagerImplemented*)&(managers[i]))->EndReloadEffect( this );
	}

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload( Manager* managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath )
{
	if(m_pManager == NULL ) return false;

	Setting* loader = GetSetting();
	
	EffectLoader* eLoader = loader->GetEffectLoader();
	if( loader == NULL ) return false;

	void* data = NULL;
	int32_t size = 0;

	if( !eLoader->Load( path, data, size ) ) return false;

	EFK_CHAR parentDir[512];
	if( materialPath == NULL )
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	for( int32_t i = 0; i < managersCount; i++)
	{
		((ManagerImplemented*)&(managers[i]))->BeginReloadEffect( this );
	}

	Reset();
	Load( data, size, m_maginificationExternal, materialPath );

	m_pManager->EndReloadEffect( this );

	for( int32_t i = 0; i < managersCount; i++)
	{
		((ManagerImplemented*)&(managers[i]))->EndReloadEffect( this );
	}

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::ReloadResources( const EFK_CHAR* materialPath )
{
	UnloadResources();

	const EFK_CHAR* matPath = materialPath != NULL ? materialPath : m_materialPath.c_str();
	
	Setting* loader = GetSetting();

	{
		TextureLoader* textureLoader = loader->GetTextureLoader();
		if( textureLoader != NULL )
		{
			for( int32_t ind = 0; ind < m_ImageCount; ind++ )
			{
				EFK_CHAR fullPath[512];
				PathCombine( fullPath, matPath, m_ImagePaths[ ind ] );
				m_pImages[ind] = textureLoader->Load( fullPath, TextureType::Color );
			}
		}
	}

	{
		TextureLoader* textureLoader = loader->GetTextureLoader();
		if (textureLoader != NULL)
		{
			for (int32_t ind = 0; ind < m_normalImageCount; ind++)
			{
				EFK_CHAR fullPath[512];
				PathCombine(fullPath, matPath, m_normalImagePaths[ind]);
				m_normalImages[ind] = textureLoader->Load(fullPath, TextureType::Normal);
			}
		}

	}
		{
			TextureLoader* textureLoader = loader->GetTextureLoader();
			if (textureLoader != NULL)
			{
				for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
				{
					EFK_CHAR fullPath[512];
					PathCombine(fullPath, matPath, m_distortionImagePaths[ind]);
					m_distortionImages[ind] = textureLoader->Load(fullPath, TextureType::Distortion);
				}
			}
		}

	

	{
		SoundLoader* soundLoader = loader->GetSoundLoader();
		if( soundLoader != NULL )
		{
			for( int32_t ind = 0; ind < m_WaveCount; ind++ )
			{
				EFK_CHAR fullPath[512];
				PathCombine( fullPath, matPath, m_WavePaths[ ind ] );
				m_pWaves[ind] = soundLoader->Load( fullPath );
			}
		}
	}

	{
		ModelLoader* modelLoader = loader->GetModelLoader();
		
		if( modelLoader != NULL )
		{
			for( int32_t ind = 0; ind < m_modelCount; ind++ )
			{
				EFK_CHAR fullPath[512];
				PathCombine( fullPath, matPath, m_modelPaths[ ind ] );
				m_pModels[ind] = modelLoader->Load( fullPath );
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::UnloadResources()
{
	Setting* loader = GetSetting();
	
	TextureLoader* textureLoader = loader->GetTextureLoader();
	if( textureLoader != NULL )
	{
		for( int32_t ind = 0; ind < m_ImageCount; ind++ )
		{
			textureLoader->Unload( m_pImages[ind] );
			m_pImages[ind] = NULL;
		}

		for (int32_t ind = 0; ind < m_normalImageCount; ind++)
		{
			textureLoader->Unload(m_normalImages[ind]);
			m_normalImages[ind] = NULL;
		}

		for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
		{
			textureLoader->Unload(m_distortionImages[ind]);
			m_distortionImages[ind] = NULL;
		}
	}

	SoundLoader* soundLoader = loader->GetSoundLoader();
	if( soundLoader != NULL )
	{
		for( int32_t ind = 0; ind < m_WaveCount; ind++ )
		{
			soundLoader->Unload( m_pWaves[ind] );
			m_pWaves[ind] = NULL;
		}
	}

	{
		ModelLoader* modelLoader = loader->GetModelLoader();
		if( modelLoader != NULL )
		{
			for( int32_t ind = 0; ind < m_modelCount; ind++ )
			{
				modelLoader->Unload( m_pModels[ind] );
				m_pModels[ind] = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
