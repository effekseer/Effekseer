#pragma once

#include "../Utils/Effekseer.BinaryReader.h"
#include "../Effekseer.InternalStruct.h"

namespace Effekseer
{

enum class NonMatchingLODBehaviour : int32_t
{
	Hide = 0,
	DontSpawn = 1,
	DontSpawnAndHide = 2
};

struct ParameterLODs
{
	int MatchingLODs = 0b1111;
	NonMatchingLODBehaviour LODBehaviour = NonMatchingLODBehaviour::Hide;

	void Load(BinaryReader<true>& pos, int version)
	{
		if (version >= Version17Alpha3)
		{
			if (!pos.Peek(this, sizeof(ParameterLODs)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(ParameterLODs));
		}
	}
};

} // namespace Effekseer