

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.EffectNodeRibbon.h"

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
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Ribbon\n");

	int32_t size = 0;

	if( m_effect->GetVersion() >= 3)
	{
		AlphaBlend = RendererCommon.AlphaBlend;
	}
	else
	{
		memcpy( &AlphaBlend, pos, sizeof(int) );
		pos += sizeof(int);
	}

	memcpy( &ViewpointDependent, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &RibbonAllColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorAllType : %d\n", RibbonAllColor.type );

	if( RibbonAllColor.type == RibbonAllColorParameter::Fixed )
	{
		memcpy( &RibbonAllColor.fixed, pos, sizeof(RibbonAllColor.fixed) );
		pos += sizeof(RibbonAllColor.fixed);
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Random )
	{
		RibbonAllColor.random.all.load( m_effect->GetVersion(), pos );
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		RibbonAllColor.easing.all.load( m_effect->GetVersion(), pos );
	}

	memcpy( &RibbonColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorType : %d\n", RibbonColor.type );

	if( RibbonColor.type == RibbonColor.Default )
	{
	}
	else if( RibbonColor.type == RibbonColor.Fixed )
	{
		memcpy( &RibbonColor.fixed, pos, sizeof(RibbonColor.fixed) );
		pos += sizeof(RibbonColor.fixed);
	}

	memcpy( &RibbonPosition.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonPosition : %d\n", RibbonPosition.type );

	if( RibbonPosition.type == RibbonPosition.Default )
	{
		if( m_effect->GetVersion() >= 8 )
		{
			memcpy( &RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed) );
			pos += sizeof(RibbonPosition.fixed);
			RibbonPosition.type = RibbonPosition.Fixed;
		}
	}
	else if( RibbonPosition.type == RibbonPosition.Fixed )
	{
		memcpy( &RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed) );
		pos += sizeof(RibbonPosition.fixed);
	}

	if( m_effect->GetVersion() >= 3)
	{
		RibbonTexture = RendererCommon.ColorTextureIndex;
	}
	else
	{
		memcpy( &RibbonTexture, pos, sizeof(int) );
		pos += sizeof(int);
	}

	// 右手系左手系変換
	if( setting->GetCoordinateSystem() == CoordinateSystem::LH )
	{
	}

	/* 位置拡大処理 */
	if( m_effect->GetVersion() >= 8 )
	{
		if( RibbonPosition.type == RibbonPosition.Default )
		{
		}
		else if( RibbonPosition.type == RibbonPosition.Fixed )
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
	if( renderer != NULL )
	{
		m_nodeParameter.AlphaBlend = AlphaBlend;
		m_nodeParameter.TextureFilter = RendererCommon.FilterType;
		m_nodeParameter.TextureWrap = RendererCommon.WrapType;
		m_nodeParameter.ZTest = RendererCommon.ZTest;
		m_nodeParameter.ZWrite = RendererCommon.ZWrite;
		m_nodeParameter.ViewpointDependent = ViewpointDependent != 0;
		m_nodeParameter.ColorTextureIndex = RibbonTexture;
		m_nodeParameter.EffectPointer = GetEffect();

		m_nodeParameter.Distortion = RendererCommon.Distortion;
		m_nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;


		renderer->BeginRendering( m_nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;

		if (group->GetFirst() != nullptr)
		{
			m_instanceParameter.UV = group->GetFirst()->GetUV();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.ribbon;
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		color _color;
		if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
		{
			_color = color::mul(instValues._original, instance.ColorParent);
		}
		else
		{
			_color = instValues._original;
		}

		_color.setValueToArg( m_instanceParameter.AllColor );
		m_instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		color color_l = _color;
		color color_r = _color;

		if( RibbonColor.type == RibbonColorParameter::Default )
		{

		}
		else if( RibbonColor.type == RibbonColorParameter::Fixed )
		{
			color_l = color::mul( color_l, RibbonColor.fixed.l );
			color_r = color::mul( color_r, RibbonColor.fixed.r );
		}

		color_l.setValueToArg( m_instanceParameter.Colors[0] );
		color_r.setValueToArg( m_instanceParameter.Colors[1] );

		if( RibbonPosition.type == RibbonPositionParameter::Default )
		{
			m_instanceParameter.Positions[0] = -0.5f;
			m_instanceParameter.Positions[1] = 0.5f;
		}
		else if( RibbonPosition.type == RibbonPositionParameter::Fixed )
		{
			m_instanceParameter.Positions[0] = RibbonPosition.fixed.l;
			m_instanceParameter.Positions[1] = RibbonPosition.fixed.r;
		}

		renderer->Rendering( m_nodeParameter, m_instanceParameter, m_userData );

		m_instanceParameter.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::EndRendering(Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		renderer->EndRendering( m_nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if( RibbonAllColor.type == RibbonAllColorParameter::Fixed )
	{
		instValues._original = RibbonAllColor.fixed.all;
		instValues.allColorValues.fixed._color = instValues._original;
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Random )
	{
		instValues._original = RibbonAllColor.random.all.getValue(*(manager));
		instValues.allColorValues.random._color = instValues._original;
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = RibbonAllColor.easing.all.getStartValue(*(manager));
		instValues.allColorValues.easing.end = RibbonAllColor.easing.all.getEndValue(*(manager));
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = color::mul(instValues._original, instance.ColorParent);
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
	else if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		RibbonAllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		instValues._original.a = (uint8_t)(instValues._original.a * fadeAlpha);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = color::mul(instValues._original, instance.ColorParent);
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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
