#include "Effect.h"

namespace Effekseer::Tool
{

bool Effect::GetNodeLifeTimes(int32_t nodeId, int32_t* frameMin, int32_t* frameMax)
{
	if (effect_ == nullptr)
		return false;

	if (auto* effect = dynamic_cast<Effekseer::EffectImplemented*>(effect_.Get()))
	{
		if (auto* node = EditorEffectNodeUserData::FindNodeByEditorNodeId(effect, nodeId))
		{
			Effekseer::EffectInstanceTerm term;
			auto cterm = node->CalculateInstanceTerm(term);
			*frameMin = cterm.FirstInstanceStartMin;
			*frameMax = cterm.LastInstanceEndMax;
			return true;
		}
	}

	return false;
}

Effekseer::EffectRef Effect::GetEffect()
{
	return effect_;
}

void Effect::SetEffect(Effekseer::EffectRef effect)
{
	effect_ = effect;
}

} // namespace Effekseer::Tool
