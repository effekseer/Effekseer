#pragma once

#include "../Math/Vector3F.h"
#include <Effekseer.h>
#include <EffekseerSoundOSMixer.h>
#include <OpenSoundMixer.h>

namespace osm
{
class Manager;
}

namespace EffekseerSound
{
class Sound;
using SoundRef = Effekseer::RefPtr<Sound>;

} // namespace EffekseerSound

namespace Effekseer::Tool
{

class SoundDevice
{
private:
	::EffekseerSound::SoundRef sound_;

	osm::Manager* manager_ = nullptr;

public:
	SoundDevice() = default;
	~SoundDevice();

	void SetListener(const Effekseer::Tool::Vector3F& pos, const Effekseer::Tool::Vector3F& at, const Effekseer::Tool::Vector3F& up);

#ifndef SWIG
	::EffekseerSound::SoundRef GetSound() const
	{
		return sound_;
	};
#endif

	static std::shared_ptr<SoundDevice> Create();
};

} // namespace Effekseer::Tool