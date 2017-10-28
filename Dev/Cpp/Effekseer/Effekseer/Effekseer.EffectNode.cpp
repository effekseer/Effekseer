

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
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
EffectNodeImplemented::EffectNodeImplemented(Effect* effect, unsigned char*& pos)
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
void EffectNodeImplemented::LoadParameter(unsigned char*& pos, EffectNode* parent, Setting* setting)
{
	int size = 0;
	int node_type = 0;
	auto ef = (EffectImplemented*) m_effect;

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
		if (m_effect->GetVersion() >= 10)
		{
			int32_t rendered = 0;
			memcpy(&rendered, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			IsRendered = rendered != 0;
		}

		memcpy( &size, pos, sizeof(int) );
		pos += sizeof(int);

		if (m_effect->GetVersion() >= 9)
		{
			assert(size == sizeof(ParameterCommonValues));
			memcpy(&CommonValues, pos, size);
			pos += size;
		}
		else
		{
			assert(size == sizeof(ParameterCommonValues_8));
			ParameterCommonValues_8 param_8;
			memcpy(&param_8, pos, size);
			pos += size;

			CommonValues.MaxGeneration = param_8.MaxGeneration;
			CommonValues.TranslationBindType = param_8.TranslationBindType;
			CommonValues.RotationBindType = param_8.RotationBindType;
			CommonValues.ScalingBindType = param_8.ScalingBindType;
			CommonValues.RemoveWhenLifeIsExtinct = param_8.RemoveWhenLifeIsExtinct;
			CommonValues.RemoveWhenParentIsRemoved = param_8.RemoveWhenParentIsRemoved;
			CommonValues.RemoveWhenChildrenIsExtinct = param_8.RemoveWhenChildrenIsExtinct;
			CommonValues.life = param_8.life;
			CommonValues.GenerationTime.max = param_8.GenerationTime;
			CommonValues.GenerationTime.min = param_8.GenerationTime;
			CommonValues.GenerationTimeOffset.max = param_8.GenerationTimeOffset;
			CommonValues.GenerationTimeOffset.min = param_8.GenerationTimeOffset;
		}
		
		memcpy( &TranslationType, pos, sizeof(int) );
		pos += sizeof(int);

		if( TranslationType == ParameterTranslationType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParameterTranslationFixed) );
			memcpy( &TranslationFixed, pos, size );
			pos += size;

			// 無効化
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

		/* 位置拡大処理 */
		if (ef->IsDyanamicMagnificationValid())
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

		// Calc attraction forces
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
		else if( LocationAbs.type == LocationAbsParameter::AttractiveForce )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(LocationAbs.attractiveForce) );
			memcpy( &LocationAbs.attractiveForce, pos, size );
			pos += size;
		}

		// Magnify attraction forces
		if (ef->IsDyanamicMagnificationValid())
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

			// 無効化
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

			// 無効化
			if( ScalingFixed.Position.X == 1.0f &&
				ScalingFixed.Position.Y == 1.0f &&
				ScalingFixed.Position.Z == 1.0f)
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

		/* Spawning Method */
		GenerationLocation.load( pos, m_effect->GetVersion());

		/* Spawning Method 拡大処理*/
		if (ef->IsDyanamicMagnificationValid()
			/* && (this->CommonValues.ScalingBindType == BindType::NotBind || parent->GetType() == EFFECT_NODE_TYPE_ROOT)*/ )
		{
			if( GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
			{
				GenerationLocation.point.location.min *= m_effect->GetMaginification();
				GenerationLocation.point.location.max *= m_effect->GetMaginification();
			}
			else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_LINE)
			{
				GenerationLocation.line.position_end.min *= m_effect->GetMaginification();
				GenerationLocation.line.position_end.max *= m_effect->GetMaginification();
				GenerationLocation.line.position_start.min *= m_effect->GetMaginification();
				GenerationLocation.line.position_start.max *= m_effect->GetMaginification();
				GenerationLocation.line.position_noize.min *= m_effect->GetMaginification();
				GenerationLocation.line.position_noize.max *= m_effect->GetMaginification();
			}
			else if( GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
			{
				GenerationLocation.sphere.radius.min *= m_effect->GetMaginification();
				GenerationLocation.sphere.radius.max *= m_effect->GetMaginification();
			}
			else if( GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE )
			{
				GenerationLocation.circle.radius.min *= m_effect->GetMaginification();
				GenerationLocation.circle.radius.max *= m_effect->GetMaginification();
			}		
		}

		// 右手系左手系変換
		if( setting->GetCoordinateSystem() == CoordinateSystem::LH )
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
			RendererCommon.load( pos, m_effect->GetVersion() );
		}
		else
		{
			RendererCommon.reset();
		}

		LoadRendererParameter( pos, m_effect->GetSetting() );
		
		if( m_effect->GetVersion() >= 1)
		{
			// サウンド
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

	// ノード
	int nodeCount = 0;
	memcpy( &nodeCount, pos, sizeof(int) );
	pos += sizeof( int );
	EffekseerPrintDebug("ChildrenCount : %d\n", nodeCount );
	m_Nodes.resize( nodeCount );
	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		m_Nodes[i] = EffectNodeImplemented::Create(m_effect, this, pos);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNodeImplemented::~EffectNodeImplemented()
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
Effect* EffectNodeImplemented::GetEffect() const
{
	return m_effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectNodeImplemented::GetChildrenCount() const
{
	return (int)m_Nodes.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNodeImplemented::GetChild(int index) const
{
	if (index >= GetChildrenCount()) return NULL;
	return m_Nodes[index];
}


EffectBasicRenderParameter EffectNodeImplemented::GetBasicRenderParameter()
{
	EffectBasicRenderParameter param;
	param.ColorTextureIndex = RendererCommon.ColorTextureIndex;
	param.AlphaBlend = RendererCommon.AlphaBlend;
	param.Distortion = RendererCommon.Distortion;
	param.DistortionIntensity = RendererCommon.DistortionIntensity;
	param.FilterType = RendererCommon.FilterType;
	param.WrapType = RendererCommon.WrapType;
	param.ZTest = RendererCommon.ZTest;
	param.ZWrite = RendererCommon.ZWrite;
	return param;
}

void EffectNodeImplemented::SetBasicRenderParameter(EffectBasicRenderParameter param)
{
	RendererCommon.ColorTextureIndex = param.ColorTextureIndex;
	RendererCommon.AlphaBlend = param.AlphaBlend;
	RendererCommon.Distortion = param.Distortion;
	RendererCommon.DistortionIntensity = param.DistortionIntensity;
	RendererCommon.FilterType = param.FilterType;
	RendererCommon.WrapType = param.WrapType;
	RendererCommon.ZTest = param.ZTest;
	RendererCommon.ZWrite = param.ZWrite;
}

EffectModelParameter EffectNodeImplemented::GetEffectModelParameter()
{
	EffectModelParameter param;
	param.Lighting = false;

	if (GetType() == EFFECT_NODE_TYPE_MODEL)
	{
		auto t = (EffectNodeModel*)this;
		param.Lighting = t->Lighting;
	}

	return param;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::LoadRendererParameter(unsigned char*& pos, Setting* setting)
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
void EffectNodeImplemented::BeginRendering(int32_t count, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::Rendering(const Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::EndRendering(Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float EffectNodeImplemented::GetFadeAlpha(const Instance& instance)
{
	float alpha = 1.0f;

	if( RendererCommon.FadeInType == ParameterRendererCommon::FADEIN_ON && instance.m_LivingTime < RendererCommon.FadeIn.Frame )
	{
		float v = 1.0f;
		RendererCommon.FadeIn.Value.setValueToArg( 
			v,
			0.0f,
			1.0f,
			(float)instance.m_LivingTime / (float)RendererCommon.FadeIn.Frame );

		alpha *= v;
	}

	if( RendererCommon.FadeOutType == ParameterRendererCommon::FADEOUT_ON && instance.m_LivingTime + RendererCommon.FadeOut.Frame > instance.m_LivedTime )
	{
		float v = 1.0f;
		RendererCommon.FadeOut.Value.setValueToArg( 
			v,
			1.0f,
			0.0f,
			(float)( instance.m_LivingTime + RendererCommon.FadeOut.Frame - instance.m_LivedTime ) / (float)RendererCommon.FadeOut.Frame );

		alpha *= v;
	}

	return alpha;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::PlaySound_(Instance& instance, SoundTag tag, Manager* manager)
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
EffectNodeImplemented* EffectNodeImplemented::Create(Effect* effect, EffectNode* parent, unsigned char*& pos)
{
	EffectNodeImplemented* effectnode = NULL;

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
		effectnode = new EffectNodeImplemented(effect, pos);
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

	effectnode->LoadParameter( pos, parent, effect->GetSetting());

	return effectnode;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
