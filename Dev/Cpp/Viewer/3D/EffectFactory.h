#pragma once

#include <Effekseer.h>
#include <memory>

namespace Effekseer::Tool
{

class Effect;
class EffectSetting;

class EffectFactory
{
	Effekseer::SettingRef setting_;
	std::vector<std::weak_ptr<Effect>> effects_;

	void ClearInvalidEffects();

public:
	EffectFactory(std::shared_ptr<Effekseer::Tool::EffectSetting> setting);

	std::shared_ptr<Effect> LoadEffect(const void* data, int size, const char16_t* path);

	void ReloadAllResources();
};

} // namespace Effekseer::Tool
