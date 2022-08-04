
#ifndef __EFFEKSEER_ParameterNODE_TRACK_H__
#define __EFFEKSEER_ParameterNODE_TRACK_H__

#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.TrackRenderer.h"

namespace Effekseer
{

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

class EffectNodeTrack : public EffectNodeImplemented
{
public:
	struct InstanceGroupValues
	{
		struct Size
		{
			union
			{
				struct
				{
					float size_;
				} fixed;
			} size;
		};

		InstanceAllTypeColorState ColorLeft;
		InstanceAllTypeColorState ColorCenter;
		InstanceAllTypeColorState ColorRight;

		InstanceAllTypeColorState ColorLeftMiddle;
		InstanceAllTypeColorState ColorCenterMiddle;
		InstanceAllTypeColorState ColorRightMiddle;

		Size SizeFor;
		Size SizeMiddle;
		Size SizeBack;
	};

	struct InstanceValues
	{
		Color colorLeft;
		Color colorCenter;
		Color colorRight;

		Color colorLeftMiddle;
		Color colorCenterMiddle;
		Color colorRightMiddle;

		Color _colorLeft;
		Color _colorCenter;
		Color _colorRight;

		Color _colorLeftMiddle;
		Color _colorCenterMiddle;
		Color _colorRightMiddle;

		float SizeFor;
		float SizeMiddle;
		float SizeBack;
	};

	TrackRenderer::NodeParameter m_nodeParameter;
	TrackRenderer::InstanceParameter m_instanceParameter;

	InstanceGroupValues m_currentGroupValues;

public:
	AllTypeColorParameter TrackColorLeft;
	AllTypeColorParameter TrackColorCenter;
	AllTypeColorParameter TrackColorRight;

	AllTypeColorParameter TrackColorLeftMiddle;
	AllTypeColorParameter TrackColorCenterMiddle;
	AllTypeColorParameter TrackColorRightMiddle;

	TrackSizeParameter TrackSizeFor;
	TrackSizeParameter TrackSizeMiddle;
	TrackSizeParameter TrackSizeBack;

	int32_t SplineDivision = 1;

	NodeRendererTextureUVTypeParameter TextureUVType;

	TrailSmoothingType SmoothingType = TrailSmoothingType::Off;
	TrailTimeType TimeType = TrailTimeType::FirstParticle;

	EffectNodeTrack(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeTrack() = default;

	void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting) override;

	void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData) override;

	void BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData) override;

	void EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData) override;

	void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData) override;

	void EndRendering(Manager* manager, void* userData) override;

	void InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager) override;

	void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	eEffectNodeType GetType() const override
	{
		return eEffectNodeType::Track;
	}

	void InitializeValues(InstanceGroupValues::Size& value, TrackSizeParameter& param, Manager* manager);
	void SetValues(Color& c,
				   const Instance& instance,
				   InstanceAllTypeColorState& value,
				   AllTypeColorParameter& param,
				   int32_t time,
				   int32_t livedTime);
	void SetValues(float& s, InstanceGroupValues::Size& value, TrackSizeParameter& param, float time);
	void LoadValues(TrackSizeParameter& param, unsigned char*& pos);
};

} // namespace Effekseer

#endif // __EFFEKSEER_ParameterNODE_TRACK_H__
