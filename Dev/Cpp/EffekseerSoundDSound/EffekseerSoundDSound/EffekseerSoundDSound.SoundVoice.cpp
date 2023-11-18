
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
	: m_sound(sound)
	, m_dsbuf(nullptr)
	, m_tag(nullptr)
{
	ES_SAFE_ADDREF(m_sound);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::~SoundVoice()
{
	Stop();

	if (m_dsbuf)
	{
		m_dsbuf->Release();
	}
	ES_SAFE_RELEASE(m_sound);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Play(::Effekseer::SoundTag tag, const ::Effekseer::SoundPlayer::InstanceParameter& parameter)
{
	SoundData* soundData = (SoundData*)parameter.Data.Get();

	Stop();

	m_data = parameter.Data;

	m_sound->GetDevice()->DuplicateSoundBuffer((IDirectSoundBuffer*)soundData->GetBuffer(), (IDirectSoundBuffer**)&m_dsbuf);

	m_tag = tag;

	m_dsbuf->SetVolume((LONG)(2000.0f * log10f(parameter.Volume)));

	m_dsbuf->SetFrequency((DWORD)(soundData->GetSampleRate() * powf(2.0f, parameter.Pitch)));

	if (parameter.Mode3D)
	{
	}
	else
	{
		float pan = m_sound->CalculatePan(parameter.Position) + parameter.Pan;
		int32_t level = (int32_t)(2000.0f * log10f(1.0f - fabsf(pan)));
		if (level < -10000)
		{
			level = -10000;
		}
		if (pan > 0.0f)
		{
			m_dsbuf->SetPan(-level);
		}
		else
		{
			m_dsbuf->SetPan(level);
		}
	}
	m_dsbuf->Play(0, 0, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	if (m_dsbuf == nullptr)
	{
		return;
	}
	m_dsbuf->Stop();
	m_dsbuf->Release();
	m_dsbuf = nullptr;
	m_data = nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause(bool pause)
{
	if (m_dsbuf == nullptr)
	{
		return;
	}
	if (pause)
	{
		m_dsbuf->Stop();
	}
	else
	{
		m_dsbuf->Play(0, 0, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	if (m_dsbuf == nullptr)
	{
		return false;
	}
	DWORD status;
	m_dsbuf->GetStatus(&status);
	return (status & DSBSTATUS_PLAYING) != 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer(SoundImplemented* sound, int num)
{
	for (int i = 0; i < num; i++)
	{
		m_voiceList.push_back(new SoundVoice(sound));
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
