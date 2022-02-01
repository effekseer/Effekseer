
#ifndef	__EFFEKSEERSOUND_SOUND_LOADER_H__
#define	__EFFEKSEERSOUND_SOUND_LOADER_H__

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

class SoundLoader
	: public ::Effekseer::SoundLoader
{
	::Effekseer::FileInterfaceRef m_fileInterface;

public:
	SoundLoader( ::Effekseer::FileInterfaceRef fileInterface = NULL );

	virtual ~SoundLoader();

	::Effekseer::SoundDataRef Load(::Effekseer::FileReaderRef reader);

public:
	::Effekseer::SoundDataRef Load( const char16_t* path ) override;

	::Effekseer::SoundDataRef Load(const void* data, int32_t size) override;

	void Unload(::Effekseer::SoundDataRef soundData) override;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_SOUND_LOADER_H__