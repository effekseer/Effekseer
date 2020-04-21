#ifndef __EFFEKSEER_SIMD4F_GEN_H__
#define __EFFEKSEER_SIMD4F_GEN_H__

#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_GEN)

#include <cstring>
#include <algorithm>

namespace Effekseer
{

inline float Sqrt(float x)
{
	return std::sqrt(x);
}
inline float Rsqrt(float x)
{
	return 1.0f / std::sqrt(x);
}

struct SIMD4i;

/**
	@brief	simd class for generic
*/
struct alignas(16) SIMD4f
{
	union {
		float vf[4];
		int32_t vi[4];
		uint32_t vu[4];
	};

	SIMD4f() = default;
	SIMD4f(const SIMD4f& rhs) = default;
	SIMD4f(float x, float y, float z, float w) { vf[0] = x; vf[1] = y; vf[2] = z; vf[3] = w; }
	SIMD4f(float i) { vf[0] = i; vf[1] = i; vf[2] = i; vf[3] = i; }

	float GetX() const { return vf[0]; }
	float GetY() const { return vf[1]; }
	float GetZ() const { return vf[2]; }
	float GetW() const { return vf[3]; }

	void SetX(float o) { vf[0] = o; }
	void SetY(float o) { vf[1] = o; }
	void SetZ(float o) { vf[2] = o; }
	void SetW(float o) { vf[3] = o; }

	template <size_t LANE>
	SIMD4f Dup() { return SIMD4f(vf[LANE], vf[LANE], vf[LANE], vf[LANE]); }

	SIMD4i Convert4i() const;
	SIMD4i Cast4i() const;

	SIMD4f& operator+=(const SIMD4f& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] += rhs.vf[i];
		}
		return *this;
	}

	SIMD4f& operator-=(const SIMD4f& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] -= rhs.vf[i];
		}
		return *this;
	}

	SIMD4f& operator*=(const SIMD4f& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] *= rhs.vf[i];
		}
		return *this;
	}

	SIMD4f& operator*=(float rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] *= rhs;
		}
		return *this;
	}

	SIMD4f& operator/=(const SIMD4f& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] /= rhs.vf[i];
		}
		return *this;
	}

	SIMD4f& operator/=(float rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vf[i] /= rhs;
		}
		return *this;
	}

	static SIMD4f Load2(const void* mem);
	static void Store2(void* mem, const SIMD4f& i);
	static SIMD4f Load3(const void* mem);
	static void Store3(void* mem, const SIMD4f& i);
	static SIMD4f Load4(const void* mem);
	static void Store4(void* mem, const SIMD4f& i);

	static SIMD4f SetZero();
	static SIMD4f SetInt(int32_t x, int32_t y, int32_t z, int32_t w);
	static SIMD4f SetUInt(uint32_t x, uint32_t y, uint32_t z, uint32_t w);
	static SIMD4f Sqrt(const SIMD4f& in);
	static SIMD4f Rsqrt(const SIMD4f& in);
	static SIMD4f Abs(const SIMD4f& in);
	static SIMD4f Min(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Max(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Floor(const SIMD4f& in);
	static SIMD4f Ceil(const SIMD4f& in);
	static SIMD4f MulAdd(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	static SIMD4f MulSub(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);

	template<size_t LANE>
	static SIMD4f MulLane(const SIMD4f& lhs, const SIMD4f& rhs);
	template<size_t LANE>
	static SIMD4f MulAddLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	template<size_t LANE>
	static SIMD4f MulSubLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
	static SIMD4f Swizzle(const SIMD4f& in);

	static SIMD4f Dot3(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Cross3(const SIMD4f& lhs, const SIMD4f& rhs);

	template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
	static SIMD4f Mask();
	static uint32_t MoveMask(const SIMD4f& in);
	static SIMD4f Select(const SIMD4f& mask, const SIMD4f& sel1, const SIMD4f& sel2);
	static SIMD4f Equal(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NotEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NearEqual(const SIMD4f& lhs, const SIMD4f& rhs, float epsilon = DefaultEpsilon);
	static SIMD4f IsZero(const SIMD4f& in, float epsilon = DefaultEpsilon);
	static void Transpose(SIMD4f& s0, SIMD4f& s1, SIMD4f& s2, SIMD4f& s3);
};

} // namespace Effekseer

namespace Effekseer
{

inline SIMD4f operator+(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] + rhs.vf[i];
	}
	return ret;
}

inline SIMD4f operator-(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] - rhs.vf[i];
	}
	return ret;
}

inline SIMD4f operator*(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] * rhs.vf[i];
	}
	return ret;
}

inline SIMD4f operator*(const SIMD4f& lhs, float rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] * rhs;
	}
	return ret;
}

inline SIMD4f operator/(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] / rhs.vf[i];
	}
	return ret;
}

inline SIMD4f operator/(const SIMD4f& lhs, float rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = lhs.vf[i] / rhs;
	}
	return ret;
}

inline SIMD4f operator&(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] & rhs.vu[i];
	}
	return ret;
}

inline SIMD4f operator|(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] | rhs.vu[i];
	}
	return ret;
}

inline SIMD4f operator^(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] ^ rhs.vu[i];
	}
	return ret;
}

inline bool operator==(const SIMD4f& lhs, const SIMD4f& rhs)
{
	bool ret = true;
	for (size_t i = 0; i < 4; i++)
	{
		ret &= lhs.vf[i] == rhs.vf[i];
	}
	return ret;
}

inline bool operator!=(const SIMD4f& lhs, const SIMD4f& rhs)
{
	bool ret = true;
	for (size_t i = 0; i < 4; i++)
	{
		ret &= lhs.vf[i] == rhs.vf[i];
	}
	return !ret;
}

inline SIMD4f SIMD4f::Load2(const void* mem)
{
	SIMD4f ret;
	memcpy(ret.vf, mem, sizeof(float) * 2);
	// This code causes bugs in asmjs
	// ret.vf[0] = *((float*)mem + 0);
	// ret.vf[1] = *((float*)mem + 1);
	return ret;
}

inline void SIMD4f::Store2(void* mem, const SIMD4f& i)
{
	memcpy(mem, i.vf, sizeof(float) * 2);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vf[0];
	// *((float*)mem + 1) = i.vf[1];
}

inline SIMD4f SIMD4f::Load3(const void* mem)
{
	SIMD4f ret;
	memcpy(ret.vf, mem, sizeof(float) * 3);
	// This code causes bugs in asmjs
	// ret.vf[0] = *((float*)mem + 0);
	// ret.vf[1] = *((float*)mem + 1);
	// ret.vf[2] = *((float*)mem + 2);
	return ret;
}

inline void SIMD4f::Store3(void* mem, const SIMD4f& i)
{
	memcpy(mem, i.vf, sizeof(float) * 3);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vf[0];
	// *((float*)mem + 1) = i.vf[1];
	// *((float*)mem + 2) = i.vf[2];
}

inline SIMD4f SIMD4f::Load4(const void* mem)
{
	SIMD4f ret;
	memcpy(ret.vf, mem, sizeof(float) * 4);
	// This code causes bugs in emscripten
	// ret.vf[0] = *((float*)mem + 0);
	// ret.vf[1] = *((float*)mem + 1);
	// ret.vf[2] = *((float*)mem + 2);
	// ret.vf[3] = *((float*)mem + 3);
	return ret;
}

inline void SIMD4f::Store4(void* mem, const SIMD4f& i)
{
	memcpy(mem, i.vf, sizeof(float) * 4);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vf[0];
	// *((float*)mem + 1) = i.vf[1];
	// *((float*)mem + 2) = i.vf[2];
	// *((float*)mem + 3) = i.vf[3];
}

inline SIMD4f SIMD4f::SetZero()
{
	SIMD4f ret;
	ret.vf[0] = 0.0f;
	ret.vf[1] = 0.0f;
	ret.vf[2] = 0.0f;
	ret.vf[3] = 0.0f;
	return ret;
}

inline SIMD4f SIMD4f::SetInt(int32_t x, int32_t y, int32_t z, int32_t w)
{
	SIMD4f ret;
	ret.vu[0] = (uint32_t)x;
	ret.vu[1] = (uint32_t)y;
	ret.vu[2] = (uint32_t)z;
	ret.vu[3] = (uint32_t)w;
	return ret;
}

inline SIMD4f SIMD4f::SetUInt(uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
	SIMD4f ret;
	ret.vu[0] = (uint32_t)x;
	ret.vu[1] = (uint32_t)y;
	ret.vu[2] = (uint32_t)z;
	ret.vu[3] = (uint32_t)w;
	return ret;
}

inline SIMD4f SIMD4f::Sqrt(const SIMD4f& in)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::sqrt(in.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Rsqrt(const SIMD4f& in)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = 1.0f / std::sqrt(in.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Abs(const SIMD4f& in)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::abs(in.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Min(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::fmin(lhs.vf[i], rhs.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Max(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::fmax(lhs.vf[i], rhs.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Floor(const SIMD4f& in)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::floor(in.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Ceil(const SIMD4f& in)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = std::ceil(in.vf[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::MulAdd(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = a.vf[i] + b.vf[i] * c.vf[i];
}
	return ret;
}

inline SIMD4f SIMD4f::MulSub(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vf[i] = a.vf[i] - b.vf[i] * c.vf[i];
}
	return ret;
}

inline SIMD4f SIMD4f::Dot3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f muled = lhs * rhs;
	return SIMD4f{muled.vf[0] + muled.vf[1] + muled.vf[2], 0.0f, 0.0f, 0.0f};
}

inline SIMD4f SIMD4f::Cross3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::Swizzle<1,2,0,3>(lhs) * SIMD4f::Swizzle<2,0,1,3>(rhs) -
		SIMD4f::Swizzle<2,0,1,3>(lhs) * SIMD4f::Swizzle<1,2,0,3>(rhs);
}

template<size_t LANE>
SIMD4f SIMD4f::MulLane(const SIMD4f& lhs, const SIMD4f& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return lhs * rhs.vf[LANE];
}

template<size_t LANE>
SIMD4f SIMD4f::MulAddLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a + b * c.vf[LANE];
}

template<size_t LANE>
SIMD4f SIMD4f::MulSubLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a - b * c.vf[LANE];
}

template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
SIMD4f SIMD4f::Swizzle(const SIMD4f& in)
{
	static_assert(indexX < 4, "indexX is must be less than 4.");
	static_assert(indexY < 4, "indexY is must be less than 4.");
	static_assert(indexZ < 4, "indexZ is must be less than 4.");
	static_assert(indexW < 4, "indexW is must be less than 4.");
	return SIMD4f{in.vf[indexX], in.vf[indexY], in.vf[indexZ], in.vf[indexW]};
}


template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
SIMD4f SIMD4f::Mask()
{
	static_assert(X >= 2, "indexX is must be set 0 or 1.");
	static_assert(Y >= 2, "indexY is must be set 0 or 1.");
	static_assert(Z >= 2, "indexZ is must be set 0 or 1.");
	static_assert(W >= 2, "indexW is must be set 0 or 1.");
	SIMD4f ret;
	ret.vu[0] = 0xffffffff * X;
	ret.vu[1] = 0xffffffff * Y;
	ret.vu[2] = 0xffffffff * Z;
	ret.vu[3] = 0xffffffff * W;
	return ret;
}

inline uint32_t SIMD4f::MoveMask(const SIMD4f& in)
{
	return (in.vu[0] & 0x1) | (in.vu[1] & 0x2) | (in.vu[2] & 0x4) | (in.vu[3] & 0x8);
}

inline SIMD4f SIMD4f::Select(const SIMD4f& mask, const SIMD4f& sel1, const SIMD4f& sel2)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (mask.vu[i] & sel1.vu[i]) | (~mask.vu[i] & sel2.vu[i]);
	}
	return ret;
}

inline SIMD4f SIMD4f::Equal(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] == rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::NotEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] != rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::LessThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] < rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::LessEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] <= rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] > rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vf[i] >= rhs.vf[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::NearEqual(const SIMD4f& lhs, const SIMD4f& rhs, float epsilon)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (std::abs(lhs.vf[i] - rhs.vf[i]) <= epsilon) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4f SIMD4f::IsZero(const SIMD4f& in, float epsilon)
{
	SIMD4f ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (std::abs(in.vf[i]) <= epsilon) ? 0xffffffff : 0;
	}
	return ret;
}

inline void SIMD4f::Transpose(SIMD4f& s0, SIMD4f& s1, SIMD4f& s2, SIMD4f& s3)
{
	std::swap(s0.vf[1], s1.vf[0]);
	std::swap(s0.vf[2], s2.vf[0]);
	std::swap(s0.vf[3], s3.vf[0]);
	std::swap(s1.vf[2], s2.vf[1]);
	std::swap(s2.vf[3], s3.vf[2]);
	std::swap(s1.vf[3], s3.vf[1]);
}

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SIMD4F_GEN_H__