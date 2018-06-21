
#ifndef	__EFFEKSEER_RIBBON_RENDERER_H__
#define	__EFFEKSEER_RIBBON_RENDERER_H__

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

			int32_t				SplineDivision;
		};

		struct InstanceParameter
		{
			int32_t			InstanceCount;
			int32_t			InstanceIndex;
			Matrix43		SRTMatrix43;
			Color		AllColor;

			// Lower left, Lower right, Upper left, Upper right
			Color	Colors[4];

			float	Positions[4];

			RectF	UV;
		};

	public:
		RibbonRenderer() {}

		virtual ~RibbonRenderer() {}

		virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData) {}

		virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData) {}

		virtual void EndRendering(const NodeParameter& parameter, void* userData) {}

		virtual void BeginRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData) {}

		virtual void EndRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData) {}
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RIBBON_RENDERER_H__