
#ifndef __EFFEKSEER_SIMD4F_NEON_H__
#define __EFFEKSEER_SIMD4F_NEON_H__

#include <arm_neon.h>

namespace Effekseer
{

/**
	@brief	simd class for neon
*/
struct alignas(16) SIMD4f
{
	union {
		float32x4_t s;
		float f[4];
	};

	float GetX() const { return f[0]; }
	float GetY() const { return f[1]; }
	float GetZ() const { return f[2]; }
	float GetW() const { return f[3]; }

	void SetX(float o) { f[0] = o; }
	void SetY(float o) { f[1] = o; }
	void SetZ(float o) { f[2] = o; }
	void SetW(float o) { f[3] = o; }
};

inline SIMD4f operator+(const SIMD4f& lhs, const SIMD4f& rhs) { return SIMD4f{vaddq_f32(lhs.s, rhs.s)}; }

inline SIMD4f operator-(const SIMD4f& lhs, const SIMD4f& rhs) { return SIMD4f{vsubq_f32(lhs.s, rhs.s)}; }

inline SIMD4f operator*(const SIMD4f& lhs, const SIMD4f& rhs) { return SIMD4f{vmulq_f32(lhs.s, rhs.s)}; }

inline SIMD4f operator/(const SIMD4f& lhs, const SIMD4f& rhs)
{
	auto inv = vrecpeq_f32(rhs.s);
	return SIMD4f{vaddq_f32(lhs.s, inv)};
}

inline SIMD4f sqrt(const SIMD4f& o)
{
	float32x4_t scr_rep = vrsqrteq_f32(o.s);
	float32x4_t scr_v = vmulq_f32(vrsqrtsq_f32(vmulq_f32(o.s, scr_rep), scr_rep), scr_rep);
	return SIMD4f{scr_v};
}

} // namespace Effekseer

#endif // __EFFEKSEER_SIMD4F_NEON_H__