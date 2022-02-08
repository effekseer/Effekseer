#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

class GraphicsDevice;
class SoundDevice;

class EffectSetting
{
	Effekseer::SettingRef setting_;

public:
#ifndef SWIG
	Effekseer::SettingRef GetSetting() const
	{
		return setting_;
	}
#endif
	static std::shared_ptr<EffectSetting> Create(std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice, std::shared_ptr<Effekseer::Tool::SoundDevice> soundDevice);
};

} // namespace Effekseer::Tool