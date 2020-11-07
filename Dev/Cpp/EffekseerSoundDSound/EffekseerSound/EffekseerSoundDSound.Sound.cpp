
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
Sound* Sound::Create(IDirectSound8* dsound)
{
	SoundImplemented* sound = new SoundImplemented();
	if (sound->Initialize(dsound))
	{
		return sound;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::SoundImplemented()
	: m_dsound(NULL)
	, m_mute(false)
	, m_voiceContainer(NULL)
	, m_leftPos(0)
	, m_rightPos(0)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::~SoundImplemented()
{
	StopAllVoices();
	if (m_voiceContainer)
	{
		delete m_voiceContainer;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::Initialize(IDirectSound8* dsound)
{
	m_dsound = dsound;

	// ボイスを作成
	m_voiceContainer = new SoundVoiceContainer(this, 32);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetListener(const ::Effekseer::Vector3D& pos,
								   const ::Effekseer::Vector3D& at,
								   const ::Effekseer::Vector3D& up)
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
::Effekseer::SoundPlayer* SoundImplemented::CreateSoundPlayer()
{
	return new SoundPlayer(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundLoader* SoundImplemented::CreateSoundLoader()
{
	return new SoundLoader(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopAllVoices()
{
	m_voiceContainer->StopAll();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetMute(bool mute)
{
	m_mute = mute;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundImplemented::GetVoice()
{
	return m_voiceContainer->GetVoice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopTag(::Effekseer::SoundTag tag)
{
	m_voiceContainer->StopTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	m_voiceContainer->PauseTag(tag, pause);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	return m_voiceContainer->CheckPlayingTag(tag);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopData(SoundData* soundData)
{
	m_voiceContainer->StopData(soundData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetPanRange(int32_t leftPos, int32_t rightPos)
{
	m_leftPos = leftPos;
	m_rightPos = rightPos;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float SoundImplemented::CalculatePan(const Effekseer::Vector3D& position)
{
	if (m_leftPos == m_rightPos)
	{
		return 0;
	}

	int32_t pos = (int32_t)position.X;
	if (pos <= m_leftPos)
	{
		return -1.0f;
	}
	else if (pos >= m_rightPos)
	{
		return 1.0f;
	}
	else
	{
		return (float)(pos - m_leftPos) / (m_rightPos - m_leftPos);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
