#ifndef __EFFEKSEER_COMPATIBLITY_H__
#define __EFFEKSEER_COMPATIBLITY_H__

#include "../Utils/Effekseer.BinaryReader.h"
#include "../Effekseer.EffectNode.h"
#include "../Effekseer.InternalStruct.h"
#include "Effekseer.BinaryVersion.h"

namespace Effekseer
{
inline void LoadFloatEasing(ParameterEasingFloat& param, BinaryReader<true>& pos, int version)
{
	if (version >= Version16Alpha9)
	{
		int32_t size = 0;
		if (!pos.Peek(&size, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		param.Load(pos, size, version);
		pos.Skip(size);
	}
	else
	{
		param.Load(pos, sizeof(easing_float), version);
		pos.Skip(sizeof(easing_float));
	}
}
} // namespace Effekseer

#endif