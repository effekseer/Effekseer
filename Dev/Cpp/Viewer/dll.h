
#ifndef	__EFFEKSEER_DLL_H__
#define	__EFFEKSEER_DLL_H__

/**
	@file
	@brief	ÉcÅ[Éãå¸ÇØDLLèoóÕ
*/

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include "EffekseerRenderer/EffekseerRendererDX9.Renderer.h"
#include "EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"
#include "EffekseerTool/EffekseerTool.Sound.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ViewerParamater
{
public:
	int32_t	GuideWidth;
	int32_t	GuideHeight;
	float	RateOfMagnification;
	bool	IsPerspective;
	bool	IsOrthographic;
	float	FocusX;
	float	FocusY;
	float	FocusZ;
	float	AngleX;
	float	AngleY;
	float	Distance;
	bool	RendersGuide;

	bool	IsCullingShown;
	float	CullingRadius;
	float	CullingX;
	float	CullingY;
	float	CullingZ;

	ViewerParamater();
};

class ViewerEffectBehavior
{
public:
	int32_t	CountX;
	int32_t	CountY;
	int32_t	CountZ;
	float	Distance;

	int32_t	RemovedTime;

	float	PositionX;
	float	PositionY;
	float	PositionZ;

	float	RotationX;
	float	RotationY;
	float	RotationZ;

	float	ScaleX;
	float	ScaleY;
	float	ScaleZ;

	float	PositionVelocityX;
	float	PositionVelocityY;
	float	PositionVelocityZ;

	float	RotationVelocityX;
	float	RotationVelocityY;
	float	RotationVelocityZ;

	float	ScaleVelocityX;
	float	ScaleVelocityY;
	float	ScaleVelocityZ;
	
	float	TargetPositionX;
	float	TargetPositionY;
	float	TargetPositionZ;

	ViewerEffectBehavior();
};

class Native
{
private:

	class TextureLoader
	: public ::Effekseer::TextureLoader
	{
	private:
		EffekseerRenderer::Renderer*	m_renderer;
		bool							m_isSRGBMode = false;
	public:
		TextureLoader( EffekseerRenderer::Renderer* renderer, bool isSRGBMode );
		virtual ~TextureLoader();

	public:
		void* Load( const EFK_CHAR* path, ::Effekseer::TextureType textureType ) override;

		void Unload( void* data );

		std::wstring RootPath;
	};

	class SoundLoader
	: public ::Effekseer::SoundLoader
	{
	private:
		::Effekseer::SoundLoader* m_loader;
	
	public:
		SoundLoader( Effekseer::SoundLoader* loader );
		virtual ~SoundLoader();

	public:
		void* Load( const EFK_CHAR* path );

		void Unload( void* data );
		
		std::wstring RootPath;
	};

	class ModelLoader
	: public ::Effekseer::ModelLoader
	{
	private:
		EffekseerRendererDX9::Renderer*	m_renderer;

	public:
		ModelLoader( EffekseerRendererDX9::Renderer* renderer );
		virtual ~ModelLoader();

	public:
		void* Load( const EFK_CHAR* path );

		void Unload( void* data );

		std::wstring RootPath;
	};

	ViewerEffectBehavior	m_effectBehavior;

	int32_t				m_time;
	
	int					m_step;

	bool				m_isSRGBMode = false;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;
public:
	Native();

	~Native();

	bool CreateWindow_Effekseer( void* handle, int width, int height, bool isSRGBMode );

	bool UpdateWindow();

	bool ResizeWindow( int width, int height );

	bool DestroyWindow();

	bool LoadEffect( void* data, int size, const wchar_t* path );

	bool RemoveEffect();

	bool PlayEffect();

	bool StopEffect();

	bool StepEffect( int frame );

	bool StepEffect();

	bool Rotate( float x, float y );

	bool Slide( float x, float y );

	bool Zoom( float zoom );

	bool SetRandomSeed( int seed );

	bool Record(const wchar_t* pathWithoutExt, const wchar_t* ext, int32_t count, int32_t offsetFrame, int32_t freq, bool isTranslucent);

	bool Record(const wchar_t* path, int32_t count, int32_t xCount, int32_t offsetFrame, int32_t freq, bool isTranslucent);

	bool RecordAsGifAnimation(const wchar_t* path, int32_t count, int32_t offsetFrame, int32_t freq, bool isTranslucent);

	ViewerParamater GetViewerParamater();

	void SetViewerParamater( ViewerParamater& paramater );

	ViewerEffectBehavior GetEffectBehavior();

	void SetViewerEffectBehavior( ViewerEffectBehavior& behavior );

	bool SetSoundMute( bool mute );

	bool SetSoundVolume( float volume );
	
	bool InvalidateTextureCache();

	void SetIsGridShown( bool value, bool xy, bool xz, bool yz );

	void SetGridLength( float length );

	void SetBackgroundColor( uint8_t r, uint8_t g, uint8_t b );

	void SetBackgroundImage( const wchar_t* path );

	void SetGridColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

	void SetMouseInverseFlag( bool rotX, bool rotY, bool slideX, bool slideY );

	void SetStep( int32_t step );

	bool StartNetwork( const char* host, uint16_t port );

	void StopNetwork();

	bool IsConnectingNetwork();

	void SendDataByNetwork( const wchar_t* key, void* data, int size, const wchar_t* path );

	void SetLightDirection( float x, float y, float z );

	void SetLightColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

	void SetLightAmbientColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

	void SetIsRightHand( bool value );

	void SetCullingParameter( bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DLL_H__