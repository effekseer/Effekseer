
#ifndef __EFFEKSEER_MATRIX44_H__
#define __EFFEKSEER_MATRIX44_H__

#include "Effekseer.Base.Pre.h"

namespace Effekseer
{

/**
	@brief
		\~English 4x4 Matrix
		\~Japanese 4x4 行列
	@note
	V[x,y,z,1] * M <BR>
	[0,0][0,1][0,2][0,3] <BR>
	[1,0][1,1][1,2][1,3] <BR>
	[2,0][2,1][2,2][2,3] <BR>
	[3,0][3,1][3,2][3,3] <BR>
*/

struct Matrix44
{
	float Values[4][4];

	Matrix44();

	/**
		@brief
		\~English	Identity matrix
		\~Japanese	単位行列化
	*/
	Matrix44& Indentity();

	/**
		@brief
		\~English	Transpose matrix
		\~Japanese	転置行列化
	*/
	Matrix44& Transpose();

	/**
		@brief
		\~English	Camera matrix (right-handed)
		\~Japanese	カメラ行列化(右手系)
	*/
	Matrix44& LookAtRH(const Vector3D& eye, const Vector3D& at, const Vector3D& up);

	/**
		@brief
		\~English	Camera matrix (left-handed)
		\~Japanese	カメラ行列化(左手系)
	*/
	Matrix44& LookAtLH(const Vector3D& eye, const Vector3D& at, const Vector3D& up);

	/**
		@brief
		\~English	Perspective projection matrix (right-handed)
		\~Japanese	射影行列化(右手系)
	*/
	Matrix44& PerspectiveFovRH(float ovY, float aspect, float zn, float zf);

	/**
		@brief
		\~English	Projection matrix for OpenGL (right-handed)
		\~Japanese	OpenGL用射影行列化(右手系)
	*/
	Matrix44& PerspectiveFovRH_OpenGL(float ovY, float aspect, float zn, float zf);

	/**
		@brief
		\~English	Perspective projection matrix (left-handed)
		\~Japanese	射影行列化(左手系)
	*/
	Matrix44& PerspectiveFovLH(float ovY, float aspect, float zn, float zf);

	/**
		@brief
		\~English	Projection matrix for OpenGL (left-handed)
		\~Japanese	OpenGL用射影行列化(左手系)
	*/
	Matrix44& PerspectiveFovLH_OpenGL(float ovY, float aspect, float zn, float zf);

	/**
		@brief
		\~English	Orthographic projection matrix (right-handed)
		\~Japanese	正射影行列化(右手系)
	*/
	Matrix44& OrthographicRH(float width, float height, float zn, float zf);

	/**
		@brief
		\~English	Orthographic projection matrix (left-handed)
		\~Japanese	正射影行列化(左手系)
	*/
	Matrix44& OrthographicLH(float width, float height, float zn, float zf);

	/**
		@brief
		\~English	Scaling matrix
		\~Japanese	拡大行列化
	*/
	void Scaling(float x, float y, float z);

	/**
		@brief
		\~English	X axis rotation matrix (right-handed)
		\~Japanese	X軸回転行列(右手)
	*/
	void RotationX(float angle);

	/**
		@brief
		\~English	Y axis rotation matrix (right-handed)
		\~Japanese	Y軸回転行列(右手)
	*/
	void RotationY(float angle);

	/**
		@brief
		\~English	Z axis rotation matrix (right-handed)
		\~Japanese	Z軸回転行列(右手)
	*/
	void RotationZ(float angle);

	/**
		@brief
		\~English	Translation matrix
		\~Japanese	移動行列
	*/
	void Translation(float x, float y, float z);

	/**
		@brief
		\~English	Rotation matrix around an arbitrary axis
		\~Japanese	任意軸反時計回転行列
	*/
	void RotationAxis(const Vector3D& axis, float angle);

	/**
		@brief
		\~English	Convert from quaternion to matrix
		\~Japanese	クオータニオンから行列に変換
	*/
	void Quaternion(float x, float y, float z, float w);

	/**
		@brief
		\~English	Matrix multiplication
		\~Japanese	行列の乗算
	*/
	static Matrix44& Mul(Matrix44& o, const Matrix44& in1, const Matrix44& in2);

	/**
		@brief
		\~English	Matrix inversion
		\~Japanese	逆行列化
	*/
	static Matrix44& Inverse(Matrix44& o, const Matrix44& in);
};

} // namespace Effekseer

#endif // __EFFEKSEER_MATRIX44_H__
