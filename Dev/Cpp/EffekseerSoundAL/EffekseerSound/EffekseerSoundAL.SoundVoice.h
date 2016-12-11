
#ifndef	__EFFEKSEERRSOUND_SOUND_VOICE_H__
#define	__EFFEKSEERRSOUND_SOUND_VOICE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <list>
#include "../EffekseerSoundAL.h"

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
	ALuint					m_source;
	::Effekseer::SoundTag	m_tag;

public:
	SoundVoice( ALuint source );
	
	~SoundVoice();

	void Play( ::Effekseer::SoundTag tag, 
		const ::Effekseer::SoundPlayer::InstanceParameter& parameter );
	
	void Pause( bool pause );

	void Stop();

	bool CheckPlaying();

	::Effekseer::SoundTag GetTag()	{return m_tag;}
};

class SoundVoiceContainer
{
	ALuint* m_sources;
	int m_num;
	std::list<SoundVoice*>	m_voiceList;

public:
	SoundVoiceContainer( SoundImplemented* sound, int num );
	
	~SoundVoiceContainer();

	SoundVoice* GetVoice();
	
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
#endif	// __EFFEKSEERRSOUND_SOUND_VOICE_H__