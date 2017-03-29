
#ifndef	__EFFEKSEER_ParameterNODE_TRACK_H__
#define	__EFFEKSEER_ParameterNODE_TRACK_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.TrackRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct TrackSizeParameter
{
	enum
	{
		Fixed = 0,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			float size;
		} fixed;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeTrack
	: public EffectNodeImplemented
{
public:

	struct InstanceGroupValues
	{
		struct Color
		{
			union 
			{
				struct
				{
					color color_;
				} fixed;

				struct
				{
					color color_;
				} random;

				struct
				{
					color start;
					color  end;
				} easing;

				struct
				{
					float	offset[4];
				} fcurve_rgba;

			} color;
		};

		struct Size
		{
			union
			{
				struct
				{
					float	size_;
				} fixed;
			} size;
		};

		Color	ColorLeft;
		Color	ColorCenter;
		Color	ColorRight;

		Color	ColorLeftMiddle;
		Color	ColorCenterMiddle;
		Color	ColorRightMiddle;

		Size	SizeFor;
		Size	SizeMiddle;
		Size	SizeBack;

	};

	struct InstanceValues
	{
		color	colorLeft;
		color	colorCenter;
		color	colorRight;

		color	colorLeftMiddle;
		color	colorCenterMiddle;
		color	colorRightMiddle;

		color	_colorLeft;
		color	_colorCenter;
		color	_colorRight;

		color	_colorLeftMiddle;
		color	_colorCenterMiddle;
		color	_colorRightMiddle;

		float	SizeFor;
		float	SizeMiddle;
		float	SizeBack;
	};

	TrackRenderer::NodeParameter	m_nodeParameter;
	TrackRenderer::InstanceParameter m_instanceParameter;

	InstanceGroupValues		m_currentGroupValues;

public:

	AlphaBlendType		AlphaBlend;

	StandardColorParameter	TrackColorLeft;
	StandardColorParameter	TrackColorCenter;
	StandardColorParameter	TrackColorRight;

	StandardColorParameter	TrackColorLeftMiddle;
	StandardColorParameter	TrackColorCenterMiddle;
	StandardColorParameter	TrackColorRightMiddle;
	
	TrackSizeParameter	TrackSizeFor;
	TrackSizeParameter	TrackSizeMiddle;
	TrackSizeParameter	TrackSizeBack;

	int TrackTexture;

	EffectNodeTrack( Effect* effect, unsigned char*& pos )
		: EffectNodeImplemented(effect, pos)
		, TrackTexture	( -1 )
	{
	}

	~EffectNodeTrack()
	{
	}

	void LoadRendererParameter(unsigned char*& pos, Setting* setting);

	void BeginRendering(int32_t count, Manager* manager);

	void BeginRenderingGroup(InstanceGroup* group, Manager* manager);

	void Rendering(const Instance& instance, Manager* manager);

	void EndRendering(Manager* manager);

	void InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager);

	void InitializeRenderedInstance(Instance& instance, Manager* manager);

	void UpdateRenderedInstance(Instance& instance, Manager* manager);

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_TRACK; }

	void InitializeValues(InstanceGroupValues::Color& value, StandardColorParameter& param, Manager* manager);
	void InitializeValues(InstanceGroupValues::Size& value, TrackSizeParameter& param, Manager* manager);
	void SetValues(Color& c, const Instance& instance, InstanceGroupValues::Color& value, StandardColorParameter& param, int32_t time, int32_t livedTime);
	void SetValues( float& s, InstanceGroupValues::Size& value, TrackSizeParameter& param, float time );
	void LoadValues( TrackSizeParameter& param, unsigned char*& pos );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_ParameterNODE_TRACK_H__
