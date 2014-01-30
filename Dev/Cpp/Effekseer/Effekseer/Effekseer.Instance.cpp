
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.Model.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer )
	: m_pManager			( pManager )
	, m_pEffectNode		( pEffectNode )
	, m_pContainer			( pContainer )
	, m_headGroups		( NULL )
	, m_pParent			( NULL )
	, m_LocalScaling	( 1.0f, 1.0f, 1.0f )
	, m_State			( INSTANCE_STATE_ACTIVE )
	, m_LivedTime		( 0 )
	, m_LivingTime		( 0 )
	, m_stepTime		( false )
	, m_sequenceNumber	( 0 )

{
	InstanceGroup* group = NULL;

	for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++ )
	{
		InstanceContainer* pContainer = m_pContainer->GetChild( i );

		if( group != NULL )
		{
			group->NextUsedByInstance = pContainer->CreateGroup();
			group = group->NextUsedByInstance;
		}
		else
		{
			group = pContainer->CreateGroup();
			m_headGroups = group;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::~Instance()
{
	assert( m_State != INSTANCE_STATE_ACTIVE );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eInstanceState Instance::GetState() const
{
	return m_State;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Matrix43& Instance::GetGlobalMatrix43() const
{
	return m_GlobalMatrix43;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Initialize( Instance* parent, int32_t instanceNumber )
{
	EffectNode* parameter = m_pEffectNode;

	// 親の設定
	m_pParent = parent;

	if( m_pParent == NULL )
	{
		// 状態の初期化
		m_State = INSTANCE_STATE_ACTIVE;

		// 時間周りの初期化
		m_LivingTime = 0.0f;
		m_LivedTime = FLT_MAX;

		// SRTの初期化
		m_LocalPosition = Vector3D( 0.0f, 0.0f, 0.0f );
		m_LocalAngle = Vector3D( 0.0f, 0.0f, 0.0f );
		m_LocalScaling = Vector3D( 1.0f, 1.0f, 1.0f );
		m_generation_location.Indentity();
		m_GlobalMatrix43.Indentity();
		m_ParentMatrix43.Indentity();

		// 親の初期化
		m_ParentMatrix43 = GetGlobalMatrix43();

		return;
	}


	// 状態の初期化
	m_State = INSTANCE_STATE_ACTIVE;

	// 時間周りの初期化
	m_LivingTime = 0.0f;
	m_LivedTime = (float)parameter->CommonValues.life.getValue( *m_pManager );
	
	// SRTの初期化
	m_LocalPosition = Vector3D( 0.0f, 0.0f, 0.0f );
	m_globalRevisionLocation.reset();
	m_LocalAngle = Vector3D( 0.0f, 0.0f, 0.0f );
	m_LocalScaling = Vector3D( 1.0f, 1.0f, 1.0f );
	m_generation_location.Indentity();
	m_GlobalMatrix43.Indentity();
	m_ParentMatrix43.Indentity();

	// 親の初期化
	if( parameter->CommonValues.TranslationBindType == BindType_WhenCreating )
	{
		m_ParentMatrix43.Value[3][0] = m_pParent->m_GlobalMatrix43.Value[3][0];
		m_ParentMatrix43.Value[3][1] = m_pParent->m_GlobalMatrix43.Value[3][1];
		m_ParentMatrix43.Value[3][2] = m_pParent->m_GlobalMatrix43.Value[3][2];
	}

	if( parameter->CommonValues.RotationBindType == BindType_WhenCreating &&
		parameter->CommonValues.ScalingBindType == BindType_WhenCreating )
	{
		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_pParent->m_GlobalMatrix43.Value[m][n];
			}
		}
	}
	else if ( parameter->CommonValues.RotationBindType == BindType_WhenCreating )
	{
		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_pParent->m_GlobalMatrix43.Value[m][n];
			}
		}

		float s[3];
		for( int m = 0; m < 3; m++ )
		{
			s[m] = 0;
			for( int n = 0; n < 3; n++ )
			{
				s[m] += m_ParentMatrix43.Value[m][n] * m_ParentMatrix43.Value[m][n];
			}
			s[m] = sqrt( s[m] );
		}

		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_ParentMatrix43.Value[m][n] / s[m];
			}
		}
	}
	else if ( parameter->CommonValues.ScalingBindType == BindType_WhenCreating )
	{
		float s[3];
		for( int m = 0; m < 3; m++ )
		{
			s[m] = 0;
			for( int n = 0; n < 3; n++ )
			{
				s[m] += m_pParent->m_GlobalMatrix43.Value[m][n] * m_pParent->m_GlobalMatrix43.Value[m][n];
			}
			s[m] = sqrt( s[m] );
		}
		m_ParentMatrix43.Value[0][0] = s[0];
		m_ParentMatrix43.Value[1][1] = s[1];
		m_ParentMatrix43.Value[2][2] = s[2];
	}



	/* 設定適用 */

	/* 位置 */
	if( m_pEffectNode->TranslationType == ParameterTranslationType_Fixed )
	{
		m_LocalPosition.X = m_pEffectNode->TranslationFixed.Position.X;
		m_LocalPosition.Y = m_pEffectNode->TranslationFixed.Position.Y;
		m_LocalPosition.Z = m_pEffectNode->TranslationFixed.Position.Z;
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_PVA )
	{
		translation_values.random.location = m_pEffectNode->TranslationPVA.location.getValue( *m_pManager );
		translation_values.random.velocity = m_pEffectNode->TranslationPVA.velocity.getValue( *m_pManager );
		translation_values.random.acceleration = m_pEffectNode->TranslationPVA.acceleration.getValue( *m_pManager );
	
		m_LocalPosition.X = translation_values.random.location.x;
		m_LocalPosition.Y = translation_values.random.location.y;
		m_LocalPosition.Z = translation_values.random.location.z;
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_Easing )
	{
		translation_values.easing.start = m_pEffectNode->TranslationEasing.start.getValue( *m_pManager );
		translation_values.easing.end = m_pEffectNode->TranslationEasing.end.getValue( *m_pManager );
		
		m_LocalPosition.X = translation_values.easing.start.x;
		m_LocalPosition.Y = translation_values.easing.start.y;
		m_LocalPosition.Z = translation_values.easing.start.z;
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_FCurve )
	{
		assert( m_pEffectNode->TranslationFCurve != NULL );

		translation_values.fcruve.offset.x = m_pEffectNode->TranslationFCurve->X.GetOffset( *m_pManager );
		translation_values.fcruve.offset.y = m_pEffectNode->TranslationFCurve->Y.GetOffset( *m_pManager );
		translation_values.fcruve.offset.z = m_pEffectNode->TranslationFCurve->Z.GetOffset( *m_pManager );

		m_LocalPosition.X = m_pEffectNode->TranslationFCurve->X.GetValue( 0 ) + translation_values.fcruve.offset.x;
		m_LocalPosition.Y = m_pEffectNode->TranslationFCurve->Y.GetValue( 0 ) + translation_values.fcruve.offset.y;
		m_LocalPosition.Z = m_pEffectNode->TranslationFCurve->Z.GetValue( 0 ) + translation_values.fcruve.offset.z;
	}

	/* 絶対位置 */
	if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::None )
	{
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::Gravity )
	{
		translation_abs_values.gravity.velocity.reset();
	}
	
	/* 回転 */
	if( m_pEffectNode->RotationType == ParameterRotationType_Fixed )
	{
		m_LocalAngle.X = m_pEffectNode->RotationFixed.Position.X;
		m_LocalAngle.Y = m_pEffectNode->RotationFixed.Position.Y;
		m_LocalAngle.Z = m_pEffectNode->RotationFixed.Position.Z;
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_PVA )
	{
		rotation_values.random.rotation = m_pEffectNode->RotationPVA.rotation.getValue( *m_pManager );
		rotation_values.random.velocity = m_pEffectNode->RotationPVA.velocity.getValue( *m_pManager );
		rotation_values.random.acceleration = m_pEffectNode->RotationPVA.acceleration.getValue( *m_pManager );

		m_LocalAngle.X = rotation_values.random.rotation.x;
		m_LocalAngle.Y = rotation_values.random.rotation.y;
		m_LocalAngle.Z = rotation_values.random.rotation.z;
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_Easing )
	{
		rotation_values.easing.start = m_pEffectNode->RotationEasing.start.getValue( *m_pManager );
		rotation_values.easing.end = m_pEffectNode->RotationEasing.end.getValue( *m_pManager );
		rotation_values.easing.start.setValueToArg( m_LocalAngle );
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA )
	{
		m_pEffectNode->RotationAxisPVA.axis.getValue( *m_pManager ).setValueToArg( m_LocalAngle );
		float length2 = Vector3D::Dot( m_LocalAngle, m_LocalAngle );
		if( length2 != 0.0f )
			Vector3D::Normal( m_LocalAngle, m_LocalAngle );
		else
			m_LocalAngle = Vector3D(0.0f, 1.0f, 0.0f);
		rotation_values.axis.random.rotation = m_pEffectNode->RotationAxisPVA.rotation.getValue( *m_pManager );
		rotation_values.axis.random.velocity = m_pEffectNode->RotationAxisPVA.velocity.getValue( *m_pManager );
		rotation_values.axis.random.acceleration = m_pEffectNode->RotationAxisPVA.acceleration.getValue( *m_pManager );
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
	{
		m_pEffectNode->RotationAxisEasing.axis.getValue( *m_pManager ).setValueToArg( m_LocalAngle );
		float length2 = Vector3D::Dot( m_LocalAngle, m_LocalAngle );
		if( length2 != 0.0f )
			Vector3D::Normal( m_LocalAngle, m_LocalAngle );
		else
			m_LocalAngle = Vector3D(0.0f, 1.0f, 0.0f);
		rotation_values.axis.easing.start = m_pEffectNode->RotationAxisEasing.easing.start.getValue( *m_pManager );
		rotation_values.axis.easing.end = m_pEffectNode->RotationAxisEasing.easing.end.getValue( *m_pManager );
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_FCurve )
	{
		assert( m_pEffectNode->RotationFCurve != NULL );

		rotation_values.fcruve.offset.x = m_pEffectNode->RotationFCurve->X.GetOffset( *m_pManager );
		rotation_values.fcruve.offset.y = m_pEffectNode->RotationFCurve->Y.GetOffset( *m_pManager );
		rotation_values.fcruve.offset.z = m_pEffectNode->RotationFCurve->Z.GetOffset( *m_pManager );

		m_LocalAngle.X = m_pEffectNode->RotationFCurve->X.GetValue( 0 ) + rotation_values.fcruve.offset.x;
		m_LocalAngle.Y = m_pEffectNode->RotationFCurve->Y.GetValue( 0 ) + rotation_values.fcruve.offset.y;
		m_LocalAngle.Z = m_pEffectNode->RotationFCurve->Z.GetValue( 0 ) + rotation_values.fcruve.offset.z;
	}

	/* 拡大縮小 */
	if( m_pEffectNode->ScalingType == ParameterScalingType_Fixed )
	{
		m_LocalScaling.X = m_pEffectNode->ScalingFixed.Position.X;
		m_LocalScaling.Y = m_pEffectNode->ScalingFixed.Position.Y;
		m_LocalScaling.Z = m_pEffectNode->ScalingFixed.Position.Z;
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_PVA )
	{
		scaling_values.random.scale = m_pEffectNode->ScalingPVA.Position.getValue( *m_pManager );
		scaling_values.random.velocity = m_pEffectNode->ScalingPVA.Velocity.getValue( *m_pManager );
		scaling_values.random.acceleration = m_pEffectNode->ScalingPVA.Acceleration.getValue( *m_pManager );
		m_LocalScaling.X = scaling_values.random.scale.x;
		m_LocalScaling.Y = scaling_values.random.scale.y;
		m_LocalScaling.Z = scaling_values.random.scale.z;
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_Easing )
	{
		scaling_values.easing.start = m_pEffectNode->ScalingEasing.start.getValue( *m_pManager );
		scaling_values.easing.end = m_pEffectNode->ScalingEasing.end.getValue( *m_pManager );
		scaling_values.easing.start.setValueToArg( m_LocalScaling );
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA )
	{
		scaling_values.single_random.scale = m_pEffectNode->ScalingSinglePVA.Position.getValue( *m_pManager );
		scaling_values.single_random.velocity = m_pEffectNode->ScalingSinglePVA.Velocity.getValue( *m_pManager );
		scaling_values.single_random.acceleration = m_pEffectNode->ScalingSinglePVA.Acceleration.getValue( *m_pManager );
		m_LocalScaling.X = scaling_values.single_random.scale;
		m_LocalScaling.Y = scaling_values.single_random.scale;
		m_LocalScaling.Z = scaling_values.single_random.scale;
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing )
	{
		scaling_values.single_easing.start = m_pEffectNode->ScalingSingleEasing.start.getValue( *m_pManager );
		scaling_values.single_easing.end = m_pEffectNode->ScalingSingleEasing.end.getValue( *m_pManager );
		m_LocalScaling.X = scaling_values.single_easing.start;
		m_LocalScaling.Y = scaling_values.single_easing.start;
		m_LocalScaling.Z = scaling_values.single_easing.start;
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_FCurve )
	{
		assert( m_pEffectNode->ScalingFCurve != NULL );

		scaling_values.fcruve.offset.x = m_pEffectNode->ScalingFCurve->X.GetOffset( *m_pManager );
		scaling_values.fcruve.offset.y = m_pEffectNode->ScalingFCurve->Y.GetOffset( *m_pManager );
		scaling_values.fcruve.offset.z = m_pEffectNode->ScalingFCurve->Z.GetOffset( *m_pManager );

		m_LocalScaling.X = m_pEffectNode->ScalingFCurve->X.GetValue( 0 ) + scaling_values.fcruve.offset.x;
		m_LocalScaling.Y = m_pEffectNode->ScalingFCurve->Y.GetValue( 0 ) + scaling_values.fcruve.offset.y;
		m_LocalScaling.Z = m_pEffectNode->ScalingFCurve->Z.GetValue( 0 ) + scaling_values.fcruve.offset.z;
	}

	/* 生成位置 */
	if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
	{
		vector3d p = m_pEffectNode->GenerationLocation.point.location.getValue( *m_pManager );
		m_generation_location.Translation( p.x, p.y, p.z );
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
	{
		Matrix43 mat_x, mat_y;
		mat_x.RotationX( m_pEffectNode->GenerationLocation.sphere.rotation_x.getValue( *m_pManager ) );
		mat_y.RotationY( m_pEffectNode->GenerationLocation.sphere.rotation_y.getValue( *m_pManager ) );
		float r = m_pEffectNode->GenerationLocation.sphere.radius.getValue( *m_pManager );
		m_generation_location.Translation( 0, r, 0 );
		Matrix43::Multiple( m_generation_location, m_generation_location, mat_x );
		Matrix43::Multiple( m_generation_location, m_generation_location, mat_y );
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_MODEL )
	{
		m_generation_location.Indentity();

		int32_t modelIndex = m_pEffectNode->GenerationLocation.model.index;
		if( modelIndex >= 0 )
		{
			Model* model = (Model*)m_pEffectNode->GetEffect()->GetModel( modelIndex );
			if( model != NULL )
			{
				Model::Emitter emitter;
				
				if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_RANDOM )
				{
					emitter = model->GetEmitter( 
						m_pManager, 
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX )
				{
					emitter = model->GetEmitterFromVertex( 
						instanceNumber,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX_RANDOM )
				{
					emitter = model->GetEmitterFromVertex( 
						m_pManager,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE )
				{
					emitter = model->GetEmitterFromFace( 
						instanceNumber,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE_RANDOM )
				{
					emitter = model->GetEmitterFromFace( 
						m_pManager,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}

				m_generation_location.Translation( 
					emitter.Position.X, 
					emitter.Position.Y,
					emitter.Position.Z );

				if( m_pEffectNode->GenerationLocation.EffectsRotation )
				{
					m_generation_location.Value[0][0] = emitter.Binormal.X;
					m_generation_location.Value[0][1] = emitter.Binormal.Y;
					m_generation_location.Value[0][2] = emitter.Binormal.Z;

					m_generation_location.Value[1][0] = emitter.Tangent.X;
					m_generation_location.Value[1][1] = emitter.Tangent.Y;
					m_generation_location.Value[1][2] = emitter.Tangent.Z;

					m_generation_location.Value[2][0] = emitter.Normal.X;
					m_generation_location.Value[2][1] = emitter.Normal.Y;
					m_generation_location.Value[2][2] = emitter.Normal.Z;
				}
			}
		}
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE )
	{
		m_generation_location.Indentity();
		float radius = m_pEffectNode->GenerationLocation.circle.radius.getValue(*m_pManager);
		float start = m_pEffectNode->GenerationLocation.circle.angle_start.getValue(*m_pManager);
		float end = m_pEffectNode->GenerationLocation.circle.angle_end.getValue(*m_pManager);
		int32_t div = Max(m_pEffectNode->GenerationLocation.circle.division, 1);

		int32_t target = 0;
		if(m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_ORDER)
		{
			target = instanceNumber % div;
		}
		else if(m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_REVERSE_ORDER)
		{
			target = div - 1 - (instanceNumber % div);
		}
		else if(m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_RANDOM)
		{
			RandFunc randFunc = m_pManager->GetRandFunc();
			int32_t randMax = m_pManager->GetRandMax();

			target = (int32_t)( (div - 1) * ( (float)randFunc() / (float)randMax ) );
		}

		float angle = (end - start) * ((float)target / (float)div) + start;
		Matrix43 mat;
		mat.RotationZ( angle );

		m_generation_location.Translation( 0, radius, 0 );
		Matrix43::Multiple( m_generation_location, m_generation_location, mat );
	}

	if( !m_pEffectNode->GenerationLocation.EffectsRotation )
	{
		m_LocalPosition.X += m_generation_location.Value[3][0];
		m_LocalPosition.Y += m_generation_location.Value[3][1];
		m_LocalPosition.Z += m_generation_location.Value[3][2];
	}

	if( m_pEffectNode->SoundType == ParameterSoundType_Use )
	{
		soundValues.delay = m_pEffectNode->Sound.Delay.getValue( *m_pManager );
	}

	m_pEffectNode->InitializeRenderedInstance( *this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Update( float deltaFrame, bool shown )
{
	if( m_stepTime && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		/* 音の更新(現状放置) */
		if( m_pEffectNode->SoundType == ParameterSoundType_Use )
		{
			float living_time = m_LivingTime;
			float living_time_p = living_time + deltaFrame;
			
			if( living_time <= (float)soundValues.delay && (float)soundValues.delay < living_time_p )
			{
				m_pEffectNode->PlaySound_( *this, m_pContainer->GetRootInstance() );
			}
		}
	}

	float originalTime = m_LivingTime;
	bool calculateMatrix = false;

	if(shown)
	{
		calculateMatrix = true;
	}
	else
	{
		/**
			見えないケースで行列計算が必要なケース 
			-子が生成される。
			-子の子が生成される。
		*/
		if( m_stepTime && (originalTime <= m_LivedTime || !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct) )
		{
			for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++ )
			{
				EffectNode* pNode = m_pEffectNode->GetChild( i );

				// インスタンス生成
			float living_time = originalTime - pNode->CommonValues.GenerationTimeOffset + Max(0.0f,pNode->CommonValues.GenerationTime-1.0f);
			float living_time_p = living_time + deltaFrame;

			living_time = Max( 0.0f, living_time );
			living_time_p = Max( 0.0f, living_time_p );

			int generation_count = (int)(living_time / pNode->CommonValues.GenerationTime);
			int generation_count_p = (int)(living_time_p / pNode->CommonValues.GenerationTime);

			generation_count = Min( generation_count, pNode->CommonValues.MaxGeneration );
			generation_count_p = Min( generation_count_p, pNode->CommonValues.MaxGeneration );

				if( generation_count != generation_count_p )
				{
					calculateMatrix = true;
					break;
				}
			}
		}
	}

	/* 親が破棄される瞬間に行列計算(条件を絞れば更に最適化可能) */
	if( !calculateMatrix && m_pParent != NULL && m_pParent->GetState() != INSTANCE_STATE_ACTIVE )
	{
		calculateMatrix = true;
	}

	if( calculateMatrix )
	{
		CalculateMatrix( deltaFrame );
	}

	/* 時間の進行 */
	if(  m_stepTime )
	{
		m_LivingTime += deltaFrame;
	}

	// 子の処理
	if( m_stepTime && (originalTime <= m_LivedTime || !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct) )
	{
		InstanceGroup* group = m_headGroups;

		for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance )
		{
			EffectNode* pNode = m_pEffectNode->GetChild( i );
			InstanceContainer* pContainer = m_pContainer->GetChild( i );
			assert( group != NULL );

			// インスタンス生成
			float living_time = originalTime - pNode->CommonValues.GenerationTimeOffset + Max(0.0f,pNode->CommonValues.GenerationTime-1.0f);
			float living_time_p = living_time + deltaFrame;

			living_time = Max( 0.0f, living_time );
			living_time_p = Max( 0.0f, living_time_p );

			int generation_count = (int)(living_time / pNode->CommonValues.GenerationTime);
			int generation_count_p = (int)(living_time_p / pNode->CommonValues.GenerationTime);

			generation_count = Min( generation_count, pNode->CommonValues.MaxGeneration );
			generation_count_p = Min( generation_count_p, pNode->CommonValues.MaxGeneration );

			for( int j = generation_count; j < generation_count_p; j++ )
			{
				// 生成処理
				Instance* pNewInstance = group->CreateInstance();
				if( pNewInstance != NULL )
				{
					pNewInstance->Initialize(this, j );
				}
				else
				{
					break;
				}
			}
		}
	}

	bool killed = false;
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{	
		// 死亡判定
		if( m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
		{
			if( m_LivingTime > m_LivedTime )
			{
				killed = true;
			}
		}

		if( !killed && m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct )
		{
			int maxcreate_count = 0;
			InstanceGroup* group = m_headGroups;

			for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance )
			{
				float last_generation_time = 
					m_pEffectNode->GetChild(i)->CommonValues.GenerationTime *
					(m_pEffectNode->GetChild(i)->CommonValues.MaxGeneration - 1) +
					m_pEffectNode->GetChild(i)->CommonValues.GenerationTimeOffset + 
					1.0f;

				if( m_LivingTime >= last_generation_time && group->GetInstanceCount() == 0 )
				{
					maxcreate_count++;
				}
				else
				{
					break;
				}
			}
			
			if( maxcreate_count == m_pEffectNode->GetChildrenCount() )
			{
				killed = true;
			}
		}
	}

	if(killed)
	{
		/* 死亡確定時、計算が必要な場合は計算をする。*/
		if( !calculateMatrix )
		{
			calculateMatrix = true;
			CalculateMatrix( deltaFrame );
		}

		/* 破棄 */
		Kill();
		return;
	}

	// 時間の進行許可
	m_stepTime = true;

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::CalculateMatrix( float deltaFrame )
{
	if( m_sequenceNumber == ((ManagerImplemented*)m_pManager)->GetSequenceNumber() ) return;
	m_sequenceNumber = ((ManagerImplemented*)m_pManager)->GetSequenceNumber();

	assert( m_pEffectNode != NULL );
	assert( m_pContainer != NULL );

	// 親の処理
	if( m_pParent != NULL )
	{
		/* 親の行列を更新(現在は必要不必要関わらず行なっている) */
		m_pParent->CalculateMatrix( deltaFrame );

		if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
		{
			BindType lType = m_pEffectNode->CommonValues.TranslationBindType;
			BindType rType = m_pEffectNode->CommonValues.RotationBindType;
			BindType sType = m_pEffectNode->CommonValues.ScalingBindType;

			const Matrix43* targetL;
			const Matrix43* targetR;
			const Matrix43* targetS;

			const Instance* rootInstance;
			InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();
			InstanceContainer* rootContainer = instanceGlobal->GetRootContainer();
			InstanceGroup* firstGloup = rootContainer->GetFirstGroup();
			if( firstGloup && firstGloup->GetInstanceCount() > 0 )
			{
				rootInstance = firstGloup->GetFirst();
			}
			else
			{
				rootInstance = NULL;
			}

			if( lType == BindType_Always )
			{
				targetL = &m_pParent->GetGlobalMatrix43();
			}
			else if( lType == BindType_NotBind_Root && rootInstance != NULL )
			{
				targetL = &rootInstance->GetGlobalMatrix43();
			}
			else
			{
				targetL = NULL;
			}

			if( rType == BindType_Always )
			{
				targetR = &m_pParent->GetGlobalMatrix43();
			}
			else if( rType == BindType_NotBind_Root && rootInstance != NULL )
			{
				targetR = &rootInstance->GetGlobalMatrix43();
			}
			else
			{
				targetR = NULL;
			}

			if( sType == BindType_Always )
			{
				targetS = &m_pParent->GetGlobalMatrix43();
			}
			else if( sType == BindType_NotBind_Root && rootInstance != NULL )
			{
				targetS = &rootInstance->GetGlobalMatrix43();
			}
			else
			{
				targetS = NULL;
			}

			if( (lType == BindType_Always && rType == BindType_Always && sType == BindType_Always) || 
				(lType == BindType_NotBind_Root && rType == BindType_NotBind_Root && sType == BindType_NotBind_Root) )
			{
				m_ParentMatrix43 = *targetL;
			}
			else
			{
				if( lType == BindType_Always || lType == BindType_NotBind_Root )
				{
					// 移動を毎回更新する場合
					m_ParentMatrix43.Value[3][0] = targetL->Value[3][0];
					m_ParentMatrix43.Value[3][1] = targetL->Value[3][1];
					m_ParentMatrix43.Value[3][2] = targetL->Value[3][2];
				}

				if( (rType == BindType_Always && sType == BindType_Always) || 
					(rType == BindType_NotBind_Root && sType == BindType_NotBind_Root ) )
				{
					// 回転と拡大を毎回更新する場合
					for( int i = 0; i < 3; i++ )
					{
						for( int j = 0; j < 3; j++ )
						{
							m_ParentMatrix43.Value[i][j] = targetR->Value[i][j];
						}
					}
				}
				else
				{
					if( rType == BindType_Always || rType == BindType_NotBind_Root )
					{
						// 回転を毎回更新する場合
						float s[3];
						for( int i = 0; i < 3; i++ )
						{
							s[i] = 0;
							for( int j = 0; j < 3; j++ )
							{
								s[i] += targetR->Value[i][j] * targetR->Value[i][j];
							}
							s[i] = sqrt( s[i] );
						}

						float s_parent[3];
						for( int i = 0; i < 3; i++ )
						{
							s_parent[i] = 0;
							for( int j = 0; j < 3; j++ )
							{
								s_parent[i] += m_ParentMatrix43.Value[i][j] * m_ParentMatrix43.Value[i][j];
							}
							s_parent[i] = sqrt(  s_parent[i] );
						}

						for( int i = 0; i < 3; i++ )
						{
							for( int j = 0; j < 3; j++ )
							{
								m_ParentMatrix43.Value[i][j] = targetR->Value[i][j] / s[i] * s_parent[i];
							}
						}
					}

					if( sType == BindType_Always || sType == BindType_NotBind_Root )
					{
						// 拡大を毎フレーム更新する場合
						float s[3];
						for( int i = 0; i < 3; i++ )
						{
							s[i] = 0;
							for( int j = 0; j < 3; j++ )
							{
								s[i] += targetS->Value[i][j] * targetS->Value[i][j];
							}
							s[i] = sqrt( s[i] );
						}

						float s_parent[3];
						for( int i = 0; i < 3; i++ )
						{
							s_parent[i] = 0;
							for( int j = 0; j < 3; j++ )
							{
								 s_parent[i] += m_ParentMatrix43.Value[i][j] * m_ParentMatrix43.Value[i][j];
							}
							s_parent[i] = sqrt(  s_parent[i] );
						}

						for( int i = 0; i < 3; i++ )
						{
							for( int j = 0; j < 3; j++ )
							{
								m_ParentMatrix43.Value[i][j] = m_ParentMatrix43.Value[i][j]  / s_parent[i] * s[i];
							}
						}
					}
				}
			}
		}
		else
		{
			// Rootの場合
			m_ParentMatrix43 = m_pParent->GetGlobalMatrix43();
		}
		
		if( m_pParent->GetState() == INSTANCE_STATE_ACTIVE )
		{
		}
		else
		{
			// 親が消えた場合、子も削除
			if( m_pEffectNode->GetType() == EFFECT_NODE_TYPE_ROOT )
			{
				
			}
			else
			{
				m_pParent = NULL;
				if( m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved )
				{
					Kill();
					return;
				}
			}
			
			m_pParent = NULL;
		}
	}

	/* 更新処理 */
	if( m_stepTime && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		/* 位置の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->TranslationType == ParameterTranslationType_None )
		{
			m_LocalPosition.X = 0;
			m_LocalPosition.Y = 0;
			m_LocalPosition.Z = 0;
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_Fixed )
		{
			m_LocalPosition.X = m_pEffectNode->TranslationFixed.Position.X;
			m_LocalPosition.Y = m_pEffectNode->TranslationFixed.Position.Y;
			m_LocalPosition.Z = m_pEffectNode->TranslationFixed.Position.Z;
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_PVA )
		{
			/* 若干計算結果がずれている */
			/*
			translation_values.random.velocity += translation_values.random.acceleration * deltaFrame;
			m_LocalPosition.X += translation_values.random.velocity.x * deltaFrame;
			m_LocalPosition.Y += translation_values.random.velocity.y * deltaFrame;
			m_LocalPosition.Z += translation_values.random.velocity.z * deltaFrame;
			*/

			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			m_LocalPosition.X = translation_values.random.location.x +
				(translation_values.random.velocity.x * m_LivingTime) +
				(translation_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalPosition.Y = translation_values.random.location.y +
				(translation_values.random.velocity.y * m_LivingTime) +
				(translation_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalPosition.Z = translation_values.random.location.z +
				(translation_values.random.velocity.z * m_LivingTime) +
				(translation_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);

		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_Easing )
		{
			m_pEffectNode->TranslationEasing.setValueToArg(
				m_LocalPosition,
				translation_values.easing.start,
				translation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_FCurve )
		{
			assert( m_pEffectNode->TranslationFCurve != NULL );
			m_LocalPosition.X = m_pEffectNode->TranslationFCurve->X.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.x;
			m_LocalPosition.Y = m_pEffectNode->TranslationFCurve->Y.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.y;
			m_LocalPosition.Z = m_pEffectNode->TranslationFCurve->Z.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.z;
		}

		if( !m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			m_LocalPosition.X += m_generation_location.Value[3][0];
			m_LocalPosition.Y += m_generation_location.Value[3][1];
			m_LocalPosition.Z += m_generation_location.Value[3][2];
		}

		/* 絶対位置の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::None )
		{	
		}
		else if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::Gravity )
		{
			m_globalRevisionLocation.x = m_pEffectNode->LocationAbs.gravity.x *
				m_LivingTime * m_LivingTime * 0.5f;
			m_globalRevisionLocation.y = m_pEffectNode->LocationAbs.gravity.y *
				m_LivingTime * m_LivingTime * 0.5f;
			m_globalRevisionLocation.z = m_pEffectNode->LocationAbs.gravity.z *
				m_LivingTime * m_LivingTime * 0.5f;
		}

		/* 回転の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->RotationType == ParameterRotationType_Fixed )
		{
		
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_PVA )
		{
			/* 若干計算結果がずれている */
			/*
			rotation_values.random.velocity += rotation_values.random.acceleration * deltaFrame;
			m_LocalAngle.X += rotation_values.random.velocity.x * deltaFrame;
			m_LocalAngle.Y += rotation_values.random.velocity.y * deltaFrame;
			m_LocalAngle.Z += rotation_values.random.velocity.z * deltaFrame;
			*/

			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			m_LocalAngle.X = rotation_values.random.rotation.x +
				(rotation_values.random.velocity.x * m_LivingTime) +
				(rotation_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalAngle.Y = rotation_values.random.rotation.y +
				(rotation_values.random.velocity.y * m_LivingTime) +
				(rotation_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalAngle.Z = rotation_values.random.rotation.z +
				(rotation_values.random.velocity.z * m_LivingTime) +
				(rotation_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);

		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_Easing )
		{
			m_pEffectNode->RotationEasing.setValueToArg(
				m_LocalAngle,
				rotation_values.easing.start,
				rotation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA )
		{
			/*
			rotation_values.axis.random.velocity += rotation_values.axis.random.acceleration * deltaFrame;
			rotation_values.axis.rotation += rotation_values.axis.random.velocity;
			*/
			rotation_values.axis.rotation = 
				rotation_values.axis.random.rotation +
				rotation_values.axis.random.velocity * m_LivingTime +
				rotation_values.axis.random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
		{
			m_pEffectNode->RotationAxisEasing.easing.setValueToArg(
				rotation_values.axis.rotation,
				rotation_values.axis.easing.start,
				rotation_values.axis.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_FCurve )
		{
			assert( m_pEffectNode->RotationFCurve != NULL );
			m_LocalAngle.X = m_pEffectNode->RotationFCurve->X.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.x;
			m_LocalAngle.Y = m_pEffectNode->RotationFCurve->Y.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.y;
			m_LocalAngle.Z = m_pEffectNode->RotationFCurve->Z.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.z;
		}

		/* 拡大の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->ScalingType == ParameterScalingType_Fixed )
		{
		
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_PVA )
		{
			/*
			scaling_values.random.velocity.x += scaling_values.random.acceleration.x * deltaFrame;
			scaling_values.random.velocity.y += scaling_values.random.acceleration.y * deltaFrame;
			scaling_values.random.velocity.z += scaling_values.random.acceleration.z * deltaFrame;
			m_LocalScaling.X += scaling_values.random.velocity.x * deltaFrame;
			m_LocalScaling.Y += scaling_values.random.velocity.y * deltaFrame;
			m_LocalScaling.Z += scaling_values.random.velocity.z * deltaFrame;
			*/

			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			m_LocalScaling.X = scaling_values.random.scale.x +
				(scaling_values.random.velocity.x * m_LivingTime) +
				(scaling_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalScaling.Y = scaling_values.random.scale.y +
				(scaling_values.random.velocity.y * m_LivingTime) +
				(scaling_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			m_LocalScaling.Z = scaling_values.random.scale.z +
				(scaling_values.random.velocity.z * m_LivingTime) +
				(scaling_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_Easing )
		{
			m_pEffectNode->ScalingEasing.setValueToArg(
				m_LocalScaling,
				scaling_values.easing.start,
				scaling_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA )
		{
			/*
			scaling_values.single_random.velocity += scaling_values.single_random.acceleration * deltaFrame;
			m_LocalScaling.X += scaling_values.single_random.velocity * deltaFrame;
			m_LocalScaling.Y += scaling_values.single_random.velocity * deltaFrame;
			m_LocalScaling.Z += scaling_values.single_random.velocity * deltaFrame;
			*/
			float s = scaling_values.single_random.scale +
				scaling_values.single_random.velocity * m_LivingTime +
				scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
			m_LocalScaling.X = s;
			m_LocalScaling.Y = s;
			m_LocalScaling.Z = s;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing )
		{
			float scale;
			m_pEffectNode->ScalingSingleEasing.setValueToArg(
				scale,
				scaling_values.single_easing.start,
				scaling_values.single_easing.end,
				m_LivingTime / m_LivedTime );
			m_LocalScaling.X = scale;
			m_LocalScaling.Y = scale;
			m_LocalScaling.Z = scale;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_FCurve )
		{
			assert( m_pEffectNode->ScalingFCurve != NULL );

			m_LocalScaling.X = m_pEffectNode->ScalingFCurve->X.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.x;
			m_LocalScaling.Y = m_pEffectNode->ScalingFCurve->Y.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.y;
			m_LocalScaling.Z = m_pEffectNode->ScalingFCurve->Z.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.z;
		}

		/* 描画部分の更新 */
		m_pEffectNode->UpdateRenderedInstance( *this );
	}
	
	// 行列の更新
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		m_GlobalMatrix43.Scaling( m_LocalScaling.X, m_LocalScaling.Y,  m_LocalScaling.Z );

		//MatScale.Scaling( m_LocalScaling.X, m_LocalScaling.Y,  m_LocalScaling.Z );
		//m_GlobalMatrix43 = MatScale;
			
		if( m_pEffectNode->RotationType == ParameterRotationType_Fixed ||
			m_pEffectNode->RotationType == ParameterRotationType_PVA ||
			m_pEffectNode->RotationType == ParameterRotationType_Easing ||
			m_pEffectNode->RotationType == ParameterRotationType_FCurve )
		{
			Matrix43 MatRot;
			MatRot.RotationZXY( m_LocalAngle.Z, m_LocalAngle.X, m_LocalAngle.Y );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRot );
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA ||
				 m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
		{
			Matrix43 MatRot;
			MatRot.RotationAxis( m_LocalAngle, rotation_values.axis.rotation );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRot );
		}

		if( m_LocalPosition.X != 0.0f ||
			m_LocalPosition.Y != 0.0f || 
			m_LocalPosition.Z != 0.0f )
		{
			Matrix43 MatTra;
			MatTra.Translation( m_LocalPosition.X, m_LocalPosition.Y, m_LocalPosition.Z );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTra );
		}

		if( m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_generation_location );
		}

		Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_ParentMatrix43 );
		
		Matrix43 MatTraGlobal;
		MatTraGlobal.Translation( m_globalRevisionLocation.x, m_globalRevisionLocation.y, m_globalRevisionLocation.z );
		Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTraGlobal );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Draw()
{
	assert( m_pEffectNode != NULL );

	if( !m_pEffectNode->IsRendered ) return;

	if( m_sequenceNumber != ((ManagerImplemented*)m_pManager)->GetSequenceNumber() )
	{
		CalculateMatrix( 0 );
	}

	m_pEffectNode->Rendering(*this, m_pManager);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Kill()
{
	if( m_State == INSTANCE_STATE_ACTIVE )
	{
		for( InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByInstance )
		{
			group->IsReferencedFromInstance = false;
		}

		m_State = INSTANCE_STATE_REMOVING;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RectF Instance::GetUV() const
{
	if( m_pEffectNode->Texture.UVType == ParameterTexture::UV_DEFAULT )
	{
		return RectF( 0.0f, 0.0f, 1.0f, 1.0f );
	}
	else if( m_pEffectNode->Texture.UVType == ParameterTexture::UV_FIXED )
	{
		return RectF(
			m_pEffectNode->Texture.UV.Fixed.Position.x,
			m_pEffectNode->Texture.UV.Fixed.Position.y,
			m_pEffectNode->Texture.UV.Fixed.Position.w,
			m_pEffectNode->Texture.UV.Fixed.Position.h );
	}
	else if( m_pEffectNode->Texture.UVType == ParameterTexture::UV_ANIMATION )
	{
		int32_t frameNum = (int32_t)(m_LivingTime / m_pEffectNode->Texture.UV.Animation.FrameLength);
		int32_t frameCount = m_pEffectNode->Texture.UV.Animation.FrameCountX * m_pEffectNode->Texture.UV.Animation.FrameCountY;

		if( m_pEffectNode->Texture.UV.Animation.LoopType == m_pEffectNode->Texture.UV.Animation.LOOPTYPE_ONCE )
		{
			if( frameNum >= frameCount )
			{
				frameNum = frameCount - 1;
			}
		}
		else if ( m_pEffectNode->Texture.UV.Animation.LoopType == m_pEffectNode->Texture.UV.Animation.LOOPTYPE_LOOP )
		{
			frameNum %= frameCount;
		}
		else if ( m_pEffectNode->Texture.UV.Animation.LoopType == m_pEffectNode->Texture.UV.Animation.LOOPTYPE_REVERSELOOP )
		{
			bool rev = (frameNum / frameCount) % 2 == 1;
			frameNum %= frameCount;
			if( rev )
			{
				frameNum = frameCount - 1 - frameNum;
			}
		}

		int32_t frameX = frameNum % m_pEffectNode->Texture.UV.Animation.FrameCountX;
		int32_t frameY = frameNum / m_pEffectNode->Texture.UV.Animation.FrameCountX;

		return RectF(
			m_pEffectNode->Texture.UV.Animation.Position.x + m_pEffectNode->Texture.UV.Animation.Position.w * frameX,
			m_pEffectNode->Texture.UV.Animation.Position.y + m_pEffectNode->Texture.UV.Animation.Position.h * frameY,
			m_pEffectNode->Texture.UV.Animation.Position.w,
			m_pEffectNode->Texture.UV.Animation.Position.h );
	}
	else if( m_pEffectNode->Texture.UVType == ParameterTexture::UV_SCROLL )
	{
		return RectF(
			m_pEffectNode->Texture.UV.Scroll.Position.x + m_pEffectNode->Texture.UV.Scroll.Speed.x * m_LivingTime,
			m_pEffectNode->Texture.UV.Scroll.Position.y + m_pEffectNode->Texture.UV.Scroll.Speed.y * m_LivingTime,
			m_pEffectNode->Texture.UV.Scroll.Position.w,
			m_pEffectNode->Texture.UV.Scroll.Position.h );
	}

	return RectF( 0.0f, 0.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------