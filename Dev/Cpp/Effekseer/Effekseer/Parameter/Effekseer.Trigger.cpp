
#include "Effekseer.Trigger.h"

#include <cstring>

#include "../Utils/Effekseer.BinaryVersion.h"

namespace Effekseer
{

void TriggerParameter::Load(uint8_t*& pos, int32_t version)
{
	ToStartGeneration = {};
	ToStopGeneration = {};
	ToRemove = {};

	if (version < Version17Alpha1)
	{
		return;
	}

	uint8_t flags = 0;
	memcpy(&flags, pos, sizeof(uint8_t));
	pos += sizeof(uint8_t);

	auto loadValue = [&pos](TriggerValues& dst) {
		memcpy(&dst, pos, sizeof(TriggerValues));
		pos += sizeof(TriggerValues);
	};

	if (flags & (1 << 0))
	{
		loadValue(ToStartGeneration);
	}

	if (flags & (1 << 1))
	{
		loadValue(ToStopGeneration);
	}

	if (flags & (1 << 2))
	{
		loadValue(ToRemove);
	}
}

} // namespace Effekseer
