
#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

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

class RingRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		eBillboardType		Billboard;
		int32_t				VertexCount;
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

	virtual void LoadRenderer(  const NodeParameter& parameter, void*& userData ) {}

	virtual void RemoveRenderer( const NodeParameter& parameter, void*& userData ) {}

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
