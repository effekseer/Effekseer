
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundDSound.h"
#include "EffekseerSoundDSound.SoundImplemented.h"
#include "EffekseerSoundDSound.SoundLoader.h"
#include "EffekseerSoundDSound.SoundPlayer.h"
#include "EffekseerSoundDSound.SoundVoice.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundRef Sound::Create(IDirectSound8* dsound)
{
	auto sound = Effekseer::MakeRefPtr<SoundImplemented>();
	if (sound->Initialize(dsound))
	{
		return sound;
	}
	return nullptr;
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
	if (voiceContainer_)
	{
		delete voiceContainer_;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::Initialize(IDirectSound8* dsound)
{
	dsound_ = dsound;

	// ボイスを作成
	voiceContainer_ = new SoundVoiceContainer(this, 32);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetListener(const ::Effekseer::Vector3D& pos, const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up)
{
	/*::Effekseer::Vector3D front;
	::Effekseer::Vector3D::Sub(front, at, pos);

	memset(&m_listener, 0, sizeof(m_listener));
	m_listener.OrientTop = *(X3DAUDIO_VECTOR*)&up;
	m_listener.OrientFront.x = -front.X;
	m_listener.OrientFront.y = -front.Y;
	m_listener.OrientFront.z = -front.Z;
	m_listener.Position = *(X3DAUDIO_VECTOR*)&pos;
	m_listener.Velocity.x = 0.0f;
	m_listener.Velocity.y = 0.0f;
	m_listener.Velocity.z = 0.0f;*/
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
::Effekseer::SoundLoaderRef SoundImplemented::CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<SoundLoader>(SoundImplementedRef::FromPinned(this), fileInterface);
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
void SoundImplemented::SetMute(bool mute)
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
void SoundImplemented::StopTag(::Effekseer::SoundTag tag)
{
	voiceContainer_->StopTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	voiceContainer_->PauseTag(tag, pause);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	return voiceContainer_->CheckPlayingTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopData(const ::Effekseer::SoundDataRef& soundData)
{
	voiceContainer_->StopData(soundData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetPanRange(int32_t leftPos, int32_t rightPos)
{
	leftPos_ = leftPos;
	rightPos_ = rightPos;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float SoundImplemented::CalculatePan(const Effekseer::Vector3D& position)
{
	if (leftPos_ == rightPos_)
	{
		return 0;
	}

	int32_t pos = (int32_t)position.X;
	if (pos <= leftPos_)
	{
		return -1.0f;
	}
	else if (pos >= rightPos_)
	{
		return 1.0f;
	}
	else
	{
		return (float)(pos - leftPos_) / (rightPos_ - leftPos_);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
