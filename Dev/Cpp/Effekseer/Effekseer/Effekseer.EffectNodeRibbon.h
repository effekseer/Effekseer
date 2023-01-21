
#ifndef __EFFEKSEER_ParameterNODE_RIBBON_H__
#define __EFFEKSEER_ParameterNODE_RIBBON_H__

#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.RibbonRenderer.h"

namespace Effekseer
{

struct RibbonColorParameter
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{

		} def;

		struct
		{
			Color l;
			Color r;
		} fixed;
	};
};

struct RibbonPositionParameter
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{

		} def;

		struct
		{
			float l;
			float r;
		} fixed;
	};
};

class EffectNodeRibbon : public EffectNodeImplemented
{
public:
	struct InstanceValues
	{
		Color _color;
		Color _original;

		InstanceAllTypeColorState allColorValues;

		union
		{

		} colorValues;

		union
		{

		} positionValues;
	};

	RibbonRenderer::NodeParameter m_nodeParameter;
	RibbonRenderer::InstanceParameter m_instanceParameter;

public:
	int ViewpointDependent;

	AllTypeColorParameter RibbonAllColor;

	RibbonColorParameter RibbonColor;
	RibbonPositionParameter RibbonPosition;

	int32_t SplineDivision = 1;

	TrailTimeType TimeType = TrailTimeType::FirstParticle;

	NodeRendererTextureUVTypeParameter TextureUVType;

	EffectNodeRibbon(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeRibbon() = default;

	void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting) override;

	void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData) override;

	void BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData) override;

	void EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData) override;

	void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData) override;

	void EndRendering(Manager* manager, void* userData) override;

	void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	EffectNodeType GetType() const override
	{
		return EffectNodeType::Ribbon;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_ParameterNODE_RIBBON_H__
