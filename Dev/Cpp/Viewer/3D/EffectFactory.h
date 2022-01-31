#pragma once

#include <Effekseer.h>
#include <memory>

class Native;

namespace Effekseer::Tool
{

class Effect;

class EffectFactory
{
	Effekseer::SettingRef setting_;
	std::vector<std::weak_ptr<Effect>> effects_;

	void ClearInvalidEffects();

public:

	EffectFactory(Native* native);

	std::shared_ptr<Effect> LoadEffect(const void* data, int size, const char16_t* path);

	void ReloadAllResources();
};

} // namespace Effekseer::Tool
