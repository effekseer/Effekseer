#pragma once

#include "../Effekseer.Resource.h"
#include "../SIMD/Vec3f.h"
#include "../Utils/Effekseer.CustomAllocator.h"
#include <array>
#include <stdint.h>

namespace Effekseer
{

enum class VectorFieldSamplingMode
{
	Clamp,
	Repeat,
};

class VectorField : public Resource
{
private:
	std::array<int32_t, 3> size_;
	CustomAlignedVector<float> data_;
	int32_t stride_ = 0;

public:
	void Init(const std::array<int32_t, 3>& size, int32_t stride);

	std::array<int32_t, 3> GetSize() const;

	std::array<float, 3> GetData(const std::array<int32_t, 3>& position, VectorFieldSamplingMode sampling_mode) const;

	void SetData(const std::array<float, 3>& value, const std::array<int32_t, 3>& position);

	std::array<float, 3> GetData(const std::array<float, 3>& position, VectorFieldSamplingMode sampling_mode) const;
};

using VectorFieldRef = RefPtr<VectorField>;

} // namespace Effekseer
