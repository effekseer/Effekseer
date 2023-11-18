
#ifndef __EFFEKSEERSOUND_SOUND_LOADER_H__
#define __EFFEKSEERSOUND_SOUND_LOADER_H__

#include "Effekseer.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"

namespace EffekseerSound
{

class SoundImplemented;

class SoundLoader : public ::Effekseer::SoundLoader
{
	SoundImplementedRef m_sound;
	::Effekseer::FileInterfaceRef m_fileInterface;

public:
	SoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface = nullptr);

	::Effekseer::SoundDataRef Load(const void* data, int32_t size) override;

	virtual ~SoundLoader() override;

public:
	::Effekseer::SoundDataRef Load(const char16_t* path) override;

	void Unload(::Effekseer::SoundDataRef soundData) override;
};

} // namespace EffekseerSound

#endif // __EFFEKSEERSOUND_SOUND_LOADER_H__