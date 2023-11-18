
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerSoundAL.SoundImplemented.h"
#include "EffekseerSoundAL.SoundVoice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::SoundVoice( ALuint source )
	: m_source(source)
	, m_tag(NULL)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice::~SoundVoice()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Play( ::Effekseer::SoundTag tag, 
	const ::Effekseer::SoundPlayer::InstanceParameter& parameter )
{
	if (parameter.Data == nullptr) {
		return;
	}
	SoundData* soundDataImpl = (SoundData*)parameter.Data.Get();

	m_tag = tag;
	alSourcei(m_source, AL_BUFFER, soundDataImpl->GetBuffer());
	alSourcef(m_source, AL_PITCH, powf(2.0f, parameter.Pitch));
	alSourcef(m_source, AL_GAIN, parameter.Volume);
	
	if (parameter.Mode3D) {
		alSourcei(m_source, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcefv(m_source, AL_POSITION, &parameter.Position.X);
		alSourcef(m_source, AL_MAX_DISTANCE, parameter.Distance);
	} else {
		float position[3] = {parameter.Pan, 0.0f, 0.0f};
		alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcefv(m_source, AL_POSITION, position);
		alSourcef(m_source, AL_MAX_DISTANCE, 1.0f);
	}
	alSourcePlay(m_source);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	alSourceStop(m_source);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause( bool pause )
{
	if (pause) {
		alSourcePause(m_source);
	} else {
		alSourcePlay(m_source);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	ALint state = 0;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING || state == AL_PAUSED;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer( SoundImplemented* sound, int num )
{
	m_num = num;
	m_sources = new ALuint[num];
	alGenSources( (ALsizei)num, m_sources );
	for (int i = 0; i < num; i++) {
		m_voiceList.push_back( new SoundVoice( m_sources[i] ) );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::~SoundVoiceContainer()
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		delete voice;
	}

	alDeleteSources( (ALsizei)m_num, m_sources );
	delete[] m_sources;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundVoiceContainer::GetVoice()
{
	if (m_voiceList.empty()) {
		return NULL;
	}

	// 停止ボイスを探す
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		if (!voice->CheckPlaying()) {
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
void SoundVoiceContainer::StopTag( ::Effekseer::SoundTag tag )
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		if (tag == voice->GetTag()) {
			voice->Stop();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::PauseTag( ::Effekseer::SoundTag tag, bool pause )
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		if (tag == voice->GetTag()) {
			voice->Pause(pause);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoiceContainer::CheckPlayingTag( ::Effekseer::SoundTag tag )
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		if (tag == voice->GetTag() && voice->CheckPlaying()) {
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoiceContainer::StopAll()
{
	std::list<SoundVoice*>::iterator it;
	for (it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		SoundVoice* voice = *it;
		voice->Stop();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
