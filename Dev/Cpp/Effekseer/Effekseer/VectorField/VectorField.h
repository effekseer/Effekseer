#pragma once

#include "../Utils/Effekseer.CustomAllocator.h"
#include <array>
#include <stdint.h>

namespace Effekseer
{

template <class T>
class VectorField
{
private:
	std::array<int32_t, 3> size_;
	CustomAlignedVector<T> data_;

public:
	void Init(const std::array<int32_t, 3>& size)
	{
		size_ = size;
		data_.resize(size_[0] * size_[1] * size_[2]);
	}

	T GetData(const std::array<int32_t, 3>& position) const
	{
		return data_[position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]];
	}

	void SetData(const T& value, const std::array<int32_t, 3>& position)
	{
		data_[position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]] = value;
	}
};

} // namespace Effekseer
