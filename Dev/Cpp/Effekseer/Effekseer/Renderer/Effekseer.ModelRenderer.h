
#ifndef __EFFEKSEER_MODEL_RENDERER_H__
#define __EFFEKSEER_MODEL_RENDERER_H__

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
