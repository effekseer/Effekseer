
#ifndef __EFFEKSEER_ParameterNODE_ROOT_H__
#define __EFFEKSEER_ParameterNODE_ROOT_H__

#include "Effekseer.EffectNode.h"

namespace Effekseer
{

class EffectNodeRoot : public EffectNodeImplemented
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

protected:
public:
	EffectNodeRoot(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeRoot() = default;

	eEffectNodeType GetType() const
	{
		return eEffectNodeType::Root;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_ParameterNODE_ROOT_H__
