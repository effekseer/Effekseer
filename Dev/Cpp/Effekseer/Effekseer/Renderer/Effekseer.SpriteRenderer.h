
#ifndef __EFFEKSEER_SPRITE_RENDERER_H__
#define __EFFEKSEER_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Color.h"
#include "../Effekseer.Matrix43.h"
#include "../Effekseer.Vector2D.h"
#include "../SIMD/Effekseer.Mat43f.h"
#include "../SIMD/Effekseer.Vec2f.h"
#include "../SIMD/Effekseer.Vec3f.h"

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
