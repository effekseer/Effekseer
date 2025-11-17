
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundOSMixer.SoundPlayer.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include <math.h>

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
{}

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

	if (parameter.Data == nullptr)
	{
		return nullptr;
	}

	auto soundDataImpl = (const SoundData*)parameter.Data.Get();

	auto device = sound_->GetDevice();
	int32_t id = device->Play((osm::Sound*)soundDataImpl->GetOsmSound());

	if (parameter.Pitch != 0.0f)
	{
		device->SetIsPlaybackSpeedEnabled(id, true);
		device->SetPlaybackSpeed(id, powf(2.0f, parameter.Pitch));
	}
	else
	{
		device->SetIsPlaybackSpeedEnabled(id, false);
		device->SetPlaybackSpeed(id, 1.0f);
	}

	if (parameter.Mode3D)
	{
		float rolloff, pan;
		sound_->Calculate3DSound(parameter.Position, parameter.Distance, rolloff, pan);
		device->SetVolume(id, rolloff * parameter.Volume);
		device->SetPanningPosition(id, pan);
	}
	else
	{
		device->SetVolume(id, parameter.Volume);
		device->SetPanningPosition(id, parameter.Pan);
	}

	SoundImplemented::Instance instance;
	instance.id_ = id;
	instance.tag_ = tag;
	instance.data_ = parameter.Data;
	sound_->AddInstance(instance);

	return reinterpret_cast<Effekseer::SoundHandle>(static_cast<int64_t>(id));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Stop(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag)
{
	int32_t id = (int32_t)(intptr_t)handle;
	sound_->GetDevice()->Stop(id);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Pause(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag, bool pause)
{
	int32_t id = (int32_t)(intptr_t)handle;
	if (pause)
	{
		sound_->GetDevice()->Pause(id);
	}
	else
	{
		sound_->GetDevice()->Resume(id);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlaying(::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag)
{
	int32_t id = (int32_t)(intptr_t)handle;
	return sound_->GetDevice()->IsPlaying(id);
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
	sound_->StopAll();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
