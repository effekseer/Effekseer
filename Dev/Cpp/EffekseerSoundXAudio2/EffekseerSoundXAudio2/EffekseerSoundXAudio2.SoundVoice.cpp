
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundXAudio2.SoundVoice.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::SoundVoice(SoundImplemented* sound, const WAVEFORMATEX* format)
	: sound_(sound)
{
	ES_SAFE_ADDREF(sound_);

	sound->GetDevice()->CreateSourceVoice(&xavoice_, format);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::~SoundVoice()
{
	if (xavoice_)
	{
		xavoice_->DestroyVoice();
	}

	ES_SAFE_RELEASE(sound_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Play(::Effekseer::SoundTag tag, const ::Effekseer::SoundPlayer::InstanceParameter& parameter)
{
	SoundData* soundData = (SoundData*)parameter.Data.Get();

	tag_ = tag;
	data_ = parameter.Data;
	xavoice_->SubmitSourceBuffer(soundData->GetBuffer());
	xavoice_->SetSourceSampleRate(soundData->GetSampleRate());
	xavoice_->SetVolume(parameter.Volume);
	xavoice_->SetFrequencyRatio(powf(2.0f, parameter.Pitch));

	float matrix[2 * 4];
	if (parameter.Mode3D)
	{
		sound_->Calculate3DSound(parameter.Position, parameter.Distance, soundData->GetChannels(), 2, matrix);
	}
	else
	{
		float rad = ((parameter.Pan + 1.0f) * 0.5f) * (3.1415926f * 0.5f);
		switch (soundData->GetChannels())
		{
		case 1:
			matrix[0] = cosf(rad);
			matrix[1] = sinf(rad);
			break;
		case 2:
			matrix[0] = matrix[3] = 1.0f;
			matrix[1] = matrix[2] = 0.0f;
			break;
		default:
			return;
		}
	}
	xavoice_->SetOutputMatrix(nullptr, soundData->GetChannels(), 2, matrix);
	xavoice_->Start();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	xavoice_->Stop();
	xavoice_->FlushSourceBuffers();
	data_ = nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause(bool pause)
{
	if (pause)
	{
		xavoice_->Stop();
	}
	else
	{
		xavoice_->Start();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	XAUDIO2_VOICE_STATE state;
	xavoice_->GetState(&state);
	return state.BuffersQueued > 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer(SoundImplemented* sound, int num, const WAVEFORMATEX* format)
{
	for (int i = 0; i < num; i++)
	{
		voiceList_.push_back(new SoundVoice(sound, format));
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
