#include <cassert>
#include "SIMD/Float4.h"
#include "SIMD/Int4.h"
#include "SIMD/Bridge.h"
#include "SIMD/Vec2f.h"
#include "SIMD/Vec3f.h"
#include "SIMD/Vec4f.h"
#include "SIMD/Mat43f.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Utils.h"
#include "Effekseer.Vector2D.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"

#define ASSERT(e) assert(e)
//#define ASSERT(e) if (!(e)) printf("assert failed.\n");

using namespace Effekseer;
using namespace Effekseer::SIMD;

void test_Float4()
{
	{
		float a[4] = {1, 2, 3, 4};
		Float4 ret = Float4::Load4(a);
		ASSERT(ret == Float4(1, 2, 3, 4));
	}
	{
		Float4 a(1, 2, 3, 4);
		float ret[4];
		Float4::Store4(ret, a);
		ASSERT(ret[0] == 1.0f && ret[1] == 2.0f && ret[2] == 3.0f && ret[3] == 4.0f);
	}
	{
		Float4 a;
		a.SetX(5);
		a.SetY(6);
		a.SetZ(7);
		a.SetW(8);
		ASSERT(a == Float4(5, 6, 7, 8));
	}
	{
		Float4 a(3, 4, 5, 6);
		float x = a.GetX();
		float y = a.GetY();
		float z = a.GetZ();
		float w = a.GetW();
		ASSERT(x == 3.0f && y == 4.0f && z == 5.0f && w == 6.0f);
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Float4 ret = a + b;
		ASSERT(ret == Float4(6, 8, 10, 12));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Float4 ret = a - b;
		ASSERT(ret == Float4(-4, -4, -4, -4));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Float4 ret = a * b;
		ASSERT(ret == Float4(5, 12, 21, 32));
	}

	{
		Float4 a(1, 2, 3, 4);
		Float4 ret = a * 5;
		ASSERT(ret == Float4(5, 10, 15, 20));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		Float4 ret = Float4::MulAdd(a, b, c);
		ASSERT(ret == Float4(46, 62, 80, 100));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		Float4 ret = Float4::MulSub(a, b, c);
		ASSERT(ret == Float4(-44, -58, -74, -92));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Float4::MulLane<0>(a, b) == Float4(5, 10, 15, 20));
		ASSERT(Float4::MulLane<1>(a, b) == Float4(6, 12, 18, 24));
		ASSERT(Float4::MulLane<2>(a, b) == Float4(7, 14, 21, 28));
		ASSERT(Float4::MulLane<3>(a, b) == Float4(8, 16, 24, 32));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Float4::MulAddLane<0>(Float4(3), a, b) == Float4( 8, 13, 18, 23));
		ASSERT(Float4::MulAddLane<1>(Float4(3), a, b) == Float4( 9, 15, 21, 27));
		ASSERT(Float4::MulAddLane<2>(Float4(3), a, b) == Float4(10, 17, 24, 31));
		ASSERT(Float4::MulAddLane<3>(Float4(3), a, b) == Float4(11, 19, 27, 35));
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Float4::MulSubLane<0>(Float4(30), a, b) == Float4(25, 20, 15, 10));
		ASSERT(Float4::MulSubLane<1>(Float4(30), a, b) == Float4(24, 18, 12,  6));
		ASSERT(Float4::MulSubLane<2>(Float4(30), a, b) == Float4(23, 16,  9,  2));
		ASSERT(Float4::MulSubLane<3>(Float4(30), a, b) == Float4(22, 14,  6, -2));
	}

	{
		Float4 a(1, -2, 3, -4);
		Float4 ret = Float4::Abs(a);
		ASSERT(ret == Float4(1, 2, 3, 4));
	}

	{
		Float4 a(1, 5, 3, 7), b(4, 8, 2, 6);
		Float4 ret = Float4::Min(a, b);
		ASSERT(ret == Float4(1, 5, 2, 6));
	}

	{
		Float4 a(1, 5, 3, 7), b(4, 8, 2, 6);
		Float4 ret = Float4::Max(a, b);
		ASSERT(ret == Float4(4, 8, 3, 7));
	}

	{
		Float4 a(1, 2, 3, 4);
		Float4 ret = Float4::Sqrt(a);
		Float4 testret = Float4(1.0f, 1.4142135623730950488016887242097f, 1.7320508075688772935274463415059f, 2.0f);
		ASSERT(Float4::MoveMask(Float4::NearEqual(ret, testret)) == 0xf);
	}

	{
		Float4 a(1, 2, 3, 4);
		Float4 ret = Float4::Rsqrt(a);
		Float4 testret = Float4(1.0f, 0.70710678118654752440084436210485f, 0.57735026918962576450914878050195f, 0.5f);
		ASSERT(Float4::MoveMask(Float4::NearEqual(ret, testret, 1e-3f)) == 0xf);
	}

	{
		Float4 a(1, 2, 3, 4), b(5, 4, 3, 2);
		ASSERT(Float4::MoveMask(Float4::Equal(a, b)) == 0x4);
		ASSERT(Float4::MoveMask(Float4::NotEqual(a, b)) == 0xb);
		ASSERT(Float4::MoveMask(Float4::LessThan(a, b)) == 0x3);
		ASSERT(Float4::MoveMask(Float4::LessEqual(a, b)) == 0x7);
		ASSERT(Float4::MoveMask(Float4::GreaterThan(a, b)) == 0x8);
		ASSERT(Float4::MoveMask(Float4::GreaterEqual(a, b)) == 0xc);
	}

	{
		Float4 a(1, 2, 3, 4);
		ASSERT(a == a.Convert4i().Convert4f());
	}
	
	{
		Float4 a(1, 2, 3, 4);
		ASSERT(a.Convert4i() == Int4(1, 2, 3, 4));
	}
	
	{
		Float4 a(1, 2, 3, 4);
		ASSERT(a.Dup<0>() == Float4(1, 1, 1, 1));
		ASSERT(a.Dup<1>() == Float4(2, 2, 2, 2));
		ASSERT(a.Dup<2>() == Float4(3, 3, 3, 3));
		ASSERT(a.Dup<3>() == Float4(4, 4, 4, 4));
	}

	{
		Float4 a(1, 2, 3, 4), b(4, 3, 2, 1);
		Float4 mask = Float4::LessThan(a, b);
		Float4 ret = Float4::Select(mask, a, b);
		ASSERT(ret == Float4(1, 2, 2, 1));
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

void test_Int4()
{
	{
		int32_t a[4] = {1, 2, 3, 4};
		Int4 ret = Int4::Load4(a);
		ASSERT(ret == Int4(1, 2, 3, 4));
	}
	{
		Int4 a(1, 2, 3, 4);
		int32_t ret[4];
		Int4::Store4(ret, a);
		ASSERT(ret[0] == 1.0f && ret[1] == 2.0f && ret[2] == 3.0f && ret[3] == 4.0f);
	}
	{
		Int4 a;
		a.SetX(5);
		a.SetY(6);
		a.SetZ(7);
		a.SetW(8);
		ASSERT(a == Int4(5, 6, 7, 8));
	}
	{
		Int4 a(3, 4, 5, 6);
		int32_t x = a.GetX();
		int32_t y = a.GetY();
		int32_t z = a.GetZ();
		int32_t w = a.GetW();
		ASSERT(x == 3.0f && y == 4.0f && z == 5.0f && w == 6.0f);
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Int4 ret = a + b;
		ASSERT(ret == Int4(6, 8, 10, 12));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Int4 ret = a - b;
		ASSERT(ret == Int4(-4, -4, -4, -4));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		Int4 ret = a * b;
		ASSERT(ret == Int4(5, 12, 21, 32));
	}

	{
		Int4 a(1, 2, 3, 4);
		Int4 ret = a * 5;
		ASSERT(ret == Int4(5, 10, 15, 20));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		Int4 ret = Int4::MulAdd(a, b, c);
		ASSERT(ret == Int4(46, 62, 80, 100));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8), c(9, 10, 11, 12);
		Int4 ret = Int4::MulSub(a, b, c);
		ASSERT(ret == Int4(-44, -58, -74, -92));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Int4::MulLane<0>(a, b) == Int4(5, 10, 15, 20));
		ASSERT(Int4::MulLane<1>(a, b) == Int4(6, 12, 18, 24));
		ASSERT(Int4::MulLane<2>(a, b) == Int4(7, 14, 21, 28));
		ASSERT(Int4::MulLane<3>(a, b) == Int4(8, 16, 24, 32));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Int4::MulAddLane<0>(Int4(3), a, b) == Int4( 8, 13, 18, 23));
		ASSERT(Int4::MulAddLane<1>(Int4(3), a, b) == Int4( 9, 15, 21, 27));
		ASSERT(Int4::MulAddLane<2>(Int4(3), a, b) == Int4(10, 17, 24, 31));
		ASSERT(Int4::MulAddLane<3>(Int4(3), a, b) == Int4(11, 19, 27, 35));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 6, 7, 8);
		ASSERT(Int4::MulSubLane<0>(Int4(30), a, b) == Int4(25, 20, 15, 10));
		ASSERT(Int4::MulSubLane<1>(Int4(30), a, b) == Int4(24, 18, 12,  6));
		ASSERT(Int4::MulSubLane<2>(Int4(30), a, b) == Int4(23, 16,  9,  2));
		ASSERT(Int4::MulSubLane<3>(Int4(30), a, b) == Int4(22, 14,  6, -2));
	}

	{
		Int4 a(1, -2, 3, -4);
		Int4 ret = Int4::Abs(a);
		ASSERT(ret == Int4(1, 2, 3, 4));
	}

	{
		Int4 a(1, 5, 3, 7), b(4, 8, 2, 6);
		Int4 ret = Int4::Min(a, b);
		ASSERT(ret == Int4(1, 5, 2, 6));
	}

	{
		Int4 a(1, 5, 3, 7), b(4, 8, 2, 6);
		Int4 ret = Int4::Max(a, b);
		ASSERT(ret == Int4(4, 8, 3, 7));
	}

	{
		Int4 a(1, 2, 3, 4);
		Int4 ret = Int4::ShiftL<1>(a);
		ASSERT(ret == Int4(2, 4, 6, 8));
	}

	{
		Int4 a(1, 2, 3, 4);
		Int4 ret = Int4::ShiftR<1>(a);
		ASSERT(ret == Int4(0, 1, 1, 2));
	}

	{
		Int4 a(-4, -3, 2, 1);
		Int4 ret = Int4::ShiftRA<1>(a);
		ASSERT(ret == Int4(-2, -2, 1, 0));
	}

	{
		Int4 a(1, 2, 3, 4), b(5, 4, 3, 2);
		ASSERT(Int4::MoveMask(Int4::Equal(a, b)) == 0x4);
		ASSERT(Int4::MoveMask(Int4::NotEqual(a, b)) == 0xb);
		ASSERT(Int4::MoveMask(Int4::LessThan(a, b)) == 0x3);
		ASSERT(Int4::MoveMask(Int4::LessEqual(a, b)) == 0x7);
		ASSERT(Int4::MoveMask(Int4::GreaterThan(a, b)) == 0x8);
		ASSERT(Int4::MoveMask(Int4::GreaterEqual(a, b)) == 0xc);
	}

	{
		Int4 a(1, 2, 3, 4);
		ASSERT(a == a.Convert4f().Convert4i());
	}
	
	{
		Int4 a(1, 2, 3, 4);
		ASSERT(a.Convert4f() == Float4(1.0f, 2.0f, 3.0f, 4.0f));
	}
}

int main(int argc, char *argv[])
{
	test_Float4();
	test_Vec2f();
	test_Vec3f();
	test_Vec4f();
	test_Mat43f();
	test_Mat44f();
	test_Int4();

	return 0;
}
