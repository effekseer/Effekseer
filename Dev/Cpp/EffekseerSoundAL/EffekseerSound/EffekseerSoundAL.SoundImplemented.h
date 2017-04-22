
#ifndef	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
#define	__EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundAL.h"

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

class SoundImplemented : public Sound
{
	SoundVoiceContainer*	m_voiceContainer;
	bool					m_mute;

public:
	SoundImplemented();
	virtual ~SoundImplemented();

	void Destroy();

	bool Initialize( int32_t numVoices );
	
	void SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up );
	
	::Effekseer::SoundPlayer* CreateSoundPlayer();

	::Effekseer::SoundLoader* CreateSoundLoader( ::Effekseer::FileInterface* fileInterface = NULL );
	
	void StopAllVoices();

	void SetMute( bool mute );

	bool GetMute()			{return m_mute;}

	SoundVoice* GetVoice();
	
	void StopTag( ::Effekseer::SoundTag tag );

	void PauseTag( ::Effekseer::SoundTag tag, bool pause );
	
	bool CheckPlayingTag( ::Effekseer::SoundTag tag );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__