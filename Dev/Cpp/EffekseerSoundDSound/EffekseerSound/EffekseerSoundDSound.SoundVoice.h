
#ifndef	__EFFEKSEERRSOUND_SOUND_VOICE_H__
#define	__EFFEKSEERRSOUND_SOUND_VOICE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <list>
#include "../EffekseerSoundDSound.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundImplemented;

class SoundVoice
{
	SoundImplemented*		m_sound;
	SoundData*				m_data;
	IDirectSoundBuffer8*	m_dsbuf;
	::Effekseer::SoundTag	m_tag;

public:
	SoundVoice( SoundImplemented* sound );
	
	~SoundVoice();

	void Play( ::Effekseer::SoundTag tag, 
		const ::Effekseer::SoundPlayer::InstanceParameter& parameter );
	
	void Pause( bool pause );

	void Stop();

	bool CheckPlaying();

	::Effekseer::SoundTag GetTag() const	{return m_tag;}

	::Effekseer::SoundTag GetData() const	{return m_data;}
};

class SoundVoiceContainer
{
	std::list<SoundVoice*>	m_voiceList;

public:
	SoundVoiceContainer( SoundImplemented* sound, int num );
	
	~SoundVoiceContainer();

	SoundVoice* GetVoice();
	
	void StopTag( ::Effekseer::SoundTag tag );

	void PauseTag( ::Effekseer::SoundTag tag, bool pause );
	
	bool CheckPlayingTag( ::Effekseer::SoundTag tag );

	void StopData( SoundData* soundData );

	void StopAll();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRSOUND_SOUND_VOICE_H__