
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <windows.h>
#include <xaudio2.h>
#include "../Effekseer/Effekseer.h"
#include "../EffekseerSoundXAudio2/EffekseerSoundXAudio2.h"
#include "sound.h"

#if _DEBUG
#pragma comment(lib, "EffekseerSoundXAudio2.Debug.lib" )
#else
#pragma comment(lib, "EffekseerSoundXAudio2.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static IXAudio2*				g_xaudio2 = NULL;
static IXAudio2MasteringVoice*	g_masteringVoice = NULL;
static ::EffekseerSound::Sound*	g_sound = NULL;
extern ::Effekseer::Manager*	g_manager;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitSound(  void* handle1, void* handle2 )
{
	HRESULT hr;
	
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = XAudio2Create(&g_xaudio2, 0);
	hr = g_xaudio2->CreateMasteringVoice(&g_masteringVoice, 2, 44100);

	g_sound = EffekseerSound::Sound::Create(g_xaudio2, 16, 16);
	
	g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermSound()
{
	g_xaudio2->StopEngine();

	if( g_sound != NULL )
	{
		g_sound->Destory();
		g_sound = NULL;
	}
	if( g_masteringVoice != NULL )
	{
		g_masteringVoice->DestroyVoice();
		g_masteringVoice = NULL;
	}
	if( g_xaudio2 != NULL )
	{
		g_xaudio2->Release();
		g_xaudio2 = NULL;
	}
	CoUninitialize();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------