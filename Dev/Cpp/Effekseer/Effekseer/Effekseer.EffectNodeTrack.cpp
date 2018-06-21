

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

#include "Effekseer.InstanceGroup.h"
#include "Effekseer.EffectNodeTrack.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Track\n");

	int32_t size = 0;


	LoadValues(TrackSizeFor, pos);
	LoadValues(TrackSizeMiddle, pos);
	LoadValues(TrackSizeBack, pos);

	if (m_effect->GetVersion() >= 13)
	{
		memcpy(&SplineDivision, pos, sizeof(int32_t));
		pos += sizeof(int32_t);
	}

	TrackColorLeft.load(pos, m_effect->GetVersion());
	TrackColorLeftMiddle.load(pos, m_effect->GetVersion());

	TrackColorCenter.load(pos, m_effect->GetVersion());
	TrackColorCenterMiddle.load(pos, m_effect->GetVersion());

	TrackColorRight.load(pos, m_effect->GetVersion());
	TrackColorRightMiddle.load(pos, m_effect->GetVersion());

	AlphaBlend = RendererCommon.AlphaBlend;
	TrackTexture = RendererCommon.ColorTextureIndex;

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::BeginRendering(int32_t count, Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if (renderer != NULL)
	{
		m_nodeParameter.AlphaBlend = AlphaBlend;
		m_nodeParameter.TextureFilter = RendererCommon.FilterType;
		m_nodeParameter.TextureWrap = RendererCommon.WrapType;
		m_nodeParameter.ZTest = RendererCommon.ZTest;
		m_nodeParameter.ZWrite = RendererCommon.ZWrite;
		m_nodeParameter.ColorTextureIndex = TrackTexture;
		m_nodeParameter.EffectPointer = GetEffect();

		m_nodeParameter.Distortion = RendererCommon.Distortion;
		m_nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		m_nodeParameter.SplineDivision = SplineDivision;

		renderer->BeginRendering(m_nodeParameter, count, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if (renderer != nullptr)
	{
		InstanceGroupValues& instValues = group->rendererValues.track;
		m_currentGroupValues = group->rendererValues.track;

		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;

		if (group->GetFirst() != nullptr)
		{
			m_instanceParameter.UV = group->GetFirst()->GetUV();
		}

		/*
		SetValues( m_instanceParameter.ColorLeft, instValues.ColorLeft, TrackColorLeft, group->GetTime() );
		SetValues( m_instanceParameter.ColorCenter,instValues.ColorCenter, TrackColorCenter, group->GetTime() );
		SetValues( m_instanceParameter.ColorRight,instValues.ColorRight, TrackColorRight, group->GetTime() );

		SetValues( m_instanceParameter.ColorLeftMiddle,instValues.ColorLeftMiddle, TrackColorLeftMiddle, group->GetTime() );
		SetValues( m_instanceParameter.ColorCenterMiddle,instValues.ColorCenterMiddle, TrackColorCenterMiddle, group->GetTime() );
		SetValues( m_instanceParameter.ColorRightMiddle,instValues.ColorRightMiddle, TrackColorRightMiddle, group->GetTime() );

		SetValues( m_instanceParameter.SizeFor, instValues.SizeFor, TrackSizeFor, group->GetTime() );
		SetValues( m_instanceParameter.SizeMiddle, instValues.SizeMiddle, TrackSizeMiddle, group->GetTime() );
		SetValues( m_instanceParameter.SizeBack, instValues.SizeBack, TrackSizeBack, group->GetTime() );
		*/
		renderer->BeginRenderingGroup(m_nodeParameter, group->GetInstanceCount(), m_userData);
	}
}

void EffectNodeTrack::EndRenderingGroup(InstanceGroup* group, Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if (renderer != NULL)
	{
		renderer->EndRenderingGroup(m_nodeParameter, group->GetInstanceCount(), m_userData);
	}
}

void EffectNodeTrack::Rendering(const Instance& instance, const Instance* next_instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.track;

	TrackRenderer* renderer = manager->GetTrackRenderer();
	if (renderer != NULL)
	{
		float t = (float)instance.m_LivingTime / (float)instance.m_LivedTime;
		int32_t time = instance.m_LivingTime;
		int32_t livedTime = instance.m_LivedTime;

		SetValues(m_instanceParameter.ColorLeft, instance, m_currentGroupValues.ColorLeft, TrackColorLeft, time, livedTime);
		SetValues(m_instanceParameter.ColorCenter, instance, m_currentGroupValues.ColorCenter, TrackColorCenter, time, livedTime);
		SetValues(m_instanceParameter.ColorRight, instance, m_currentGroupValues.ColorRight, TrackColorRight, time, livedTime);

		SetValues(m_instanceParameter.ColorLeftMiddle, instance, m_currentGroupValues.ColorLeftMiddle, TrackColorLeftMiddle, time, livedTime);
		SetValues(m_instanceParameter.ColorCenterMiddle, instance, m_currentGroupValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime);
		SetValues(m_instanceParameter.ColorRightMiddle, instance, m_currentGroupValues.ColorRightMiddle, TrackColorRightMiddle, time, livedTime);

		SetValues(m_instanceParameter.SizeFor, m_currentGroupValues.SizeFor, TrackSizeFor, t);
		SetValues(m_instanceParameter.SizeMiddle, m_currentGroupValues.SizeMiddle, TrackSizeMiddle, t);
		SetValues(m_instanceParameter.SizeBack, m_currentGroupValues.SizeBack, TrackSizeBack, t);

		m_instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		renderer->Rendering(m_nodeParameter, m_instanceParameter, m_userData);
		m_instanceParameter.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::EndRendering(Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if (renderer != NULL)
	{
		renderer->EndRendering(m_nodeParameter, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceGroupValues& instValues = instanceGroup.rendererValues.track;
	auto instanceGlobal = instanceGroup.GetInstanceGlobal();

	InitializeValues(instValues.ColorLeft, TrackColorLeft, instanceGlobal);
	InitializeValues(instValues.ColorCenter, TrackColorCenter, instanceGlobal);
	InitializeValues(instValues.ColorRight, TrackColorRight, instanceGlobal);

	InitializeValues(instValues.ColorLeftMiddle, TrackColorLeftMiddle, instanceGlobal);
	InitializeValues(instValues.ColorCenterMiddle, TrackColorCenterMiddle, instanceGlobal);
	InitializeValues(instValues.ColorRightMiddle, TrackColorRightMiddle, instanceGlobal);

	InitializeValues(instValues.SizeFor, TrackSizeFor, manager);
	InitializeValues(instValues.SizeBack, TrackSizeBack, manager);
	InitializeValues(instValues.SizeMiddle, TrackSizeMiddle, manager);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.track;

	// Calculate only center
	float t = (float)instance.m_LivingTime / (float)instance.m_LivedTime;
	int32_t time = instance.m_LivingTime;
	int32_t livedTime = instance.m_LivedTime;

	Color c;
	SetValues(c, instance, m_currentGroupValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime);

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		c = Color::Mul(c, instance.ColorParent);
	}

	instance.ColorInheritance = c;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.track;

	// Calculate only center
	float t = (float)instance.m_LivingTime / (float)instance.m_LivedTime;
	int32_t time = instance.m_LivingTime;
	int32_t livedTime = instance.m_LivedTime;

	Color c;
	SetValues(c, instance, m_currentGroupValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime);

	instance.ColorInheritance = c;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeValues(InstanceGroupValues::Color& value, StandardColorParameter& param, InstanceGlobal* instanceGlobal)
{
	if (param.type == StandardColorParameter::Fixed)
	{
		value.color.fixed.color_ = param.fixed.all;
	}
	else if (param.type == StandardColorParameter::Random)
	{
		value.color.random.color_ = param.random.all.getValue(*(instanceGlobal));
	}
	else if (param.type == StandardColorParameter::Easing)
	{
		value.color.easing.start = param.easing.all.getStartValue(*(instanceGlobal));
		value.color.easing.end = param.easing.all.getEndValue(*(instanceGlobal));
	}
	else if (param.type == StandardColorParameter::FCurve_RGBA)
	{
		value.color.fcurve_rgba.offset[0] = param.fcurve_rgba.FCurve->R.GetOffset(*instanceGlobal);
		value.color.fcurve_rgba.offset[1] = param.fcurve_rgba.FCurve->G.GetOffset(*instanceGlobal);
		value.color.fcurve_rgba.offset[2] = param.fcurve_rgba.FCurve->B.GetOffset(*instanceGlobal);
		value.color.fcurve_rgba.offset[3] = param.fcurve_rgba.FCurve->A.GetOffset(*instanceGlobal);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeValues(InstanceGroupValues::Size& value, TrackSizeParameter& param, Manager* manager)
{
	if (param.type == TrackSizeParameter::Fixed)
	{
		value.size.fixed.size_ = param.fixed.size;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::SetValues(Color& c, const Instance& instance, InstanceGroupValues::Color& value, StandardColorParameter& param, int32_t time, int32_t livedTime)
{
	if (param.type == StandardColorParameter::Fixed)
	{
		c = value.color.fixed.color_;
	}
	else if (param.type == StandardColorParameter::Random)
	{
		c = value.color.random.color_;
	}
	else if (param.type == StandardColorParameter::Easing)
	{
		float t = (float)time / (float)livedTime;
		param.easing.all.setValueToArg(
			c,
			value.color.easing.start,
			value.color.easing.end,
			t);
	}
	else if (param.type == StandardColorParameter::FCurve_RGBA)
	{
		c.R = (uint8_t)Clamp((value.color.fcurve_rgba.offset[0] + param.fcurve_rgba.FCurve->R.GetValue((int32_t)time)), 255, 0);
		c.G = (uint8_t)Clamp((value.color.fcurve_rgba.offset[1] + param.fcurve_rgba.FCurve->G.GetValue((int32_t)time)), 255, 0);
		c.B = (uint8_t)Clamp((value.color.fcurve_rgba.offset[2] + param.fcurve_rgba.FCurve->B.GetValue((int32_t)time)), 255, 0);
		c.A = (uint8_t)Clamp((value.color.fcurve_rgba.offset[3] + param.fcurve_rgba.FCurve->A.GetValue((int32_t)time)), 255, 0);
	}

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::SetValues(float& s, InstanceGroupValues::Size& value, TrackSizeParameter& param, float time)
{
	if (param.type == TrackSizeParameter::Fixed)
	{
		s = value.size.fixed.size_;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------