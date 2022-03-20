#pragma once

#include <Effekseer.h>
#include <Effekseer/Effekseer.EffectImplemented.h>
#include <Effekseer/Effekseer.EffectNode.h>

namespace Effekseer::Tool
{

#if !defined(SWIG)
class EditorEffectNodeUserData : public ::Effekseer::RenderingUserData
{
public:
	// Identifier to use when referring to a node from the editor.
	int32_t editorNodeId_ = 0;

	static ::Effekseer::EffectNodeImplemented* FindNodeByEditorNodeId(::Effekseer::EffectImplemented* effect, int32_t editorNodeId)
	{
		auto* root = effect->GetRoot();
		if (!root)
			return nullptr;

		::Effekseer::EffectNodeImplemented* result = nullptr;

		const auto& visitor = [&](::Effekseer::EffectNodeImplemented* node) -> bool {
			const auto userData = node->GetRenderingUserData();
			if (userData != nullptr)
			{
				const auto* editorUserData = static_cast<EditorEffectNodeUserData*>(userData.Get());

				if (editorUserData->editorNodeId_ == editorNodeId)
				{
					result = node;
					return false;
				}
			}
			return true;
		};

		static_cast<::Effekseer::EffectNodeImplemented*>(root)->Traverse(visitor);

		return result;
	}
};
#endif

class Effect
{
	Effekseer::EffectRef effect_;

public:
	bool GetNodeLifeTimes(int32_t nodeId, int32_t* frameMin, int32_t* frameMax);

#if !defined(SWIG)
	Effekseer::EffectRef GetEffect();
	void SetEffect(Effekseer::EffectRef effect);
#endif
};

} // namespace Effekseer::Tool
