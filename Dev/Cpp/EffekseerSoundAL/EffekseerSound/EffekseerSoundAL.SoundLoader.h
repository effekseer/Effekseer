
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
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	SoundLoader( ::Effekseer::FileInterface* fileInterface = NULL );

	virtual ~SoundLoader();

	void* Load(::Effekseer::FileReader* reader);

public:
	void* Load( const EFK_CHAR* path ) override;

	void* Load(const void* data, int32_t size) override;

	void Unload(void* data) override;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_SOUND_LOADER_H__