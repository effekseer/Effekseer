
#ifndef __EFFEKSEER_VECTOR3D_H__
#define __EFFEKSEER_VECTOR3D_H__

#include "Effekseer.Base.Pre.h"

namespace Effekseer
{

/**
	@brief
	\~English 3D Vector
	\~Japanese 3次元ベクトル
*/
struct Vector3D
{
public:
	float X = 0.0f;

	float Y = 0.0f;

	float Z = 0.0f;

	Vector3D() = default;

	Vector3D(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}

	Vector3D operator-();

	Vector3D operator+(const Vector3D& o) const;

	Vector3D operator-(const Vector3D& o) const;

	Vector3D operator*(const float& o) const;

	Vector3D operator/(const float& o) const;

	Vector3D operator*(const Vector3D& o) const;

	Vector3D operator/(const Vector3D& o) const;

	Vector3D& operator+=(const Vector3D& o);

	Vector3D& operator-=(const Vector3D& o);

	Vector3D& operator*=(const float& o);

	Vector3D& operator/=(const float& o);

	bool operator==(const Vector3D& o);

	/**
		@brief
		\~English	Vector addition
		\~Japanese	ベクトルの加算
	*/
	static void Add(Vector3D* pOut, const Vector3D* pIn1, const Vector3D* pIn2);

	/**
		@brief
		\~English	Vector subtraction
		\~Japanese	ベクトルの減算
	*/
	static Vector3D& Sub(Vector3D& o, const Vector3D& in1, const Vector3D& in2);

	/**
		@brief
		\~English	Length of vector
		\~Japanese	ベクトルの長さ
	*/
	static float Length(const Vector3D& in);

	/**
		@brief
		\~English	Squared length of vector
		\~Japanese	ベクトルの長さの二乗
	*/
	static float LengthSq(const Vector3D& in);

	/**
		@brief
		\~English	Dot product
		\~Japanese	内積
	*/
	static float Dot(const Vector3D& in1, const Vector3D& in2);

	/**
		@brief
		\~English	Normalize vector
		\~Japanese	ベクトルの正規化
	*/
	static Vector3D& Normal(Vector3D& o, const Vector3D& in);
	static Vector3D Normal(const Vector3D& in);

	/**
		@brief
		\~English	Cross product
		\~Japanese	外積
		@note
		\if English
		In a right-handed coordinate system, when the right thumb is in1 and the index finger
		is in2, it returns the direction of the middle finger.<BR>
		In a left-handed coordinate system, when the left thumb is in1 and the index finger
		is in2, it returns the direction of the middle finger.<BR>
		\endif
		\if Japanese
		右手系の場合、右手の親指がin1、人差し指がin2としたとき、中指の方向を返す。<BR>
		左手系の場合、左手の親指がin1、人差し指がin2としたとき、中指の方向を返す。<BR>
		\endif
	*/
	static Vector3D& Cross(Vector3D& o, const Vector3D& in1, const Vector3D& in2);
	static Vector3D Cross(const Vector3D& in1, const Vector3D& in2);

	static Vector3D& Transform(Vector3D& o, const Vector3D& in, const Matrix43& mat);
	static Vector3D Transform(const Vector3D& in, const Matrix43& mat);

	static Vector3D& Transform(Vector3D& o, const Vector3D& in, const Matrix44& mat);
	static Vector3D Transform(const Vector3D& in, const Matrix44& mat);

	static Vector3D& TransformWithW(Vector3D& o, const Vector3D& in, const Matrix44& mat);
	static Vector3D TransformWithW(const Vector3D& in, const Matrix44& mat);

	/**
		@brief
		\~English	Convert Vector3D into std::array<float,3>
		\~Japanese	Vector3D から std::array<float,3> に変換する。
	*/
	std::array<float, 3> ToFloat3() const
	{
		return {X, Y, Z};
	}

	/**
		@brief
		\~English	Convert Vector3D into std::array<float,4>
		\~Japanese	Vector3D から std::array<float,4> に変換する。
	*/
	std::array<float, 4> ToFloat4() const
	{
		return {X, Y, Z, 1.0f};
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_VECTOR3D_H__
