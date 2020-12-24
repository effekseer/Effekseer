
#ifndef	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
#define	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <XAudio2.h>
#include <X3DAudio.h>
#include "../EffekseerSoundXAudio2.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------


#if(_WIN32_WINNT < _WIN32_WINNT_WIN8)

#pragma comment(lib, "X3DAudio.lib")

#else

#if _DEBUG

#pragma comment(lib,"xaudio2.lib")

#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundVoice;
class SoundVoiceContainer;

class SoundImplemented : public Sound, public Effekseer::ReferenceObject
{
	IXAudio2*				m_xaudio2;
	X3DAUDIO_HANDLE			m_x3daudio;
	X3DAUDIO_LISTENER		m_listener;

	SoundVoiceContainer*	m_voiceContainer[2];
	bool					m_mute;

public:
	SoundImplemented();
	virtual ~SoundImplemented();

	void Destroy();

	bool Initialize( IXAudio2* xaudio2, int32_t num1chVoices, int32_t num2chVoices );
	
	void SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up );
	
	::Effekseer::SoundPlayerRef CreateSoundPlayer();

	::Effekseer::SoundLoaderRef CreateSoundLoader( ::Effekseer::FileInterface* fileInterface = NULL );
	
	void StopAllVoices();

	void SetMute( bool mute );

	bool GetMute()			{return m_mute;}

	IXAudio2* GetDevice()	{return m_xaudio2;}

	SoundVoice* GetVoice(int32_t channel);
	
	void StopTag( ::Effekseer::SoundTag tag );

	void PauseTag( ::Effekseer::SoundTag tag, bool pause );
	
	bool CheckPlayingTag( ::Effekseer::SoundTag tag );

	void StopData( const ::Effekseer::SoundDataRef& soundData );

	void Calculate3DSound(const ::Effekseer::Vector3D& position, 
		float distance, int32_t input, int32_t output, float matrix[]);

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};
using SoundImplementedRef = ::Effekseer::RefPtr<SoundImplemented>;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__