
#ifndef __EFFEKSEER_MAT44F_H__
#define __EFFEKSEER_MAT44F_H__

#include "Effekseer.Vec4f.h"
#include "Effekseer.SIMD44f.h"

namespace Effekseer
{

struct Mat44f
{
	Vec4f X;
	Vec4f Y;
	Vec4f Z;
	Vec4f W;

	void Transpose() { transpose(X.s, Y.s, Z.s, W.s, X.s, Y.s, Z.s, W.s); }

	Mat44f GetTransposed()
	{
		auto self = *this;
		self.Transpose();
		return self;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_VEC4F_H__