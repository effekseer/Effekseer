

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector3D.h"
#include "SIMD/Effekseer.SIMDUtils.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.InstanceGroup.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Ribbon\n");

	if (m_effect->GetVersion() >= 15)
	{
		TextureUVType.Load(pos, m_effect->GetVersion());
	}

	if (m_effect->GetVersion() >= 3)
	{
		AlphaBlend = RendererCommon.AlphaBlend;
	}
	else
	{
		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);
	}

	memcpy(&ViewpointDependent, pos, sizeof(int));
	pos += sizeof(int);

	memcpy(&RibbonAllColor.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorAllType : %d\n", RibbonAllColor.type);

	if (RibbonAllColor.type == RibbonAllColorParameter::Fixed)
	{
		memcpy(&RibbonAllColor.fixed, pos, sizeof(RibbonAllColor.fixed));
		pos += sizeof(RibbonAllColor.fixed);
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Random)
	{
		RibbonAllColor.random.all.load(m_effect->GetVersion(), pos);
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Easing)
	{
		RibbonAllColor.easing.all.load(m_effect->GetVersion(), pos);
	}

	memcpy(&RibbonColor.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorType : %d\n", RibbonColor.type);

	if (RibbonColor.type == RibbonColor.Default)
	{
	}
	else if (RibbonColor.type == RibbonColor.Fixed)
	{
		memcpy(&RibbonColor.fixed, pos, sizeof(RibbonColor.fixed));
		pos += sizeof(RibbonColor.fixed);
	}

	memcpy(&RibbonPosition.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonPosition : %d\n", RibbonPosition.type);

	if (RibbonPosition.type == RibbonPosition.Default)
	{
		if (m_effect->GetVersion() >= 8)
		{
			memcpy(&RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed));
			pos += sizeof(RibbonPosition.fixed);
			RibbonPosition.type = RibbonPosition.Fixed;
		}
	}
	else if (RibbonPosition.type == RibbonPosition.Fixed)
	{
		memcpy(&RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed));
		pos += sizeof(RibbonPosition.fixed);
	}

	if (m_effect->GetVersion() >= 13)
	{
		memcpy(&SplineDivision, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}

	if (m_effect->GetVersion() >= 3)
	{
		RibbonTexture = RendererCommon.ColorTextureIndex;
	}
	else
	{
		memcpy(&RibbonTexture, pos, sizeof(int));
		pos += sizeof(int);
	}

	// 右手系左手系変換
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
	}

	/* 位置拡大処理 */
	if (m_effect->GetVersion() >= 8)
	{
		if (RibbonPosition.type == RibbonPosition.Default)
		{
		}
		else if (RibbonPosition.type == RibbonPosition.Fixed)
		{
			RibbonPosition.fixed.l *= m_effect->GetMaginification();
			RibbonPosition.fixed.r *= m_effect->GetMaginification();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRendering(int32_t count, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if (renderer != NULL)
	{
		//m_nodeParameter.TextureFilter = RendererCommon.FilterType;
		//m_nodeParameter.TextureWrap = RendererCommon.WrapType;
		m_nodeParameter.ZTest = RendererCommon.ZTest;
		m_nodeParameter.ZWrite = RendererCommon.ZWrite;
		m_nodeParameter.ViewpointDependent = ViewpointDependent != 0;
		m_nodeParameter.EffectPointer = GetEffect();

		m_nodeParameter.SplineDivision = SplineDivision;
		m_nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
		m_nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		m_nodeParameter.TextureUVTypeParameterPtr = &TextureUVType;
		m_nodeParameter.IsRightHand = manager->GetCoordinateSystem() == CoordinateSystem::RH;
		renderer->BeginRendering(m_nodeParameter, count, m_userData);
	}
}

void EffectNodeRibbon::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if (renderer != NULL)
	{
		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;

		if (group->GetFirst() != nullptr)
		{
#ifdef __EFFEKSEER_BUILD_VERSION16__
			m_instanceParameter.UV = group->GetFirst()->GetUV(0);
			m_instanceParameter.AlphaUV = group->GetFirst()->GetUV(1);

			m_instanceParameter.FlipbookIndexAndNextRate = group->GetFirst()->m_flipbookIndexAndNextRate;

			m_instanceParameter.AlphaThreshold = group->GetFirst()->m_AlphaThreshold;
#else
			m_instanceParameter.UV = group->GetFirst()->GetUV();
#endif
			CalcCustomData(group->GetFirst(), m_instanceParameter.CustomData1, m_instanceParameter.CustomData2);
		}

		renderer->BeginRenderingGroup(m_nodeParameter, m_instanceParameter.InstanceCount, m_userData);
	}
}

void EffectNodeRibbon::EndRenderingGroup(InstanceGroup* group, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if (renderer != NULL)
	{
		renderer->EndRenderingGroup(m_nodeParameter, m_instanceParameter.InstanceCount, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::Rendering(const Instance& instance, const Instance* next_instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.ribbon;
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if (renderer != NULL)
	{
		Color _color;
		if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
		{
			_color = Color::Mul(instValues._original, instance.ColorParent);
		}
		else
		{
			_color = instValues._original;
		}

		m_instanceParameter.AllColor = _color;
		m_instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		Color color_l = _color;
		Color color_r = _color;
		Color color_nl = _color;
		Color color_nr = _color;

		if (next_instance != nullptr)
		{
			const InstanceValues& instValues_next = next_instance->rendererValues.ribbon;
			Color _color_next;
			if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
			{
				_color_next = Color::Mul(instValues_next._original, next_instance->ColorParent);
			}
			else
			{
				_color_next = instValues_next._original;
			}

			color_nl = _color_next;
			color_nr = _color_next;
		}

		if (RibbonColor.type == RibbonColorParameter::Default)
		{
		}
		else if (RibbonColor.type == RibbonColorParameter::Fixed)
		{
			color_l = Color::Mul(color_l, RibbonColor.fixed.l);
			color_r = Color::Mul(color_r, RibbonColor.fixed.r);
			color_nl = Color::Mul(color_nl, RibbonColor.fixed.l);
			color_nr = Color::Mul(color_nr, RibbonColor.fixed.r);
		}

		m_instanceParameter.Colors[0] = color_l;
		m_instanceParameter.Colors[1] = color_r;
		m_instanceParameter.Colors[2] = color_nl;
		m_instanceParameter.Colors[3] = color_nr;

		// Apply global Color
		if (instance.m_pContainer->GetRootInstance()->IsGlobalColorSet)
		{
			m_instanceParameter.Colors[0] = Color::Mul(m_instanceParameter.Colors[0], instance.m_pContainer->GetRootInstance()->GlobalColor);
			m_instanceParameter.Colors[1] = Color::Mul(m_instanceParameter.Colors[1], instance.m_pContainer->GetRootInstance()->GlobalColor);
		}

		if (RibbonPosition.type == RibbonPositionParameter::Default)
		{
			m_instanceParameter.Positions[0] = -0.5f;
			m_instanceParameter.Positions[1] = 0.5f;
		}
		else if (RibbonPosition.type == RibbonPositionParameter::Fixed)
		{
			m_instanceParameter.Positions[0] = RibbonPosition.fixed.l;
			m_instanceParameter.Positions[1] = RibbonPosition.fixed.r;
		}

		renderer->Rendering(m_nodeParameter, m_instanceParameter, m_userData);

		m_instanceParameter.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::EndRendering(Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if (renderer != NULL)
	{
		renderer->EndRendering(m_nodeParameter, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;
	auto instanceGlobal = instance.m_pContainer->GetRootInstance();

	if (RibbonAllColor.type == RibbonAllColorParameter::Fixed)
	{
		instValues._original = RibbonAllColor.fixed.all;
		instValues.allColorValues.fixed._color = instValues._original;
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Random)
	{
		instValues._original = RibbonAllColor.random.all.getValue(*instanceGlobal);
		instValues.allColorValues.random._color = instValues._original;
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Easing)
	{
		instValues.allColorValues.easing.start = RibbonAllColor.easing.all.getStartValue(*instanceGlobal);
		instValues.allColorValues.easing.end = RibbonAllColor.easing.all.getEndValue(*instanceGlobal);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = Color::Mul(instValues._original, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._original;
	}

	instance.ColorInheritance = instValues._color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if (RibbonAllColor.type == RibbonAllColorParameter::Fixed)
	{
		instValues._original = instValues.allColorValues.fixed._color;
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Random)
	{
		instValues._original = instValues.allColorValues.random._color;
	}
	else if (RibbonAllColor.type == RibbonAllColorParameter::Easing)
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		RibbonAllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t);
	}

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		instValues._original.A = (uint8_t)(instValues._original.A * fadeAlpha);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = Color::Mul(instValues._original, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._original;
	}

	instance.ColorInheritance = instValues._color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
