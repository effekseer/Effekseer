#include "VectorField.h"

namespace Effekseer
{
void VectorField::Init(const std::array<int32_t, 3>& size, int32_t stride)
{
	size_ = size;
	stride_ = stride;
	data_.resize((size_[0] * size_[1] * size_[2]) * stride_);
}

std::array<int32_t, 3> VectorField::GetSize() const
{
	return size_;
}

std::array<float, 3> VectorField::GetData(const std::array<int32_t, 3>& position, VectorFieldSamplingMode sampling_mode) const
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

	std::array<float, 3> ret;
	ret.fill(0.0f);
	for (int i = 0; i < stride_; i++)
	{
		ret[i] = data_[(position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]) * stride_ + i];
	}

	return ret;
}

void VectorField::SetData(const std::array<float, 3>& value, const std::array<int32_t, 3>& position)
{
	for (int i = 0; i < stride_; i++)
	{
		data_[(position[0] + position[1] * size_[0] + position[2] * size_[0] * size_[1]) * stride_ + i] = value[i];
	}
}

std::array<float, 3> VectorField::GetData(const std::array<float, 3>& position, VectorFieldSamplingMode sampling_mode) const
{
	const auto lerp = [&](std::array<float, 3> v1, std::array<float, 3> v2, float a)
	{
		std::array<float, 3> ret{};
		for (int i = 0; i < stride_; i++)
		{
			ret[i] = (v2[i] - v1[i]) * a + v1[i];
		}
		return ret;
	};

	std::array<int32_t, 3> iv[2];
	std::array<float, 3> dv;
	for (size_t i = 0; i < 3; i++)
	{
		iv[0][i] = static_cast<int>(position[i]);
		dv[i] = position[i] - iv[0][i];
		iv[1][i] = iv[0][i] + 1;
	}

	std::array<float, 3> v[2][2][2];
	for (int z = 0; z < 2; z++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 2; x++)
			{
				v[x][y][z] = GetData(std::array<int32_t, 3>{iv[x][0], iv[y][1], iv[z][2]}, sampling_mode);
			}
		}
	}

	std::array<float, 3> vxy[2][2];
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 2; x++)
		{
			vxy[x][y] = lerp(v[x][y][0], v[x][y][1], dv[2]);
		}
	}

	std::array<float, 3> vx[2];
	for (int x = 0; x < 2; x++)
	{
		vx[x] = lerp(vxy[x][0], vxy[x][1], dv[1]);
	}

	auto ret = lerp(vx[0], vx[1], dv[0]);

	for (size_t i = stride_; i < ret.size(); i++)
	{
		ret[i] = 0.0f;
	}

	return ret;
}
} // namespace Effekseer
