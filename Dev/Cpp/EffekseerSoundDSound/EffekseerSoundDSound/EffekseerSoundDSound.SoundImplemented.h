
#ifndef __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
#define __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundDSound.h"
#include <dsound.h>

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundVoice;
class SoundVoiceContainer;

class SoundImplemented : public Sound, public Effekseer::ReferenceObject
{
	IDirectSound8* dsound_ = nullptr;

	SoundVoiceContainer* voiceContainer_ = nullptr;
	bool mute_ = false;
	int32_t leftPos_ = 0;
	int32_t rightPos_ = 0;

public:
	SoundImplemented();
	virtual ~SoundImplemented();

	void Destroy();

	bool Initialize(IDirectSound8* dsound);

	void SetListener(const ::Effekseer::Vector3D& pos,
					 const ::Effekseer::Vector3D& at,
					 const ::Effekseer::Vector3D& up);

	::Effekseer::SoundPlayerRef CreateSoundPlayer() override;

	::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface) override;

	void StopAllVoices();

	void SetMute(bool mute);

	bool GetMute()
	{
		return mute_;
	}

	IDirectSound8* GetDevice()
	{
		return dsound_;
	}

	SoundVoice* GetVoice();

	void StopTag(::Effekseer::SoundTag tag);

	void PauseTag(::Effekseer::SoundTag tag, bool pause);

	bool CheckPlayingTag(::Effekseer::SoundTag tag);

	void StopData(const ::Effekseer::SoundDataRef& soundData);

	void SetPanRange(int32_t leftPos, int32_t rightPos);

	float CalculatePan(const Effekseer::Vector3D& position);

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};
using SoundImplementedRef = ::Effekseer::RefPtr<SoundImplemented>;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
