
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
	: m_sound(sound)
	, m_xavoice(nullptr)
	, m_tag(nullptr)
	, m_data(nullptr)
{
	ES_SAFE_ADDREF(m_sound);

	sound->GetDevice()->CreateSourceVoice(&m_xavoice, format);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::~SoundVoice()
{
	if (m_xavoice)
	{
		m_xavoice->DestroyVoice();
	}

	ES_SAFE_RELEASE(m_sound);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Play(::Effekseer::SoundTag tag, const ::Effekseer::SoundPlayer::InstanceParameter& parameter)
{
	SoundData* soundData = (SoundData*)parameter.Data.Get();

	m_tag = tag;
	m_data = parameter.Data;
	m_xavoice->SubmitSourceBuffer(soundData->GetBuffer());
	m_xavoice->SetSourceSampleRate(soundData->GetSampleRate());
	m_xavoice->SetVolume(parameter.Volume);
	m_xavoice->SetFrequencyRatio(powf(2.0f, parameter.Pitch));

	float matrix[2 * 4];
	if (parameter.Mode3D)
	{
		m_sound->Calculate3DSound(parameter.Position, parameter.Distance, soundData->GetChannels(), 2, matrix);
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
	m_xavoice->SetOutputMatrix(nullptr, soundData->GetChannels(), 2, matrix);
	m_xavoice->Start();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	m_xavoice->Stop();
	m_xavoice->FlushSourceBuffers();
	m_data = nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause(bool pause)
{
	if (pause)
	{
		m_xavoice->Stop();
	}
	else
	{
		m_xavoice->Start();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	XAUDIO2_VOICE_STATE state;
	m_xavoice->GetState(&state);
	return state.BuffersQueued > 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer(SoundImplemented* sound, int num, const WAVEFORMATEX* format)
{
	for (int i = 0; i < num; i++)
	{
		m_voiceList.push_back(new SoundVoice(sound, format));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::~SoundVoiceContainer()
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
	{
		SoundVoice* voice = *it;
		delete voice;
	}
	m_voiceList.clear();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundVoiceContainer::GetVoice()
{
	if (m_voiceList.empty())
	{
		return nullptr;
	}

	// 停止ボイスを探す
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
	{
		SoundVoice* voice = *it;
		if (!voice->CheckPlaying())
		{
			m_voiceList.erase(it);
			m_voiceList.push_back(voice);
			return voice;
		}
	}

	// 停止ボイスがないときは最前ボイスを使用
	SoundVoice* voice = m_voiceList.front();
	m_voiceList.pop_front();
	m_voiceList.push_back(voice);
	voice->Stop();

	return voice;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::StopTag(::Effekseer::SoundTag tag)
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
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
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
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
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
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
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
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
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++)
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
