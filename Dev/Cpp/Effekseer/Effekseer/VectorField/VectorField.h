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

template <class T>
class VectorField : public Resource
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

	std::array<int32_t, 3> GetSize() const
	{
		return size_;
	}

	T GetData(const std::array<int32_t, 3>& position, VectorFieldSamplingMode sampling_mode) const
	{
		auto p = position;
		if (sampling_mode == VectorFieldSamplingMode::Clamp)
		{
			for (int i = 0; i < 3; i++)
			{
				p[i] = Clamp(p[i], size_[i] - 1, 0);
			}
		}
		else if (sampling_mode == VectorFieldSamplingMode::Repeat)
		{
			for (int i = 0; i < 3; i++)
			{
				p[i] = p[i] % size_[i];
			}
		}

		return data_[position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]];
	}

	void SetData(const T& value, const std::array<int32_t, 3>& position)
	{
		data_[position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]] = value;
	}

	T GetData(const std::array<float, 3>& position, VectorFieldSamplingMode sampling_mode) const
	{
		std::array<int32_t, 3> iv[2];
		std::array<float, 3> dv;
		for (size_t i = 0; i < 3; i++)
		{
			iv[0][i] = static_cast<int>(position[i]);
			dv[i] = position[i] - iv[0][i];
			iv[1][i] = iv[0][i] + 1;
		}

		T v[2][2][2];
		for (int z = 0; z < 2; z++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					v[x][y][z] = GetData({iv[x][0], iv[y][1], iv[z][2]}, sampling_mode);
				}
			}
		}

		T vxy[2][2];
		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 2; x++)
			{
				vxy[x][y] = v[x][y][0] * (1.0f - dv[2]) + v[x][y][1] * (dv[2]);
			}
		}

		T vx[2];
		for (int x = 0; x < 2; x++)
		{
			vx[x] = vxy[x][0] * (1.0f - dv[1]) + vxy[x][1] * (dv[1]);
		}

		return vx[0] * (1.0f - dv[0]) + vx[1] * (dv[0]);
	}
};

using VectorFieldScalar = VectorField<float>;
using VectorFieldVector = VectorField<SIMD::Vec3f>;

using VectorFieldScalarRef = RefPtr<VectorFieldScalar>;
using VectorFieldVectorRef = RefPtr<VectorFieldVector>;
} // namespace Effekseer
