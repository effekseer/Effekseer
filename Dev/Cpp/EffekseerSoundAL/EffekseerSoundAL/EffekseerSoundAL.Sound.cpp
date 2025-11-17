
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundAL.h"
#include "EffekseerSoundAL.SoundPlayer.h"
#include "EffekseerSoundAL.SoundVoice.h"
#include "EffekseerSoundAL.SoundLoader.h"
#include "EffekseerSoundAL.SoundImplemented.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundRef Sound::Create( int32_t numVoices )
{
	auto sound = Effekseer::MakeRefPtr<SoundImplemented>();
	if( sound->Initialize( numVoices ) )
	{
		return sound;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::SoundImplemented() = default;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::~SoundImplemented()
{
	StopAllVoices();
	delete voiceContainer_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::Initialize( int32_t numVoices )
{
	voiceContainer_ = new SoundVoiceContainer( this, numVoices );
	
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up )
{
	::Effekseer::Vector3D front;
	::Effekseer::Vector3D::Normal(front, at - pos);
	float orientation[3 * 2];
	orientation[0] = front.X;
	orientation[1] = front.Y;
	orientation[2] = front.Z;
	orientation[3] = up.X;
	orientation[4] = up.Y;
	orientation[5] = up.Z;

	alListenerfv(AL_POSITION, &pos.X);
	alListenerfv(AL_ORIENTATION, orientation);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::Destroy()
{
	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundPlayerRef SoundImplemented::CreateSoundPlayer()
{
	return ::Effekseer::MakeRefPtr<SoundPlayer>(SoundImplementedRef::FromPinned(this));
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundLoaderRef SoundImplemented::CreateSoundLoader( ::Effekseer::FileInterfaceRef fileInterface )
{
	return ::Effekseer::MakeRefPtr<SoundLoader>(fileInterface);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopAllVoices()
{
	voiceContainer_->StopAll();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetMute( bool mute )
{
	mute_ = mute;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundImplemented::GetVoice()
{
	return voiceContainer_->GetVoice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopTag( ::Effekseer::SoundTag tag )
{
	voiceContainer_->StopTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::PauseTag( ::Effekseer::SoundTag tag, bool pause )
{
	voiceContainer_->PauseTag(tag, pause);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::CheckPlayingTag( ::Effekseer::SoundTag tag )
{
	return voiceContainer_->CheckPlayingTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
