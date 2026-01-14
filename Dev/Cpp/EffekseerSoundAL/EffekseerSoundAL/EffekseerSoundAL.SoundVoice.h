
#ifndef __EFFEKSEERRSOUND_SOUND_VOICE_H__
#define __EFFEKSEERRSOUND_SOUND_VOICE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundAL.h"
#include <list>

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
	ALuint source_ = 0;
	::Effekseer::SoundTag tag_ = nullptr;

public:
	SoundVoice(ALuint source);

	~SoundVoice();

	void Play(::Effekseer::SoundTag tag,
			  const ::Effekseer::SoundPlayer::InstanceParameter& parameter);

	void Pause(bool pause);

	void Stop();

	bool CheckPlaying();

	::Effekseer::SoundTag GetTag()
	{
		return tag_;
	}
};

class SoundVoiceContainer
{
	ALuint* sources_ = nullptr;
	int num_ = 0;
	std::list<SoundVoice*> voiceList_;

public:
	SoundVoiceContainer(SoundImplemented* sound, int num);

	~SoundVoiceContainer();

	SoundVoice* GetVoice();

	void StopTag(::Effekseer::SoundTag tag);

	void PauseTag(::Effekseer::SoundTag tag, bool pause);

	bool CheckPlayingTag(::Effekseer::SoundTag tag);

	void StopAll();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRSOUND_SOUND_VOICE_H__
