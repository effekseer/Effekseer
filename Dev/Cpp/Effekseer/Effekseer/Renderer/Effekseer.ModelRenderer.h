
#ifndef	__EFFEKSEER_MODEL_RENDERER_H__
#define	__EFFEKSEER_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Vector2D.h"
#include "../Effekseer.Vector3D.h"
#include "../Effekseer.Matrix43.h"
#include "../Effekseer.Color.h"

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
