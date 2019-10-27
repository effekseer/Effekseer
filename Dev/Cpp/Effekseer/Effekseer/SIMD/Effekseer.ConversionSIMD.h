
#ifndef __EFFEKSEER_SIMD_CONVERSION_H__
#define __EFFEKSEER_SIMD_CONVERSION_H__

#include "../Effekseer.Matrix44.h"
#include "Effekseer.Mat44fBlock4.h"

namespace Effekseer
{

Mat44f ToSIMD(const Matrix44& o)
{
	Mat44f ret;

	ret.X.SetX(o.Values[0][0]);
	ret.X.SetY(o.Values[0][1]);
	ret.X.SetZ(o.Values[0][2]);
	ret.X.SetW(o.Values[0][3]);

	ret.Y.SetX(o.Values[1][0]);
	ret.Y.SetY(o.Values[1][1]);
	ret.Y.SetZ(o.Values[1][2]);
	ret.Y.SetW(o.Values[1][3]);

	ret.Z.SetX(o.Values[2][0]);
	ret.Z.SetY(o.Values[2][1]);
	ret.Z.SetZ(o.Values[2][2]);
	ret.Z.SetW(o.Values[2][3]);

	ret.W.SetX(o.Values[3][0]);
	ret.W.SetY(o.Values[3][1]);
	ret.W.SetZ(o.Values[3][2]);
	ret.W.SetW(o.Values[3][3]);

	return ret;
}

Matrix44 ToStruct(const Mat44f& o)
{
	Matrix44 ret;

	ret.Values[0][0] = o.X.GetX();
	ret.Values[0][1] = o.X.GetY();
	ret.Values[0][2] = o.X.GetZ();
	ret.Values[0][3] = o.X.GetW();

	ret.Values[1][0] = o.Y.GetX();
	ret.Values[1][1] = o.Y.GetY();
	ret.Values[1][2] = o.Y.GetZ();
	ret.Values[1][3] = o.Y.GetW();

	ret.Values[2][0] = o.Z.GetX();
	ret.Values[2][1] = o.Z.GetY();
	ret.Values[2][2] = o.Z.GetZ();
	ret.Values[2][3] = o.Z.GetW();

	ret.Values[3][0] = o.W.GetX();
	ret.Values[3][1] = o.W.GetY();
	ret.Values[3][2] = o.W.GetZ();
	ret.Values[3][3] = o.W.GetW();

	return ret;
}

} // namespace Effekseer

#endif
