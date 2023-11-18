
#ifndef	__EFFEKSEERRSOUND_SOUND_PLAYER_H__
#define	__EFFEKSEERRSOUND_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundPlayer
	: public ::Effekseer::SoundPlayer
{
private:
	SoundImplementedRef	m_sound;

public:
	SoundPlayer( const SoundImplementedRef& sound );

	virtual ~SoundPlayer();

public:
	::Effekseer::SoundHandle Play( ::Effekseer::SoundTag tag, 
		const ::Effekseer::SoundPlayer::InstanceParameter& parameter );
	
	void Stop( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag );

	void Pause( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag, bool pause );

	bool CheckPlaying( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag );

	void StopTag( ::Effekseer::SoundTag tag );

	void PauseTag( ::Effekseer::SoundTag tag, bool pause );
	
	bool CheckPlayingTag( ::Effekseer::SoundTag tag );

	void StopAll();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRSOUND_SOUND_PLAYER_H__