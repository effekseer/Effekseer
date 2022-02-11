#include "EffectFactory.h"

#include "Effect.h"
#include "EffectSetting.h"

#include <Effekseer/Effekseer.Base.h>
#include <EffekseerSoundOSMixer.h>

#include <algorithm>

namespace Effekseer::Tool
{

void EffectFactory::ClearInvalidEffects()
{
	const auto it = std::remove_if(effects_.begin(), effects_.end(), [](const std::weak_ptr<Effect>& effect) { return effect.lock() == nullptr; });

	effects_.erase(it, effects_.end());
}

EffectFactory::EffectFactory(std::shared_ptr<Effekseer::Tool::EffectSetting> setting)
{
	setting_ = setting->GetSetting();
}

std::shared_ptr<Effect> EffectFactory::LoadEffect(const void* data, int size, const char16_t* path)
{
	ClearInvalidEffects();

	const auto normalizedPath = Effekseer::PathHelper::Normalize(std::u16string(path));
	auto pathElements = Effekseer::StringHelper::Split(normalizedPath, u'/');
	if (pathElements.size() > 0)
	{
		pathElements.pop_back();
	}
	const auto dirPath = Effekseer::StringHelper::Join(pathElements, std::u16string(u"/"));

	auto effect = Effekseer::Effect::Create(setting_, data, size, 1.0f, dirPath.c_str());

	// Create UserData while assigning NodeId.
	{
		int nextEditorNodeId = 1;
		const auto& visitor = [&](::Effekseer::EffectNodeImplemented* node) {
			auto userData = ::Effekseer::MakeRefPtr<EditorEffectNodeUserData>();
			userData->editorNodeId_ = nextEditorNodeId;
			node->SetRenderingUserData(userData);
			nextEditorNodeId++;
			return true;
		};

		static_cast<::Effekseer::EffectNodeImplemented*>(effect->GetRoot())->Traverse(visitor);
	}

	auto ret = std::make_shared<Effect>();

	ret->SetEffect(effect);

	effects_.emplace_back(ret);

	return ret;
}

void EffectFactory::ReloadAllResources()
{
	ClearInvalidEffects();

	for (auto& effect : effects_)
	{
		effect.lock()->GetEffect()->UnloadResources();
	}

	for (auto& effect : effects_)
	{
		effect.lock()->GetEffect()->ReloadResources();
	}
}

} // namespace Effekseer::Tool
