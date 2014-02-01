

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"

#include "Effekseer.Instance.h"

#include "Effekseer.EffectNodeRoot.h"
#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode::EffectNode( Effect* effect, unsigned char*& pos )
	: m_effect		( effect )
	, IsRendered		( true )
	, SoundType			( ParameterSoundType_None )
	, RenderingOrder	( RenderingOrder_FirstCreatedInstanceIsFirst )
	, m_userData		( NULL )
	, TranslationFCurve	( NULL )
	, RotationFCurve	( NULL )
	, ScalingFCurve		( NULL )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadParameter(unsigned char*& pos, Setting* setting)
{
	int size = 0;
	int node_type = 0;
	memcpy( &node_type, pos, sizeof(int) );
	pos += sizeof(int);

	if( node_type == -1 )
	{
		TranslationType = ParameterTranslationType_None;
		LocationAbs.type = LocationAbsParameter::None;
		RotationType = ParameterRotationType_None;
		ScalingType = ParameterScalingType_None;
		CommonValues.MaxGeneration = 1;

		GenerationLocation.EffectsRotation = 0;
		GenerationLocation.type = ParameterGenerationLocation::TYPE_POINT;
		GenerationLocation.point.location.reset();
	}
	else
	{
		memcpy( &size, pos, sizeof(int) );
		pos += sizeof(int);
		assert( size == sizeof(ParameterCommonValues) );
		memcpy( &CommonValues, pos, size );
		pos += size;
		

		memcpy( &TranslationType, pos, sizeof(int) );
		pos += sizeof(int);

		if( TranslationType == ParameterTranslationType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterTranslationFixed) );
			memcpy( &TranslationFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( TranslationFixed.Position.X == 0.0f &&
				TranslationFixed.Position.Y == 0.0f &&
				TranslationFixed.Position.Z == 0.0f )
			{
				TranslationType = ParameterTranslationType_None;
				EffekseerPrintDebug("LocationType Change None\n");
			}
		}
		else if( TranslationType == ParameterTranslationType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterTranslationPVA) );
			memcpy( &TranslationPVA, pos, size );
			pos += size;
		}
		else if( TranslationType == ParameterTranslationType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &TranslationEasing, pos, size );
			pos += size;
		}
		else if( TranslationType == ParameterTranslationType_FCurve )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			
			TranslationFCurve = new FCurveVector3D();
			pos += TranslationFCurve->Load( pos, m_effect->GetVersion() );
		}

		/* à íuägëÂèàóù */
		if( m_effect->GetVersion() >= 8 )
		{
			if( TranslationType == ParameterTranslationType_Fixed )
			{
				TranslationFixed.Position *= m_effect->GetMaginification();
			}
			else if( TranslationType == ParameterTranslationType_PVA )
			{
				TranslationPVA.location.min *= m_effect->GetMaginification();
				TranslationPVA.location.max *= m_effect->GetMaginification();
				TranslationPVA.velocity.min *= m_effect->GetMaginification();
				TranslationPVA.velocity.max *= m_effect->GetMaginification();
				TranslationPVA.acceleration.min *= m_effect->GetMaginification();
				TranslationPVA.acceleration.max *= m_effect->GetMaginification();
			}
			else if( TranslationType == ParameterTranslationType_Easing )
			{
				TranslationEasing.start.min *= m_effect->GetMaginification();
				TranslationEasing.start.max *= m_effect->GetMaginification();	
				TranslationEasing.end.min *= m_effect->GetMaginification();
				TranslationEasing.end.max *= m_effect->GetMaginification();
			}
			else if( TranslationType == ParameterTranslationType_FCurve )
			{
				TranslationFCurve->X.Maginify( m_effect->GetMaginification() );
				TranslationFCurve->Y.Maginify( m_effect->GetMaginification() );
				TranslationFCurve->Z.Maginify( m_effect->GetMaginification() );
			}
		}

		memcpy( &LocationAbs.type, pos, sizeof(int) );
		pos += sizeof(int);

		/* ê‚ëŒà íu */
		if( LocationAbs.type == LocationAbsParameter::None )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == 0 );
			memcpy( &LocationAbs.none, pos, size );
			pos += size;
		}
		else if( LocationAbs.type == LocationAbsParameter::Gravity )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(vector3d) );
			memcpy( &LocationAbs.gravity, pos, size );
			pos += size;
		}

		/* ê‚ëŒà íuägëÂèàóù */
		if( m_effect->GetVersion() >= 8 )
		{
			if( LocationAbs.type == LocationAbsParameter::None )
			{
			}
			else if( LocationAbs.type == LocationAbsParameter::Gravity )
			{
				LocationAbs.gravity *= m_effect->GetMaginification();
			}
		}

		memcpy( &RotationType, pos, sizeof(int) );
		pos += sizeof(int);
		EffekseerPrintDebug("RotationType %d\n", RotationType);
		if( RotationType == ParameterRotationType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterRotationFixed) );
			memcpy( &RotationFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( RotationFixed.Position.X == 0.0f &&
				RotationFixed.Position.Y == 0.0f &&
				RotationFixed.Position.Z == 0.0f )
			{
				RotationType = ParameterRotationType_None;
				EffekseerPrintDebug("RotationType Change None\n");
			}
		}
		else if( RotationType == ParameterRotationType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterRotationPVA) );
			memcpy( &RotationPVA, pos, size );
			pos += size;
		}
		else if( RotationType == ParameterRotationType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &RotationEasing, pos, size );
			pos += size;
		}
		else if( RotationType == ParameterRotationType_AxisPVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterRotationAxisPVA) );
			memcpy( &RotationAxisPVA, pos, size );
			pos += size;
		}
		else if( RotationType == ParameterRotationType_AxisEasing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterRotationAxisEasing) );
			memcpy( &RotationAxisEasing, pos, size );
			pos += size;
		}
		else if( RotationType == ParameterRotationType_FCurve )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			
			RotationFCurve = new FCurveVector3D();
			pos += RotationFCurve->Load( pos, m_effect->GetVersion() );
		}

		memcpy( &ScalingType, pos, sizeof(int) );
		pos += sizeof(int);
		EffekseerPrintDebug("ScalingType %d\n", ScalingType);
		if( ScalingType == ParameterScalingType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterScalingFixed) );
			memcpy( &ScalingFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( ScalingFixed.Position.X == 1.0f &&
				ScalingFixed.Position.Y == 1.0f &&
				ScalingFixed.Position.Z == 1.0f )
			{
				ScalingType = ParameterScalingType_None;
				EffekseerPrintDebug("ScalingType Change None\n");
			}
		}
		else if( ScalingType == ParameterScalingType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterScalingPVA) );
			memcpy( &ScalingPVA, pos, size );
			pos += size;
		}
		else if( ScalingType == ParameterScalingType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &ScalingEasing, pos, size );
			pos += size;
		}
		else if( ScalingType == ParameterScalingType_SinglePVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterScalingSinglePVA) );
			memcpy( &ScalingSinglePVA, pos, size );
			pos += size;
		}
		else if( ScalingType == ParameterScalingType_SingleEasing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_float) );
			memcpy( &ScalingSingleEasing, pos, size );
			pos += size;
		}
		else if( ScalingType == ParameterScalingType_FCurve )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			
			ScalingFCurve = new FCurveVector3D();
			pos += ScalingFCurve->Load( pos, m_effect->GetVersion() );
			ScalingFCurve->X.SetDefaultValue( 1.0f );
			ScalingFCurve->Y.SetDefaultValue( 1.0f );
			ScalingFCurve->Z.SetDefaultValue( 1.0f );
		}

		/* ê∂ê¨à íu */
		GenerationLocation.load( pos );

		/* ê∂ê¨à íuägëÂèàóù*/
		if( m_effect->GetVersion() >= 8 && this->CommonValues.ScalingBindType == BindType_NotBind)
		{
			if( GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
			{
				GenerationLocation.point.location.min *= m_effect->GetMaginification();
				GenerationLocation.point.location.max *= m_effect->GetMaginification();
			}
			else if( GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
			{
				GenerationLocation.sphere.radius.min *= m_effect->GetMaginification();
				GenerationLocation.sphere.radius.max *= m_effect->GetMaginification();
			}			
		}

		// âEéËånç∂éËånïœä∑
		if( setting->GetCoordinateSystem() == COORDINATE_SYSTEM_LH )
		{
			// Translation
			if( TranslationType == ParameterTranslationType_Fixed )
			{
				TranslationFixed.Position.Z *= -1.0f;
			}
			else if( TranslationType == ParameterTranslationType_PVA )
			{
				TranslationPVA.location.max.z *= -1.0f;
				TranslationPVA.location.min.z *= -1.0f;
				TranslationPVA.velocity.max.z *= -1.0f;
				TranslationPVA.velocity.min.z *= -1.0f;
				TranslationPVA.acceleration.max.z *= -1.0f;
				TranslationPVA.acceleration.min.z *= -1.0f;
			}
			else if( TranslationType == ParameterTranslationType_Easing )
			{
				TranslationEasing.start.max.z *= -1.0f;
				TranslationEasing.start.min.z *= -1.0f;
				TranslationEasing.end.max.z *= -1.0f;
				TranslationEasing.end.min.z *= -1.0f;
			}

			// Rotation
			if( RotationType == ParameterRotationType_Fixed )
			{
				RotationFixed.Position.X *= -1.0f;
				RotationFixed.Position.Y *= -1.0f;
			}
			else if( RotationType == ParameterRotationType_PVA )
			{
				RotationPVA.rotation.max.x *= -1.0f;
				RotationPVA.rotation.min.x *= -1.0f;
				RotationPVA.rotation.max.y *= -1.0f;
				RotationPVA.rotation.min.y *= -1.0f;
				RotationPVA.velocity.max.x *= -1.0f;
				RotationPVA.velocity.min.x *= -1.0f;
				RotationPVA.velocity.max.y *= -1.0f;
				RotationPVA.velocity.min.y *= -1.0f;
				RotationPVA.acceleration.max.x *= -1.0f;
				RotationPVA.acceleration.min.x *= -1.0f;
				RotationPVA.acceleration.max.y *= -1.0f;
				RotationPVA.acceleration.min.y *= -1.0f;
			}
			else if( RotationType == ParameterRotationType_Easing )
			{
				RotationEasing.start.max.x *= -1.0f;
				RotationEasing.start.min.x *= -1.0f;
				RotationEasing.start.max.y *= -1.0f;
				RotationEasing.start.min.y *= -1.0f;
				RotationEasing.end.max.x *= -1.0f;
				RotationEasing.end.min.x *= -1.0f;
				RotationEasing.end.max.y *= -1.0f;
				RotationEasing.end.min.y *= -1.0f;
			}
			else if( RotationType == ParameterRotationType_AxisPVA )
			{
				RotationAxisPVA.axis.max.z *= -1.0f;
				RotationAxisPVA.axis.min.z *= -1.0f;
			}
			else if( RotationType == ParameterRotationType_AxisEasing )
			{
				RotationAxisEasing.axis.max.z *= -1.0f;
				RotationAxisEasing.axis.min.z *= -1.0f;
			}
			else if( RotationType == ParameterRotationType_FCurve )
			{
				RotationFCurve->X.ChangeCoordinate();
				RotationFCurve->Y.ChangeCoordinate();
			}

			// GenerationLocation
			if( GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
			{
			
			}
			else if( GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
			{
				GenerationLocation.sphere.rotation_x.max *= -1.0f;
				GenerationLocation.sphere.rotation_x.min *= -1.0f;
				GenerationLocation.sphere.rotation_y.max *= -1.0f;
				GenerationLocation.sphere.rotation_y.min *= -1.0f;
			}
		}

		if( m_effect->GetVersion() >= 3)
		{
			Texture.load( pos, m_effect->GetVersion() );
		}
		else
		{
			Texture.reset();
		}

		LoadRendererParameter( pos, m_effect->GetSetting() );
		
		if( m_effect->GetVersion() >= 1)
		{
			// ÉTÉEÉìÉh
			memcpy( &SoundType, pos, sizeof(int) );
			pos += sizeof(int);
			if( SoundType == ParameterSoundType_Use )
			{
				memcpy( &Sound.WaveId, pos, sizeof(int32_t) );
				pos += sizeof(int32_t);
				memcpy( &Sound.Volume, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.Pitch, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.PanType, pos, sizeof(ParameterSoundPanType) );
				pos += sizeof(ParameterSoundPanType);
				memcpy( &Sound.Pan, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.Distance, pos, sizeof(float) );
				pos += sizeof(float);
				memcpy( &Sound.Delay, pos, sizeof(random_int) );
				pos += sizeof(random_int);
			}
		}
	}

	// ÉmÅ[Éh
	int nodeCount = 0;
	memcpy( &nodeCount, pos, sizeof(int) );
	pos += sizeof( int );
	EffekseerPrintDebug("ChildrenCount : %d\n", nodeCount );
	m_Nodes.resize( nodeCount );
	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		m_Nodes[i] = EffectNode::Create( m_effect, pos );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode::~EffectNode()
{
	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		ES_SAFE_DELETE( m_Nodes[i] );
	}

	ES_SAFE_DELETE( TranslationFCurve );
	ES_SAFE_DELETE( RotationFCurve );
	ES_SAFE_DELETE( ScalingFCurve );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadOption( uint8_t*& pos )
{
	int is_rendered = 0;
	memcpy( &is_rendered, pos, sizeof(int) );
	pos += sizeof(int);

	IsRendered = is_rendered != 0;

	int count = 0;
	memcpy( &count, pos, sizeof(int) );
	pos += sizeof(int);

	for( int i = 0; i < count; i++ )
	{
		m_Nodes[i]->LoadOption( pos );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectNode::GetEffect() const
{
	return m_effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectNode::GetChildrenCount() const
{
	return (int)m_Nodes.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNode::GetChild( int num ) const
{
	if( num >= GetChildrenCount() ) return NULL;
	return m_Nodes[ num ];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : None\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::BeginRendering(int32_t count, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::Rendering(const Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::EndRendering(Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float EffectNode::GetFadeAlpha( const Instance& instance )
{
	float alpha = 1.0f;

	if( Texture.FadeInType == ParameterTexture::FADEIN_ON && instance.m_LivingTime < Texture.FadeIn.Frame )
	{
		float v = 1.0f;
		Texture.FadeIn.Value.setValueToArg( 
			v,
			0.0f,
			1.0f,
			(float)instance.m_LivingTime / (float)Texture.FadeIn.Frame );

		alpha *= v;
	}

	if( Texture.FadeOutType == ParameterTexture::FADEOUT_ON && instance.m_LivingTime + Texture.FadeOut.Frame > instance.m_LivedTime )
	{
		float v = 1.0f;
		Texture.FadeOut.Value.setValueToArg( 
			v,
			1.0f,
			0.0f,
			(float)( instance.m_LivingTime + Texture.FadeOut.Frame - instance.m_LivedTime ) / (float)Texture.FadeOut.Frame );

		alpha *= v;
	}

	return alpha;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::PlaySound_(Instance& instance, SoundTag tag, Manager* manager)
{
	SoundPlayer* player = manager->GetSoundPlayer();
	if( player == NULL )
	{
		return;
	}

	if( Sound.WaveId >= 0 )
	{
		SoundPlayer::InstanceParameter parameter;
		parameter.Data = m_effect->GetWave( Sound.WaveId );
		parameter.Volume = Sound.Volume.getValue( *manager );
		parameter.Pitch = Sound.Pitch.getValue( *manager );
		parameter.Pan = Sound.Pan.getValue( *manager );
		
		parameter.Mode3D = (Sound.PanType == ParameterSoundPanType_3D);
		Vector3D::Transform( parameter.Position, 
			Vector3D(0.0f, 0.0f, 0.0f), instance.GetGlobalMatrix43() );
		parameter.Distance = Sound.Distance;

		player->Play( tag, parameter );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNode::Create( Effect* effect, unsigned char*& pos )
{
	EffectNode* effectnode = NULL;

	int node_type = 0;
	memcpy( &node_type, pos, sizeof(int) );

	if( node_type == EFFECT_NODE_TYPE_ROOT )
	{
		EffekseerPrintDebug("* Create : EffectNodeRoot\n");
		effectnode = new EffectNodeRoot( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_NONE )
	{
		EffekseerPrintDebug("* Create : EffectNodeNone\n");
		effectnode = new EffectNode( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_SPRITE )
	{
		EffekseerPrintDebug("* Create : EffectNodeSprite\n");
		effectnode = new EffectNodeSprite( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_RIBBON )
	{
		EffekseerPrintDebug("* Create : EffectNodeRibbon\n");
		effectnode = new EffectNodeRibbon( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_RING )
	{
		EffekseerPrintDebug("* Create : EffectNodeRing\n");
		effectnode = new EffectNodeRing( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_MODEL )
	{
		EffekseerPrintDebug("* Create : EffectNodeModel\n");
		effectnode = new EffectNodeModel( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_TRACK )
	{
		EffekseerPrintDebug("* Create : EffectNodeTrack\n");
		effectnode = new EffectNodeTrack( effect, pos );
	}
	else
	{
		assert(0);
	}

	effectnode->LoadParameter( pos, effect->GetSetting());

	return effectnode;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
