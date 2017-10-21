
#ifndef	__EFFEKSEER_BASE_PRE_H__
#define	__EFFEKSEER_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <stdint.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
#define	EFK_STDCALL	__stdcall
#else
#define	EFK_STDCALL
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
#elif defined(_PSVITA)
#include "Effekseer.PSVita.h"
#elif defined(_PS4)
#include "Effekseer.PS4.h"
#elif defined(_SWITCH)
#include "Effekseer.Switch.h"
#elif defined(_XBOXONE)
#include "Effekseer.XBoxOne.h"
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef uint16_t			EFK_CHAR;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Vector2D;
struct Vector3D;
struct Matrix43;
struct Matrix44;
struct RectF;

class Manager;
class Effect;
class EffectNode;

class ParticleRenderer;
class SpriteRenderer;
class RibbonRenderer;
class RingRenderer;
class ModelRenderer;
class TrackRenderer;

class Setting;
class EffectLoader;
class TextureLoader;

class SoundPlayer;
class SoundLoader;

class ModelLoader;

class Model;

typedef	int	Handle;

/**
	@brief	メモリ確保関数
*/
typedef void* ( EFK_STDCALL *MallocFunc ) ( unsigned int size );

/**
	@brief	メモリ破棄関数
*/
typedef	void ( EFK_STDCALL *FreeFunc ) ( void* p, unsigned int size );

/**
	@brief	ランダム関数
*/
typedef	int ( EFK_STDCALL *RandFunc ) (void);

/**
	@brief	エフェクトのインスタンス破棄時のコールバックイベント
	@param	manager	[in]	所属しているマネージャー
	@param	handle	[in]	エフェクトのインスタンスのハンドル
	@param	isRemovingManager	[in]	マネージャーを破棄したときにエフェクトのインスタンスを破棄しているか
*/
typedef	void ( EFK_STDCALL *EffectInstanceRemovingCallback ) ( Manager* manager, Handle handle, bool isRemovingManager );

#define ES_SAFE_ADDREF(val)						if ( (val) != NULL ) { (val)->AddRef(); }
#define ES_SAFE_RELEASE(val)					if ( (val) != NULL ) { (val)->Release(); (val) = NULL; }
#define ES_SAFE_DELETE(val)						if ( (val) != NULL ) { delete (val); (val) = NULL; }
#define ES_SAFE_DELETE_ARRAY(val)				if ( (val) != NULL ) { delete [] (val); (val) = NULL; }

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	アルファブレンド
*/
enum class AlphaBlendType : int32_t
{
	/// <summary>
	/// 不透明
	/// </summary>
	Opacity = 0,
	/// <summary>
	/// 透明
	/// </summary>
	Blend = 1,
	/// <summary>
	/// 加算
	/// </summary>
	Add = 2,
	/// <summary>
	/// 減算
	/// </summary>
	Sub = 3,
	/// <summary>
	/// 乗算
	/// </summary>
	Mul = 4,
};

enum class TextureFilterType : int32_t
{
	Nearest = 0,
	Linear = 1,
};

enum class TextureWrapType : int32_t
{
	Repeat = 0,
	Clamp = 1,
};

enum class CullingType : int32_t
{
	Front = 0,
	Back = 1,
	Double = 2,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum class BillboardType : int32_t
{
	Billboard = 0,
	YAxisFixed = 1,
	Fixed = 2,
	RotatedBillboard = 3,
};

enum class CoordinateSystem : int32_t
{
	LH,
	RH,
};

enum class CullingShape : int32_t
{
	NoneShape = 0,
	Sphere = 1,
};

enum class TextureType : int32_t
{
	Color,
	Normal,
	Distortion,
};

enum class TextureFormatType : int32_t
{
	ABGR8,
	BC1,
	BC2,
	BC3,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	最大値取得
*/
template <typename T,typename U>
T Max( T t, U u )
{
	if( t > (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	最小値取得
*/
template <typename T,typename U>
T Min( T t, U u )
{
	if( t < (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	範囲内値取得
*/
template <typename T,typename U,typename V>
T Clamp( T t, U max_, V min_ )
{
	if( t > (T)max_ )
	{
		t = (T)max_;
	}

	if( t < (T)min_ )
	{
		t = (T)min_;
	}

	return t;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
inline float NormalizeAngle(float angle)
{
    int32_t ofs = (*(int32_t*)&angle & 0x80000000) | 0x3F000000; 
    return (angle - ((int)(angle * 0.159154943f + *(float*)&ofs) * 6.283185307f)); 
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
inline void SinCos(float x, float& s, float& c)
{
	x = NormalizeAngle(x);
	float x2 = x * x;
	float x4 = x * x * x * x;
	float x6 = x * x * x * x * x * x;
	float x8 = x * x * x * x * x * x * x * x;
	float x10 = x * x * x * x * x * x * x * x * x * x;
	s = x * (1.0f - x2 / 6.0f + x4 / 120.0f - x6 / 5040.0f + x8 / 362880.0f - x10 / 39916800.0f);
	c = 1.0f - x2 / 2.0f + x4 / 24.0f - x6 / 720.0f + x8 / 40320.0f - x10 / 3628800.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	文字コードを変換する。(UTF16 -> UTF8)
	@param	dst	[out]	出力配列の先頭ポインタ
	@param	dst_size	[in]	出力配列の長さ
	@param	src			[in]	入力配列の先頭ポインタ
	@return	文字数
*/
inline int32_t ConvertUtf16ToUtf8( int8_t* dst, int32_t dst_size, const int16_t* src )
{
	int32_t cnt = 0;
	const int16_t* wp = src;
	int8_t* cp = dst;

	if (dst_size == 0) return 0;
	
	dst_size -= 3;

	for (cnt = 0; cnt < dst_size; )
	{
		int16_t wc = *wp++;
		if (wc == 0)
		{
			break;
		}
		if ((wc & ~0x7f) == 0)
		{
			*cp++ = wc & 0x7f;
			cnt += 1;
		} else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >>  6) & 0x1f) | 0xc0;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 2;
		} else {
			*cp++ = ((wc >> 12) &  0xf) | 0xe0;
			*cp++ = ((wc >>  6) & 0x3f) | 0x80;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 3;
		}
	}
	*cp = '\0';
	return cnt;
}

/**
	@brief	文字コードを変換する。(UTF8 -> UTF16)
	@param	dst	[out]	出力配列の先頭ポインタ
	@param	dst_size	[in]	出力配列の長さ
	@param	src			[in]	入力配列の先頭ポインタ
	@return	文字数
*/
inline int32_t ConvertUtf8ToUtf16( int16_t* dst, int32_t dst_size, const int8_t* src )
{
	int32_t i, code;
	int8_t c0, c1, c2;

	if (dst_size == 0) return 0;
	
	dst_size -= 1;

	for (i = 0; i < dst_size; i++)
	{
		int16_t wc;
		
		c0 = *src++;
		if (c0 == '\0')
		{
			break;
		}
		// UTF8からUTF16に変換
		code = (uint8_t)c0 >> 4;
		if (code <= 7)
		{
			// 8bit文字
			wc = c0;
		} 
		else if (code >= 12 && code <= 13)
		{
			// 16bit文字
			c1 = *src++;
			wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
		} 
		else if (code == 14)
		{
			// 24bit文字
			c1 = *src++;
			c2 = *src++;
			wc = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
		} 
		else
		{
			continue;
		}
		dst[i] = wc;
	}
	dst[i] = 0;
	return i;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
@brief	参照カウンタのインターフェース
*/
class IReference
{
public:
	/**
	@brief	参照カウンタを加算する。
	@return	加算後の参照カウンタ
	*/
	virtual int AddRef() = 0;

	/**
	@brief	参照カウンタを取得する。
	@return	参照カウンタ
	*/
	virtual int GetRef() = 0;

	/**
	@brief	参照カウンタを減算する。0になった時、インスタンスを削除する。
	@return	減算後の参照カウンタ
	*/
	virtual int Release() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
@brief	参照カウンタオブジェクト
*/
class ReferenceObject
	: public IReference
{
private:
	mutable std::atomic<int32_t> m_reference;

public:
	ReferenceObject()
		: m_reference(1)
	{
	}

	virtual ~ReferenceObject()
	{}

	virtual int AddRef()
	{
		std::atomic_fetch_add_explicit(&m_reference, 1, std::memory_order_consume);

		return m_reference;
	}

	virtual int GetRef()
	{
		return m_reference;
	}

	virtual int Release()
	{
		bool destroy = std::atomic_fetch_sub_explicit(&m_reference, 1, std::memory_order_consume) == 1;
		if (destroy)
		{
			delete this;
			return 0;
		}

		return m_reference;
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	\~english	Texture data
			\~japanese	テクスチャデータ
*/
struct TextureData
{
	int32_t Width;
	int32_t Height;
	TextureFormatType	TextureFormat;
	void*	UserPtr;
	int64_t	UserID;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_BASE_PRE_H__
#ifndef	__EFFEKSEER_VECTOR2D_H__
#define	__EFFEKSEER_VECTOR2D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3次元ベクトル
*/
struct Vector2D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	コンストラクタ
	*/
	Vector2D();

	/**
		@brief	コンストラクタ
	*/
	Vector2D( float x, float y );

	Vector2D& operator+=( const Vector2D& value );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_VECTOR3D_H__
#define	__EFFEKSEER_VECTOR3D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3次元ベクトル
*/
struct Vector3D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	Z
	*/
	float	Z;

	/**
		@brief	コンストラクタ
	*/
	Vector3D();

	/**
		@brief	コンストラクタ
	*/
	Vector3D( float x, float y, float z );

	Vector3D operator + ( const Vector3D& o ) const;

	Vector3D operator - ( const Vector3D& o ) const;

	Vector3D operator * ( const float& o ) const;

	Vector3D operator / ( const float& o ) const;

	Vector3D& operator += ( const Vector3D& o );

	Vector3D& operator -= ( const Vector3D& o );

	Vector3D& operator *= ( const float& o );

	Vector3D& operator /= ( const float& o );

	/**
		@brief	加算
	*/
	static void Add( Vector3D* pOut, const Vector3D* pIn1, const Vector3D* pIn2 );

	/**
		@brief	減算
	*/
	static Vector3D& Sub( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	長さ
	*/
	static float Length( const Vector3D& in );

	/**
		@brief	長さの二乗
	*/
	static float LengthSq( const Vector3D& in );

	/**
		@brief	内積
	*/
	static float Dot( const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	単位ベクトル
	*/
	static void Normal( Vector3D& o, const Vector3D& in );

	/**
		@brief	外積
		@note
		右手系の場合、右手の親指がin1、人差し指がin2としたとき、中指の方向を返す。<BR>
		左手系の場合、左手の親指がin1、人差し指がin2としたとき、中指の方向を返す。<BR>
	*/
	static Vector3D& Cross( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix43& mat );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix44& mat );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_COLOR_H__
#define	__EFFEKSEER_COLOR_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ColorMode
{
	COLOR_MODE_RGBA,
	COLOR_MODE_HSVA,
	COLOR_MODE_DWORD = 0x7FFFFFFF
};

/**
	@brief	色
*/
#pragma pack(push,1)
struct Color
{
	/**
		@brief	赤
	*/
	uint8_t		R;

	/**
		@brief	緑
	*/
	uint8_t		G;

	/**
		@brief	青
	*/
	uint8_t		B;
	
	/**
		@brief	透明度
	*/
	uint8_t		A;

	/**
		@brief	コンストラクタ
	*/
	Color();

	/**
		@brief	コンストラクタ
	*/
	Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );

	/**
		@brief	乗算
	*/
	static void Mul( Color& o, const Color& in1, const Color& in2 );
};
#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_COLOR_H__

#ifndef	__EFFEKSEER_RECTF_H__
#define	__EFFEKSEER_RECTF_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	四角形
*/
struct RectF
{
private:

public:
	float	X;

	float	Y;

	float	Width;

	float	Height;

	RectF();

	RectF( float x, float y, float width, float height );

	Vector2D Position() const;

	Vector2D Size() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RECTF_H__

#ifndef	__EFFEKSEER_MATRIX43_H__
#define	__EFFEKSEER_MATRIX43_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	4x3行列
	@note
	右手系(回転:反時計回り)<BR>
	V[x,y,z,1] * M の形でベクトルとの乗算が可能である。<BR>
	[0,0][0,1][0,2]<BR>
	[1,0][1,1][1,2]<BR>
	[2,0][2,1][2,2]<BR>
	[3,0][3,1][3,2]<BR>
*/
#pragma pack(push,1)
struct Matrix43
{
private:

public:
	/**
		@brief	行列の値
	*/
	float	Value[4][3];
	
	/**
		@brief	単位行列化を行う。
	*/
	void Indentity();

	/**
		@brief	拡大行列化を行う。
		@param	x	[in]	X方向拡大率
		@param	y	[in]	Y方向拡大率
		@param	z	[in]	Z方向拡大率
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	反時計周り方向のX軸回転行列化を行う。
		@param	angle	[in]	角度(ラジアン)
	*/
	void RotationX( float angle );

	/**
		@brief	反時計周り方向のY軸回転行列化を行う。
		@param	angle	[in]	角度(ラジアン)
	*/
	void RotationY( float angle );

	/**
		@brief	反時計周り方向のZ軸回転行列化を行う。
		@param	angle	[in]	角度(ラジアン)
	*/
	void RotationZ( float angle );
	
	/**
		@brief	反時計周り方向のXYZ軸回転行列化を行う。
		@param	rx	[in]	角度(ラジアン)
		@param	ry	[in]	角度(ラジアン)
		@param	rz	[in]	角度(ラジアン)
	*/
	void RotationXYZ( float rx, float ry, float rz );
	
	/**
		@brief	反時計周り方向のZXY軸回転行列化を行う。
		@param	rz	[in]	角度(ラジアン)
		@param	rx	[in]	角度(ラジアン)
		@param	ry	[in]	角度(ラジアン)
	*/
	void RotationZXY( float rz, float rx, float ry );

	/**
		@brief	任意軸に対する反時計周り方向回転行列化を行う。
		@param	axis	[in]	回転軸
		@param	angle	[in]	角度(ラジアン)
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	任意軸に対する反時計周り方向回転行列化を行う。
		@param	axis	[in]	回転軸
		@param	s	[in]	サイン
		@param	c	[in]	コサイン
	*/
	void RotationAxis( const Vector3D& axis, float s, float c );

	/**
		@brief	移動行列化を行う。
		@param	x	[in]	X方向移動
		@param	y	[in]	Y方向移動
		@param	z	[in]	Z方向移動
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	行列を、拡大、回転、移動の行列とベクトルに分解する。
		@param	s	[out]	拡大行列
		@param	r	[out]	回転行列
		@param	t	[out]	位置
	*/
	void GetSRT( Vector3D& s, Matrix43& r, Vector3D& t ) const; 
	
	/**
		@brief	行列から拡大ベクトルを取得する。
		@param	s	[out]	拡大ベクトル
	*/
	void GetScale( Vector3D& s ) const;
	
	/**
		@brief	行列から回転行列を取得する。
		@param	s	[out]	回転行列
	*/
	void GetRotation( Matrix43& r ) const;

	/**
		@brief	行列から移動ベクトルを取得する。
		@param	t	[out]	移動ベクトル
	*/
	void GetTranslation( Vector3D& t ) const;
	
	/**
		@brief	行列の拡大、回転、移動を設定する。
		@param	s	[in]	拡大行列
		@param	r	[in]	回転行列
		@param	t	[in]	位置
	*/
	void SetSRT( const Vector3D& s, const Matrix43& r, const Vector3D& t );

	/**
		@brief	行列同士の乗算を行う。
		@param	out	[out]	結果
		@param	in1	[in]	乗算の左側
		@param	in2	[in]	乗算の右側
	*/
	static void Multiple( Matrix43& out, const Matrix43& in1, const Matrix43& in2 );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX43_H__

#ifndef	__EFFEKSEER_MATRIX44_H__
#define	__EFFEKSEER_MATRIX44_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	行列
	@note
	右手系<BR>
	左手系<BR>
	V[x,y,z,1] * M の形<BR>
	[0,0][0,1][0,2][0,3]
	[1,0][1,1][1,2][1,3]
	[2,0][2,1][2,2][2,3]
	[3,0][3,1][3,2][3,3]
*/
#pragma pack(push,1)
struct Matrix44
{
private:

public:

	/**
		@brief	コンストラクタ
	*/
	Matrix44();

	/**
		@brief	行列の値
	*/
	float	Values[4][4];

	/**
		@brief	単位行列化
	*/
	Matrix44& Indentity();

	/**
	@brief	転置行列化
	*/
	Matrix44& Transpose();

	/**
		@brief	カメラ行列化(右手系)
	*/
	Matrix44& LookAtRH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	カメラ行列化(左手系)
	*/
	Matrix44& LookAtLH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	射影行列化(右手系)
	*/
	Matrix44& PerspectiveFovRH( float ovY, float aspect, float zn, float zf );

	/**
		@brief	OpenGL用射影行列化(右手系)
	*/
	Matrix44& PerspectiveFovRH_OpenGL( float ovY, float aspect, float zn, float zf );

	/**
		@brief	射影行列化(左手系)
	*/
	Matrix44& PerspectiveFovLH( float ovY, float aspect, float zn, float zf );
	
	/**
	 @brief	OpenGL用射影行列化(左手系)
	 */
	Matrix44& PerspectiveFovLH_OpenGL( float ovY, float aspect, float zn, float zf );
	
	/**
		@brief	正射影行列化(右手系)
	*/
	Matrix44& OrthographicRH( float width, float height, float zn, float zf );

	/**
		@brief	正射影行列化(左手系)
	*/
	Matrix44& OrthographicLH( float width, float height, float zn, float zf );

	/**
		@brief	拡大行列化
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	X軸回転行列(右手)
	*/
	void RotationX( float angle );

	/**
		@brief	Y軸回転行列(右手)
	*/
	void RotationY( float angle );

	/**
		@brief	Z軸回転行列(右手)
	*/
	void RotationZ( float angle );

	/**
		@brief	移動行列
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	任意軸反時計回転行列
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	クオータニオンから行列に変換
	*/
	void Quaternion( float x, float y, float z, float w );

	/**
		@brief	乗算
	*/
	static Matrix44& Mul( Matrix44& o, const Matrix44& in1, const Matrix44& in2 );

	/**
		@brief	逆行列
	*/
	static Matrix44& Inverse( Matrix44& o, const Matrix44& in );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX44_H__

#ifndef	__EFFEKSEER_FILE_H__
#define	__EFFEKSEER_FILE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	ファイル読み込みクラス
*/
class FileReader
{
private:

public:
	FileReader() {}

	virtual ~FileReader() {}

	virtual size_t Read( void* buffer, size_t size ) = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	ファイル書き込みクラス
*/
class FileWriter
{
private:

public:
	FileWriter() {}

	virtual ~FileWriter() {}

	virtual size_t Write( const void* buffer, size_t size ) = 0;

	virtual void Flush() = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	ファイルアクセス用のファクトリクラス
*/
class FileInterface
{
private:

public:
	virtual FileReader* OpenRead( const EFK_CHAR* path ) = 0;

	virtual FileWriter* OpenWrite( const EFK_CHAR* path ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_FILE_H__

#ifndef	__EFFEKSEER_DEFAULT_FILE_H__
#define	__EFFEKSEER_DEFAULT_FILE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	標準のファイル読み込みクラス
*/

class DefaultFileReader : public FileReader
{
private:
	FILE* m_filePtr;

public:
	DefaultFileReader( FILE* filePtr );

	~DefaultFileReader();

	size_t Read( void* buffer, size_t size );

	void Seek( int position );

	int GetPosition();

	size_t GetLength();
};

class DefaultFileWriter : public FileWriter
{
private:
	FILE* m_filePtr;

public:
	DefaultFileWriter( FILE* filePtr );

	~DefaultFileWriter();

	size_t Write( const void* buffer, size_t size );

	void Flush();

	void Seek( int position );

	int GetPosition();

	size_t GetLength();
};

class DefaultFileInterface : public FileInterface
{
private:

public:
	FileReader* OpenRead( const EFK_CHAR* path );

	FileWriter* OpenWrite( const EFK_CHAR* path );
};


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DEFAULT_FILE_H__

#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	エフェクトパラメータークラス
	@note
	エフェクトに設定されたパラメーター。
*/
class Effect
	: public IReference
{
protected:
	Effect() {}
    ~Effect() {}

public:

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

		/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Setting*	setting, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Setting*	setting, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
	@brief	標準のエフェクト読込インスタンスを生成する。
	*/
	static ::Effekseer::EffectLoader* CreateEffectLoader(::Effekseer::FileInterface* fileInterface = NULL);

	/**
	@brief	設定を取得する。
	@return	設定
	*/
	virtual Setting* GetSetting() const = 0;

	/**
	@brief	\~English	Get the magnification multiplied by the magnification at the time of loaded and exported.
			\~Japanese	読み込み時と出力時の拡大率をかけた拡大率を取得する。
	*/
	virtual float GetMaginification() const = 0;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	virtual int GetVersion() const = 0;

	/**
		@brief	格納されている色画像のポインタを取得する。
		@param	n	[in]	画像のインデックス
		@return	画像のポインタ
	*/
	virtual TextureData* GetColorImage( int n ) const = 0;

	/**
	@brief	格納されている画像のポインタの個数を取得する。
	*/
	virtual int32_t GetColorImageCount() const = 0;

	/**
	@brief	格納されている法線画像のポインタを取得する。
	@param	n	[in]	画像のインデックス
	@return	画像のポインタ
	*/
	virtual TextureData* GetNormalImage(int n) const = 0;

	/**
	@brief	格納されている法線画像のポインタの個数を取得する。
	*/
	virtual int32_t GetNormalImageCount() const = 0;

	/**
	@brief	格納されている歪み画像のポインタを取得する。
	@param	n	[in]	画像のインデックス
	@return	画像のポインタ
	*/
	virtual TextureData* GetDistortionImage(int n) const = 0;

	/**
	@brief	格納されている歪み画像のポインタの個数を取得する。
	*/
	virtual int32_t GetDistortionImageCount() const = 0;

	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	virtual void* GetWave( int n ) const = 0;

	/**
	@brief	格納されている音波形のポインタの個数を取得する。
	*/
	virtual int32_t GetWaveCount() const = 0;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	virtual void* GetModel( int n ) const = 0;

	/**
	@brief	格納されているモデルのポインタの個数を取得する。
	*/
	virtual int32_t GetModelCount() const = 0;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	virtual bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	virtual bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	エフェクトのリロードを行う。
		@param	managers	[in]	マネージャーの配列
		@param	managersCount	[in]	マネージャーの個数
		@param	data	[in]	エフェクトのデータ
		@param	size	[in]	エフェクトのデータサイズ
		@param	materialPath	[in]	リソースの読み込み元
		@return	成否
		@note
		Settingを用いてエフェクトを生成したときに、Managerを指定することで対象のManager内のエフェクトのリロードを行う。
	*/
	virtual bool Reload( Manager* managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
	@brief	エフェクトのリロードを行う。
	@param	managers	[in]	マネージャーの配列
	@param	managersCount	[in]	マネージャーの個数
	@param	path	[in]	エフェクトの読み込み元
	@param	materialPath	[in]	リソースの読み込み元
	@return	成否
	@note
	Settingを用いてエフェクトを生成したときに、Managerを指定することで対象のManager内のエフェクトのリロードを行う。
	*/
	virtual bool Reload( Manager* managers, int32_t managersCount,const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	virtual void ReloadResources( const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	画像等リソースの破棄を行う。
	*/
	virtual void UnloadResources() = 0;

	/**
	@brief	Rootを取得する。
	*/
	virtual EffectNode* GetRoot() const = 0;
};

/**
@brief	共通描画パラメーター
@note
大きく変更される可能性があります。
*/
struct EffectBasicRenderParameter
{
	int32_t				ColorTextureIndex;
	AlphaBlendType		AlphaBlend;
	TextureFilterType	FilterType;
	TextureWrapType		WrapType;
	bool				ZWrite;
	bool				ZTest;
	bool				Distortion;
	float				DistortionIntensity;
};

/**
@brief	
	\~English	Model parameter
	\~Japanese	モデルパラメーター
@note
	\~English	It may change greatly.
	\~Japanese	大きく変更される可能性があります。

*/
struct EffectModelParameter
{
	bool				Lighting;
};

/**
@brief	ノードインスタンス生成クラス
@note
エフェクトのノードの実体を生成する。
*/
class EffectNode
{
public:
	EffectNode() {}
	virtual ~EffectNode(){}

	/**
	@brief	ノードが所属しているエフェクトを取得する。
	*/
	virtual Effect* GetEffect() const = 0;

	/**
	@brief	子のノードの数を取得する。
	*/
	virtual int GetChildrenCount() const = 0;

	/**
	@brief	子のノードを取得する。
	*/
	virtual EffectNode* GetChild(int index) const = 0;

	/**
	@brief	共通描画パラメーターを取得する。
	*/
	virtual EffectBasicRenderParameter GetBasicRenderParameter() = 0;

	/**
	@brief	共通描画パラメーターを設定する。
	*/
	virtual void SetBasicRenderParameter(EffectBasicRenderParameter param) = 0;

	/**
	@brief	
	\~English	Get a model parameter
	\~Japanese	モデルパラメーターを取得する。
	*/
	virtual EffectModelParameter GetEffectModelParameter() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_H__

#ifndef	__EFFEKSEER_MANAGER_H__
#define	__EFFEKSEER_MANAGER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief エフェクト管理クラス
*/
class Manager
	: public IReference
{
protected:
	Manager() {}
    ~Manager() {}

public:
	/**
		@brief マネージャーを生成する。
		@param	instance_max	[in]	最大インスタンス数
		@param	autoFlip		[in]	自動でスレッド間のデータを入れ替えるかどうか、を指定する。trueの場合、Update時に入れ替わる。
		@return	マネージャー
	*/
	static Manager* Create( int instance_max, bool autoFlip = true );

	/**
		@brief マネージャーを破棄する。
		@note
		このマネージャーから生成されたエフェクトは全て強制的に破棄される。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	メモリ確保関数を取得する。
	*/
	virtual MallocFunc GetMallocFunc() const = 0;

	/**
		@brief	メモリ確保関数を設定する。
	*/
	virtual void SetMallocFunc( MallocFunc func ) = 0;

	/**
		@brief	メモリ破棄関数を取得する。
	*/
	virtual FreeFunc GetFreeFunc() const = 0;

	/**
		@brief	メモリ破棄関数を設定する。
	*/
	virtual void SetFreeFunc( FreeFunc func ) = 0;

	/**
		@brief	ランダム関数を取得する。
	*/
	virtual RandFunc GetRandFunc() const = 0;

	/**
		@brief	ランダム関数を設定する。
	*/
	virtual void SetRandFunc( RandFunc func ) = 0;

	/**
		@brief	ランダム最大値を取得する。
	*/
	virtual int GetRandMax() const = 0;

	/**
		@brief	ランダム関数を設定する。
	*/
	virtual void SetRandMax( int max_ ) = 0;

	/**
		@brief	座標系を取得する。
		@return	座標系
	*/
	virtual CoordinateSystem GetCoordinateSystem() const = 0;

	/**
		@brief	座標系を設定する。
		@param	coordinateSystem	[in]	座標系
		@note
		座標系を設定する。
		エフェクトファイルを読み込む前に設定する必要がある。
	*/
	virtual void SetCoordinateSystem( CoordinateSystem coordinateSystem ) = 0;

	/**
		@brief	スプライト描画機能を取得する。
	*/
	virtual SpriteRenderer* GetSpriteRenderer() = 0;

	/**
		@brief	スプライト描画機能を設定する。
	*/
	virtual void SetSpriteRenderer( SpriteRenderer* renderer ) = 0;

	/**
		@brief	ストライプ描画機能を取得する。
	*/
	virtual RibbonRenderer* GetRibbonRenderer() = 0;

	/**
		@brief	ストライプ描画機能を設定する。
	*/
	virtual void SetRibbonRenderer( RibbonRenderer* renderer ) = 0;

	/**
		@brief	リング描画機能を取得する。
	*/
	virtual RingRenderer* GetRingRenderer() = 0;

	/**
		@brief	リング描画機能を設定する。
	*/
	virtual void SetRingRenderer( RingRenderer* renderer ) = 0;

	/**
		@brief	モデル描画機能を取得する。
	*/
	virtual ModelRenderer* GetModelRenderer() = 0;

	/**
		@brief	モデル描画機能を設定する。
	*/
	virtual void SetModelRenderer( ModelRenderer* renderer ) = 0;

	/**
		@brief	軌跡描画機能を取得する。
	*/
	virtual TrackRenderer* GetTrackRenderer() = 0;

	/**
		@brief	軌跡描画機能を設定する。
	*/
	virtual void SetTrackRenderer( TrackRenderer* renderer ) = 0;

	/**
		@brief	設定クラスを取得する。
	*/
	virtual Setting* GetSetting() = 0;

	/**
		@brief	設定クラスを設定する。
		@param	setting	[in]	設定
	*/
	virtual void SetSetting(Setting* setting) = 0;

	/**
		@brief	エフェクト読込クラスを取得する。
	*/
	virtual EffectLoader* GetEffectLoader() = 0;

	/**
		@brief	エフェクト読込クラスを設定する。
	*/
	virtual void SetEffectLoader( EffectLoader* effectLoader ) = 0;

	/**
		@brief	テクスチャ読込クラスを取得する。
	*/
	virtual TextureLoader* GetTextureLoader() = 0;

	/**
		@brief	テクスチャ読込クラスを設定する。
	*/
	virtual void SetTextureLoader( TextureLoader* textureLoader ) = 0;
	
	/**
		@brief	サウンド再生機能を取得する。
	*/
	virtual SoundPlayer* GetSoundPlayer() = 0;

	/**
		@brief	サウンド再生機能を設定する。
	*/
	virtual void SetSoundPlayer( SoundPlayer* soundPlayer ) = 0;
	
	/**
		@brief	サウンド読込クラスを取得する
	*/
	virtual SoundLoader* GetSoundLoader() = 0;
	
	/**
		@brief	サウンド読込クラスを設定する。
	*/
	virtual void SetSoundLoader( SoundLoader* soundLoader ) = 0;

	/**
		@brief	モデル読込クラスを取得する。
	*/
	virtual ModelLoader* GetModelLoader() = 0;

	/**
		@brief	モデル読込クラスを設定する。
	*/
	virtual void SetModelLoader( ModelLoader* modelLoader ) = 0;

	/**
		@brief	エフェクトを停止する。
		@param	handle	[in]	インスタンスのハンドル
	*/
	virtual void StopEffect( Handle handle ) = 0;

	/**
		@brief	全てのエフェクトを停止する。
	*/
	virtual void StopAllEffects() = 0;

	/**
		@brief	エフェクトのルートだけを停止する。
		@param	handle	[in]	インスタンスのハンドル
	*/
	virtual void StopRoot( Handle handle ) = 0;

	/**
		@brief	エフェクトのルートだけを停止する。
		@param	effect	[in]	エフェクト
	*/
	virtual void StopRoot( Effect* effect ) = 0;

	/**
		@brief	エフェクトのインスタンスが存在しているか取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	存在してるか?
	*/
	virtual bool Exists( Handle handle ) = 0;

	/**
		@brief	エフェクトに使用されているインスタンス数を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	インスタンス数
		@note
		Rootも個数に含まれる。つまり、Root削除をしていない限り、
		Managerに残っているインスタンス数+エフェクトに使用されているインスタンス数は存在しているRootの数だけ
		最初に確保した個数よりも多く存在する。
	*/
	virtual int32_t GetInstanceCount( Handle handle ) = 0;

	/**
		@brief	エフェクトのインスタンスに設定されている行列を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	行列
	*/
	virtual Matrix43 GetMatrix( Handle handle ) = 0;

	/**
		@brief	エフェクトのインスタンスに変換行列を設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	mat		[in]	変換行列
	*/
	virtual void SetMatrix( Handle handle, const Matrix43& mat ) = 0;

	/**
		@brief	エフェクトのインスタンスの位置を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	位置
	*/
	virtual Vector3D GetLocation( Handle handle ) = 0;

	/**
		@brief	エフェクトのインスタンスの位置を指定する。
		@param	x	[in]	X座標
		@param	y	[in]	Y座標
		@param	z	[in]	Z座標
	*/
	virtual void SetLocation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	エフェクトのインスタンスの位置を指定する。
		@param	location	[in]	位置
	*/
	virtual void SetLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	エフェクトのインスタンスの位置に加算する。
		@param	location	[in]	加算する値
	*/
	virtual void AddLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	エフェクトのインスタンスの回転角度を指定する。(ラジアン)
	*/
	virtual void SetRotation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	エフェクトのインスタンスの任意軸周りの反時計周りの回転角度を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	axis	[in]	軸
		@param	angle	[in]	角度(ラジアン)
	*/
	virtual void SetRotation( Handle handle, const Vector3D& axis, float angle ) = 0;
	
	/**
		@brief	エフェクトのインスタンスの拡大率を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	x		[in]	X方向拡大率
		@param	y		[in]	Y方向拡大率
		@param	z		[in]	Z方向拡大率
	*/
	virtual void SetScale( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	エフェクトのインスタンスのターゲット位置を指定する。
		@param	x	[in]	X座標
		@param	y	[in]	Y座標
		@param	z	[in]	Z座標
	*/
	virtual void SetTargetLocation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	エフェクトのインスタンスのターゲット位置を指定する。
		@param	location	[in]	位置
	*/
	virtual void SetTargetLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	エフェクトのベース行列を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	ベース行列
	*/
	virtual Matrix43 GetBaseMatrix( Handle handle ) = 0;

	/**
		@brief	エフェクトのベース行列を設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	mat		[in]	設定する行列
		@note
		エフェクト全体の表示位置を指定する行列を設定する。
	*/
	virtual void SetBaseMatrix( Handle handle, const Matrix43& mat ) = 0;

	/**
		@brief	エフェクトのインスタンスに廃棄時のコールバックを設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	callback	[in]	コールバック
	*/
	virtual void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback ) = 0;

	/**
	@brief	\~English	Get status that a particle of effect specified is shown.
	\~Japanese	指定したエフェクトのパーティクルが表示されているか取得する。

	@param	handle	\~English	Particle's handle
	\~Japanese	パーティクルのハンドル
	*/
	virtual bool GetShown(Handle handle) = 0;

	/**
		@brief	エフェクトのインスタンスをDraw時に描画するか設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	shown	[in]	描画するか?
	*/
	virtual void SetShown( Handle handle, bool shown ) = 0;

	/**
	@brief	\~English	Get status that a particle of effect specified is paused.
	\~Japanese	指定したエフェクトのパーティクルが一時停止されているか取得する。

	@param	handle	\~English	Particle's handle
			\~Japanese	パーティクルのハンドル
	*/
	virtual bool GetPaused(Handle handle) = 0;

	/**
		@brief	\~English	Pause or resume a particle of effect specified.
		\~Japanese	指定したエフェクトのパーティクルを一時停止、もしくは再開する。

		@param	handle	[in]	インスタンスのハンドル
		@param	paused	[in]	更新するか?
	*/
	virtual void SetPaused( Handle handle, bool paused ) = 0;

	/**
			@brief	\~English	Pause or resume all particle of effects.
			\~Japanese	全てのエフェクトのパーティクルを一時停止、もしくは再開する。
			@param	paused \~English	Pause or resume
			\~Japanese	一時停止、もしくは再開
	*/
	virtual void SetPausedToAllEffects(bool paused) = 0;

	/**
		@brief	エフェクトのインスタンスを再生スピードを設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	speed	[in]	スピード
	*/
	virtual void SetSpeed( Handle handle, float speed ) = 0;

	/**
		@brief	エフェクトがDrawで描画されるか設定する。
				autoDrawがfalseの場合、DrawHandleで描画する必要がある。
		@param	autoDraw	[in]	自動描画フラグ
	*/
	virtual void SetAutoDrawing( Handle handle, bool autoDraw ) = 0;

	/**
		@brief	今までのPlay等の処理をUpdate実行時に適用するようにする。
	*/
	virtual void Flip() = 0;

	/**
		@brief	更新処理を行う。
		@param	deltaFrame	[in]	更新するフレーム数(60fps基準)	
	*/
	virtual void Update( float deltaFrame = 1.0f ) = 0;

	/**
		@brief	更新処理を開始する。
		@note
		Updateを実行する際は、実行する必要はない。
	*/
	virtual void BeginUpdate() = 0;

	/**
		@brief	更新処理を終了する。
		@note
		Updateを実行する際は、実行する必要はない。
	*/
	virtual void EndUpdate() = 0;

	/**
		@brief	ハンドル単位の更新を行う。
		@param	handle		[in]	ハンドル
		@param	deltaFrame	[in]	更新するフレーム数(60fps基準)
		@note
		更新する前にBeginUpdate、更新し終わった後にEndUpdateを実行する必要がある。
	*/
	virtual void UpdateHandle( Handle handle, float deltaFrame = 1.0f ) = 0;

	/**
		@brief	描画処理を行う。
	*/
	virtual void Draw() = 0;
	
	/**
		@brief	ハンドル単位の描画処理を行う。
	*/
	virtual void DrawHandle( Handle handle ) = 0;

	/**
		@brief	再生する。
		@param	effect	[in]	エフェクト
		@param	x	[in]	X座標
		@param	y	[in]	Y座標
		@param	z	[in]	Z座標
		@return	エフェクトのインスタンスのハンドル
	*/
	virtual Handle Play( Effect* effect, float x, float y, float z ) = 0;
	
	/**
		@brief	Update処理時間を取得。
	*/
	virtual int GetUpdateTime() const = 0;
	
	/**
		@brief	Draw処理時間を取得。
	*/
	virtual int GetDrawTime() const = 0;

	/**
		@brief	残りの確保したインスタンス数を取得する。
	*/
	virtual int32_t GetRestInstancesCount() const = 0;

	/**
		@brief	エフェクトをカリングし描画負荷を減らすための空間を生成する。
		@param	xsize	X方向幅
		@param	ysize	Y方向幅
		@param	zsize	Z方向幅
		@param	layerCount	層数(大きいほどカリングの効率は上がるがメモリも大量に使用する)
	*/
	virtual void CreateCullingWorld( float xsize, float ysize, float zsize, int32_t layerCount) = 0;

	/**
		@brief	カリングを行い、カリングされたオブジェクトのみを描画するようにする。
		@param	cameraProjMat	カメラプロジェクション行列
		@param	isOpenGL		OpenGLによる描画か?
	*/
	virtual void CalcCulling(const Matrix44& cameraProjMat, bool isOpenGL) = 0;

	/**
		@brief	現在存在するエフェクトのハンドルからカリングの空間を配置しなおす。
	*/
	virtual void RessignCulling() = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MANAGER_H__

#ifndef	__EFFEKSEER_SPRITE_RENDERER_H__
#define	__EFFEKSEER_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SpriteRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		AlphaBlendType			AlphaBlend;
		TextureFilterType	TextureFilter;
		TextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		BillboardType		Billboard;

		bool				Distortion;
		float				DistortionIntensity;
	};

	struct InstanceParameter
	{
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// 左下、右下、左上、右上
		Color		Colors[4];

		Vector2D	Positions[4];

		RectF	UV;
	};

public:
	SpriteRenderer() {}

	virtual ~SpriteRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SPRITE_RENDERER_H__

#ifndef	__EFFEKSEER_RIBBON_RENDERER_H__
#define	__EFFEKSEER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RibbonRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		AlphaBlendType			AlphaBlend;
		TextureFilterType	TextureFilter;
		TextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		bool				ViewpointDependent;

		bool				Distortion;
		float				DistortionIntensity;
	};

	struct InstanceParameter
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// 左、右
		Color		Colors[2];

		float	Positions[2];

		RectF	UV;
	};

public:
	RibbonRenderer() {}

	virtual ~RibbonRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RIBBON_RENDERER_H__

#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RingRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		AlphaBlendType			AlphaBlend;
		TextureFilterType	TextureFilter;
		TextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		BillboardType		Billboard;
		int32_t				VertexCount;

		bool				Distortion;
		float				DistortionIntensity;
	};

	struct InstanceParameter
	{
		Matrix43	SRTMatrix43;
		float		ViewingAngle;
		Vector2D	OuterLocation;
		Vector2D	InnerLocation;
		float		CenterRatio;
		Color		OuterColor;
		Color		CenterColor;
		Color		InnerColor;
		
		RectF	UV;
	};

public:
	RingRenderer() {}

	virtual ~RingRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RING_RENDERER_H__

#ifndef	__EFFEKSEER_MODEL_RENDERER_H__
#define	__EFFEKSEER_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class ModelRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		AlphaBlendType		AlphaBlend;
		TextureFilterType	TextureFilter;
		TextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		bool				Lighting;
		CullingType		Culling;
		int32_t				ModelIndex;
		int32_t				ColorTextureIndex;
		int32_t				NormalTextureIndex;
		float				Magnification;
		bool				IsRightHand;

		bool				Distortion;
		float				DistortionIntensity;
	};

	struct InstanceParameter
	{
		Matrix43		SRTMatrix43;
		RectF			UV;
		Color			AllColor;
	};

public:
	ModelRenderer() {}

	virtual ~ModelRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODEL_RENDERER_H__

#ifndef	__EFFEKSEER_TRACK_RENDERER_H__
#define	__EFFEKSEER_TRACK_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class TrackRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		AlphaBlendType			AlphaBlend;
		TextureFilterType	TextureFilter;
		TextureWrapType		TextureWrap;
		bool				ZTest;
		bool				ZWrite;

		bool				Distortion;
		float				DistortionIntensity;
	};

	struct InstanceGroupParameter
	{
		
	};

	struct InstanceParameter
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;

		Color	ColorLeft;
		Color	ColorCenter;
		Color	ColorRight;

		Color	ColorLeftMiddle;
		Color	ColorCenterMiddle;
		Color	ColorRightMiddle;

		float	SizeFor;
		float	SizeMiddle;
		float	SizeBack;

		RectF	UV;
	};

public:
	TrackRenderer() {}

	virtual ~TrackRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TRACK_RENDERER_H__

#ifndef	__EFFEKSEER_EFFECTLOADER_H__
#define	__EFFEKSEER_EFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	エフェクトファイル読み込み破棄関数指定クラス
*/
class EffectLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	EffectLoader() {}

	/**
		@brief	デストラクタ
	*/
	virtual ~EffectLoader() {}

	/**
		@brief	エフェクトファイルを読み込む。
		@param	path	[in]	読み込み元パス
		@param	data	[out]	データ配列の先頭のポインタを出力する先
		@param	size	[out]	データ配列の長さを出力する先
		@return	成否
		@note
		エフェクトファイルを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual bool Load( const EFK_CHAR* path, void*& data, int32_t& size ) = 0;

	/**
		@brief	エフェクトファイルを破棄する。
		@param	data	[in]	データ配列の先頭のポインタ
		@param	size	[int]	データ配列の長さ
		@note
		エフェクトファイルを破棄する。
		::Effekseer::Effect::Create実行終了時に使用される。
	*/
	virtual void Unload( void* data, int32_t size ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECTLOADER_H__

#ifndef	__EFFEKSEER_TEXTURELOADER_H__
#define	__EFFEKSEER_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	テクスチャ読み込み破棄関数指定クラス
*/
class TextureLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	TextureLoader() {}

	/**
		@brief	デストラクタ
	*/
	virtual ~TextureLoader() {}

	/**
		@brief	テクスチャを読み込む。
		@param	path	[in]	読み込み元パス
		@param	textureType	[in]	テクスチャの種類
		@return	テクスチャのポインタ
		@note
		テクスチャを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual TextureData* Load( const EFK_CHAR* path, TextureType textureType ) { return nullptr; }

	/**
		@brief	テクスチャを破棄する。
		@param	data	[in]	テクスチャ
		@note
		テクスチャを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload(TextureData* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TEXTURELOADER_H__

#ifndef	__EFFEKSEER_MODELLOADER_H__
#define	__EFFEKSEER_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	モデル読み込み破棄関数指定クラス
*/
class ModelLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	ModelLoader() {}

	/**
		@brief	デストラクタ
	*/
	virtual ~ModelLoader() {}

	/**
		@brief	モデルを読み込む。
		@param	path	[in]	読み込み元パス
		@return	モデルのポインタ
		@note
		モデルを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	モデルを破棄する。
		@param	data	[in]	モデル
		@note
		モデルを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload( void* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODELLOADER_H__

#ifndef	__EFFEKSEER_MODEL_H__
#define	__EFFEKSEER_MODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief
	\~English	Model class
	\~Japanese	モデルクラス
*/
class Model
{
public:
	static const int32_t	Version = 1;

	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
	};

	struct VertexWithIndex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
		uint8_t Index[4];
	};

	struct Face
	{
		int32_t	Indexes[3];
	};

	struct Emitter
	{
		Vector3D	Position;
		Vector3D	Normal;
		Vector3D	Binormal;
		Vector3D	Tangent;
	};

private:
	uint8_t*	m_data;
	int32_t		m_size;

	int32_t		m_version;

	int32_t		m_vertexCount;
	Vertex*		m_vertexes;

	int32_t		m_faceCount;
	Face*		m_faces;

	int32_t		m_modelCount;

public:

	/**
		@brief
		\~English	Constructor
		\~Japanese	コンストラクタ
	*/
	Model( void* data, int32_t size ) 
		: m_data	( NULL )
		, m_size	( size )
		, m_version	( 0 )
		, m_vertexCount	( 0 )
		, m_vertexes	( NULL )
		, m_faceCount	( 0 )
		, m_faces		( NULL )
	{
		m_data = new uint8_t[m_size];
		memcpy( m_data, data, m_size );

		uint8_t* p = (uint8_t*)m_data;
	
		memcpy( &m_version, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		// load scale except version 3(for compatibility)
		if (m_version == 2)
		{
			// Scale
			p += sizeof(int32_t);
		}

		memcpy( &m_modelCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		memcpy( &m_vertexCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		if (m_version >= 1)
		{
			m_vertexes = (Vertex*) p;
			p += (sizeof(Vertex) * m_vertexCount);
		}
		else
		{
			// allocate new buffer
			m_vertexes = new Vertex[m_vertexCount];

			for (int32_t i = 0; i < m_vertexCount; i++)
			{
				memcpy(&m_vertexes[i], p, sizeof(Vertex) - sizeof(Color));
				m_vertexes[i].VColor = Color(255, 255, 255, 255);

				p += sizeof(Vertex) - sizeof(Color);
			}
		}
		
		memcpy( &m_faceCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		m_faces = (Face*)p;
		p += ( sizeof(Face) * m_faceCount );
	}

	Vertex* GetVertexes() const { return m_vertexes; }
	int32_t GetVertexCount() { return m_vertexCount; }

	Face* GetFaces() const { return m_faces; }
	int32_t GetFaceCount() { return m_faceCount; }

	int32_t GetModelCount() { return m_modelCount; }

	/**
		@brief
		\~English	Destructor
		\~Japanese	デストラクタ
	*/
	virtual ~Model()
	{
		if (m_version == 0)
		{
			ES_SAFE_DELETE_ARRAY(m_vertexes);
		}

		ES_SAFE_DELETE_ARRAY( m_data );
	}

	Emitter GetEmitter( Manager* manager, CoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p1 = ( (float)randFunc() / (float)randMax );
		float p2 = ( (float)randFunc() / (float)randMax );

		// Fit within plane
		if( p1 + p2 > 1.0f )
		{
			p1 = 1.0f - p1;
			p2 = 1.0f - p2;			
		}

		float p0 = 1.0f - p1 - p2;
		
		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == CoordinateSystem::LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( Manager* manager, CoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t vertexInd = (int32_t)( (GetVertexCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		vertexInd = Clamp( vertexInd, GetVertexCount() - 1, 0 );
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == CoordinateSystem::LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( int32_t index, CoordinateSystem coordinate, float magnification )
	{
		int32_t vertexInd = index % GetVertexCount();
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == CoordinateSystem::LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( Manager* manager, CoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == CoordinateSystem::LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( int32_t index, CoordinateSystem coordinate, float magnification )
	{
		int32_t faceInd = index % (GetFaceCount() - 1);
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == CoordinateSystem::LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODEL_H__

#ifndef	__EFFEKSEER_SOUND_PLAYER_H__
#define	__EFFEKSEER_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

typedef void* SoundHandle;
typedef void* SoundTag;

class SoundPlayer
{
public:
	struct InstanceParameter
	{
		void*		Data;
		float		Volume;
		float		Pan;
		float		Pitch;
		bool		Mode3D;
		Vector3D	Position;
		float		Distance;
	};

public:
	SoundPlayer() {}

	virtual ~SoundPlayer() {}

	virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ) = 0;
	
	virtual void Stop( SoundHandle handle, SoundTag tag ) = 0;

	virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlaying( SoundHandle handle, SoundTag tag ) = 0;

	virtual void StopTag( SoundTag tag ) = 0;

	virtual void PauseTag( SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlayingTag( SoundTag tag ) = 0;

	virtual void StopAll() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUND_PLAYER_H__

#ifndef	__EFFEKSEER_SOUNDLOADER_H__
#define	__EFFEKSEER_SOUNDLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	サウンド読み込み破棄関数指定クラス
*/
class SoundLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	SoundLoader() {}

	/**
		@brief	デストラクタ
	*/
	virtual ~SoundLoader() {}

	/**
		@brief	サウンドを読み込む。
		@param	path	[in]	読み込み元パス
		@return	サウンドのポインタ
		@note
		サウンドを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	サウンドを破棄する。
		@param	data	[in]	サウンド
		@note
		サウンドを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload( void* source ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUNDLOADER_H__

#ifndef	__EFFEKSEER_LOADER_H__
#define	__EFFEKSEER_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	設定クラス
	@note
	EffectLoader等、ファイル読み込みに関する設定することができる。
	Managerの代わりにエフェクト読み込み時に使用することで、Managerとは独立してEffectインスタンスを生成することができる。
*/
	class Setting
		: public ReferenceObject
	{
	private:
		/* 座標系 */
		CoordinateSystem		m_coordinateSystem;

		EffectLoader*	m_effectLoader;
		TextureLoader*	m_textureLoader;
		SoundLoader*	m_soundLoader;
		ModelLoader*	m_modelLoader;

	protected:
		/**
			@brief	コンストラクタ
			*/
		Setting();

		/**
			@brief	デストラクタ
			*/
		 ~Setting();
	public:

		/**
			@brief	設定インスタンスを生成する。
		*/
		static Setting* Create();

		/**
		@brief	座標系を取得する。
		@return	座標系
		*/
		CoordinateSystem GetCoordinateSystem() const;

		/**
		@brief	座標系を設定する。
		@param	coordinateSystem	[in]	座標系
		@note
		座標系を設定する。
		エフェクトファイルを読み込む前に設定する必要がある。
		*/
		void SetCoordinateSystem(CoordinateSystem coordinateSystem);

		/**
			@brief	エフェクトローダーを取得する。
			@return	エフェクトローダー
			*/
		EffectLoader* GetEffectLoader();

		/**
			@brief	エフェクトローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetEffectLoader(EffectLoader* loader);

		/**
			@brief	テクスチャローダーを取得する。
			@return	テクスチャローダー
			*/
		TextureLoader* GetTextureLoader();

		/**
			@brief	テクスチャローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetTextureLoader(TextureLoader* loader);

		/**
			@brief	モデルローダーを取得する。
			@return	モデルローダー
			*/
		ModelLoader* GetModelLoader();

		/**
			@brief	モデルローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetModelLoader(ModelLoader* loader);

		/**
			@brief	サウンドローダーを取得する。
			@return	サウンドローダー
			*/
		SoundLoader* GetSoundLoader();

		/**
			@brief	サウンドローダーを設定する。
			@param	loader	[in]		ローダー
			*/
		void SetSoundLoader(SoundLoader* loader);
	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

#ifndef	__EFFEKSEER_SERVER_H__
#define	__EFFEKSEER_SERVER_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Server
{
public:

	Server() {}
	virtual ~Server() {}

	static Server* Create();

	/**
		@brief	サーバーを開始する。
	*/
	virtual bool Start( uint16_t port ) = 0;

	virtual void Stop() = 0;

	/**
		@brief	エフェクトをリロードの対象として登録する。
		@param	key	[in]	検索用キー
		@param	effect	[in]	リロードする対象のエフェクト
	*/
	virtual void Regist( const EFK_CHAR* key, Effect* effect ) = 0;

	/**
		@brief	エフェクトをリロードの対象から外す。
		@param	effect	[in]	リロードから外すエフェクト
	*/
	virtual void Unregist( Effect* effect ) = 0;

	/**
		@brief	サーバーを更新し、エフェクトのリロードを行う。
	*/
	virtual void Update() = 0;

	/**
		@brief	素材パスを設定する。
	*/
	virtual void SetMaterialPath( const EFK_CHAR* materialPath ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_SERVER_H__

#ifndef	__EFFEKSEER_CLIENT_H__
#define	__EFFEKSEER_CLIENT_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Client
{
public:
	Client() {}
	virtual ~Client() {}

	static Client* Create();

	virtual bool Start( char* host, uint16_t port ) = 0;
	virtual void Stop()= 0;

	virtual void Reload( const EFK_CHAR* key, void* data, int32_t size ) = 0;
	virtual void Reload( Manager* manager, const EFK_CHAR* path, const EFK_CHAR* key ) = 0;
	virtual bool IsConnected() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_CLIENT_H__

#ifndef	__EFFEKSEER_CRITICALSESSION_H__
#define	__EFFEKSEER_CRITICALSESSION_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	クリティカルセクション
*/
class CriticalSection
{
private:
#ifdef _WIN32
	mutable CRITICAL_SECTION m_criticalSection;
#elif defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE)
	mutable CONSOLE_GAME_MUTEX	m_mutex;
#else
	mutable pthread_mutex_t m_mutex;
#endif

public:

	CriticalSection();

	~CriticalSection();

	void Enter() const;

	void Leave() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	//	__EFFEKSEER_CRITICALSESSION_H__

#ifndef	__EFFEKSEER_THREAD_H__
#define	__EFFEKSEER_THREAD_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	
class Thread
{
private:
#ifdef _WIN32
	/* DWORDを置きかえ */
	static unsigned long EFK_STDCALL ThreadProc(void* arguments);
#elif defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE)

#else
	static void* ThreadProc( void* arguments );
#endif

private:
#ifdef _WIN32
	HANDLE m_thread;
#elif defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE)

#else
	pthread_t m_thread;
	bool m_running;
#endif

	void* m_data;
	void (*m_mainProc)( void* );
	CriticalSection m_cs;

public:

	Thread();
	~Thread();


	/**
		@brief スレッドを生成する。
		@param threadFunc	[in] スレッド関数
		@param pData		[in] スレッドに引き渡すデータポインタ
		@return	成否
	*/
	bool Create( void (*threadFunc)( void* ), void* data );

	/**
		@brief スレッド終了を確認する。
	*/
	bool IsExitThread() const;

	/**
		@brief スレッド終了を待つ。
	*/
	bool Wait() const;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__
