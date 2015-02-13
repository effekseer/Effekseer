

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
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
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Track\n");

	int32_t size = 0;

	
	LoadValues( TrackSizeFor, pos );
	LoadValues( TrackSizeMiddle, pos );
	LoadValues( TrackSizeBack, pos );

	TrackColorLeft.load( pos, m_effect->GetVersion() );
	TrackColorLeftMiddle.load( pos, m_effect->GetVersion() );

	TrackColorCenter.load( pos, m_effect->GetVersion() );
	TrackColorCenterMiddle.load( pos, m_effect->GetVersion() );

	TrackColorRight.load( pos, m_effect->GetVersion() );
	TrackColorRightMiddle.load( pos, m_effect->GetVersion() );

	AlphaBlend = Texture.AlphaBlend;
	TrackTexture = Texture.ColorTextureIndex;

	EffekseerPrintDebug("TrackColorLeft : %d\n", TrackColorLeft.type );
	EffekseerPrintDebug("TrackColorLeftMiddle : %d\n", TrackColorLeftMiddle.type );
	EffekseerPrintDebug("TrackColorCenter : %d\n", TrackColorCenter.type );
	EffekseerPrintDebug("TrackColorCenterMiddle : %d\n", TrackColorCenterMiddle.type );
	EffekseerPrintDebug("TrackColorRight : %d\n", TrackColorRight.type );
	EffekseerPrintDebug("TrackColorRightMiddle : %d\n", TrackColorRightMiddle.type );

	// ‰EŽèŒn¶ŽèŒn•ÏŠ·
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
	}

	/* ˆÊ’uŠg‘åˆ— */
	if( m_effect->GetVersion() >= 8 )
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
	if( renderer != NULL )
	{
		m_nodeParameter.AlphaBlend = AlphaBlend;
		m_nodeParameter.TextureFilter = Texture.FilterType;
		m_nodeParameter.TextureWrap = Texture.WrapType;
		m_nodeParameter.ZTest = Texture.ZTest;
		m_nodeParameter.ZWrite = Texture.ZWrite;
		m_nodeParameter.ColorTextureIndex = TrackTexture;
		m_nodeParameter.EffectPointer = GetEffect();
		renderer->BeginRendering( m_nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if( renderer != NULL )
	{
		InstanceGroupValues& instValues = group->rendererValues.track;
		m_currentGroupValues = group->rendererValues.track;

		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;

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
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.track;

	TrackRenderer* renderer = manager->GetTrackRenderer();
	if( renderer != NULL )
	{
		float t = (float)instance.m_LivingTime / (float)instance.m_LivedTime;
		int32_t time = instance.m_LivingTime;
		int32_t livedTime = instance.m_LivedTime;

		SetValues( m_instanceParameter.ColorLeft, m_currentGroupValues.ColorLeft, TrackColorLeft, time, livedTime );
		SetValues( m_instanceParameter.ColorCenter, m_currentGroupValues.ColorCenter, TrackColorCenter, time, livedTime );
		SetValues( m_instanceParameter.ColorRight, m_currentGroupValues.ColorRight, TrackColorRight, time, livedTime );

		SetValues( m_instanceParameter.ColorLeftMiddle, m_currentGroupValues.ColorLeftMiddle, TrackColorLeftMiddle, time, livedTime );
		SetValues( m_instanceParameter.ColorCenterMiddle, m_currentGroupValues.ColorCenterMiddle, TrackColorCenterMiddle, time, livedTime );
		SetValues( m_instanceParameter.ColorRightMiddle, m_currentGroupValues.ColorRightMiddle, TrackColorRightMiddle, time, livedTime );
	
		SetValues( m_instanceParameter.SizeFor, m_currentGroupValues.SizeFor, TrackSizeFor, t );
		SetValues( m_instanceParameter.SizeMiddle, m_currentGroupValues.SizeMiddle, TrackSizeMiddle, t );
		SetValues( m_instanceParameter.SizeBack, m_currentGroupValues.SizeBack, TrackSizeBack, t );

		m_instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();
		renderer->Rendering( m_nodeParameter, m_instanceParameter, m_userData );
		m_instanceParameter.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::EndRendering(Manager* manager)
{
	TrackRenderer* renderer = manager->GetTrackRenderer();
	if( renderer != NULL )
	{
		renderer->EndRendering( m_nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceGroupValues& instValues = instanceGroup.rendererValues.track;

	InitializeValues(instValues.ColorLeft, TrackColorLeft, manager);
	InitializeValues(instValues.ColorCenter, TrackColorCenter, manager);
	InitializeValues(instValues.ColorRight, TrackColorRight, manager);

	InitializeValues(instValues.ColorLeftMiddle, TrackColorLeftMiddle, manager);
	InitializeValues(instValues.ColorCenterMiddle, TrackColorCenterMiddle, manager);
	InitializeValues(instValues.ColorRightMiddle, TrackColorRightMiddle, manager);

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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.track;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeValues(InstanceGroupValues::Color& value, StandardColorParameter& param, Manager* manager)
{
	if( param.type == StandardColorParameter::Fixed )
	{
		value.color.fixed.color_ = param.fixed.all;
	}
	else if( param.type == StandardColorParameter::Random )
	{
		value.color.random.color_ = param.random.all.getValue(*(manager));
	}
	else if( param.type == StandardColorParameter::Easing )
	{
		value.color.easing.start = param.easing.all.getStartValue(*(manager));
		value.color.easing.end = param.easing.all.getEndValue(*(manager));
	}
	else if( param.type == StandardColorParameter::FCurve_RGBA )
	{
		value.color.fcurve_rgba.offset[0] = param.fcurve_rgba.FCurve->R.GetOffset(*(manager));
		value.color.fcurve_rgba.offset[1] = param.fcurve_rgba.FCurve->G.GetOffset(*(manager));
		value.color.fcurve_rgba.offset[2] = param.fcurve_rgba.FCurve->B.GetOffset(*(manager));
		value.color.fcurve_rgba.offset[3] = param.fcurve_rgba.FCurve->A.GetOffset(*(manager));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::InitializeValues(InstanceGroupValues::Size& value, TrackSizeParameter& param, Manager* manager)
{
	if( param.type == TrackSizeParameter::Fixed )
	{
		value.size.fixed.size_ = param.fixed.size;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::SetValues( Color& c, InstanceGroupValues::Color& value, StandardColorParameter& param, int32_t time, int32_t livedTime )
{
	if( param.type == StandardColorParameter::Fixed )
	{
		value.color.fixed.color_.setValueToArg( c );
	}
	else if(param.type == StandardColorParameter::Random )
	{
		value.color.random.color_.setValueToArg( c );
	}
	else if( param.type == StandardColorParameter::Easing )
	{
		float t = (float)time / (float)livedTime;
		param.easing.all.setValueToArg(
			c, 
			value.color.easing.start,
			value.color.easing.end,
			t );
	}
	else if( param.type == StandardColorParameter::FCurve_RGBA )
	{
		c.R = (uint8_t)Clamp( (value.color.fcurve_rgba.offset[0] + param.fcurve_rgba.FCurve->R.GetValue( (int32_t)time )), 255, 0);
		c.G = (uint8_t)Clamp( (value.color.fcurve_rgba.offset[1] + param.fcurve_rgba.FCurve->G.GetValue( (int32_t)time )), 255, 0);
		c.B = (uint8_t)Clamp( (value.color.fcurve_rgba.offset[2] + param.fcurve_rgba.FCurve->B.GetValue( (int32_t)time )), 255, 0);
		c.A = (uint8_t)Clamp( (value.color.fcurve_rgba.offset[3] + param.fcurve_rgba.FCurve->A.GetValue( (int32_t)time )), 255, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::SetValues( float& s, InstanceGroupValues::Size& value, TrackSizeParameter& param, float time )
{
	if( param.type == TrackSizeParameter::Fixed )
	{
		s = value.size.fixed.size_;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeTrack::LoadValues( TrackSizeParameter& param, unsigned char*& pos )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);
	
	if( param.type == TrackSizeParameter::Fixed )
	{	
		memcpy( &param.fixed, pos, sizeof(param.fixed) );
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
