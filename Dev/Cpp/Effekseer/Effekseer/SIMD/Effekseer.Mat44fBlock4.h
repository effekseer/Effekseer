
#ifndef __EFFEKSEER_MAT44FBLOCK4_H__
#define __EFFEKSEER_MAT44FBLOCK4_H__

#include "Effekseer.Mat44f.h"
#include "Effekseer.Vec3f.h"
#include "Effekseer.Vec3fBlock4.h"
#include "Effekseer.Vec4f.h"

namespace Effekseer
{

/**
	@brief	Operate Mat44 simultaneously
*/
struct Mat44fBlock4
{
	Vec4f Values[4][4];

	void Get(Mat44f& v1, Mat44f& v2, Mat44f& v3, Mat44f& v4)
	{
		SIMD4f x, y, z, w;
		transpose(x, y, z, w, Values[0][0].s, Values[0][1].s, Values[0][2].s, Values[0][3].s);
		v1.X.s = x;
		v2.X.s = y;
		v3.X.s = z;
		v4.X.s = w;

		transpose(x, y, z, w, Values[1][0].s, Values[1][1].s, Values[1][2].s, Values[1][3].s);
		v1.Y.s = x;
		v2.Y.s = y;
		v3.Y.s = z;
		v4.Y.s = w;

		transpose(x, y, z, w, Values[2][0].s, Values[2][1].s, Values[2][2].s, Values[2][3].s);
		v1.Z.s = x;
		v2.Z.s = y;
		v3.Z.s = z;
		v4.Z.s = w;

		transpose(x, y, z, w, Values[3][0].s, Values[3][1].s, Values[3][2].s, Values[3][3].s);
		v1.W.s = x;
		v2.W.s = y;
		v3.W.s = z;
		v4.W.s = w;
	}

	void Set(const Mat44f& v1, const Mat44f& v2, const Mat44f& v3, const Mat44f& v4)
	{
		SIMD4f x, y, z, w;
		transpose(x, y, z, w, v1.X.s, v2.X.s, v3.X.s, v4.X.s);
		Values[0][0].s = x;
		Values[0][1].s = y;
		Values[0][2].s = z;
		Values[0][3].s = w;

		transpose(x, y, z, w, v1.Y.s, v2.Y.s, v3.Y.s, v4.Y.s);
		Values[1][0].s = x;
		Values[1][1].s = y;
		Values[1][2].s = z;
		Values[1][3].s = w;

		transpose(x, y, z, w, v1.Z.s, v2.Z.s, v3.Z.s, v4.Z.s);
		Values[2][0].s = x;
		Values[2][1].s = y;
		Values[2][2].s = z;
		Values[2][3].s = w;

		transpose(x, y, z, w, v1.W.s, v2.W.s, v3.W.s, v4.W.s);
		Values[3][0].s = x;
		Values[3][1].s = y;
		Values[3][2].s = z;
		Values[3][3].s = w;
	}

	static Mat44fBlock4& Mul(Mat44fBlock4& o, const Mat44fBlock4& in1, const Mat44fBlock4& in2)
	{
		Mat44fBlock4 _in1 = in1;
		Mat44fBlock4 _in2 = in2;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Vec4f v;
				v.SetX(0.0f);
				v.SetY(0.0f);
				v.SetZ(0.0f);
				v.SetW(0.0f);

				for (int k = 0; k < 4; k++)
				{
					v += _in1.Values[i][k] * _in2.Values[k][j];
				}
				o.Values[i][j] = v;
			}
		}
		return o;
	}

	Vec3fBlock4& Transform(Vec3fBlock4& o, const Vec3fBlock4& in, const Mat44fBlock4& mat)
	{
		Vec4f values[3];

		for (int i = 0; i < 3; i++)
		{
			values[i].SetX(0.0f);
			values[i].SetY(0.0f);
			values[i].SetZ(0.0f);
			values[i].SetW(0.0f);

			values[i] += in.X * mat.Values[0][i];
			values[i] += in.Y * mat.Values[1][i];
			values[i] += in.Z * mat.Values[2][i];
			values[i] += mat.Values[3][i];
		}

		o.X = values[0];
		o.Y = values[1];
		o.Z = values[2];
		return o;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_Vec4fBlock4_H__