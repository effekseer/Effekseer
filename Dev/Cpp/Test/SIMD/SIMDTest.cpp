#include <cassert>
#include "SIMD/Effekseer.SIMD4f.h"
#include "SIMD/Effekseer.SIMD4i.h"
#include "SIMD/Effekseer.Vec2f.h"
#include "SIMD/Effekseer.Vec3f.h"
#include "SIMD/Effekseer.Vec4f.h"
#include "SIMD/Effekseer.Mat43f.h"
#include "SIMD/Effekseer.Mat44f.h"
#include "SIMD/Effekseer.SIMDUtils.h"
#include "Effekseer.Vector2D.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"

#define ASSERT(e) assert(e)
//#define ASSERT(e) if (!(e)) printf("assert failed.\n");

using namespace Effekseer;

void test_SIMD4f()
{
	{
		float a[4] = {1, 2, 3, 4};
		SIMD4f ret = SIMD4f::Load4(a);
		ASSERT(ret == SIMD4f(1, 2, 3, 4));
	}
	{
		SIMD4f a(1, 2, 3, 4);
		float ret[4];
		SIMD4f::Store4(ret, a);
		ASSERT(ret[0] == 1.0f && ret[1] == 2.0f && ret[2] == 3.0f && ret[3] == 4.0f);
	}
	{
		SIMD4f a;
		a.SetX(5);
		a.SetY(6);
		a.SetZ(7);
		a.SetW(8);
		ASSERT(a == SIMD4f(5, 6, 7, 8));
	}
	{
		SIMD4f a(3, 4, 5, 6);
		float x = a.GetX();
		float y = a.GetY();
		float z = a.GetZ();
		float w = a.GetW();
		ASSERT(x == 3.0f && y == 4.0f && z == 5.0f && w == 6.0f);
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4f ret = a + b;
		ASSERT(ret == SIMD4f(6, 8, 10, 12));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4f ret = a - b;
		ASSERT(ret == SIMD4f(-4, -4, -4, -4));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4f ret = a * b;
		ASSERT(ret == SIMD4f(5, 12, 21, 32));
	}

	{
		SIMD4f a(1, 2, 3, 4);
		SIMD4f ret = a * 5;
		ASSERT(ret == SIMD4f(5, 10, 15, 20));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		SIMD4f ret = SIMD4f::MulAdd(a, b, c);
		ASSERT(ret == SIMD4f(46, 62, 80, 100));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		SIMD4f ret = SIMD4f::MulSub(a, b, c);
		ASSERT(ret == SIMD4f(-44, -58, -74, -92));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4f::MulLane<0>(a, b) == SIMD4f(5, 10, 15, 20));
		ASSERT(SIMD4f::MulLane<1>(a, b) == SIMD4f(6, 12, 18, 24));
		ASSERT(SIMD4f::MulLane<2>(a, b) == SIMD4f(7, 14, 21, 28));
		ASSERT(SIMD4f::MulLane<3>(a, b) == SIMD4f(8, 16, 24, 32));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4f::MulAddLane<0>(SIMD4f(3), a, b) == SIMD4f( 8, 13, 18, 23));
		ASSERT(SIMD4f::MulAddLane<1>(SIMD4f(3), a, b) == SIMD4f( 9, 15, 21, 27));
		ASSERT(SIMD4f::MulAddLane<2>(SIMD4f(3), a, b) == SIMD4f(10, 17, 24, 31));
		ASSERT(SIMD4f::MulAddLane<3>(SIMD4f(3), a, b) == SIMD4f(11, 19, 27, 35));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4f::MulSubLane<0>(SIMD4f(30), a, b) == SIMD4f(25, 20, 15, 10));
		ASSERT(SIMD4f::MulSubLane<1>(SIMD4f(30), a, b) == SIMD4f(24, 18, 12,  6));
		ASSERT(SIMD4f::MulSubLane<2>(SIMD4f(30), a, b) == SIMD4f(23, 16,  9,  2));
		ASSERT(SIMD4f::MulSubLane<3>(SIMD4f(30), a, b) == SIMD4f(22, 14,  6, -2));
	}

	{
		SIMD4f a(1, -2, 3, -4);
		SIMD4f ret = SIMD4f::Abs(a);
		ASSERT(ret == SIMD4f(1, 2, 3, 4));
	}

	{
		SIMD4f a(1, 5, 3, 7), b(4, 8, 2, 6);
		SIMD4f ret = SIMD4f::Min(a, b);
		ASSERT(ret == SIMD4f(1, 5, 2, 6));
	}

	{
		SIMD4f a(1, 5, 3, 7), b(4, 8, 2, 6);
		SIMD4f ret = SIMD4f::Max(a, b);
		ASSERT(ret == SIMD4f(4, 8, 3, 7));
	}

	{
		SIMD4f a(1, 2, 3, 4);
		SIMD4f ret = SIMD4f::Sqrt(a);
		SIMD4f testret = SIMD4f(1.0f, 1.4142135623730950488016887242097f, 1.7320508075688772935274463415059f, 2.0f);
		ASSERT(SIMD4f::MoveMask(SIMD4f::NearEqual(ret, testret)) == 0xf);
	}

	{
		SIMD4f a(1, 2, 3, 4);
		SIMD4f ret = SIMD4f::Rsqrt(a);
		SIMD4f testret = SIMD4f(1.0f, 0.70710678118654752440084436210485f, 0.57735026918962576450914878050195f, 0.5f);
		ASSERT(SIMD4f::MoveMask(SIMD4f::NearEqual(ret, testret, 1e-3f)) == 0xf);
	}
	
	{
		SIMD4f a(1, 2, 3, 4);
		ASSERT(a == a.Convert4i().Convert4f());
	}
	
	{
		SIMD4f a(1, 2, 3, 4);
		ASSERT(a.Convert4i() == SIMD4i(1, 2, 3, 4));
	}
	
	{
		SIMD4f a(1, 2, 3, 4);
		ASSERT(a.Dup<0>() == SIMD4f(1, 1, 1, 1));
		ASSERT(a.Dup<1>() == SIMD4f(2, 2, 2, 2));
		ASSERT(a.Dup<2>() == SIMD4f(3, 3, 3, 3));
		ASSERT(a.Dup<3>() == SIMD4f(4, 4, 4, 4));
	}

	{
		SIMD4f a(1, 2, 3, 4), b(4, 3, 2, 1);
		SIMD4f mask = SIMD4f::LessThan(a, b);
		SIMD4f ret = SIMD4f::Select(mask, a, b);
		ASSERT(ret == SIMD4f(1, 2, 2, 1));
	}
}

void test_Vec2f()
{
	{
		Vec2f a(2, 3), b(4, 5);
		Vec2f c = a + b;
		ASSERT(c == Vec2f(6, 8));
	}

	{
		Vec2f a(2, 3), b(4, 5);
		Vec2f c = a - b;
		ASSERT(c == Vec2f(-2, -2));
	}
}

void test_Vec3f()
{
	{
		Vec3f a(2, 3, 4), b(4, 5, 6);
		Vec3f c = a + b;
		ASSERT(c == Vec3f(6, 8, 10));
	}

	{
		Vec3f a(2, 3, 4), b(4, 5, 6);
		Vec3f c = a - b;
		ASSERT(c == Vec3f(-2, -2, -2));
	}

	{
		Vec3f a(2, 3, 4);
		Vec3f c = a * 5.0f;
		ASSERT(c == Vec3f(10, 15, 20));
	}

	{
		Vec3f a(2, 3, 4);
		Vec3f c = a / 2.0f;
		ASSERT(c == Vec3f(1.0f, 1.5f, 2.0f));
	}

	{
		Vec3f a(2, 3, 4), b(4, 5, 6);
		float c = Vec3f::Dot(a, b);
		ASSERT(c == 47);
	}

	{
		Vec3f a(2, 3, 4), b(4, 5, 6);
		Vec3f c = Vec3f::Cross(a, b);
		ASSERT(c == Vec3f(-2, 4, -2));
	}
}

void test_Vec4f()
{
}

void test_Mat43f()
{
	{
		Mat43f a(
			1,  2,  3,
			4,  5,  6,
			7,  8,  9,
			10, 11, 12);
		Mat43f b(
			12, 11, 10,
			9,  8,  7,
			6,  5,  4,
			3,  2,  1);
		Mat43f ret = a * b;
		ASSERT(ret == Mat43f(
			48,  42,  36,
			129, 114,  99, 
			210, 186, 162, 
			294, 260, 226)
		);
	}

	{
		Vec3f a(1, 2, 3);
		Mat43f b(
			1,  2,  3,
			4,  5,  6,
			7,  8,  9,
			10, 11, 12);
		Vec3f ret = Vec3f::Transform(a, b);
		ASSERT(ret == Vec3f(40, 47, 54));
	}

	{
		Mat43f a = Mat43f::Scaling(5.0f, 6.0f, 7.0f);
		Matrix43 b; b.Scaling(5.0f, 6.0f, 7.0f);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::RotationX(12);
		Matrix43 b; b.RotationX(12);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::RotationY(12);
		Matrix43 b; b.RotationY(12);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::RotationZ(12);
		Matrix43 b; b.RotationZ(12);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::RotationXYZ(12, 34, 56);
		Matrix43 b; b.RotationXYZ(12, 34, 56);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::RotationAxis(Vec3f(1, 2, 3), 56);
		Matrix43 b; b.RotationAxis(Vector3D(1, 2, 3), 56);
		ASSERT(a == Mat43f(b));
	}
	{
		Mat43f a = Mat43f::SRT(Vec3f(1, 2, 3), Mat43f::RotationXYZ(12, 34, 56), Vec3f(3.0f, 2.0f, 0.5f));
		Matrix43 b; b.RotationXYZ(12, 34, 56);
		Matrix43 c; c.SetSRT(Vector3D(1, 2, 3), b, Vector3D(3.0f, 2.0f, 0.5f));
		ASSERT(a == Mat43f(c));

		Vec3f s1, t1;
		Mat43f r1;
		a.GetSRT(s1, r1, t1);

		Vector3D s2, t2;
		Matrix43 r2;
		c.GetSRT(s2, r2, t2);

		ASSERT(Vec3f::Equal(s1, Vec3f(s2), 0.001f));
		ASSERT(Mat43f::Equal(r1, Mat43f(r2), 0.001f));
		ASSERT(Vec3f::Equal(t1, Vec3f(t2), 0.001f));
	}
}

void test_Mat44f()
{
	{
		Mat44f a(
			1,  2,  3,  4,
			5,  6,  7,  8,
			9, 10, 11, 12,
			13, 14, 15, 16);
		Mat44f b(
			16, 15, 14, 13,
			12, 11, 10,  9,
			8,  7,  6,  5,
			4,  3,  2,  1);
		Mat44f ret = a * b;
		ASSERT(ret == Mat44f(
			80,  70,  60,  50, 
			240, 214, 188, 162, 
			400, 358, 316, 274, 
			560, 502, 444, 386)
		);
	}

	{
		Vec3f a(1, 2, 3);
		Mat44f b(
			1,  2,  3,  4,
			5,  6,  7,  8,
			9, 10, 11, 12,
			13, 14, 15, 16);
		Vec3f ret = Vec3f::Transform(a, b);
		ASSERT(ret == Vec3f(51, 58, 65));
	}

	{
		Mat44f a(
			1,  2,  3,  4,
			5,  6,  7,  8,
			9, 10, 11, 12,
			13, 14, 15, 16);
		Mat44f b = a.Transpose();
		ASSERT(b == Mat44f(
			1, 5,  9, 13,
			2, 6, 10, 14,
			3, 7, 11, 15,
			4, 8, 12, 16));
	}
}

void test_SIMD4i()
{
	{
		int32_t a[4] = {1, 2, 3, 4};
		SIMD4i ret = SIMD4i::Load4(a);
		ASSERT(ret == SIMD4i(1, 2, 3, 4));
	}
	{
		SIMD4i a(1, 2, 3, 4);
		int32_t ret[4];
		SIMD4i::Store4(ret, a);
		ASSERT(ret[0] == 1.0f && ret[1] == 2.0f && ret[2] == 3.0f && ret[3] == 4.0f);
	}
	{
		SIMD4i a;
		a.SetX(5);
		a.SetY(6);
		a.SetZ(7);
		a.SetW(8);
		ASSERT(a == SIMD4i(5, 6, 7, 8));
	}
	{
		SIMD4i a(3, 4, 5, 6);
		int32_t x = a.GetX();
		int32_t y = a.GetY();
		int32_t z = a.GetZ();
		int32_t w = a.GetW();
		ASSERT(x == 3.0f && y == 4.0f && z == 5.0f && w == 6.0f);
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4i ret = a + b;
		ASSERT(ret == SIMD4i(6, 8, 10, 12));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4i ret = a - b;
		ASSERT(ret == SIMD4i(-4, -4, -4, -4));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		SIMD4i ret = a * b;
		ASSERT(ret == SIMD4i(5, 12, 21, 32));
	}

	{
		SIMD4i a(1, 2, 3, 4);
		SIMD4i ret = a * 5;
		ASSERT(ret == SIMD4i(5, 10, 15, 20));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		SIMD4i ret = SIMD4i::MulAdd(a, b, c);
		ASSERT(ret == SIMD4i(46, 62, 80, 100));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		SIMD4i ret = SIMD4i::MulSub(a, b, c);
		ASSERT(ret == SIMD4i(-44, -58, -74, -92));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4i::MulLane<0>(a, b) == SIMD4i(5, 10, 15, 20));
		ASSERT(SIMD4i::MulLane<1>(a, b) == SIMD4i(6, 12, 18, 24));
		ASSERT(SIMD4i::MulLane<2>(a, b) == SIMD4i(7, 14, 21, 28));
		ASSERT(SIMD4i::MulLane<3>(a, b) == SIMD4i(8, 16, 24, 32));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4i::MulAddLane<0>(SIMD4i(3), a, b) == SIMD4i( 8, 13, 18, 23));
		ASSERT(SIMD4i::MulAddLane<1>(SIMD4i(3), a, b) == SIMD4i( 9, 15, 21, 27));
		ASSERT(SIMD4i::MulAddLane<2>(SIMD4i(3), a, b) == SIMD4i(10, 17, 24, 31));
		ASSERT(SIMD4i::MulAddLane<3>(SIMD4i(3), a, b) == SIMD4i(11, 19, 27, 35));
	}

	{
		SIMD4i a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(SIMD4i::MulSubLane<0>(SIMD4i(30), a, b) == SIMD4i(25, 20, 15, 10));
		ASSERT(SIMD4i::MulSubLane<1>(SIMD4i(30), a, b) == SIMD4i(24, 18, 12,  6));
		ASSERT(SIMD4i::MulSubLane<2>(SIMD4i(30), a, b) == SIMD4i(23, 16,  9,  2));
		ASSERT(SIMD4i::MulSubLane<3>(SIMD4i(30), a, b) == SIMD4i(22, 14,  6, -2));
	}

	{
		SIMD4i a(1, -2, 3, -4);
		SIMD4i ret = SIMD4i::Abs(a);
		ASSERT(ret == SIMD4i(1, 2, 3, 4));
	}

	{
		SIMD4i a(1, 5, 3, 7), b(4, 8, 2, 6);
		SIMD4i ret = SIMD4i::Min(a, b);
		ASSERT(ret == SIMD4i(1, 5, 2, 6));
	}

	{
		SIMD4i a(1, 5, 3, 7), b(4, 8, 2, 6);
		SIMD4i ret = SIMD4i::Max(a, b);
		ASSERT(ret == SIMD4i(4, 8, 3, 7));
	}

	{
		SIMD4i a(1, 2, 3, 4);
		SIMD4i ret = SIMD4i::ShiftL<1>(a);
		ASSERT(ret == SIMD4i(2, 4, 6, 8));
	}

	{
		SIMD4i a(1, 2, 3, 4);
		SIMD4i ret = SIMD4i::ShiftR<1>(a);
		ASSERT(ret == SIMD4i(0, 1, 1, 2));
	}

	{
		SIMD4i a(-4, -3, 2, 1);
		SIMD4i ret = SIMD4i::ShiftRA<1>(a);
		ASSERT(ret == SIMD4i(-2, -2, 1, 0));
	}

	{
		SIMD4i a(1, 2, 3, 4);
		ASSERT(a == a.Convert4f().Convert4i());
	}
	
	{
		SIMD4i a(1, 2, 3, 4);
		ASSERT(a.Convert4f() == SIMD4f(1.0f, 2.0f, 3.0f, 4.0f));
	}
}

int main(int argc, char *argv[])
{
	test_SIMD4f();
	test_Vec2f();
	test_Vec3f();
	test_Vec4f();
	test_Mat43f();
	test_Mat44f();
	test_SIMD4i();

	return 0;
}
