
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundDSound.SoundPlayer.h"
#include "EffekseerSoundDSound.SoundImplemented.h"
#include "EffekseerSoundDSound.SoundVoice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer::SoundPlayer(const SoundImplementedRef& sound)
	: m_sound(sound)
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
	if (m_sound->GetMute())
	{
		return nullptr;
	}

	if (parameter.Data != nullptr)
	{
		SoundVoice* voice = m_sound->GetVoice();
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
	m_sound->StopTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	m_sound->PauseTag(tag, pause);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	return m_sound->CheckPlayingTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::StopAll()
{
	m_sound->StopAllVoices();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
