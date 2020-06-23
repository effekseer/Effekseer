
#ifndef __EFFEKSEERSOUND_SOUND_LOADER_H__
#define __EFFEKSEERSOUND_SOUND_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class SoundImplemented;

class SoundLoader : public ::Effekseer::SoundLoader
{
	SoundImplemented* m_sound;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	SoundLoader(SoundImplemented* sound, ::Effekseer::FileInterface* fileInterface = NULL);

	void* Load(const void* data, int32_t size) override;

	virtual ~SoundLoader();

public:
	void* Load(const EFK_CHAR* path);

	void Unload(void* data);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERSOUND_SOUND_LOADER_H__