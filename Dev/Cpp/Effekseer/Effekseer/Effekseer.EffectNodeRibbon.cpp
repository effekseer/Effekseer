#include "Effekseer.EffectNodeRibbon.h"

#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector3D.h"
#include "SIMD/Utils.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

#include "Effekseer.InstanceGroup.h"

#include "Effekseer.Setting.h"

namespace Effekseer
{

void EffectNodeRibbon::LoadRendererParameter(unsigned char*& pos, const SettingRef& setting)
{
	EffectNodeType type = EffectNodeType::NoneType;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Ribbon\n");

	if (m_effect->GetVersion() >= 15)
	{
		TextureUVType.Load(pos, m_effect->GetVersion());
	}

	if (m_effect->GetVersion() >= Version17Alpha1)
	{
		memcpy(&TimeType, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}

	if (m_effect->GetVersion() >= 3)
	{
	}
	else
	{
		int32_t AlphaBlend = 0;
		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);
	}

	memcpy(&ViewpointDependent, pos, sizeof(int));
	pos += sizeof(int);

	RibbonAllColor.load(pos, m_effect->GetVersion());

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
	}
	else
	{
		int RibbonTexture = 0;
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

void EffectNodeRibbon::BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData)
{
	RibbonRendererRef renderer = manager->GetRibbonRenderer();
	if (renderer != nullptr)
	{
		m_nodeParameter.ZTest = RendererCommon.ZTest;
		m_nodeParameter.ZWrite = RendererCommon.ZWrite;
		m_nodeParameter.ViewpointDependent = ViewpointDependent != 0;
		m_nodeParameter.EffectPointer = GetEffect();
		m_nodeParameter.LocalTime = global->GetUpdatedFrame() / 60.0f;

		m_nodeParameter.SplineDivision = SplineDivision;
		m_nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
		m_nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		m_nodeParameter.TextureUVTypeParameterPtr = &TextureUVType;
		m_nodeParameter.IsRightHand = manager->GetCoordinateSystem() == CoordinateSystem::RH;
		m_nodeParameter.Maginification = GetEffect()->GetMaginification();

		m_nodeParameter.EnableViewOffset = (TranslationParam.TranslationType == ParameterTranslationType_ViewOffset);
		m_nodeParameter.UserData = GetRenderingUserData();

		renderer->BeginRendering(m_nodeParameter, count, userData);
	}
}

void EffectNodeRibbon::BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
	RibbonRendererRef renderer = manager->GetRibbonRenderer();
	if (renderer != nullptr)
	{
		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;

		if (group->GetFirst() != nullptr)
		{
			Instance* groupFirst = group->GetFirst();

			int livingTime{};
			int livedTime{};

			if (TimeType == TrailTimeType::FirstParticle)
			{
				livingTime = groupFirst->m_LivingTime;
				livedTime = groupFirst->m_LivedTime;
			}
			else if (TimeType == TrailTimeType::ParticleGroup)
			{
				livingTime = group->GetTime();
				livedTime = 100;
			}
			else
			{
				assert(false);
			}

			m_instanceParameter.UV = groupFirst->GetUV(0, livingTime, livedTime);
			m_instanceParameter.AlphaUV = groupFirst->GetUV(1, livingTime, livedTime);
			m_instanceParameter.UVDistortionUV = groupFirst->GetUV(2, livingTime, livedTime);
			m_instanceParameter.BlendUV = groupFirst->GetUV(3, livingTime, livedTime);
			m_instanceParameter.BlendAlphaUV = groupFirst->GetUV(4, livingTime, livedTime);
			m_instanceParameter.BlendUVDistortionUV = groupFirst->GetUV(5, livingTime, livedTime);

			m_instanceParameter.FlipbookIndexAndNextRate = groupFirst->GetFlipbookIndexAndNextRate();

			m_instanceParameter.AlphaThreshold = groupFirst->m_AlphaThreshold;

			if (m_nodeParameter.EnableViewOffset)
			{
				m_instanceParameter.ViewOffsetDistance = groupFirst->translation_values.view_offset.distance;
			}

			CalcCustomData(group->GetFirst(), m_instanceParameter.CustomData1, m_instanceParameter.CustomData2);
		}

		renderer->BeginRenderingGroup(m_nodeParameter, m_instanceParameter.InstanceCount, userData);
	}
}

void EffectNodeRibbon::EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
	RibbonRendererRef renderer = manager->GetRibbonRenderer();
	if (renderer != nullptr)
	{
		renderer->EndRenderingGroup(m_nodeParameter, m_instanceParameter.InstanceCount, userData);
	}
}

void EffectNodeRibbon::Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData)
{
	const InstanceValues& instValues = instance.rendererValues.ribbon;
	RibbonRendererRef renderer = manager->GetRibbonRenderer();
	if (renderer != nullptr)
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
		m_instanceParameter.SRTMatrix43 = instance.GetRenderedGlobalMatrix();

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
			m_instanceParameter.Colors[0] =
				Color::Mul(m_instanceParameter.Colors[0], instance.m_pContainer->GetRootInstance()->GlobalColor);
			m_instanceParameter.Colors[1] =
				Color::Mul(m_instanceParameter.Colors[1], instance.m_pContainer->GetRootInstance()->GlobalColor);
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

		m_instanceParameter.InstanceIndex = index;
		renderer->Rendering(m_nodeParameter, m_instanceParameter, userData);
	}
}

void EffectNodeRibbon::EndRendering(Manager* manager, void* userData)
{
	RibbonRendererRef renderer = manager->GetRibbonRenderer();
	if (renderer != nullptr)
	{
		renderer->EndRendering(m_nodeParameter, userData);
	}
}

void EffectNodeRibbon::InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;
	IRandObject& rand = instance.GetRandObject();

	AllTypeColorFunctions::Init(instValues.allColorValues, rand, RibbonAllColor);
	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, RibbonAllColor, instance.m_LivingTime, instance.m_LivedTime);

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

void EffectNodeRibbon::UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, RibbonAllColor, instance.m_LivingTime, instance.m_LivedTime);

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

} // namespace Effekseer
