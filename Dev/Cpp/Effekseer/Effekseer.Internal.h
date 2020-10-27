#pragma once
#include "Effekseer.h"
#include "Effekseer/SIMD/Effekseer.Mat43f.h"
#include "Effekseer/SIMD/Effekseer.Mat44f.h"
#include "Effekseer/SIMD/Effekseer.SIMDUtils.h"
#include "Effekseer/SIMD/Effekseer.Vec2f.h"
#include "Effekseer/SIMD/Effekseer.Vec3f.h"
#include "Effekseer/SIMD/Effekseer.Vec4f.h"

// A header to access internal data of effekseer

#ifndef __EFFEKSEER_PARAMETERS_H__
#define __EFFEKSEER_PARAMETERS_H__

namespace Effekseer
{

/**
	@brief
	\~English	How to treat an uv of texture type for ribbon and track
	\~Japanese	リボンと軌跡向けのテクスチャのUVの扱い方
*/
enum class TextureUVType : int32_t
{
	Strech,
	Tile,
};

/**
	@brief	\~english	uv texture parameters which is passed into a renderer
			\~japanese	レンダラーに渡されるUVTextureに関するパラメーター
*/
struct NodeRendererTextureUVTypeParameter
{
	TextureUVType Type = TextureUVType::Strech;
	float TileLength = 0.0f;
	int32_t TileEdgeHead = 0;
	int32_t TileEdgeTail = 0;
	float TileLoopAreaBegin = 0.0f;
	float TileLoopAreaEnd = 0.0f;

	void Load(uint8_t*& pos, int32_t version);
};

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETERS_H__
#ifndef __EFFEKSEER_SPRITE_RENDERER_H__
#define __EFFEKSEER_SPRITE_RENDERER_H__

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
		Effect* EffectPointer;
		//int32_t				ColorTextureIndex;
		//AlphaBlendType			AlphaBlend;
		//TextureFilterType	TextureFilter;
		//TextureWrapType	TextureWrap;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;
		bool IsRightHand;

		//bool				Distortion;
		//float				DistortionIntensity;

		//float				DepthOffset;
		//bool				IsDepthOffsetScaledWithCamera;
		//bool				IsDepthOffsetScaledWithParticleScale;

		ZSortType ZSort;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		//RendererMaterialType MaterialType = RendererMaterialType::Default;
		//MaterialParameter* MaterialParameterPtr = nullptr;
	};

	struct InstanceParameter
	{
		Mat43f SRTMatrix43;
		Color AllColor;

		// Lower left, Lower right, Upper left, Upper right
		Color Colors[4];

		Vec2f Positions[4];

		RectF UV;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		RectF AlphaUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;
#endif

		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	SpriteRenderer()
	{
	}

	virtual ~SpriteRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_SPRITE_RENDERER_H__

#ifndef __EFFEKSEER_RIBBON_RENDERER_H__
#define __EFFEKSEER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

struct NodeRendererTextureUVTypeParameter;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RibbonRenderer
{
public:
	struct NodeParameter
	{
		Effect* EffectPointer;
		//int32_t				ColorTextureIndex;
		//AlphaBlendType			AlphaBlend;
		//TextureFilterType	TextureFilter;
		//TextureWrapType	TextureWrap;
		bool ZTest;
		bool ZWrite;
		bool ViewpointDependent;

		//bool				Distortion;
		//float				DistortionIntensity;

		bool IsRightHand;
		int32_t SplineDivision;
		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;
		NodeRendererTextureUVTypeParameter* TextureUVTypeParameterPtr = nullptr;
		//RendererMaterialType MaterialType = RendererMaterialType::Default;
		//MaterialParameter* MaterialParameterPtr = nullptr;
	};

	struct InstanceParameter
	{
		int32_t InstanceCount;
		int32_t InstanceIndex;
		Mat43f SRTMatrix43;
		Color AllColor;

		// Lower left, Lower right, Upper left, Upper right
		Color Colors[4];

		float Positions[4];

		RectF UV;
#ifdef __EFFEKSEER_BUILD_VERSION16__
		RectF AlphaUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;
#endif
		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	RibbonRenderer()
	{
	}

	virtual ~RibbonRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}

	virtual void BeginRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void EndRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_RIBBON_RENDERER_H__
#ifndef __EFFEKSEER_RING_RENDERER_H__
#define __EFFEKSEER_RING_RENDERER_H__

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
		Effect* EffectPointer;
		//int32_t				ColorTextureIndex;
		//AlphaBlendType			AlphaBlend;
		//TextureFilterType	TextureFilter;
		//TextureWrapType	TextureWrap;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;
		int32_t VertexCount;
		bool IsRightHand;
		float StartingFade = 0.0f;
		float EndingFade = 0.0f;
		//bool				Distortion;
		//float				DistortionIntensity;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		//RendererMaterialType MaterialType = RendererMaterialType::Default;
		//MaterialParameter* MaterialParameterPtr = nullptr;

		//float				DepthOffset;
		//bool				IsDepthOffsetScaledWithCamera;
		//bool				IsDepthOffsetScaledWithParticleScale;

		NodeRendererBasicParameter BasicParameter;
	};

	struct InstanceParameter
	{
		Mat43f SRTMatrix43;
		Vec2f OuterLocation;
		Vec2f InnerLocation;
		float ViewingAngleStart;
		float ViewingAngleEnd;
		float CenterRatio;
		Color OuterColor;
		Color CenterColor;
		Color InnerColor;

		RectF UV;
#ifdef __EFFEKSEER_BUILD_VERSION16__
		RectF AlphaUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;
#endif
		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	RingRenderer()
	{
	}

	virtual ~RingRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_RING_RENDERER_H__

#ifndef __EFFEKSEER_MODEL_RENDERER_H__
#define __EFFEKSEER_MODEL_RENDERER_H__

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
		Effect* EffectPointer;
		//AlphaBlendType		AlphaBlend;
		//TextureFilterType	TextureFilter;
		//TextureWrapType	TextureWrap;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;

		//bool				Lighting;
		CullingType Culling;
		int32_t ModelIndex;
		//int32_t				ColorTextureIndex;
		//int32_t				NormalTextureIndex;
		float Magnification;
		bool IsRightHand;

		//bool				Distortion;
		//float				DistortionIntensity;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		//RendererMaterialType MaterialType = RendererMaterialType::Default;
		//MaterialParameter* MaterialParameterPtr = nullptr;

		//float				DepthOffset;
		//bool				IsDepthOffsetScaledWithCamera;
		//bool				IsDepthOffsetScaledWithParticleScale;
	};

	struct InstanceParameter
	{
		Mat43f SRTMatrix43;
		RectF UV;
#ifdef __EFFEKSEER_BUILD_VERSION16__
		RectF AlphaUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;
#endif
		Color AllColor;
		int32_t Time;
		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	ModelRenderer()
	{
	}

	virtual ~ModelRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODEL_RENDERER_H__

#ifndef __EFFEKSEER_TRACK_RENDERER_H__
#define __EFFEKSEER_TRACK_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

struct NodeRendererTextureUVTypeParameter;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class TrackRenderer
{
public:
	struct NodeParameter
	{
		Effect* EffectPointer;
		//int32_t				ColorTextureIndex;
		//AlphaBlendType			AlphaBlend;
		//TextureFilterType	TextureFilter;
		//TextureWrapType		TextureWrap;
		bool ZTest;
		bool ZWrite;

		//bool				Distortion;
		//float				DistortionIntensity;

		int32_t SplineDivision;

		bool IsRightHand;
		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;
		NodeRendererTextureUVTypeParameter* TextureUVTypeParameterPtr = nullptr;

		RendererMaterialType MaterialType = RendererMaterialType::Default;
		MaterialParameter* MaterialParameterPtr = nullptr;
	};

	struct InstanceGroupParameter
	{
	};

	struct InstanceParameter
	{
		int32_t InstanceCount;
		int32_t InstanceIndex;
		Mat43f SRTMatrix43;

		Color ColorLeft;
		Color ColorCenter;
		Color ColorRight;

		Color ColorLeftMiddle;
		Color ColorCenterMiddle;
		Color ColorRightMiddle;

		float SizeFor;
		float SizeMiddle;
		float SizeBack;

		RectF UV;
#ifdef __EFFEKSEER_BUILD_VERSION16__
		RectF AlphaUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;
#endif
		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	TrackRenderer()
	{
	}

	virtual ~TrackRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}

	virtual void BeginRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void EndRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_TRACK_RENDERER_H__