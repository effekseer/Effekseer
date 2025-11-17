
#ifndef __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__
#define __EFFEKSEERRSOUND_SOUND_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundOSMixer.h"
#include <OpenSoundMixer.h>
#include <vector>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundImplemented : public Sound, public Effekseer::ReferenceObject
{
public:
	struct Instance
	{
		int32_t id_ = 0;
		Effekseer::SoundTag tag_ = nullptr;
		Effekseer::SoundDataRef data_;
	};

	struct Listener
	{
		Effekseer::Vector3D position_;
		Effekseer::Vector3D forwardVector_;
		Effekseer::Vector3D upVector_;
		Effekseer::Vector3D rightVector_;
	};

private:
	osm::Manager* manager_ = nullptr;
	bool mute_ = false;

	std::vector<Instance> instances_;
	Listener listener_;

public:
	SoundImplemented();
	virtual ~SoundImplemented();

	void Destroy();

	bool Initialize(osm::Manager* soundManager);

	void SetListener(const ::Effekseer::Vector3D& pos,
					 const ::Effekseer::Vector3D& at,
					 const ::Effekseer::Vector3D& up);

	void Update();

	::Effekseer::SoundPlayerRef CreateSoundPlayer() override;

	::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	void StopAll();

	void SetMute(bool mute);

	bool GetMute()
	{
		return mute_;
	}

	osm::Manager* GetDevice()
	{
		return manager_;
	}

	void AddInstance(const Instance& instance);

	void StopTag(::Effekseer::SoundTag tag);

	void PauseTag(::Effekseer::SoundTag tag, bool pause);

	bool CheckPlayingTag(::Effekseer::SoundTag tag);

	void StopData(const ::Effekseer::SoundDataRef& soundData);

	void Calculate3DSound(const ::Effekseer::Vector3D& position,
						  float rolloffDistance,
						  float& rolloff,
						  float& pan);

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
