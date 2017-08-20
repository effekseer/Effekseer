

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.EffectNodeRing.h"

#include "Renderer/Effekseer.RingRenderer.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Ring\n");

	int32_t size = 0;

	memcpy( &RenderingOrder, pos, sizeof(int) );
	pos += sizeof(int);

	if( m_effect->GetVersion() >= 3)
	{
		AlphaBlend = RendererCommon.AlphaBlend;
	}
	else
	{
		memcpy( &AlphaBlend, pos, sizeof(int) );
		pos += sizeof(int);
	}

	memcpy( &Billboard, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &VertexCount, pos, sizeof(int) );
	pos += sizeof(int);
	
	LoadSingleParameter( pos, ViewingAngle );

	LoadLocationParameter( pos, OuterLocation );
	
	LoadLocationParameter( pos, InnerLocation );
	
	LoadSingleParameter( pos, CenterRatio );

	LoadColorParameter( pos, OuterColor);

	LoadColorParameter( pos, CenterColor);

	LoadColorParameter( pos, InnerColor);

	if( m_effect->GetVersion() >= 3)
	{
		RingTexture = RendererCommon.ColorTextureIndex;
	}
	else
	{
		memcpy( &RingTexture, pos, sizeof(int) );
		pos += sizeof(int);
	}
	
	// 右手系左手系変換
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
		if( OuterLocation.type == RingLocationParameter::Fixed )
		{
			OuterLocation.fixed.location.y *= -1;
		}
		else if( OuterLocation.type == RingLocationParameter::PVA )
		{
			OuterLocation.pva.location.min.y *= -1;
			OuterLocation.pva.location.max.y *= -1;
			OuterLocation.pva.velocity.min.y *= -1;
			OuterLocation.pva.velocity.max.y *= -1;
			OuterLocation.pva.acceleration.min.y *= -1;
			OuterLocation.pva.acceleration.max.y *= -1;
		}
		else if( OuterLocation.type == RingLocationParameter::Easing )
		{
			OuterLocation.easing.start.min.y *= -1;
			OuterLocation.easing.start.max.y *= -1;
			OuterLocation.easing.end.min.y *= -1;
			OuterLocation.easing.end.max.y *= -1;
		}

		if( InnerLocation.type == RingLocationParameter::Fixed )
		{
			InnerLocation.fixed.location.y *= -1;
		}
		else if( InnerLocation.type == RingLocationParameter::PVA )
		{
			InnerLocation.pva.location.min.y *= -1;
			InnerLocation.pva.location.max.y *= -1;
			InnerLocation.pva.velocity.min.y *= -1;
			InnerLocation.pva.velocity.max.y *= -1;
			InnerLocation.pva.acceleration.min.y *= -1;
			InnerLocation.pva.acceleration.max.y *= -1;
		}
		else if( InnerLocation.type == RingLocationParameter::Easing )
		{
			InnerLocation.easing.start.min.y *= -1;
			InnerLocation.easing.start.max.y *= -1;
			InnerLocation.easing.end.min.y *= -1;
			InnerLocation.easing.end.max.y *= -1;
		}
	}

	/* 位置拡大処理 */
	if( m_effect->GetVersion() >= 8 )
	{
		if( OuterLocation.type == RingLocationParameter::Fixed )
		{
			OuterLocation.fixed.location *= m_effect->GetMaginification();
		}
		else if( OuterLocation.type == RingLocationParameter::PVA )
		{
			OuterLocation.pva.location.min *= m_effect->GetMaginification();
			OuterLocation.pva.location.max *= m_effect->GetMaginification();
			OuterLocation.pva.velocity.min *= m_effect->GetMaginification();
			OuterLocation.pva.velocity.max *= m_effect->GetMaginification();
			OuterLocation.pva.acceleration.min *= m_effect->GetMaginification();
			OuterLocation.pva.acceleration.max *= m_effect->GetMaginification();
		}
		else if( OuterLocation.type == RingLocationParameter::Easing )
		{
			OuterLocation.easing.start.min *= m_effect->GetMaginification();
			OuterLocation.easing.start.max *= m_effect->GetMaginification();
			OuterLocation.easing.end.min *= m_effect->GetMaginification();
			OuterLocation.easing.end.max *= m_effect->GetMaginification();
		}

		if( InnerLocation.type == RingLocationParameter::Fixed )
		{
			InnerLocation.fixed.location *= m_effect->GetMaginification();
		}
		else if( InnerLocation.type == RingLocationParameter::PVA )
		{
			InnerLocation.pva.location.min *= m_effect->GetMaginification();
			InnerLocation.pva.location.max *= m_effect->GetMaginification();
			InnerLocation.pva.velocity.min *= m_effect->GetMaginification();
			InnerLocation.pva.velocity.max *= m_effect->GetMaginification();
			InnerLocation.pva.acceleration.min *= m_effect->GetMaginification();
			InnerLocation.pva.acceleration.max *= m_effect->GetMaginification();
		}
		else if( InnerLocation.type == RingLocationParameter::Easing )
		{
			InnerLocation.easing.start.min *= m_effect->GetMaginification();
			InnerLocation.easing.start.max *= m_effect->GetMaginification();
			InnerLocation.easing.end.min *= m_effect->GetMaginification();
			InnerLocation.easing.end.max *= m_effect->GetMaginification();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::BeginRendering(int32_t count, Manager* manager)
{
	RingRenderer* renderer = manager->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.VertexCount = VertexCount;
		nodeParameter.EffectPointer = GetEffect();

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		renderer->BeginRendering( nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.ring;
	RingRenderer* renderer = manager->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.VertexCount = VertexCount;
		nodeParameter.ColorTextureIndex = RingTexture;

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		color _outerColor;
		color _centerColor;
		color _innerColor;

		if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
		{
			_outerColor = color::mul(instValues.outerColor.original, instance.ColorParent);
			_centerColor = color::mul(instValues.centerColor.original, instance.ColorParent);
			_innerColor = color::mul(instValues.innerColor.original, instance.ColorParent);
		}
		else
		{
			_outerColor = instValues.outerColor.original;
			_centerColor = instValues.centerColor.original;
			_innerColor = instValues.innerColor.original;
		}

		RingRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		instanceParameter.ViewingAngle = instValues.viewingAngle.current;
		
		instValues.outerLocation.current.setValueToArg( instanceParameter.OuterLocation );
		instValues.innerLocation.current.setValueToArg( instanceParameter.InnerLocation );

		instanceParameter.CenterRatio = instValues.centerRatio.current;

		_outerColor.setValueToArg( instanceParameter.OuterColor );
		_centerColor.setValueToArg( instanceParameter.CenterColor );
		_innerColor.setValueToArg( instanceParameter.InnerColor );
		
		instanceParameter.UV = instance.GetUV();
		renderer->Rendering( nodeParameter, instanceParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::EndRendering(Manager* manager)
{
	RingRenderer* renderer = manager->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.EffectPointer = GetEffect();

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ring;

	InitializeSingleValues(ViewingAngle, instValues.viewingAngle, manager);

	InitializeLocationValues(OuterLocation, instValues.outerLocation, manager);
	InitializeLocationValues(InnerLocation, instValues.innerLocation, manager);
	
	InitializeSingleValues(CenterRatio, instValues.centerRatio, manager);

	InitializeColorValues(OuterColor, instValues.outerColor, manager);
	InitializeColorValues(CenterColor, instValues.centerColor, manager);
	InitializeColorValues(InnerColor, instValues.innerColor, manager);

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues.outerColor.current = color::mul(instValues.outerColor.original, instance.ColorParent);
		instValues.centerColor.current = color::mul(instValues.centerColor.original, instance.ColorParent);
		instValues.innerColor.current = color::mul(instValues.innerColor.original, instance.ColorParent);
	}
	else
	{
		instValues.outerColor.current = instValues.outerColor.original;
		instValues.centerColor.current = instValues.centerColor.original;
		instValues.innerColor.current = instValues.innerColor.original;
	}

	instance.ColorInheritance = instValues.centerColor.current;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ring;
	
	UpdateSingleValues( instance, ViewingAngle, instValues.viewingAngle );

	UpdateLocationValues( instance, OuterLocation, instValues.outerLocation );
	UpdateLocationValues( instance, InnerLocation, instValues.innerLocation );
	
	UpdateSingleValues( instance, CenterRatio, instValues.centerRatio );

	UpdateColorValues( instance, OuterColor, instValues.outerColor );
	UpdateColorValues( instance, CenterColor, instValues.centerColor );
	UpdateColorValues( instance, InnerColor, instValues.innerColor );

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues.outerColor.current = color::mul(instValues.outerColor.original, instance.ColorParent);
		instValues.centerColor.current = color::mul(instValues.centerColor.original, instance.ColorParent);
		instValues.innerColor.current = color::mul(instValues.innerColor.original, instance.ColorParent);
	}
	else
	{
		instValues.outerColor.current = instValues.outerColor.original;
		instValues.centerColor.current = instValues.centerColor.original;
		instValues.innerColor.current = instValues.innerColor.original;
	}

	instance.ColorInheritance = instValues.centerColor.current;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadSingleParameter( unsigned char*& pos, RingSingleParameter& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);

	if( param.type == RingSingleParameter::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(float) );
		pos += sizeof(float);
	}
	else if( param.type == RingSingleParameter::Random )
	{
		memcpy( &param.random, pos, sizeof(param.random) );
		pos += sizeof(param.random);
	}
	else if( param.type == RingSingleParameter::Easing )
	{
		memcpy( &param.easing, pos, sizeof(param.easing) );
		pos += sizeof(param.easing);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadLocationParameter( unsigned char*& pos, RingLocationParameter& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);
	
	if( param.type == RingLocationParameter::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(param.fixed) );
		pos += sizeof(param.fixed);
	}
	else if( param.type == RingLocationParameter::PVA )
	{
		memcpy( &param.pva, pos, sizeof(param.pva) );
		pos += sizeof(param.pva);
	}
	else if( param.type == RingLocationParameter::Easing )
	{
		memcpy( &param.easing, pos, sizeof(param.easing) );
		pos += sizeof(param.easing);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadColorParameter( unsigned char*& pos, RingColorParameter& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);
	
	if( param.type == RingColorParameter::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(param.fixed) );
		pos += sizeof(param.fixed);
	}
	else if( param.type == RingColorParameter::Random )
	{
		param.random.load( m_effect->GetVersion(), pos );
	}
	else if( param.type == RingColorParameter::Easing )
	{
		param.easing.load( m_effect->GetVersion(), pos );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeSingleValues(const RingSingleParameter& param, RingSingleValues& values, Manager* manager)
{
	switch( param.type )
	{
		case RingSingleParameter::Fixed:
			values.current = param.fixed;
			break;
		case RingSingleParameter::Random:
			values.current = param.random.getValue( *manager );
			break;
		case RingSingleParameter::Easing:
			values.easing.start = param.easing.start.getValue( *manager );
			values.easing.end = param.easing.end.getValue( *manager );
			values.current = values.easing.start;
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeLocationValues(const RingLocationParameter& param, RingLocationValues& values, Manager* manager)
{
	switch( param.type )
	{
		case RingLocationParameter::Fixed:
			values.current = param.fixed.location;
			break;
		case RingLocationParameter::PVA:
			values.pva.start = param.pva.location.getValue( *manager );
			values.pva.velocity = param.pva.velocity.getValue( *manager );
			values.pva.acceleration = param.pva.acceleration.getValue( *manager );
			values.current = values.pva.start;
			break;
		case RingLocationParameter::Easing:
			values.easing.start = param.easing.start.getValue( *manager );
			values.easing.end = param.easing.end.getValue( *manager );
			values.current = values.easing.start;
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeColorValues(const RingColorParameter& param, RingColorValues& values, Manager* manager)
{
	switch( param.type )
	{
		case RingColorParameter::Fixed:
			values.original = param.fixed;
			values.fixed._color = values.original;
			break;
		case RingColorParameter::Random:
			values.original = param.random.getValue(*manager);
			values.random._color = values.original;
			break;
		case RingColorParameter::Easing:
			values.easing.start = param.easing.getStartValue( *manager );
			values.easing.end = param.easing.getEndValue( *manager );
			values.original = values.easing.start;
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateSingleValues( Instance& instance, const RingSingleParameter& param, RingSingleValues& values )
{
	if( param.type == RingSingleParameter::Easing )
	{
		param.easing.setValueToArg(
			values.current,
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateLocationValues( Instance& instance, const RingLocationParameter& param, RingLocationValues& values )
{
	if( param.type == RingLocationParameter::PVA )
	{
		values.current = values.pva.start +
			values.pva.velocity * instance.m_LivingTime +
			values.pva.acceleration * instance.m_LivingTime * instance.m_LivingTime * 0.5f;
	}
	else if( param.type == RingLocationParameter::Easing )
	{
		param.easing.setValueToArg(
			values.current,
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateColorValues( Instance& instance, const RingColorParameter& param, RingColorValues& values )
{
	if (param.type == RingColorParameter::Fixed)
	{
		values.original = values.fixed._color;
	}
	else if (param.type == RingColorParameter::Random)
	{
		values.original = values.random._color;
	}
	else if( param.type == RingColorParameter::Easing )
	{
		param.easing.setValueToArg(
			values.original, 
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		values.original.a = (uint8_t)(values.original.a * fadeAlpha);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
