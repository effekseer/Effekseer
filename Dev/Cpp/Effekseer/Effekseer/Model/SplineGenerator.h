
#ifndef __EFFEKSEER_SPLINE_GENERATOR_H__
#define __EFFEKSEER_SPLINE_GENERATOR_H__

#include "../SIMD/Effekseer.Vec3f.h"
#include "../Utils/Effekseer.CustomAllocator.h"
#include <cstdint>
#include <vector>

namespace Effekseer
{

/**
	@brief Spline generator
	@note
	Reference https://qiita.com/edo_m18/items/f2f0c6bf9032b0ec12d4
*/
class SplineGenerator
{
	CustomAlignedVector<Vec3f> a;
	CustomAlignedVector<Vec3f> b;
	CustomAlignedVector<Vec3f> c;
	CustomAlignedVector<Vec3f> d;
	CustomAlignedVector<Vec3f> w;
	CustomVector<bool> isSame;
	CustomVector<float> distances_;

public:
	void AddVertex(const Vec3f& v);

	void Calculate();

	void CalculateDistances();

	void Reset();

	Vec3f GetValue(float t) const;
};

} // namespace Effekseer

#endif