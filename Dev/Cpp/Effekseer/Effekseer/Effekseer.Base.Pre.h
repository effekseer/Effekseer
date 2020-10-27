
#ifndef __EFFEKSEER_BASE_PRE_H__
#define __EFFEKSEER_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <array>
#include <atomic>
#include <cfloat>
#include <climits>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
#define EFK_STDCALL __stdcall
#else
#define EFK_STDCALL
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#ifdef _WIN32
//#include <windows.h>
#elif defined(_PSVITA)
#include "Effekseer.PSVita.h"
#elif defined(_PS4)
#include "Effekseer.PS4.h"
#elif defined(_SWITCH)
#include "Effekseer.Switch.h"
#elif defined(_XBOXONE)
#include "Effekseer.XBoxOne.h"
#else
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef char16_t EFK_CHAR;

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
class MaterialLoader;

class SoundPlayer;
class SoundLoader;

class ModelLoader;

class Model;

typedef int Handle;

/**
	@brief	Memory Allocation function
*/
typedef void*(EFK_STDCALL* MallocFunc)(unsigned int size);

/**
	@brief	Memory Free function
*/
typedef void(EFK_STDCALL* FreeFunc)(void* p, unsigned int size);

/**
	@brief	AlignedMemory Allocation function
*/
typedef void*(EFK_STDCALL* AlignedMallocFunc)(unsigned int size, unsigned int alignment);

/**
	@brief	AlignedMemory Free function
*/
typedef void(EFK_STDCALL* AlignedFreeFunc)(void* p, unsigned int size);

/**
	@brief	Random Function
*/
typedef int(EFK_STDCALL* RandFunc)(void);

/**
	@brief	エフェクトのインスタンス破棄時のコールバックイベント
	@param	manager	[in]	所属しているマネージャー
	@param	handle	[in]	エフェクトのインスタンスのハンドル
	@param	isRemovingManager	[in]	マネージャーを破棄したときにエフェクトのインスタンスを破棄しているか
*/
typedef void(EFK_STDCALL* EffectInstanceRemovingCallback)(Manager* manager, Handle handle, bool isRemovingManager);

#define ES_SAFE_ADDREF(val) \
	if ((val) != NULL)      \
	{                       \
		(val)->AddRef();    \
	}
#define ES_SAFE_RELEASE(val) \
	if ((val) != NULL)       \
	{                        \
		(val)->Release();    \
		(val) = NULL;        \
	}
#define ES_SAFE_DELETE(val) \
	if ((val) != NULL)      \
	{                       \
		delete (val);       \
		(val) = NULL;       \
	}
#define ES_SAFE_DELETE_ARRAY(val) \
	if ((val) != NULL)            \
	{                             \
		delete[](val);            \
		(val) = NULL;             \
	}

#define EFK_ASSERT(x) assert(x)

//! the maximum number of texture slot which can be specified by an user
const int32_t UserTextureSlotMax = 6;

//! the maximum number of texture slot including textures system specified
const int32_t TextureSlotMax = 8;

const int32_t LocalFieldSlotMax = 4;

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

enum class MaterialFileType : int32_t
{
	Code,
	Compiled,
};

enum class TextureFormatType : int32_t
{
	ABGR8,
	BC1,
	BC2,
	BC3,
};

enum class ZSortType : int32_t
{
	None,
	NormalOrder,
	ReverseOrder,
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
enum class RenderMode : int32_t
{
	Normal,	   // 通常描画
	Wireframe, // ワイヤーフレーム描画
};

/**
	@brief
	\~English	A thread where reload function is called
	\~Japanese	リロードの関数が呼ばれるスレッド
*/
enum class ReloadingThreadType
{
	Main,
	Render,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	最大値取得
*/
template <typename T, typename U>
T Max(T t, U u)
{
	if (t > (T)u)
	{
		return t;
	}
	return u;
}

/**
	@brief	最小値取得
*/
template <typename T, typename U>
T Min(T t, U u)
{
	if (t < (T)u)
	{
		return t;
	}
	return u;
}

/**
	@brief	範囲内値取得
*/
template <typename T, typename U, typename V>
T Clamp(T t, U max_, V min_)
{
	if (t > (T)max_)
	{
		t = (T)max_;
	}

	if (t < (T)min_)
	{
		t = (T)min_;
	}

	return t;
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
inline int32_t ConvertUtf16ToUtf8(int8_t* dst, int32_t dst_size, const int16_t* src)
{
	int32_t cnt = 0;
	const int16_t* wp = src;
	int8_t* cp = dst;

	if (dst_size == 0)
		return 0;

	dst_size -= 3;

	for (cnt = 0; cnt < dst_size;)
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
		}
		else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >> 6) & 0x1f) | 0xc0;
			*cp++ = ((wc)&0x3f) | 0x80;
			cnt += 2;
		}
		else
		{
			*cp++ = ((wc >> 12) & 0xf) | 0xe0;
			*cp++ = ((wc >> 6) & 0x3f) | 0x80;
			*cp++ = ((wc)&0x3f) | 0x80;
			cnt += 3;
		}
	}
	*cp = '\0';
	return cnt;
}

/**
	@brief    Convert UTF8 into UTF16
	@param    dst    a pointer to destination buffer
	@param    dst_size    a length of destination buffer
	@param    src            a source buffer
	@return    length except 0
*/
inline int32_t ConvertUtf8ToUtf16(char16_t* dst, int32_t dst_size, const char* src)
{
	int32_t i, code = 0;
	int8_t c0, c1, c2 = 0;
	int8_t* srci = reinterpret_cast<int8_t*>(const_cast<char*>(src));
	if (dst_size == 0)
		return 0;

	dst_size -= 1;

	for (i = 0; i < dst_size; i++)
	{
		uint16_t wc;

		c0 = *srci;
		srci++;
		if (c0 == '\0')
		{
			break;
		}
		// convert UTF8 to UTF16
		code = (uint8_t)c0 >> 4;
		if (code <= 7)
		{
			// 8bit character
			wc = c0;
		}
		else if (code >= 12 && code <= 13)
		{
			// 16bit  character
			c1 = *srci;
			srci++;
			wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
		}
		else if (code == 14)
		{
			// 24bit character
			c1 = *srci;
			srci++;
			c2 = *srci;
			srci++;
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

/**
	@brief	文字コードを変換する。(UTF8 -> UTF16)
	@param	dst	[out]	出力配列の先頭ポインタ
	@param	dst_size	[in]	出力配列の長さ
	@param	src			[in]	入力配列の先頭ポインタ
	@return	文字数
*/
inline int32_t ConvertUtf8ToUtf16(int16_t* dst, int32_t dst_size, const int8_t* src)
{
	int32_t i, code;
	int8_t c0, c1, c2;

	if (dst_size == 0)
		return 0;

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

/**
	@brief	a deleter for IReference
*/
template <typename T>
struct ReferenceDeleter
{
	void operator()(T* ptr) const
	{
		if (ptr != nullptr)
		{
			ptr->Release();
		}
	}
};

template <typename T>
inline std::unique_ptr<T, ReferenceDeleter<T>> CreateUniqueReference(T* ptr, bool addRef = false)
{
	if (ptr == nullptr)
		return std::unique_ptr<T, ReferenceDeleter<T>>(nullptr);

	if (addRef)
	{
		ptr->AddRef();
	}

	return std::unique_ptr<T, ReferenceDeleter<T>>(ptr);
}

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
	{
	}

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

/**
	@brief	This object generates random values.
*/
class IRandObject
{
public:
	IRandObject() = default;
	virtual ~IRandObject() = default;

	virtual float GetRand() = 0;

	virtual float GetRand(float min_, float max_) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

enum class ColorSpaceType : int32_t
{
	Gamma,
	Linear,
};

/**
	@brief	\~english	Texture data
			\~japanese	テクスチャデータ
*/
struct TextureData
{
	int32_t Width;
	int32_t Height;
	TextureFormatType TextureFormat;
	void* UserPtr;
	int64_t UserID;

	//! for OpenGL, it is ignored in other apis
	bool HasMipmap = true;
};

enum class ShadingModelType : int32_t
{
	Lit,
	Unlit,
};

/**
	@brief	material type
*/
enum class RendererMaterialType : int32_t
{
	Default = 0,
	BackDistortion = 6,
	Lighting = 7,
	File = 128,
};

/**
	@brief	\~english	Material data
			\~japanese	マテリアルデータ
*/
class MaterialData
{
public:
	ShadingModelType ShadingModel = ShadingModelType::Lit;
	bool IsSimpleVertex = false;
	bool IsRefractionRequired = false;
	int32_t CustomData1 = 0;
	int32_t CustomData2 = 0;
	int32_t TextureCount = 0;
	int32_t UniformCount = 0;
	std::array<TextureWrapType, UserTextureSlotMax> TextureWrapTypes;
	void* UserPtr = nullptr;
	void* ModelUserPtr = nullptr;
	void* RefractionUserPtr = nullptr;
	void* RefractionModelUserPtr = nullptr;

	MaterialData() = default;
	virtual ~MaterialData() = default;
};

/**
	@brief	\~english	Textures used by material
			\~japanese	マテリアルに使用されるテクスチャ
*/
struct MaterialTextureParameter
{
	//! 0 - color, 1 - value
	int32_t Type = 0;
	int32_t Index = 0;
};

/**
	@brief	\~english	Material parameter for shaders
			\~japanese	シェーダー向けマテリアルパラメーター
*/
struct MaterialParameter
{
	//! material index in MaterialType::File
	int32_t MaterialIndex = -1;

	//! used textures in MaterialType::File
	std::vector<MaterialTextureParameter> MaterialTextures;

	//! used uniforms in MaterialType::File
	std::vector<std::array<float, 4>> MaterialUniforms;
};

/**
	@brief	\~english	Parameters about a depth which is passed into a renderer
			\~japanese	レンダラーに渡されるデプスに関するパラメーター
*/
struct NodeRendererDepthParameter
{
	float DepthOffset = 0.0f;
	bool IsDepthOffsetScaledWithCamera = false;
	bool IsDepthOffsetScaledWithParticleScale = false;
	ZSortType ZSort = ZSortType::None;
	float SuppressionOfScalingByDepth = 1.0f;
	float DepthClipping = FLT_MAX;
};

/**
	@brief	\~english	Common parameters which is passed into a renderer
			\~japanese	レンダラーに渡される共通に関するパラメーター
*/
struct NodeRendererBasicParameter
{
	RendererMaterialType MaterialType = RendererMaterialType::Default;
	int32_t Texture1Index = -1;
	int32_t Texture2Index = -1;
#ifdef __EFFEKSEER_BUILD_VERSION16__
	int32_t Texture3Index = -1;
#endif
	float DistortionIntensity = 0.0f;
	MaterialParameter* MaterialParameterPtr = nullptr;
	AlphaBlendType AlphaBlend = AlphaBlendType::Blend;

	TextureFilterType TextureFilter1 = TextureFilterType::Nearest;
	TextureWrapType TextureWrap1 = TextureWrapType::Repeat;
	TextureFilterType TextureFilter2 = TextureFilterType::Nearest;
	TextureWrapType TextureWrap2 = TextureWrapType::Repeat;
#ifdef __EFFEKSEER_BUILD_VERSION16__
	TextureFilterType TextureFilter3 = TextureFilterType::Nearest;
	TextureWrapType TextureWrap3 = TextureWrapType::Repeat;

	bool EnableInterpolation = false;
	int32_t UVLoopType = 0;
	int32_t InterpolationType = 0;
	int32_t FlipbookDivideX = 1;
	int32_t FlipbookDivideY = 1;
#endif
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_BASE_PRE_H__