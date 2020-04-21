
#ifndef __EFFEKSEER_SIMD4I_GEN_H__
#define __EFFEKSEER_SIMD4I_GEN_H__

#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_GEN)

#include <cstring>
#include <algorithm>

namespace Effekseer
{

struct SIMD4f;

/**
	@brief	simd class for generic
*/
struct alignas(16) SIMD4i
{
	union {
		float vf[4];
		int32_t vi[4];
		uint32_t vu[4];
	};

	SIMD4i() = default;
	SIMD4i(const SIMD4i& rhs) = default;
	SIMD4i(int32_t x, int32_t y, int32_t z, int32_t w) { vi[0] = x; vi[1] = y; vi[2] = z; vi[3] = w; }
	SIMD4i(int32_t i) { vi[0] = i; vi[1] = i; vi[2] = i; vi[3] = i; }

	int32_t GetX() const { return vi[0]; }
	int32_t GetY() const { return vi[1]; }
	int32_t GetZ() const { return vi[2]; }
	int32_t GetW() const { return vi[3]; }

	void SetX(int32_t o) { vi[0] = o; }
	void SetY(int32_t o) { vi[1] = o; }
	void SetZ(int32_t o) { vi[2] = o; }
	void SetW(int32_t o) { vi[3] = o; }

	SIMD4f Convert4f() const;
	SIMD4f Cast4f() const;

	SIMD4i& operator+=(const SIMD4i& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] += rhs.vi[i];
		}
		return *this;
	}

	SIMD4i& operator-=(const SIMD4i& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] -= rhs.vi[i];
		}
		return *this;
	}

	SIMD4i& operator*=(const SIMD4i& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] *= rhs.vi[i];
		}
		return *this;
	}

	SIMD4i& operator*=(int32_t rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] *= rhs;
		}
		return *this;
	}

	SIMD4i& operator/=(const SIMD4i& rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] /= rhs.vi[i];
		}
		return *this;
	}

	SIMD4i& operator/=(int32_t rhs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			vi[i] /= rhs;
		}
		return *this;
	}

	static SIMD4i Load2(const void* mem);
	static void Store2(void* mem, const SIMD4i& i);
	static SIMD4i Load3(const void* mem);
	static void Store3(void* mem, const SIMD4i& i);
	static SIMD4i Load4(const void* mem);
	static void Store4(void* mem, const SIMD4i& i);

	static SIMD4i SetZero();
	static SIMD4i Abs(const SIMD4i& in);
	static SIMD4i Min(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i Max(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i MulAdd(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	static SIMD4i MulSub(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);

	template<size_t LANE>
	static SIMD4i MulLane(const SIMD4i& lhs, const SIMD4i& rhs);
	template<size_t LANE>
	static SIMD4i MulAddLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	template<size_t LANE>
	static SIMD4i MulSubLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
	static SIMD4i Swizzle(const SIMD4i& in);

	template <int COUNT>
	static SIMD4i ShiftL(const SIMD4i& in);
	template <int COUNT>
	static SIMD4i ShiftR(const SIMD4i& in);
	template <int COUNT>
	static SIMD4i ShiftRA(const SIMD4i& in);

	template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
	static SIMD4i Mask();
	static uint32_t MoveMask(const SIMD4i& in);
	static SIMD4i Equal(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i NotEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i LessThan(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i LessEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i GreaterThan(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i GreaterEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i NearEqual(const SIMD4i& lhs, const SIMD4i& rhs, float epsilon = DefaultEpsilon);
	static SIMD4i IsZero(const SIMD4i& in, float epsilon = DefaultEpsilon);
	static void Transpose(SIMD4i& s0, SIMD4i& s1, SIMD4i& s2, SIMD4i& s3);
};

} // namespace Effekseer

namespace Effekseer
{

inline SIMD4i operator+(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] + rhs.vi[i];
	}
	return ret;
}

inline SIMD4i operator-(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] - rhs.vi[i];
	}
	return ret;
}

inline SIMD4i operator*(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] * rhs.vi[i];
	}
	return ret;
}

inline SIMD4i operator*(const SIMD4i& lhs, int32_t rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] * rhs;
	}
	return ret;
}

inline SIMD4i operator/(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] / rhs.vi[i];
	}
	return ret;
}

inline SIMD4i operator/(const SIMD4i& lhs, int32_t rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] / rhs;
	}
	return ret;
}

inline SIMD4i operator&(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] & rhs.vu[i];
	}
	return ret;
}

inline SIMD4i operator|(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] | rhs.vu[i];
	}
	return ret;
}

inline SIMD4i operator^(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] ^ rhs.vu[i];
	}
	return ret;
}

inline bool operator==(const SIMD4i& lhs, const SIMD4i& rhs)
{
	bool ret = true;
	for (size_t i = 0; i < 4; i++)
	{
		ret &= lhs.vi[i] == rhs.vi[i];
	}
	return ret;
}

inline bool operator!=(const SIMD4i& lhs, const SIMD4i& rhs)
{
	bool ret = true;
	for (size_t i = 0; i < 4; i++)
	{
		ret &= lhs.vi[i] == rhs.vi[i];
	}
	return !ret;
}

inline SIMD4i SIMD4i::Load2(const void* mem)
{
	SIMD4i ret;
	memcpy(ret.vi, mem, sizeof(float) * 2);
	// This code causes bugs in asmjs
	// ret.vi[0] = *((float*)mem + 0);
	// ret.vi[1] = *((float*)mem + 1);
	return ret;
}

inline void SIMD4i::Store2(void* mem, const SIMD4i& i)
{
	memcpy(mem, i.vi, sizeof(float) * 2);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vi[0];
	// *((float*)mem + 1) = i.vi[1];
}

inline SIMD4i SIMD4i::Load3(const void* mem)
{
	SIMD4i ret;
	memcpy(ret.vi, mem, sizeof(float) * 3);
	// This code causes bugs in asmjs
	// ret.vi[0] = *((float*)mem + 0);
	// ret.vi[1] = *((float*)mem + 1);
	// ret.vi[2] = *((float*)mem + 2);
	return ret;
}

inline void SIMD4i::Store3(void* mem, const SIMD4i& i)
{
	memcpy(mem, i.vi, sizeof(float) * 3);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vi[0];
	// *((float*)mem + 1) = i.vi[1];
	// *((float*)mem + 2) = i.vi[2];
}

inline SIMD4i SIMD4i::Load4(const void* mem)
{
	SIMD4i ret;
	memcpy(ret.vi, mem, sizeof(float) * 4);
	// This code causes bugs in emscripten
	// ret.vi[0] = *((float*)mem + 0);
	// ret.vi[1] = *((float*)mem + 1);
	// ret.vi[2] = *((float*)mem + 2);
	// ret.vi[3] = *((float*)mem + 3);
	return ret;
}

inline void SIMD4i::Store4(void* mem, const SIMD4i& i)
{
	memcpy(mem, i.vi, sizeof(float) * 4);
	// This code causes bugs in asmjs
	// *((float*)mem + 0) = i.vi[0];
	// *((float*)mem + 1) = i.vi[1];
	// *((float*)mem + 2) = i.vi[2];
	// *((float*)mem + 3) = i.vi[3];
}

inline SIMD4i SIMD4i::SetZero()
{
	SIMD4i ret;
	ret.vi[0] = 0;
	ret.vi[1] = 0;
	ret.vi[2] = 0;
	ret.vi[3] = 0;
	return ret;
}

inline SIMD4i SIMD4i::Abs(const SIMD4i& in)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = std::abs(in.vi[i]);
	}
	return ret;
}

inline SIMD4i SIMD4i::Min(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = (lhs.vi[i] < rhs.vi[i]) ? lhs.vi[i] : rhs.vi[i];
	}
	return ret;
}

inline SIMD4i SIMD4i::Max(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = (lhs.vi[i] > rhs.vi[i]) ? lhs.vi[i] : rhs.vi[i];
	}
	return ret;
}

inline SIMD4i SIMD4i::MulAdd(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = a.vi[i] + b.vi[i] * c.vi[i];
}
	return ret;
}

inline SIMD4i SIMD4i::MulSub(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = a.vi[i] - b.vi[i] * c.vi[i];
}
	return ret;
}

template<size_t LANE>
SIMD4i SIMD4i::MulLane(const SIMD4i& lhs, const SIMD4i& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return lhs * rhs.vi[LANE];
}

template<size_t LANE>
SIMD4i SIMD4i::MulAddLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a + b * c.vi[LANE];
}

template<size_t LANE>
SIMD4i SIMD4i::MulSubLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a - b * c.vi[LANE];
}

template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
SIMD4i SIMD4i::Swizzle(const SIMD4i& in)
{
	static_assert(indexX < 4, "indexX is must be less than 4.");
	static_assert(indexY < 4, "indexY is must be less than 4.");
	static_assert(indexZ < 4, "indexZ is must be less than 4.");
	static_assert(indexW < 4, "indexW is must be less than 4.");
	return SIMD4i{in.vi[indexX], in.vi[indexY], in.vi[indexZ], in.vi[indexW]};
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftL(const SIMD4i& lhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] << COUNT;
	}
	return ret;
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftR(const SIMD4i& lhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = lhs.vu[i] >> COUNT;
	}
	return ret;
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftRA(const SIMD4i& lhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vi[i] = lhs.vi[i] >> COUNT;
	}
	return ret;
}

template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
SIMD4i SIMD4i::Mask()
{
	static_assert(X >= 2, "indexX is must be set 0 or 1.");
	static_assert(Y >= 2, "indexY is must be set 0 or 1.");
	static_assert(Z >= 2, "indexZ is must be set 0 or 1.");
	static_assert(W >= 2, "indexW is must be set 0 or 1.");
	SIMD4i ret;
	ret.vu[0] = 0xffffffff * X;
	ret.vu[1] = 0xffffffff * Y;
	ret.vu[2] = 0xffffffff * Z;
	ret.vu[3] = 0xffffffff * W;
	return ret;
}

inline uint32_t SIMD4i::MoveMask(const SIMD4i& in)
{
	return (in.vu[0] & 0x1) | (in.vu[1] & 0x2) | (in.vu[2] & 0x4) | (in.vu[3] & 0x8);
}

inline SIMD4i SIMD4i::Equal(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] == rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::NotEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] != rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::LessThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] < rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::LessEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] <= rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::GreaterThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] > rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::GreaterEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (lhs.vi[i] >= rhs.vi[i]) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::NearEqual(const SIMD4i& lhs, const SIMD4i& rhs, float epsilon)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (std::abs(lhs.vi[i] - rhs.vi[i]) <= epsilon) ? 0xffffffff : 0;
	}
	return ret;
}

inline SIMD4i SIMD4i::IsZero(const SIMD4i& in, float epsilon)
{
	SIMD4i ret;
	for (size_t i = 0; i < 4; i++)
	{
		ret.vu[i] = (std::abs(in.vi[i]) <= epsilon) ? 0xffffffff : 0;
	}
	return ret;
}

inline void SIMD4i::Transpose(SIMD4i& s0, SIMD4i& s1, SIMD4i& s2, SIMD4i& s3)
{
	std::swap(s0.vi[1], s1.vi[0]);
	std::swap(s0.vi[2], s2.vi[0]);
	std::swap(s0.vi[3], s3.vi[0]);
	std::swap(s1.vi[2], s2.vi[1]);
	std::swap(s2.vi[3], s3.vi[2]);
	std::swap(s1.vi[3], s3.vi[1]);
}

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SIMD4I_GEN_H__