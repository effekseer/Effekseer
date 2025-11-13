
#ifndef __EFFEKSEER_PARAMETER_TRIGGER_H__
#define __EFFEKSEER_PARAMETER_TRIGGER_H__

#include "../Effekseer.Base.h"

namespace Effekseer
{

class Instance;
class InstanceGlobal;

enum class TriggerType : uint8_t
{
	None = 0,
	ExternalTrigger = 1,
	ParentRemoved = 2,
	ParentCollided = 3,
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

bool IsTriggerActivated(const TriggerValues& trigger, InstanceGlobal* global, Instance* parent);

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETER_TRIGGER_H__
