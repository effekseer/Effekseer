

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
void EffectNodeRing::LoadRendererParameter( unsigned char*& pos )
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
		AlphaBlend = Texture.AlphaBlend;
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
		RingTexture = Texture.ColorTextureIndex;
	}
	else
	{
		memcpy( &RingTexture, pos, sizeof(int) );
		pos += sizeof(int);
	}

	/* ˆÊ’uŠg‘åˆ— */
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
void EffectNodeRing::InitializeRenderer( Setting* setting )
{
	RingRenderer* renderer = setting->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.EffectPointer = GetEffect();
		renderer->LoadRenderer( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::FinalizeRenderer( Setting* setting )
{
	RingRenderer* renderer = setting->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.EffectPointer = GetEffect();
		renderer->RemoveRenderer( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::BeginRendering(int32_t count, Setting* setting)
{
	RingRenderer* renderer = setting->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.VertexCount = VertexCount;
		nodeParameter.EffectPointer = GetEffect();
		renderer->BeginRendering( nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::Rendering(const Instance& instance, Setting* setting)
{
	const InstanceValues& instValues = instance.rendererValues.ring;
	RingRenderer* renderer = setting->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.VertexCount = VertexCount;
		nodeParameter.ColorTextureIndex = RingTexture;

		RingRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		instanceParameter.ViewingAngle = instValues.viewingAngle.current;
		
		instValues.outerLocation.current.setValueToArg( instanceParameter.OuterLocation );
		instValues.innerLocation.current.setValueToArg( instanceParameter.InnerLocation );

		instanceParameter.CenterRatio = instValues.centerRatio.current;

		instValues.outerColor.current.setValueToArg( instanceParameter.OuterColor );
		instValues.centerColor.current.setValueToArg( instanceParameter.CenterColor );
		instValues.innerColor.current.setValueToArg( instanceParameter.InnerColor );
		
		float fadeAlpha = GetFadeAlpha( instance );
		if( fadeAlpha != 1.0f )
		{
			instanceParameter.OuterColor.A = (uint8_t)(instanceParameter.OuterColor.A * fadeAlpha);
			instanceParameter.CenterColor.A = (uint8_t)(instanceParameter.CenterColor.A * fadeAlpha);
			instanceParameter.InnerColor.A = (uint8_t)(instanceParameter.InnerColor.A * fadeAlpha);
		}

		instanceParameter.UV = instance.GetUV();
		renderer->Rendering( nodeParameter, instanceParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::EndRendering(Setting* setting)
{
	RingRenderer* renderer = setting->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = RingTexture;
		nodeParameter.EffectPointer = GetEffect();
		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ring;

	InitializeSingleValues( ViewingAngle, instValues.viewingAngle );

	InitializeLocationValues( OuterLocation, instValues.outerLocation );
	InitializeLocationValues( InnerLocation, instValues.innerLocation );
	
	InitializeSingleValues( CenterRatio, instValues.centerRatio );

	InitializeColorValues( OuterColor, instValues.outerColor );
	InitializeColorValues( CenterColor, instValues.centerColor );
	InitializeColorValues( InnerColor, instValues.innerColor );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ring;
	
	UpdateSingleValues( instance, ViewingAngle, instValues.viewingAngle );

	UpdateLocationValues( instance, OuterLocation, instValues.outerLocation );
	UpdateLocationValues( instance, InnerLocation, instValues.innerLocation );
	
	UpdateSingleValues( instance, CenterRatio, instValues.centerRatio );

	UpdateColorValues( instance, OuterColor, instValues.outerColor );
	UpdateColorValues( instance, CenterColor, instValues.centerColor );
	UpdateColorValues( instance, InnerColor, instValues.innerColor );
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
void EffectNodeRing::InitializeSingleValues( const RingSingleParameter& param, RingSingleValues& values )
{
	Manager* manager = m_effect->GetManager();
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
void EffectNodeRing::InitializeLocationValues( const RingLocationParameter& param, RingLocationValues& values )
{
	Manager* manager = m_effect->GetManager();
	switch( param.type )
	{
		case RingLocationParameter::Fixed:
			values.current = param.fixed.location;
			break;
		case RingLocationParameter::PVA:
			values.current = param.pva.location.getValue( *manager );
			values.pva.velocity = param.pva.velocity.getValue( *manager );
			values.pva.acceleration = param.pva.acceleration.getValue( *manager );
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
void EffectNodeRing::InitializeColorValues( const RingColorParameter& param, RingColorValues& values )
{
	Manager* manager = m_effect->GetManager();
	switch( param.type )
	{
		case RingColorParameter::Fixed:
			values.current = param.fixed;
			break;
		case RingColorParameter::Random:
			values.current = param.random.getValue( *manager );
			break;
		case RingColorParameter::Easing:
			values.easing.start = param.easing.getStartValue( *manager );
			values.easing.end = param.easing.getEndValue( *manager );
			values.current = values.easing.start;
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
		values.pva.velocity += values.pva.acceleration;
		values.current += values.pva.velocity;
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
	if( param.type == RingColorParameter::Easing )
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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
