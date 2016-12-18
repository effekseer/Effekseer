
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#if _WIN32
#include <windows.h>
#endif

#if __APPLE__
#include <OpenAl/alc.h>
#else
#include <AL/alc.h>
#endif

#include "../Effekseer/Effekseer.h"
#include "../EffekseerSoundAL/EffekseerSoundAL.h"
#include "sound.h"

#if _WIN32
#pragma comment( lib, "../EffekseerSoundAL/AL/libs/Win32/OpenAL32.lib" )

#if _DEBUG
#pragma comment( lib, "EffekseerSoundAL.Debug.lib" )
#else
#pragma comment( lib, "EffekseerSoundAL.Release.lib" )
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static ALCdevice*	g_alcdev = NULL;
static ALCcontext*	g_alcctx = NULL;
static ::EffekseerSound::Sound*	g_sound = NULL;
extern ::Effekseer::Manager*	g_manager;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitSound()
{
	// OpenALデバイスを作成
	g_alcdev = alcOpenDevice(NULL);

	// OpenALコンテキストを作成
	g_alcctx = alcCreateContext(g_alcdev, NULL);

	alcMakeContextCurrent(g_alcctx);
	
	g_sound = EffekseerSound::Sound::Create( 32 );
	
	g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermSound()
{
	if( g_sound != NULL )
	{
		g_sound->Destory();
		g_sound = NULL;
	}

	alcDestroyContext(g_alcctx);
	alcCloseDevice(g_alcdev);
	
	g_alcctx = NULL;
	g_alcdev = NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------