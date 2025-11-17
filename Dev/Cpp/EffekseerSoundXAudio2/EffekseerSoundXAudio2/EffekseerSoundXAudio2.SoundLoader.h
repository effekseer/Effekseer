
#ifndef __EFFEKSEERSOUND_SOUND_LOADER_H__
#define __EFFEKSEERSOUND_SOUND_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SoundLoader : public ::Effekseer::SoundLoader
{
	SoundImplementedRef sound_ = nullptr;
	::Effekseer::FileInterfaceRef fileInterface_ = nullptr;

public:
	SoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface = nullptr);

	virtual ~SoundLoader();

	::Effekseer::SoundDataRef Load(::Effekseer::FileReaderRef reader);

public:
	::Effekseer::SoundDataRef Load(const char16_t* path) override;

	::Effekseer::SoundDataRef Load(const void* data, int32_t size) override;

	void Unload(::Effekseer::SoundDataRef soundData) override;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERSOUND_SOUND_LOADER_H__
