
#ifndef	__EFFEKSEERRSOUND_SOUND_VOICE_H__
#define	__EFFEKSEERRSOUND_SOUND_VOICE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <list>
#include "../EffekseerSoundXAudio2.h"

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
	IXAudio2SourceVoice*	m_xavoice;
	::Effekseer::SoundTag	m_tag;
	SoundData*				m_data;

public:
	SoundVoice( SoundImplemented* sound, const WAVEFORMATEX* format );
	
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
	SoundVoiceContainer( SoundImplemented* sound, int num, const WAVEFORMATEX* format );
	
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