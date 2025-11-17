
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
	: source_(source)
	, tag_(nullptr)
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

	tag_ = tag;
	alSourcei(source_, AL_BUFFER, soundDataImpl->GetBuffer());
	alSourcef(source_, AL_PITCH, powf(2.0f, parameter.Pitch));
	alSourcef(source_, AL_GAIN, parameter.Volume);
	
	if (parameter.Mode3D) {
		alSourcei(source_, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcefv(source_, AL_POSITION, &parameter.Position.X);
		alSourcef(source_, AL_MAX_DISTANCE, parameter.Distance);
	} else {
		float position[3] = {parameter.Pan, 0.0f, 0.0f};
		alSourcei(source_, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcefv(source_, AL_POSITION, position);
		alSourcef(source_, AL_MAX_DISTANCE, 1.0f);
	}
	alSourcePlay(source_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Stop()
{
	alSourceStop(source_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundVoice::Pause( bool pause )
{
	if (pause) {
		alSourcePause(source_);
	} else {
		alSourcePlay(source_);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundVoice::CheckPlaying()
{
	ALint state = 0;
	alGetSourcei(source_, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING || state == AL_PAUSED;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::SoundVoiceContainer( SoundImplemented* sound, int num )
{
	num_ = num;
	sources_ = new ALuint[num];
	alGenSources( (ALsizei)num, sources_ );
	for (int i = 0; i < num; i++) {
		voiceList_.push_back( new SoundVoice( sources_[i] ) );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoiceContainer::~SoundVoiceContainer()
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
		SoundVoice* voice = *it;
		delete voice;
	}

	alDeleteSources( (ALsizei)num_, sources_ );
	delete[] sources_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundVoiceContainer::GetVoice()
{
	if (voiceList_.empty()) {
		return NULL;
	}

	// 停止ボイスを探す
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
		SoundVoice* voice = *it;
		if (!voice->CheckPlaying()) {
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
void SoundVoiceContainer::StopTag( ::Effekseer::SoundTag tag )
{
	std::list<SoundVoice*>::iterator it;
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
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
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
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
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
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
	for (it = voiceList_.begin(); it != voiceList_.end(); it++) {
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
