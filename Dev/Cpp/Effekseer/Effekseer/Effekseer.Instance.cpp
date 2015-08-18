
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

	// 子の初期化
	for (int32_t i = 0; i < Min(ChildrenMax, parameter->GetChildrenCount()); i++)
	{
		EffectNode* pNode = parameter->GetChild(i);

		m_generatedChildrenCount[i] = 0;
		m_nextGenerationTime[i] = pNode->CommonValues.GenerationTimeOffset.getValue(*m_pManager);
	}

	if( m_pParent == NULL )
	{
		// ROOTの場合

		// 状態の初期化
		m_State = INSTANCE_STATE_ACTIVE;

		// 時間周りの初期化
		m_LivingTime = 0.0f;
		m_LivedTime = FLT_MAX;

		// SRTの初期化
		m_GenerationLocation.Indentity();
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
	m_pParent->GetGlobalMatrix43().GetTranslation( m_GlobalPosition );
	m_GlobalRevisionLocation = Vector3D(0.0f, 0.0f, 0.0f);
	m_GlobalRevisionVelocity = Vector3D(0.0f, 0.0f, 0.0f);
	m_GenerationLocation.Indentity();
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
	
	/* 位置 */
	if( m_pEffectNode->TranslationType == ParameterTranslationType_Fixed )
	{
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_PVA )
	{
		translation_values.random.location = m_pEffectNode->TranslationPVA.location.getValue( *m_pManager );
		translation_values.random.velocity = m_pEffectNode->TranslationPVA.velocity.getValue( *m_pManager );
		translation_values.random.acceleration = m_pEffectNode->TranslationPVA.acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_Easing )
	{
		translation_values.easing.start = m_pEffectNode->TranslationEasing.start.getValue( *m_pManager );
		translation_values.easing.end = m_pEffectNode->TranslationEasing.end.getValue( *m_pManager );
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_FCurve )
	{
		assert( m_pEffectNode->TranslationFCurve != NULL );

		translation_values.fcruve.offset.x = m_pEffectNode->TranslationFCurve->X.GetOffset( *m_pManager );
		translation_values.fcruve.offset.y = m_pEffectNode->TranslationFCurve->Y.GetOffset( *m_pManager );
		translation_values.fcruve.offset.z = m_pEffectNode->TranslationFCurve->Z.GetOffset( *m_pManager );
	}
	
	/* 回転 */
	if( m_pEffectNode->RotationType == ParameterRotationType_Fixed )
	{
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_PVA )
	{
		rotation_values.random.rotation = m_pEffectNode->RotationPVA.rotation.getValue( *m_pManager );
		rotation_values.random.velocity = m_pEffectNode->RotationPVA.velocity.getValue( *m_pManager );
		rotation_values.random.acceleration = m_pEffectNode->RotationPVA.acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_Easing )
	{
		rotation_values.easing.start = m_pEffectNode->RotationEasing.start.getValue( *m_pManager );
		rotation_values.easing.end = m_pEffectNode->RotationEasing.end.getValue( *m_pManager );
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA )
	{
		rotation_values.axis.random.rotation = m_pEffectNode->RotationAxisPVA.rotation.getValue( *m_pManager );
		rotation_values.axis.random.velocity = m_pEffectNode->RotationAxisPVA.velocity.getValue( *m_pManager );
		rotation_values.axis.random.acceleration = m_pEffectNode->RotationAxisPVA.acceleration.getValue( *m_pManager );
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisPVA.axis.getValue(*m_pManager);
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
	{
		rotation_values.axis.easing.start = m_pEffectNode->RotationAxisEasing.easing.start.getValue( *m_pManager );
		rotation_values.axis.easing.end = m_pEffectNode->RotationAxisEasing.easing.end.getValue( *m_pManager );
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisEasing.axis.getValue(*m_pManager);
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_FCurve )
	{
		assert( m_pEffectNode->RotationFCurve != NULL );

		rotation_values.fcruve.offset.x = m_pEffectNode->RotationFCurve->X.GetOffset( *m_pManager );
		rotation_values.fcruve.offset.y = m_pEffectNode->RotationFCurve->Y.GetOffset( *m_pManager );
		rotation_values.fcruve.offset.z = m_pEffectNode->RotationFCurve->Z.GetOffset( *m_pManager );
	}

	/* 拡大縮小 */
	if( m_pEffectNode->ScalingType == ParameterScalingType_Fixed )
	{
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_PVA )
	{
		scaling_values.random.scale = m_pEffectNode->ScalingPVA.Position.getValue( *m_pManager );
		scaling_values.random.velocity = m_pEffectNode->ScalingPVA.Velocity.getValue( *m_pManager );
		scaling_values.random.acceleration = m_pEffectNode->ScalingPVA.Acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_Easing )
	{
		scaling_values.easing.start = m_pEffectNode->ScalingEasing.start.getValue( *m_pManager );
		scaling_values.easing.end = m_pEffectNode->ScalingEasing.end.getValue( *m_pManager );
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA )
	{
		scaling_values.single_random.scale = m_pEffectNode->ScalingSinglePVA.Position.getValue( *m_pManager );
		scaling_values.single_random.velocity = m_pEffectNode->ScalingSinglePVA.Velocity.getValue( *m_pManager );
		scaling_values.single_random.acceleration = m_pEffectNode->ScalingSinglePVA.Acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing )
	{
		scaling_values.single_easing.start = m_pEffectNode->ScalingSingleEasing.start.getValue( *m_pManager );
		scaling_values.single_easing.end = m_pEffectNode->ScalingSingleEasing.end.getValue( *m_pManager );
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_FCurve )
	{
		assert( m_pEffectNode->ScalingFCurve != NULL );

		scaling_values.fcruve.offset.x = m_pEffectNode->ScalingFCurve->X.GetOffset( *m_pManager );
		scaling_values.fcruve.offset.y = m_pEffectNode->ScalingFCurve->Y.GetOffset( *m_pManager );
		scaling_values.fcruve.offset.z = m_pEffectNode->ScalingFCurve->Z.GetOffset( *m_pManager );
	}

	/* 生成位置 */
	if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
	{
		vector3d p = m_pEffectNode->GenerationLocation.point.location.getValue( *m_pManager );
		m_GenerationLocation.Translation( p.x, p.y, p.z );
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
	{
		Matrix43 mat_x, mat_y;
		mat_x.RotationX( m_pEffectNode->GenerationLocation.sphere.rotation_x.getValue( *m_pManager ) );
		mat_y.RotationY( m_pEffectNode->GenerationLocation.sphere.rotation_y.getValue( *m_pManager ) );
		float r = m_pEffectNode->GenerationLocation.sphere.radius.getValue( *m_pManager );
		m_GenerationLocation.Translation( 0, r, 0 );
		Matrix43::Multiple( m_GenerationLocation, m_GenerationLocation, mat_x );
		Matrix43::Multiple( m_GenerationLocation, m_GenerationLocation, mat_y );
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_MODEL )
	{
		m_GenerationLocation.Indentity();

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

				m_GenerationLocation.Translation( 
					emitter.Position.X, 
					emitter.Position.Y,
					emitter.Position.Z );

				if( m_pEffectNode->GenerationLocation.EffectsRotation )
				{
					m_GenerationLocation.Value[0][0] = emitter.Binormal.X;
					m_GenerationLocation.Value[0][1] = emitter.Binormal.Y;
					m_GenerationLocation.Value[0][2] = emitter.Binormal.Z;

					m_GenerationLocation.Value[1][0] = emitter.Tangent.X;
					m_GenerationLocation.Value[1][1] = emitter.Tangent.Y;
					m_GenerationLocation.Value[1][2] = emitter.Tangent.Z;

					m_GenerationLocation.Value[2][0] = emitter.Normal.X;
					m_GenerationLocation.Value[2][1] = emitter.Normal.Y;
					m_GenerationLocation.Value[2][2] = emitter.Normal.Z;
				}
			}
		}
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE )
	{
		m_GenerationLocation.Indentity();
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

			target = (int32_t)( (div) * ( (float)randFunc() / (float)randMax ) );
			if (target == div) div -= 1;
		}

		float angle = (end - start) * ((float)target / (float)div) + start;
		Matrix43 mat;
		mat.RotationZ( angle );

		m_GenerationLocation.Translation( 0, radius, 0 );
		Matrix43::Multiple( m_GenerationLocation, m_GenerationLocation, mat );
	}

	if( m_pEffectNode->SoundType == ParameterSoundType_Use )
	{
		soundValues.delay = m_pEffectNode->Sound.Delay.getValue( *m_pManager );
	}

	m_pEffectNode->InitializeRenderedInstance( *this, m_pManager );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Update( float deltaFrame, bool shown )
{
	if (m_stepTime && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		/* 音の更新(現状放置) */
		if (m_pEffectNode->SoundType == ParameterSoundType_Use)
		{
			float living_time = m_LivingTime;
			float living_time_p = living_time + deltaFrame;

			if (living_time <= (float) soundValues.delay && (float) soundValues.delay < living_time_p)
			{
				m_pEffectNode->PlaySound_(*this, m_pContainer->GetRootInstance(), m_pManager);
			}
		}
	}

	float originalTime = m_LivingTime;
	bool calculateMatrix = false;

	if(shown)
	{
		calculateMatrix = true;
	}
	else if( m_pEffectNode->LocationAbs.type != LocationAbsParameter::None )
	{
		// 絶対位置が設定されている場合は毎回計算が必要
		calculateMatrix = true;
	}
	else
	{
		/**
			見えないケースで行列計算が必要なケース
			-子が生成される。
			-子の子が生成される。
			*/
		if (m_stepTime && (originalTime <= m_LivedTime || !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct))
		{
			for (int i = 0; i < Min(ChildrenMax, m_pEffectNode->GetChildrenCount()); i++)
			{
				EffectNode* pNode = m_pEffectNode->GetChild(i);

				// インスタンス生成
				if (pNode->CommonValues.MaxGeneration > m_generatedChildrenCount[i] &&
					originalTime + deltaFrame > m_nextGenerationTime[i])
				{
					calculateMatrix = true;
					break;
				}
			}
		}
	}

	/* 親が破棄される瞬間に行列計算(条件を絞れば更に最適化可能) */
	if( !calculateMatrix && m_pParent != NULL && m_pParent->GetState() != INSTANCE_STATE_ACTIVE &&
		!(m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved && m_pEffectNode->GetChildrenCount() == 0))
	{
		calculateMatrix = true;
	}

	if( calculateMatrix )
	{
		CalculateMatrix( deltaFrame );
	}

	/* 親の削除処理 */
	if (m_pParent != NULL && m_pParent->GetState() != INSTANCE_STATE_ACTIVE)
	{
		m_pParent = nullptr;
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

		for (int i = 0; i < Min(ChildrenMax, m_pEffectNode->GetChildrenCount()); i++, group = group->NextUsedByInstance)
		{
			EffectNode* pNode = m_pEffectNode->GetChild( i );
			InstanceContainer* pContainer = m_pContainer->GetChild( i );
			assert( group != NULL );

			// インスタンス生成
			while (true)
			{
				if (pNode->CommonValues.MaxGeneration > m_generatedChildrenCount[i] &&
					originalTime + deltaFrame > m_nextGenerationTime[i])
				{
					// 生成処理
					Instance* pNewInstance = group->CreateInstance();
					if (pNewInstance != NULL)
					{
						pNewInstance->Initialize(this, m_generatedChildrenCount[i]);
					}
					else
					{
						break;
					}

					m_generatedChildrenCount[i]++;
					m_nextGenerationTime[i] += Max(0.0f, pNode->CommonValues.GenerationTime.getValue(*m_pManager));
				}
				else
				{
					break;
				}
			}
		}
	}
	
	// 死亡判定
	bool killed = false;
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		// 時間経過
		if( m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
		{
			if( m_LivingTime > m_LivedTime )
			{
				killed = true;
			}
		}

		// 親が消えた場合
		if( m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved )
		{
			if( m_pParent == nullptr || m_pParent->GetState() != INSTANCE_STATE_ACTIVE )
			{
				m_pParent = nullptr;
				killed = true;
			}
		}

		// 子が全て消えた場合
		if( !killed && m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct )
		{
			int maxcreate_count = 0;
			InstanceGroup* group = m_headGroups;

			for (int i = 0; i < Min(ChildrenMax, m_pEffectNode->GetChildrenCount()); i++, group = group->NextUsedByInstance)
			{
				auto child = m_pEffectNode->GetChild(i);
				float last_generation_time = 
					child->CommonValues.GenerationTime.max *
					(child->CommonValues.MaxGeneration - 1) +
					child->CommonValues.GenerationTimeOffset.max +
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
		if( !calculateMatrix &&
			m_pEffectNode->GetChildrenCount() > 0)
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
		CalculateParentMatrix();
	}

	Vector3D localPosition;
	Vector3D localAngle;
	Vector3D localScaling;

	/* 更新処理 */
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		/* 位置の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->TranslationType == ParameterTranslationType_None )
		{
			localPosition.X = 0;
			localPosition.Y = 0;
			localPosition.Z = 0;
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_Fixed )
		{
			localPosition.X = m_pEffectNode->TranslationFixed.Position.X;
			localPosition.Y = m_pEffectNode->TranslationFixed.Position.Y;
			localPosition.Z = m_pEffectNode->TranslationFixed.Position.Z;
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_PVA )
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localPosition.X = translation_values.random.location.x +
				(translation_values.random.velocity.x * m_LivingTime) +
				(translation_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			localPosition.Y = translation_values.random.location.y +
				(translation_values.random.velocity.y * m_LivingTime) +
				(translation_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			localPosition.Z = translation_values.random.location.z +
				(translation_values.random.velocity.z * m_LivingTime) +
				(translation_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);

		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_Easing )
		{
			m_pEffectNode->TranslationEasing.setValueToArg(
				localPosition,
				translation_values.easing.start,
				translation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->TranslationType == ParameterTranslationType_FCurve )
		{
			assert( m_pEffectNode->TranslationFCurve != NULL );
			localPosition.X = m_pEffectNode->TranslationFCurve->X.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.x;
			localPosition.Y = m_pEffectNode->TranslationFCurve->Y.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.y;
			localPosition.Z = m_pEffectNode->TranslationFCurve->Z.GetValue( (int)m_LivingTime ) + translation_values.fcruve.offset.z;
		}

		if( !m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			localPosition.X += m_GenerationLocation.Value[3][0];
			localPosition.Y += m_GenerationLocation.Value[3][1];
			localPosition.Z += m_GenerationLocation.Value[3][2];
		}

		/* 回転の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->RotationType == ParameterRotationType_None )
		{
			localAngle.X = 0;
			localAngle.Y = 0;
			localAngle.Z = 0;
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_Fixed )
		{
			localAngle.X = m_pEffectNode->RotationFixed.Position.X;
			localAngle.Y = m_pEffectNode->RotationFixed.Position.Y;
			localAngle.Z = m_pEffectNode->RotationFixed.Position.Z;
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_PVA )
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localAngle.X = rotation_values.random.rotation.x +
				(rotation_values.random.velocity.x * m_LivingTime) +
				(rotation_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			localAngle.Y = rotation_values.random.rotation.y +
				(rotation_values.random.velocity.y * m_LivingTime) +
				(rotation_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			localAngle.Z = rotation_values.random.rotation.z +
				(rotation_values.random.velocity.z * m_LivingTime) +
				(rotation_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);

		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_Easing )
		{
			m_pEffectNode->RotationEasing.setValueToArg(
				localAngle,
				rotation_values.easing.start,
				rotation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA )
		{
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
			localAngle.X = m_pEffectNode->RotationFCurve->X.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.x;
			localAngle.Y = m_pEffectNode->RotationFCurve->Y.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.y;
			localAngle.Z = m_pEffectNode->RotationFCurve->Z.GetValue( (int)m_LivingTime ) + rotation_values.fcruve.offset.z;
		}

		/* 拡大の更新(時間から直接求めれるよう対応済み) */
		if( m_pEffectNode->ScalingType == ParameterScalingType_None )
		{
			localScaling.X = 1.0f;
			localScaling.Y = 1.0f;
			localScaling.Z = 1.0f;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_Fixed )
		{
			localScaling.X = m_pEffectNode->ScalingFixed.Position.X;
			localScaling.Y = m_pEffectNode->ScalingFixed.Position.Y;
			localScaling.Z = m_pEffectNode->ScalingFixed.Position.Z;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_PVA )
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localScaling.X = scaling_values.random.scale.x +
				(scaling_values.random.velocity.x * m_LivingTime) +
				(scaling_values.random.acceleration.x * m_LivingTime * m_LivingTime * 0.5f);

			localScaling.Y = scaling_values.random.scale.y +
				(scaling_values.random.velocity.y * m_LivingTime) +
				(scaling_values.random.acceleration.y * m_LivingTime * m_LivingTime * 0.5f);

			localScaling.Z = scaling_values.random.scale.z +
				(scaling_values.random.velocity.z * m_LivingTime) +
				(scaling_values.random.acceleration.z * m_LivingTime * m_LivingTime * 0.5f);
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_Easing )
		{
			m_pEffectNode->ScalingEasing.setValueToArg(
				localScaling,
				scaling_values.easing.start,
				scaling_values.easing.end,
				m_LivingTime / m_LivedTime );
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA )
		{
			float s = scaling_values.single_random.scale +
				scaling_values.single_random.velocity * m_LivingTime +
				scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
			localScaling.X = s;
			localScaling.Y = s;
			localScaling.Z = s;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing )
		{
			float scale;
			m_pEffectNode->ScalingSingleEasing.setValueToArg(
				scale,
				scaling_values.single_easing.start,
				scaling_values.single_easing.end,
				m_LivingTime / m_LivedTime );
			localScaling.X = scale;
			localScaling.Y = scale;
			localScaling.Z = scale;
		}
		else if( m_pEffectNode->ScalingType == ParameterScalingType_FCurve )
		{
			assert( m_pEffectNode->ScalingFCurve != NULL );

			localScaling.X = m_pEffectNode->ScalingFCurve->X.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.x;
			localScaling.Y = m_pEffectNode->ScalingFCurve->Y.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.y;
			localScaling.Z = m_pEffectNode->ScalingFCurve->Z.GetValue( (int32_t)m_LivingTime ) + scaling_values.fcruve.offset.z;
		}

		/* 描画部分の更新 */
		m_pEffectNode->UpdateRenderedInstance( *this, m_pManager );
	}
	
	// 行列の更新
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		m_GlobalMatrix43.Scaling( localScaling.X, localScaling.Y,  localScaling.Z );

		if( m_pEffectNode->RotationType == ParameterRotationType_Fixed ||
			m_pEffectNode->RotationType == ParameterRotationType_PVA ||
			m_pEffectNode->RotationType == ParameterRotationType_Easing ||
			m_pEffectNode->RotationType == ParameterRotationType_FCurve )
		{
			Matrix43 MatRot;
			MatRot.RotationZXY( localAngle.Z, localAngle.X, localAngle.Y );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRot );
		}
		else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA ||
				 m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
		{
			Matrix43 MatRot;
			Vector3D axis;
			axis.X = rotation_values.axis.axis.x;
			axis.Y = rotation_values.axis.axis.y;
			axis.Z = rotation_values.axis.axis.z;

			MatRot.RotationAxis( axis, rotation_values.axis.rotation );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRot );
		}

		if( localPosition.X != 0.0f ||
			localPosition.Y != 0.0f || 
			localPosition.Z != 0.0f )
		{
			Matrix43 MatTra;
			MatTra.Translation( localPosition.X, localPosition.Y, localPosition.Z );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTra );
		}

		if( m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_GenerationLocation );
		}

		Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_ParentMatrix43 );
		
		Vector3D currentPosition;
		m_GlobalMatrix43.GetTranslation( currentPosition );
		m_GlobalVelocity = currentPosition - m_GlobalPosition;
		m_GlobalPosition = currentPosition;

		if( m_pEffectNode->LocationAbs.type != LocationAbsParameter::None )
		{
			ModifyMatrixFromLocationAbs( deltaFrame );
		}
	}
}

void Instance::CalculateParentMatrix()
{
	/* 親の行列を更新(現在は必要不必要関わらず行なっている) */
	//m_pParent->CalculateMatrix( deltaFrame );

	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		BindType lType = m_pEffectNode->CommonValues.TranslationBindType;
		BindType rType = m_pEffectNode->CommonValues.RotationBindType;
		BindType sType = m_pEffectNode->CommonValues.ScalingBindType;

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
		
		if( (lType == BindType_Always && rType == BindType_Always && sType == BindType_Always) )
		{
			m_ParentMatrix43 = m_pParent->GetGlobalMatrix43();
		}
		else if ( lType == BindType_NotBind_Root && rType == BindType_NotBind_Root && sType == BindType_NotBind_Root )
		{
			m_ParentMatrix43 = rootInstance->GetGlobalMatrix43();
		}
		else
		{
			Vector3D s, t;
			Matrix43 r;

			m_ParentMatrix43.GetSRT( s, r, t );
			
			if( lType == BindType_Always )
			{
				m_pParent->GetGlobalMatrix43().GetTranslation( t );
			}
			else if( lType == BindType_NotBind_Root && rootInstance != NULL )
			{
				rootInstance->GetGlobalMatrix43().GetTranslation( t );
			}
			
			if( rType == BindType_Always )
			{
				m_pParent->GetGlobalMatrix43().GetRotation( r );
			}
			else if( rType == BindType_NotBind_Root && rootInstance != NULL )
			{
				rootInstance->GetGlobalMatrix43().GetRotation( r );
			}
			

			if( sType == BindType_Always )
			{
				m_pParent->GetGlobalMatrix43().GetScale( s );
			}
			else if( sType == BindType_NotBind_Root && rootInstance != NULL )
			{
				rootInstance->GetGlobalMatrix43().GetScale( s );
			}

			m_ParentMatrix43.SetSRT( s, r, t );
		}
	}
	else
	{
		// Rootの場合
		m_ParentMatrix43 = m_pParent->GetGlobalMatrix43();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::ModifyMatrixFromLocationAbs( float deltaFrame )
{
	InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();

	/* 絶対位置の更新(時間から直接求めれるよう対応済み) */
	if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::None )
	{	
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::Gravity )
	{
		m_GlobalRevisionLocation.X = m_pEffectNode->LocationAbs.gravity.x *
			m_LivingTime * m_LivingTime * 0.5f;
		m_GlobalRevisionLocation.Y = m_pEffectNode->LocationAbs.gravity.y *
			m_LivingTime * m_LivingTime * 0.5f;
		m_GlobalRevisionLocation.Z = m_pEffectNode->LocationAbs.gravity.z *
			m_LivingTime * m_LivingTime * 0.5f;
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsParameter::AttractiveForce )
	{
		InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();

		float force = m_pEffectNode->LocationAbs.attractiveForce.force;
		float control = m_pEffectNode->LocationAbs.attractiveForce.control;
		float minRange = m_pEffectNode->LocationAbs.attractiveForce.minRange;
		float maxRange = m_pEffectNode->LocationAbs.attractiveForce.maxRange;
		
		Vector3D position = m_GlobalPosition - m_GlobalVelocity + m_GlobalRevisionLocation;

		Vector3D targetDifference = instanceGlobal->GetTargetLocation() - position;
		float targetDistance = Vector3D::Length( targetDifference );
		if( targetDistance > 0.0f )
		{
			Vector3D targetDirection = targetDifference / targetDistance;
		
			if( minRange > 0.0f || maxRange > 0.0f )
			{
				if( targetDistance >= m_pEffectNode->LocationAbs.attractiveForce.maxRange )
				{
					force = 0.0f;
				}
				else if( targetDistance > m_pEffectNode->LocationAbs.attractiveForce.minRange )
				{
					force *= 1.0f - (targetDistance - minRange) / (maxRange - minRange);
				}
			}

			m_GlobalRevisionVelocity += targetDirection * force * deltaFrame;
			float currentVelocity = Vector3D::Length( m_GlobalRevisionVelocity );
			Vector3D currentDirection = m_GlobalRevisionVelocity / currentVelocity;
		
			m_GlobalRevisionVelocity = (targetDirection * control + currentDirection * (1.0f - control)) * currentVelocity;
			m_GlobalRevisionLocation += m_GlobalRevisionVelocity * deltaFrame;
		}
	}

	Matrix43 MatTraGlobal;
	MatTraGlobal.Translation( m_GlobalRevisionLocation.X, m_GlobalRevisionLocation.Y, m_GlobalRevisionLocation.Z );
	Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTraGlobal );
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