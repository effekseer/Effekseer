#pragma once

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

	void Load(unsigned char*& pos, int version)
	{
		if (version >= Version17Alpha3)
		{
			memcpy(this, pos, sizeof(ParameterLODs));
			pos += sizeof(ParameterLODs);
		}
	}
};

} // namespace Effekseer