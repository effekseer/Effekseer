
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundDSound.SoundVoice.h"
#include "EffekseerSoundDSound.SoundImplemented.h"
#include <cmath>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::SoundVoice(SoundImplemented* sound)
	: sound_(sound)
{
	ES_SAFE_ADDREF(sound_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::~SoundVoice()
{
	Stop();

	if (dsbuf_)
	{
		dsbuf_->Release();
	}
	ES_SAFE_RELEASE(sound_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Play(::Effekseer::SoundTag tag, const ::Effekseer::SoundPlayer::InstanceParameter& parameter)
{
	SoundData* soundData = (SoundData*)parameter.Data.Get();

	Stop();

	data_ = parameter.Data;

	sound_->GetDevice()->DuplicateSoundBuffer((IDirectSoundBuffer*)soundData->GetBuffer(), (IDirectSoundBuffer**)&dsbuf_);

	tag_ = tag;

	dsbuf_->SetVolume((LONG)(2000.0f * log10f(parameter.Volume)));

	dsbuf_->SetFrequency((DWORD)(soundData->GetSampleRate() * powf(2.0f, parameter.Pitch)));

	if (parameter.Mode3D)
	{
	}
	else
	{
		float pan = sound_->CalculatePan(parameter.Position) + parameter.Pan;
		int32_t level = (int32_t)(2000.0f * log10f(1.0f - fabsf(pan)));
		if (level < -10000)
		{
			level = -10000;
		}
		if (pan > 0.0f)
		{
			dsbuf_->SetPan(-level);
		}
		else
		{
			dsbuf_->SetPan(level);
		}
	}
	dsbuf_->Play(0, 0, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	if (dsbuf_ == nullptr)
	{
		return;
	}
	dsbuf_->Stop();
	dsbuf_->Release();
	dsbuf_ = nullptr;
	data_ = nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause(bool pause)
{
	if (dsbuf_ == nullptr)
	{
		return;
	}
	if (pause)
	{
		dsbuf_->Stop();
	}
	else
	{
		dsbuf_->Play(0, 0, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	if (dsbuf_ == nullptr)
	{
		return false;
	}
	DWORD status;
	dsbuf_->GetStatus(&status);
	return (status & DSBSTATUS_PLAYING) != 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer(SoundImplemented* sound, int num)
{
	for (int i = 0; i < num; i++)
	{
		voiceList_.push_back(new SoundVoice(sound));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::~SoundVoiceContainer()
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		delete voice;
	}
	voiceList_.clear();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundVoiceContainer::GetVoice()
{
	if (voiceList_.empty())
	{
		return nullptr;
	}

	// 停止ボイスを探す
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		if (!voice->CheckPlaying())
		{
			voiceList_.erase(it);
			voiceList_.push_back(voice);
			return voice;
		}
	}

	// 停止ボイスがないときは最前ボイスを使用
	SoundVoice* voice = voiceList_.front();
	voiceList_.pop_front();
	voiceList_.push_back(voice);
	voice->Stop();

	return voice;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::StopTag(::Effekseer::SoundTag tag)
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		if (tag == voice->GetTag())
		{
			voice->Stop();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		if (tag == voice->GetTag())
		{
			voice->Pause(pause);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoiceContainer::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		if (tag == voice->GetTag() && voice->CheckPlaying())
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::StopData(const ::Effekseer::SoundDataRef& soundData)
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		if (soundData == voice->GetData())
		{
			voice->Stop();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::StopAll()
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++)
	{
		SoundVoice* voice = *it;
		voice->Stop();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
