
#ifndef __EFFEKSEERRSOUND_SOUND_VOICE_H__
#define __EFFEKSEERRSOUND_SOUND_VOICE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundXAudio2.h"
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
	SoundImplemented* sound_ = nullptr;
	IXAudio2SourceVoice* xavoice_ = nullptr;
	::Effekseer::SoundTag tag_ = nullptr;
	::Effekseer::SoundDataRef data_;

public:
	SoundVoice(SoundImplemented* sound, const WAVEFORMATEX* format);

	~SoundVoice();

	void Play(::Effekseer::SoundTag tag,
			  const ::Effekseer::SoundPlayer::InstanceParameter& parameter);

	void Pause(bool pause);

	void Stop();

	bool CheckPlaying();

	::Effekseer::SoundTag GetTag() const
	{
		return tag_;
	}

	const ::Effekseer::SoundDataRef& GetData() const
	{
		return data_;
	}
};

class SoundVoiceContainer
{
	std::list<SoundVoice*> voiceList_;

public:
	SoundVoiceContainer(SoundImplemented* sound, int num, const WAVEFORMATEX* format);

	~SoundVoiceContainer();

	SoundVoice* GetVoice();

	void StopTag(::Effekseer::SoundTag tag);

	void PauseTag(::Effekseer::SoundTag tag, bool pause);

	bool CheckPlayingTag(::Effekseer::SoundTag tag);

	void StopData(const ::Effekseer::SoundDataRef& soundData);

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
