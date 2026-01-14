
#include "Effekseer.Trigger.h"

#include "../Effekseer.Instance.h"
#include "../Effekseer.InstanceGlobal.h"

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

	auto loadValue = [&pos](TriggerValues& dst)
	{
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

bool IsTriggerActivated(const TriggerValues& trigger, InstanceGlobal* global, Instance* parent)
{
	switch (trigger.type)
	{
	case TriggerType::None:
		return false;
	case TriggerType::ExternalTrigger:
		return global != nullptr && global->GetInputTriggerCount(trigger.index) > 0;
	case TriggerType::ParentRemoved:
		return parent != nullptr && parent->GetState() != eInstanceState::INSTANCE_STATE_ACTIVE;
	case TriggerType::ParentCollided:
		return parent != nullptr && parent->HasCollidedThisFrame();
	default:
		return false;
	}
}

uint32_t GetTriggerCount(const TriggerValues& trigger, InstanceGlobal* global, Instance* parent)
{
	switch (trigger.type)
	{
	case TriggerType::None:
		return 0;
	case TriggerType::ExternalTrigger:
		return global != nullptr ? global->GetInputTriggerCount(trigger.index) : 0;
	case TriggerType::ParentRemoved:
		return (parent != nullptr && parent->GetState() != eInstanceState::INSTANCE_STATE_ACTIVE) ? 1 : 0;
	case TriggerType::ParentCollided:
		return (parent != nullptr && parent->HasCollidedThisFrame()) ? 1 : 0;
	default:
		return 0;
	}
}

} // namespace Effekseer
