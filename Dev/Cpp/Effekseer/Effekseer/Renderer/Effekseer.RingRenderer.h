
#ifndef __EFFEKSEER_RING_RENDERER_H__
#define __EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Color.h"
#include "../Effekseer.Matrix43.h"
#include "../Effekseer.Vector2D.h"
#include "../Effekseer.Vector3D.h"
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
