
#ifndef __EFFEKSEER_PARAMETER_TRIGGER_H__
#define __EFFEKSEER_PARAMETER_TRIGGER_H__

#include "../Effekseer.Base.h"

namespace Effekseer
{

enum class TriggerType : uint8_t
{
	None = 0,
	ExternalTrigger = 1,
};

struct alignas(2) TriggerValues
{
	TriggerType type = TriggerType::None;
	uint8_t index = 0;
};

struct TriggerParameter
{
	TriggerValues ToStartGeneration;
	TriggerValues ToStopGeneration;
	TriggerValues ToRemove;

	void Load(uint8_t*& pos, int32_t version);
};

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETER_TRIGGER_H__
