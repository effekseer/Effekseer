#include "Effekseer.EffectNodeTrack.h"
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

void EffectNodeTrack::LoadRendererParameter(unsigned char*& pos, const SettingRef& setting)
{
	eEffectNodeType type = eEffectNodeType::NoneType;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Track\n");

	if (m_effect->GetVersion() >= 15)
	{
		TextureUVType.Load(pos, m_effect->GetVersion());
	}

	LoadValues(TrackSizeFor, pos);
	LoadValues(TrackSizeMiddle, pos);
	LoadValues(TrackSizeBack, pos);

	if (m_effect->GetVersion() >= 13)
	{
		memcpy(&SplineDivision, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}

	if (m_effect->GetVersion() >= Version17Alpha1)
	{
		memcpy(&SmoothingType, pos, sizeof(int32_t));
		pos += sizeof(int32_t);

		memcpy(&TimeType, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}

	TrackColorLeft.load(pos, m_effect->GetVersion());
	TrackColorLeftMiddle.load(pos, m_effect->GetVersion());

	TrackColorCenter.load(pos, m_effect->GetVersion());
	TrackColorCenterMiddle.load(pos, m_effect->GetVersion());

	TrackColorRight.load(pos, m_effect->GetVersion());
	TrackColorRightMiddle.load(pos, m_effect->GetVersion());

	EffekseerPrintDebug("TrackColorLeft : %d\n", TrackColorLeft.type);
	EffekseerPrintDebug("TrackColorLeftMiddle : %d\n", TrackColorLeftMiddle.type);
	EffekseerPrintDebug("TrackColorCenter : %d\n", TrackColorCenter.type);
	EffekseerPrintDebug("TrackColorCenterMiddle : %d\n", TrackColorCenterMiddle.type);
	EffekseerPrintDebug("TrackColorRight : %d\n", TrackColorRight.type);
	EffekseerPrintDebug("TrackColorRightMiddle : %d\n", TrackColorRightMiddle.type);

	// 右手系左手系変換
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
	}

	/* 位置拡大処理 */
	if (m_effect->GetVersion() >= 8)
	{
		TrackSizeFor.fixed.size *= m_effect->GetMaginification();
		TrackSizeMiddle.fixed.size *= m_effect->GetMaginification();
		TrackSizeBack.fixed.size *= m_effect->GetMaginification();
	}
}

void EffectNodeTrack::BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData)
{
	TrackRendererRef renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		m_nodeParameter.ZTest = RendererCommon.ZTest;
		m_nodeParameter.ZWrite = RendererCommon.ZWrite;
		m_nodeParameter.EffectPointer = GetEffect();
		m_nodeParameter.LocalTime = global->GetUpdatedFrame() / 60.0f;

		m_nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;

		m_nodeParameter.SplineDivision = SplineDivision;
		m_nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		m_nodeParameter.TextureUVTypeParameterPtr = &TextureUVType;
		m_nodeParameter.IsRightHand = manager->GetCoordinateSystem() == CoordinateSystem::RH;
		m_nodeParameter.Maginification = GetEffect()->GetMaginification();

		m_nodeParameter.EnableViewOffset = (TranslationParam.TranslationType == ParameterTranslationType_ViewOffset);
		m_nodeParameter.SmoothingType = SmoothingType;
		m_nodeParameter.UserData = GetRenderingUserData();
		renderer->BeginRendering(m_nodeParameter, count, userData);
	}
}

void EffectNodeTrack::BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
	TrackRendererRef renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		m_currentGroupValues = group->rendererValues.track;

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

		renderer->BeginRenderingGroup(m_nodeParameter, group->GetInstanceCount(), userData);
	}
}

void EffectNodeTrack::EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
	TrackRendererRef renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		renderer->EndRenderingGroup(m_nodeParameter, group->GetInstanceCount(), userData);
	}
}

void EffectNodeTrack::Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData)
{
	TrackRendererRef renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		float t = instance.GetNormalizedLivetime();
		int32_t time = (int32_t)instance.m_LivingTime;
		int32_t livedTime = (int32_t)instance.m_LivedTime;

		SetValues(m_instanceParameter.ColorLeft, instance, m_currentGroupValues.ColorLeft, TrackColorLeft, time, livedTime);
		SetValues(m_instanceParameter.ColorCenter, instance, m_currentGroupValues.ColorCenter, TrackColorCenter, time, livedTime);
		SetValues(m_instanceParameter.ColorRight, instance, m_currentGroupValues.ColorRight, TrackColorRight, time, livedTime);

		SetValues(
			m_instanceParameter.ColorLeftMiddle, instance, m_currentGroupValues.ColorLeftMiddle, TrackColorLeftMiddle, time, livedTime);
		SetValues(m_instanceParameter.ColorCenterMiddle,
				  instance,
				  m_currentGroupValues.ColorCenterMiddle,
				  TrackColorCenterMiddle,
				  time,
				  livedTime);
		SetValues(
			m_instanceParameter.ColorRightMiddle, instance, m_currentGroupValues.ColorRightMiddle, TrackColorRightMiddle, time, livedTime);

		SetValues(m_instanceParameter.SizeFor, m_currentGroupValues.SizeFor, TrackSizeFor, t);
		SetValues(m_instanceParameter.SizeMiddle, m_currentGroupValues.SizeMiddle, TrackSizeMiddle, t);
		SetValues(m_instanceParameter.SizeBack, m_currentGroupValues.SizeBack, TrackSizeBack, t);

		m_instanceParameter.SRTMatrix43 = instance.GetRenderedGlobalMatrix();

		m_instanceParameter.InstanceIndex = index;
		renderer->Rendering(m_nodeParameter, m_instanceParameter, userData);
	}
}

void EffectNodeTrack::EndRendering(Manager* manager, void* userData)
{
	TrackRendererRef renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		renderer->EndRendering(m_nodeParameter, userData);
	}
}

void EffectNodeTrack::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceGroupValues& instValues = instanceGroup.rendererValues.track;
	auto instanceGlobal = instanceGroup.GetRootInstance();
	IRandObject* rand = &instanceGlobal->GetRandObject();

	AllTypeColorFunctions::Init(instValues.ColorLeft, *rand, TrackColorLeft);
	AllTypeColorFunctions::Init(instValues.ColorCenter, *rand, TrackColorCenter);
	AllTypeColorFunctions::Init(instValues.ColorRight, *rand, TrackColorRight);

	AllTypeColorFunctions::Init(instValues.ColorLeftMiddle, *rand, TrackColorLeftMiddle);
	AllTypeColorFunctions::Init(instValues.ColorCenterMiddle, *rand, TrackColorCenterMiddle);
	AllTypeColorFunctions::Init(instValues.ColorRightMiddle, *rand, TrackColorRightMiddle);

	InitializeValues(instValues.SizeFor, TrackSizeFor, manager);
	InitializeValues(instValues.SizeBack, TrackSizeBack, manager);
	InitializeValues(instValues.SizeMiddle, TrackSizeMiddle, manager);
}

void EffectNodeTrack::InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	auto& instValues = instanceGroup.rendererValues.track;

	// Calculate only center
	int32_t time = (int32_t)instance.m_LivingTime;
	int32_t livedTime = (int32_t)instance.m_LivedTime;

	Color c;
	SetValues(c, instance, instValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime);

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		c = Color::Mul(c, instance.ColorParent);
	}

	instance.ColorInheritance = c;
}

void EffectNodeTrack::UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	auto& instValues = instanceGroup.rendererValues.track;
	// Calculate only center
	int32_t time = (int32_t)instance.m_LivingTime;
	int32_t livedTime = (int32_t)instance.m_LivedTime;

	Color c;
	SetValues(c, instance, instValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime);

	instance.ColorInheritance = c;
}

void EffectNodeTrack::InitializeValues(InstanceGroupValues::Size& value, TrackSizeParameter& param, Manager* manager)
{
	if (param.type == TrackSizeParameter::Fixed)
	{
		value.size.fixed.size_ = param.fixed.size;
	}
}

void EffectNodeTrack::SetValues(
	Color& c, const Instance& instance, InstanceAllTypeColorState& value, AllTypeColorParameter& param, int32_t time, int32_t livedTime)
{
	c = AllTypeColorFunctions::Calculate(value, param, time, livedTime);

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		c = Color::Mul(c, instance.ColorParent);
	}

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		c.A = (uint8_t)(c.A * fadeAlpha);
	}

	// Apply global Color
	if (instance.m_pContainer->GetRootInstance()->IsGlobalColorSet)
	{
		c = Color::Mul(c, instance.m_pContainer->GetRootInstance()->GlobalColor);
	}
}

void EffectNodeTrack::SetValues(float& s, InstanceGroupValues::Size& value, TrackSizeParameter& param, float time)
{
	if (param.type == TrackSizeParameter::Fixed)
	{
		s = value.size.fixed.size_;
	}
}

void EffectNodeTrack::LoadValues(TrackSizeParameter& param, unsigned char*& pos)
{
	memcpy(&param.type, pos, sizeof(int));
	pos += sizeof(int);

	if (param.type == TrackSizeParameter::Fixed)
	{
		memcpy(&param.fixed, pos, sizeof(param.fixed));
		pos += sizeof(param.fixed);
	}
}

} // namespace Effekseer
