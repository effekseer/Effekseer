
#ifndef __EFFEKSEERTOOL_SOUND_H__
#define __EFFEKSEERTOOL_SOUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerSoundOSMixer.h>
#include <OpenSoundMixer.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Sound
{
private:
	::EffekseerSound::SoundRef m_sound;
	osm::Manager* m_manager;

public:
	Sound();

	~Sound();

	bool Initialize();

	void SetListener(const Effekseer::Vector3D& pos, const Effekseer::Vector3D& at, const Effekseer::Vector3D& up);

	::EffekseerSound::SoundRef GetSound()
	{
		return m_sound;
	};
};

} // namespace EffekseerTool

#endif // __EFFEKSEERTOOL_SOUND_H__