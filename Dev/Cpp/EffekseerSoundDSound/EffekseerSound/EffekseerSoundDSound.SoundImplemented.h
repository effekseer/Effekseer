
#ifndef	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
#define	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <dsound.h>
#include "../EffekseerSoundDSound.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

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
	IDirectSound8*			m_dsound;

	SoundVoiceContainer*	m_voiceContainer;
	bool					m_mute;
	int32_t					m_leftPos, m_rightPos;

public:
	SoundImplemented();
	virtual ~SoundImplemented();

	void Destroy();

	bool Initialize( IDirectSound8* dsound );
	
	void SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up );
	
	::Effekseer::SoundPlayerRef CreateSoundPlayer();

	::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterface* fileInterface);
	
	void StopAllVoices();

	void SetMute( bool mute );

	bool GetMute()			{return m_mute;}

	IDirectSound8* GetDevice()	{return m_dsound;}

	SoundVoice* GetVoice();
	
	void StopTag( ::Effekseer::SoundTag tag );

	void PauseTag( ::Effekseer::SoundTag tag, bool pause );
	
	bool CheckPlayingTag( ::Effekseer::SoundTag tag );
	
	void StopData( const ::Effekseer::SoundDataRef& soundData );

	void SetPanRange( int32_t leftPos, int32_t rightPos );

	float CalculatePan( const Effekseer::Vector3D& position );

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