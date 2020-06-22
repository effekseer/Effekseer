
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include "EffekseerSoundOSMixer.SoundPlayer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer::SoundPlayer( SoundImplemented* sound )
	: m_sound	( sound )
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
::Effekseer::SoundHandle SoundPlayer::Play( ::Effekseer::SoundTag tag, 
		const ::Effekseer::SoundPlayer::InstanceParameter& parameter )
{
	if( m_sound->GetMute() )
	{
		return nullptr;
	}
	SoundData* soundData = (SoundData*)parameter.Data;
	if( soundData == nullptr )
	{
		return nullptr;
	}
	
	auto device = m_sound->GetDevice();
	int32_t id = device->Play( soundData );
	
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
	
	if( parameter.Mode3D )
	{
		float rolloff, pan;
		m_sound->Calculate3DSound( parameter.Position, parameter.Distance, rolloff, pan );
		device->SetVolume( id, rolloff * parameter.Volume );
		device->SetPanningPosition( id, pan );
	}
	else
	{
		device->SetVolume( id, parameter.Volume );
		device->SetPanningPosition( id, parameter.Pan );
	}

	SoundImplemented::Instance instance;
	instance.id = id;
	instance.tag = tag;
	instance.data = soundData;
	m_sound->AddInstance( instance );

	return reinterpret_cast<Effekseer::SoundHandle>( id );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Stop( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag )
{
	int32_t id = (int32_t)(intptr_t)handle;
	m_sound->GetDevice()->Stop( id );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::Pause( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag, bool pause )
{
	int32_t id = (int32_t)(intptr_t)handle;
	if( pause )
	{
		m_sound->GetDevice()->Pause( id );
	}
	else
	{
		m_sound->GetDevice()->Resume( id );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlaying( ::Effekseer::SoundHandle handle, ::Effekseer::SoundTag tag )
{
	int32_t id = (int32_t)(intptr_t)handle;
	return m_sound->GetDevice()->IsPlaying( id );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::StopTag( ::Effekseer::SoundTag tag )
{
	m_sound->StopTag( tag );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::PauseTag( ::Effekseer::SoundTag tag, bool pause )
{
	m_sound->PauseTag( tag, pause );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundPlayer::CheckPlayingTag( ::Effekseer::SoundTag tag )
{
	return m_sound->CheckPlayingTag( tag );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundPlayer::StopAll()
{
	m_sound->StopAll();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
