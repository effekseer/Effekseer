
#ifndef __EFFEKSEER_SIMD44F_NEON_H__
#define __EFFEKSEER_SIMD44F_NEON_H__

#include "Effekseer.SIMD4f_NEON.h"

namespace Effekseer
{

inline void transpose(
	SIMD4f& outx, SIMD4f& outy, SIMD4f& outz, SIMD4f& outw, const SIMD4f& inx, const SIMD4f& iny, const SIMD4f& inz, const SIMD4f& inw)
{
	auto tempx = inx;
	auto tempy = iny;
	auto tempz = inz;
	auto tempw = inw;

	outx.SetX(tempx.GetX());
	outx.SetY(tempy.GetX());
	outx.SetZ(tempz.GetX());
	outx.SetW(tempw.GetX());

	outy.SetX(tempx.GetY());
	outy.SetY(tempy.GetY());
	outy.SetZ(tempz.GetY());
	outy.SetW(tempw.GetY());

	outz.SetX(tempx.GetZ());
	outz.SetY(tempy.GetZ());
	outz.SetZ(tempz.GetZ());
	outz.SetW(tempw.GetZ());

	outw.SetX(tempx.GetW());
	outw.SetY(tempy.GetW());
	outw.SetZ(tempz.GetW());
	outw.SetW(tempw.GetW());
}

} // namespace Effekseer

#endif // __EFFEKSEER_SIMD4F_H__