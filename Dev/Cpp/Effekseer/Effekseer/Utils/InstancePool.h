#ifndef __EFFEKSEER_INSTANCE_POOL_H__
#define __EFFEKSEER_INSTANCE_POOL_H__

#include <stdint.h>
#include <vector>

#include "Effekseer.CustomAllocator.h"

namespace Effekseer
{

template <class T>
class InstancePool
{
	struct ReservedBlock
	{
		CustomAlignedVector<uint8_t> buffer;
	};

	std::vector<std::shared_ptr<ReservedBlock>> blocks_;

	std::queue<T*> containers_;

	void AllocateNewBlock(int count)
	{
		auto block = std::make_shared<ReservedBlock>();
		assert(sizeof(T) % alignment_ == 0);
		block->buffer.resize(count * sizeof(T));
		blocks_.emplace_back(block);

		for (size_t i = 0; i < count; i++)
		{
			containers_.push(reinterpret_cast<T*>(block->buffer.data() + i * sizeof(T)));
		}
	}

	int count_ = 0;
	int alignment_ = 0;
	bool extendEnabled_ = false;

public:
	void Init(int count, int alignment, bool extendEnabled)
	{
		count_ = count;
		alignment_ = alignment;
		extendEnabled_ = extendEnabled;

		AllocateNewBlock(count_);
	}

	T* Pop()
	{
		if (containers_.empty())
		{
			if (extendEnabled_)
			{
				AllocateNewBlock(count_);
			}
			else
			{
				return nullptr;
			}
		}

		auto front = containers_.front();
		containers_.pop();
		return front;
	}

	void Push(T* value)
	{
		containers_.push(value);
	}

	int32_t GetRestInstance() const
	{
		return static_cast<int>(containers_.size());
	}
};

} // namespace Effekseer

#endif