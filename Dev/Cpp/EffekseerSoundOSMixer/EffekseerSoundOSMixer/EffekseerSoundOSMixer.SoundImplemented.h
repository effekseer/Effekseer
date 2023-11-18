
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
		int32_t id;
		Effekseer::SoundTag tag;
		Effekseer::SoundDataRef data;
	};

	struct Listener
	{
		Effekseer::Vector3D position;
		Effekseer::Vector3D forwardVector;
		Effekseer::Vector3D upVector;
		Effekseer::Vector3D rightVector;
	};

private:
	osm::Manager* m_manager;
	bool m_mute;

	std::vector<Instance> m_instances;
	Listener m_listener;

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
		return m_mute;
	}

	osm::Manager* GetDevice()
	{
		return m_manager;
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