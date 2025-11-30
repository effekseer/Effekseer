
#ifndef __EFFEKSEER_MATRIX43_H__
#define __EFFEKSEER_MATRIX43_H__

#include "Effekseer.Base.Pre.h"

namespace Effekseer
{

struct Matrix44;

/**
	@brief
		\~English 4x3 Matrix
		\~Japanese 4x3 行列
	@note
	右手系(回転:反時計回り) <BR>
	V[x,y,z,1] * M <BR>
	[0,0][0,1][0,2] <BR>
	[1,0][1,1][1,2] <BR>
	[2,0][2,1][2,2] <BR>
	[3,0][3,1][3,2] <BR>
*/
struct Matrix43
{
public:
	/**
		@brief
		\~English	Matrix values
		\~Japanese	行列の値
	*/
	float Value[4][3];

	/**
		@brief
		\~English	convert into identity matrix
		\~Japanese	単位行列化を行う。
	*/
	void Indentity();

	/**
	@if English
		@brief	convert into scaling matrix
		@param	x	[in]	scaling factor in X axis
		@param	y	[in]	scaling factor in Y axis
		@param	z	[in]	scaling factor in Z axis
	@elseif Japanese
		@brief	拡大行列化を行う。
		@param	x	[in]	X方向拡大率
		@param	y	[in]	Y方向拡大率
		@param	z	[in]	Z方向拡大率
	@endif
	*/
	void Scaling(float x, float y, float z);

	/**
	@if English
		@brief	X axis rotation matrix (counter-clock-wise)
		@param	angle	[in]	X axis angle (radian)
	@elseif Japanese
		@brief	反時計周り方向のX軸回転行列化を行う。
		@param	angle	[in]	X軸の角度 (ラジアン)
	@endif
	*/
	void RotationX(float angle);

	/**
	@if English
		@brief	Y axis rotation matrix (counter-clock-wise)
		@param	angle	[in]	Y axis angle (radian)
	@elseif Japanese
		@brief	反時計周り方向のY軸回転行列化を行う。
		@param	angle	[in]	Y軸の角度 (ラジアン)
	@endif
	*/
	void RotationY(float angle);

	/**
	@if English
		@brief	Z axis rotation matrix (counter-clock-wise)
		@param	angle	[in]	Z axis angle (radian)
	@elseif Japanese
		@brief	反時計周り方向のZ軸回転行列化を行う。
		@param	angle	[in]	Z軸の角度 (ラジアン)
	@endif
	*/
	void RotationZ(float angle);

	/**
	@if English
		@brief	XYZ axis rotation matrix (counter-clock-wise)
		@param	rx	[in]	X axis angle (radian)
		@param	ry	[in]	Y axis angle (radian)
		@param	rz	[in]	Z axis angle (radian)
	@elseif Japanese
		@brief	反時計周り方向のXYZ軸回転行列化を行う。
		@param	rx	[in]	X軸の角度 (ラジアン)
		@param	ry	[in]	Y軸の角度 (ラジアン)
		@param	rz	[in]	Z軸の角度 (ラジアン)
	@endif
	*/
	void RotationXYZ(float rx, float ry, float rz);

	/**
	@if English
		@brief	ZXY axis rotation matrix (counter-clock-wise)
		@param	rz	[in]	Z axis angle (radian)
		@param	rx	[in]	X axis angle (radian)
		@param	ry	[in]	Y axis angle (radian)
	@elseif Japanese
		@brief	反時計周り方向のZXY軸回転行列化を行う。
		@param	rz	[in]	Z軸の角度 (ラジアン)
		@param	rx	[in]	X軸の角度 (ラジアン)
		@param	ry	[in]	Y軸の角度 (ラジアン)
	@endif
	*/
	void RotationZXY(float rz, float rx, float ry);

	/**
	@if English
		@brief	Rotation matrix around an arbitrary axis (counter-clock-wise)
		@param	axis	[in]	rotation axis
		@param	angle	[in]	angle (radian)
	@elseif Japanese
		@brief	任意軸に対する反時計周り方向回転行列化を行う。
		@param	axis	[in]	回転軸
		@param	angle	[in]	角度 (ラジアン)
	@endif
	*/
	void RotationAxis(const Vector3D& axis, float angle);

	/**
	@if English
		@brief	Rotation matrix around an arbitrary axis (counter-clock-wise)
		@param	axis	[in]	rotation axis
		@param	s		[in]	sine value
		@param	c		[in]	cosine value
	@elseif Japanese
		@brief	任意軸に対する反時計周り方向回転行列化を行う。
		@param	axis	[in]	回転軸
		@param	s		[in]	サイン値
		@param	c		[in]	コサイン値
	@endif
	*/
	void RotationAxis(const Vector3D& axis, float s, float c);

	/**
	@if English
		@brief	Translation matrix
		@param	x	[in]	translation in X axis
		@param	y	[in]	translation in Y axis
		@param	z	[in]	translation in Z axis
	@elseif Japanese
		@brief	移動行列化を行う。
		@param	x	[in]	X方向移動
		@param	y	[in]	Y方向移動
		@param	z	[in]	Z方向移動
	@endif
	*/
	void Translation(float x, float y, float z);

	/**
	@if English
		@brief	Decompose matrix into scaling vector, rotation matrix and translation vector.
		@param	s	[out]	scaling vector
		@param	r	[out]	rotation matrix
		@param	t	[out]	position vector
	@elseif Japanese
		@brief	行列を、拡大、回転、移動の行列とベクトルに分解する。
		@param	s	[out]	拡大ベクトル
		@param	r	[out]	回転行列
		@param	t	[out]	位置ベクトル
	@endif
	*/
	void GetSRT(Vector3D& s, Matrix43& r, Vector3D& t) const;

	/**
	@if English
		@brief	Get scaling vector from matrix.
		@param	s	[out]	scaling vector
	@elseif Japanese
		@brief	行列から拡大ベクトルを取得する。
		@param	s	[out]	拡大ベクトル
	@endif
	*/
	void GetScale(Vector3D& s) const;

	/**
	@if English
		@brief	Get rotation matrix from matrix.
		@param	s	[out]	rotation matrix
	@elseif Japanese
		@brief	行列から回転行列を取得する。
		@param	s	[out]	回転行列
	@endif
	*/
	void GetRotation(Matrix43& r) const;

	/**
	@if English
		@brief	Get translation vector from matrix.
		@param	t	[out]	translation vector
	@elseif Japanese
		@brief	行列から移動ベクトルを取得する。
		@param	t	[out]	移動ベクトル
	@endif
	*/
	void GetTranslation(Vector3D& t) const;

	/**
	@if English
		@brief	Set scaling vector, rotation matrix and translation vector into matrix.
		@param	s	[in]	scaling vector
		@param	r	[in]	rotation matrix
		@param	t	[in]	position vector
	@elseif Japanese
		@brief	行列の拡大、回転、移動を設定する。
		@param	s	[in]	拡大ベクトル
		@param	r	[in]	回転行列
		@param	t	[in]	位置ベクトル
	@endif
	*/
	void SetSRT(const Vector3D& s, const Matrix43& r, const Vector3D& t);

	/**
		@brief
		\~English	convert into matrix44
		\~Japanese	Matrix44 に変換する
	*/
	void ToMatrix44(Matrix44& dst);

	/**
		@brief
		\~English	check whether all values are not valid number (not nan, not inf)
		\~Japanese	全ての値が有効な数値かどうかを確認する。(nan でも inf でもない)
	*/
	bool IsValid() const;

	/**
	@if English
		@brief	Matrix multiplication
		@param	out	[out]	result
		@param	in1	[in]	left side of multiplication
		@param	in2	[in]	right side of multiplication
	@elseif Japanese
		@brief	行列同士の乗算を行う。
		@param	out	[out]	結果
		@param	in1	[in]	乗算の左側
		@param	in2	[in]	乗算の右側
	@endif
	*/
	static void Multiple(Matrix43& out, const Matrix43& in1, const Matrix43& in2);
};

} // namespace Effekseer

#endif // __EFFEKSEER_MATRIX43_H__
