
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundXAudio2.SoundPlayer.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"
#include "EffekseerSoundXAudio2.SoundVoice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer::SoundPlayer(const SoundImplementedRef& sound)
	: sound_(sound)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer::~SoundPlayer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundHandle SoundPlayer::Play(::Effekseer::SoundTag tag, const ::Effekseer::SoundPlayer::InstanceParameter& parameter)
{
	if (sound_->GetMute())
	{
		return nullptr;
	}

	if (parameter.Data != nullptr)
	{
		SoundData* soundDataImpl = (SoundData*)parameter.Data.Get();
		SoundVoice* voice = sound_->GetVoice(soundDataImpl->GetChannels());
		if (voice)
		{
			voice->Play(tag, parameter);
			return (::Effekseer::SoundHandle)voice;
		}
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Stop(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag)
{
	SoundVoice* voice = (SoundVoice*)handle;
	if (tag == voice->GetTag())
	{
		voice->Stop();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Pause(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag, bool pause)
{
	SoundVoice* voice = (SoundVoice*)handle;
	if (tag == voice->GetTag())
	{
		voice->Pause(pause);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlaying(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag)
{
	SoundVoice* voice = (SoundVoice*)handle;
	if (tag == voice->GetTag())
	{
		return voice->CheckPlaying();
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::StopTag(::Effekseer::SoundTag tag)
{
	sound_->StopTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	sound_->PauseTag(tag, pause);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	return sound_->CheckPlayingTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::StopAll()
{
	sound_->StopAllVoices();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
