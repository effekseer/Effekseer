

#include "Effekseer.FCurves.h"
#include "Effekseer.InstanceGlobal.h"
#include <cmath>

namespace Effekseer
{

FCurve::FCurve(float defaultValue)
	: defaultValue_(defaultValue)
{
}

int32_t FCurve::Load(const void* data, int32_t version)
{
	return Load(BinaryReader<true>(static_cast<const uint8_t*>(data), std::numeric_limits<int32_t>::max()), version);
}

int32_t FCurve::Load(BinaryReader<true> reader, int32_t version)
{
	const auto begin = reader.GetOffset();
	int32_t count = 0;
	if (!reader.Read(start_) || !reader.Read(end_) || !reader.Read(offsetMax_) || !reader.Read(offsetMin_) ||
		!reader.Read(offset_) || !reader.Read(len_) || !reader.Read(freq_) || !reader.Read(count) ||
		count < 0 || count > 65536 || (count > 0 && freq_ <= 0) || !reader.Read(keys_, count))
	{
		keys_.clear();
		return -1;
	}
	return static_cast<int32_t>(reader.GetOffset() - begin);
}

float FCurve::GetValue(float living, float life, FCurveTimelineType type) const
{
	if (keys_.size() == 0)
		return defaultValue_;

	float frame = 0;
	if (type == FCurveTimelineType::Time)
	{
		frame = living;
	}
	else
	{
		frame = living / life * 100.0f;
	}

	frame -= offset_;
	auto flen = static_cast<float>(len_);

	if (len_ == 0)
	{
		return keys_[0];
	}

	if (frame < 0)
	{
		if (start_ == FCurveEdge::Constant)
		{
			return keys_[0];
		}
		else if (start_ == FCurveEdge::Loop)
		{
			frame = len_ - fmodf(-frame, flen);
		}
		else if (start_ == FCurveEdge::LoopInversely)
		{
			frame = fmodf(-frame, flen);
		}
	}

	if (len_ < frame)
	{
		if (end_ == FCurveEdge::Constant)
		{
			return keys_[keys_.size() - 1];
		}
		else if (end_ == FCurveEdge::Loop)
		{
			frame = fmodf(frame - flen, flen);
		}
		else if (end_ == FCurveEdge::LoopInversely)
		{
			frame = flen - fmodf(frame - flen, flen);
		}
	}

	assert(frame / freq_ >= 0.0f);
	uint32_t ind = static_cast<uint32_t>(frame / freq_);
	auto ep = 0.0001f;
	if (std::abs(frame - flen) < ep)
	{
		return keys_[keys_.size() - 1];
	}
	else if (ind == keys_.size() - 2)
	{
		float subF = (float)(len_ - ind * freq_);
		float subV = keys_[ind + 1] - keys_[ind];
		return subV / (float)(subF) * (float)(frame - ind * freq_) + keys_[ind];
	}
	else
	{
		float subF = (float)(freq_);
		float subV = keys_[ind + 1] - keys_[ind];
		return subV / (float)(subF) * (float)(frame - ind * freq_) + keys_[ind];
	}
}

float FCurve::GetOffset(IRandObject& g) const
{
	return g.GetRand(offsetMin_, offsetMax_);
}

void FCurve::ChangeCoordinate()
{
	offsetMax_ *= -1.0f;
	offsetMin_ *= -1.0f;

	for (size_t i = 0; i < keys_.size(); i++)
	{
		keys_[i] *= -1.0f;
	}
}

void FCurve::Maginify(float value)
{
	offsetMax_ *= value;
	offsetMin_ *= value;

	for (size_t i = 0; i < keys_.size(); i++)
	{
		keys_[i] *= value;
	}
}

int32_t FCurveScalar::Load(const void* data, int32_t version)
{
	return Load(BinaryReader<true>(static_cast<const uint8_t*>(data), std::numeric_limits<int32_t>::max()), version);
}

int32_t FCurveScalar::Load(BinaryReader<true> reader, int32_t version)
{
	const auto begin = reader.GetOffset();
	if (version >= 1600 && !reader.Read(Timeline))
	{
		return -1;
	}

	for (auto* curve : {&S})
	{
		const auto size = curve->Load(reader, version);
		if (size < 0 || !reader.Skip(size))
		{
			return -1;
		}
	}
	return static_cast<int32_t>(reader.GetOffset() - begin);
}

float FCurveScalar::GetValues(float living, float life) const
{
	return S.GetValue(living, life, Timeline);
}

float FCurveScalar::GetOffsets(IRandObject& g) const
{
	return S.GetOffset(g);
}

int32_t FCurveVector2D::Load(const void* data, int32_t version)
{
	return Load(BinaryReader<true>(static_cast<const uint8_t*>(data), std::numeric_limits<int32_t>::max()), version);
}

int32_t FCurveVector2D::Load(BinaryReader<true> reader, int32_t version)
{
	const auto begin = reader.GetOffset();
	if (version >= 15 && !reader.Read(Timeline))
	{
		return -1;
	}

	for (auto* curve : {&X, &Y})
	{
		const auto size = curve->Load(reader, version);
		if (size < 0 || !reader.Skip(size))
		{
			return -1;
		}
	}
	return static_cast<int32_t>(reader.GetOffset() - begin);
}

SIMD::Vec2f FCurveVector2D::GetValues(float living, float life) const
{
	auto x = X.GetValue(living, life, Timeline);
	auto y = Y.GetValue(living, life, Timeline);
	return SIMD::Vec2f{x, y};
}

SIMD::Vec2f FCurveVector2D::GetOffsets(IRandObject& g) const
{
	auto x = X.GetOffset(g);
	auto y = Y.GetOffset(g);
	return SIMD::Vec2f{x, y};
}

int32_t FCurveVector3D::Load(const void* data, int32_t version)
{
	return Load(BinaryReader<true>(static_cast<const uint8_t*>(data), std::numeric_limits<int32_t>::max()), version);
}

int32_t FCurveVector3D::Load(BinaryReader<true> reader, int32_t version)
{
	const auto begin = reader.GetOffset();
	if (version >= 15 && !reader.Read(Timeline))
	{
		return -1;
	}

	for (auto* curve : {&X, &Y, &Z})
	{
		const auto size = curve->Load(reader, version);
		if (size < 0 || !reader.Skip(size))
		{
			return -1;
		}
	}
	return static_cast<int32_t>(reader.GetOffset() - begin);
}

SIMD::Vec3f FCurveVector3D::GetValues(float living, float life) const
{
	auto x = X.GetValue(living, life, Timeline);
	auto y = Y.GetValue(living, life, Timeline);
	auto z = Z.GetValue(living, life, Timeline);
	return {x, y, z};
}

SIMD::Vec3f FCurveVector3D::GetOffsets(IRandObject& g) const
{
	auto x = X.GetOffset(g);
	auto y = Y.GetOffset(g);
	auto z = Z.GetOffset(g);
	return {x, y, z};
}

int32_t FCurveVectorColor::Load(const void* data, int32_t version)
{
	return Load(BinaryReader<true>(static_cast<const uint8_t*>(data), std::numeric_limits<int32_t>::max()), version);
}

int32_t FCurveVectorColor::Load(BinaryReader<true> reader, int32_t version)
{
	const auto begin = reader.GetOffset();
	if (version >= 15 && !reader.Read(Timeline))
	{
		return -1;
	}

	for (auto* curve : {&R, &G, &B, &A})
	{
		const auto size = curve->Load(reader, version);
		if (size < 0 || !reader.Skip(size))
		{
			return -1;
		}
	}
	return static_cast<int32_t>(reader.GetOffset() - begin);
}

std::array<float, 4> FCurveVectorColor::GetValues(float living, float life) const
{
	auto r = R.GetValue(living, life, Timeline);
	auto g = G.GetValue(living, life, Timeline);
	auto b = B.GetValue(living, life, Timeline);
	auto a = A.GetValue(living, life, Timeline);

	return std::array<float, 4>{r, g, b, a};
}

std::array<float, 4> FCurveVectorColor::GetOffsets(IRandObject& gl) const
{
	auto r = R.GetOffset(gl);
	auto g = G.GetOffset(gl);
	auto b = B.GetOffset(gl);
	auto a = A.GetOffset(gl);
	return std::array<float, 4>{r, g, b, a};
}

} // namespace Effekseer
