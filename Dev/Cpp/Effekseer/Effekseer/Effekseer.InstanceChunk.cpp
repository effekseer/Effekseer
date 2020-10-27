

#include "Effekseer.InstanceChunk.h"
#include "Effekseer.InstanceGlobal.h"
#include <assert.h>

namespace Effekseer
{

InstanceChunk::InstanceChunk()
{
	std::fill(instancesAlive_.begin(), instancesAlive_.end(), false);
}

InstanceChunk::~InstanceChunk()
{
}

void InstanceChunk::UpdateInstances()
{
	for (int32_t i = 0; i < InstancesOfChunk; i++)
	{
		if (instancesAlive_[i])
		{
			Instance* instance = reinterpret_cast<Instance*>(instances_[i]);

			if (instance->m_State == INSTANCE_STATE_ACTIVE)
			{
				auto deltaTime = instance->GetInstanceGlobal()->NextDeltaFrame;
				instance->Update(deltaTime, true);
			}
			else if (instance->m_State == INSTANCE_STATE_REMOVING)
			{
				// start to remove
				instance->m_State = INSTANCE_STATE_REMOVED;
			}
			else if (instance->m_State == INSTANCE_STATE_REMOVED)
			{
				instance->~Instance();
				instancesAlive_[i] = false;
				aliveCount_--;
			}
		}
	}
}
void InstanceChunk::UpdateInstancesByInstanceGlobal(const InstanceGlobal* global)
{
	for (int32_t i = 0; i < InstancesOfChunk; i++)
	{
		if (instancesAlive_[i])
		{
			Instance* instance = reinterpret_cast<Instance*>(instances_[i]);

			if (global != instance->GetInstanceGlobal())
			{
				continue;
			}

			if (instance->m_State == INSTANCE_STATE_ACTIVE)
			{
				auto deltaTime = global->NextDeltaFrame;
				instance->Update(deltaTime, true);
			}
			else if (instance->m_State == INSTANCE_STATE_REMOVING)
			{
				// start to remove
				instance->m_State = INSTANCE_STATE_REMOVED;
			}
			else if (instance->m_State == INSTANCE_STATE_REMOVED)
			{
				instance->~Instance();
				instancesAlive_[i] = false;
				aliveCount_--;
			}
		}
	}
}

Instance* InstanceChunk::CreateInstance(Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup)
{
	for (int32_t i = 0; i < InstancesOfChunk; i++)
	{
		if (!instancesAlive_[i])
		{
			instancesAlive_[i] = true;
			aliveCount_++;
			return new (instances_[i]) Instance(pManager, pEffectNode, pContainer, pGroup);
		}
	}
	return nullptr;
}

} // namespace Effekseer
