
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

template<typename T, typename U>
void Instance::ApplyEq(T& dstParam, Effect* e, InstanceGlobal* instg, int dpInd, const U& originalParam)
{
	static_assert(sizeof(T) == sizeof(U), "size is not mismatched");
	const int count = sizeof(T) / 4;

	EFK_ASSERT(e != nullptr);
	EFK_ASSERT(0 <= dpInd && dpInd < static_cast<int>(instg->dynamicEqResults.size()));

	auto dst = reinterpret_cast<float*>(&(dstParam));
	auto src = reinterpret_cast<const float*>(&(originalParam));

	auto eqresult = instg->dynamicEqResults[dpInd];
	std::array<float, 1> globals;
	globals[0] = instg->GetUpdatedFrame() / 60.0f;

	std::array<float, 5> locals;

	for (int i = 0; i < count; i++)
	{
		locals[i] = src[i];
	}

	for (int i = count; i < 4; i++)
	{
		locals[i] = 0.0f;
	}

	locals[4] = m_pParent != nullptr ? m_pParent->m_LivingTime / 60.0f : 0.0f;

	auto e_ = static_cast<EffectImplemented*>(e);
	auto& dp = e_->dynamicEquation[dpInd];

	if (dp.GetRunningPhase() == InternalScript::RunningPhaseType::Local)
	{
		eqresult = dp.Execute(instg->dynamicInputParameters, globals, locals, InstanceGlobal::Rand, InstanceGlobal::RandSeed, instg);
	}

	for (int i = 0; i < count; i++)
	{
		dst[i] = eqresult[i];
	}
}

template <typename S> Vector3D Instance::ApplyEq(const int& dpInd, Vector3D originalParam, const S& scale, const S& scaleInv)
{
	const auto& e = this->m_pEffectNode->m_effect;
	const auto& instg = this->m_pContainer->GetRootInstance();

	if (dpInd >= 0)
	{
		originalParam.X *= scaleInv[0];
		originalParam.Y *= scaleInv[1];
		originalParam.Z *= scaleInv[2];

		ApplyEq(originalParam, e, instg, dpInd, originalParam);

		originalParam.X *= scale[0];
		originalParam.Y *= scale[1];
		originalParam.Z *= scale[2];
	}
	return originalParam;
}

random_float Instance::ApplyEq(const RefMinMax& dpInd, random_float originalParam)
{
	const auto& e = this->m_pEffectNode->m_effect;
	const auto& instg = this->m_pContainer->GetRootInstance();

	if (dpInd.Max >= 0)
	{
		ApplyEq(originalParam.max, e, instg, dpInd.Max, originalParam.max);
	}

	if (dpInd.Min >= 0)
	{
		ApplyEq(originalParam.min, e, instg, dpInd.Min, originalParam.min);
	}

	return originalParam;
}

template <typename S>
random_vector3d Instance::ApplyEq(const RefMinMax& dpInd, random_vector3d originalParam, const S& scale, const S& scaleInv)
{
	const auto& e = this->m_pEffectNode->m_effect;
	const auto& instg = this->m_pContainer->GetRootInstance();

	if (dpInd.Max >= 0)
	{
		originalParam.max.x *= scaleInv[0];
		originalParam.max.y *= scaleInv[1];
		originalParam.max.z *= scaleInv[2];

		ApplyEq(originalParam.max, e, instg, dpInd.Max, originalParam.max);

		originalParam.max.x *= scale[0];
		originalParam.max.y *= scale[1];
		originalParam.max.z *= scale[2];
	}

	if (dpInd.Min >= 0)
	{
		originalParam.min.x *= scaleInv[0];
		originalParam.min.y *= scaleInv[1];
		originalParam.min.z *= scaleInv[2];

		ApplyEq(originalParam.min, e, instg, dpInd.Min, originalParam.min);

		originalParam.min.x *= scale[0];
		originalParam.min.y *= scale[1];
		originalParam.min.z *= scale[2];
	}

	return originalParam;
}

random_int Instance::ApplyEq(const RefMinMax& dpInd, random_int originalParam)
{
	const auto& e = this->m_pEffectNode->m_effect;
	const auto& instg = this->m_pContainer->GetRootInstance();

	if (dpInd.Max >= 0)
	{
		float value = static_cast<float>(originalParam.max);
		ApplyEq(value, e, instg, dpInd.Max, value);
		originalParam.max = static_cast<int32_t>(value);
	}

	if (dpInd.Min >= 0)
	{
		float value = static_cast<float>(originalParam.min);
		ApplyEq(value, e, instg, dpInd.Min, value);
		originalParam.min = static_cast<int32_t>(value);
	}

	return originalParam;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::Instance(Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer)
	: m_pManager(pManager)
	, m_pEffectNode((EffectNodeImplemented*)pEffectNode)
	, m_pContainer(pContainer)
	, m_headGroups(NULL)
	, m_pParent(NULL)
	, m_State(INSTANCE_STATE_ACTIVE)
	, m_LivedTime(0)
	, m_LivingTime(0)
	, uvTimeOffset(0)
	, m_flexibleGeneratedChildrenCount(nullptr)
	, m_flexibleNextGenerationTime(nullptr)
	, m_GlobalMatrix43Calculated(false)
	, m_ParentMatrix43Calculated(false)
	, is_time_step_allowed(false)
	, m_sequenceNumber(0)
{
	m_generatedChildrenCount = m_fixedGeneratedChildrenCount;
	maxGenerationChildrenCount = fixedMaxGenerationChildrenCount_;
	m_nextGenerationTime = m_fixedNextGenerationTime;
	
	ColorInheritance = Color(255, 255, 255, 255);
	ColorParent = Color(255, 255, 255, 255);

	InstanceGroup* group = NULL;

	for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++ )
	{
		InstanceContainer* childContainer = m_pContainer->GetChild( i );

		if( group != NULL )
		{
			group->NextUsedByInstance = childContainer->CreateGroup();
			group = group->NextUsedByInstance;
		}
		else
		{
			group = childContainer->CreateGroup();
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

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	if (m_flexibleGeneratedChildrenCount != nullptr)
	{
		m_pManager->GetFreeFunc()(m_flexibleGeneratedChildrenCount, sizeof(int32_t) * parameter->GetChildrenCount());
	}

	if (flexibleMaxGenerationChildrenCount_ != nullptr)
	{
		m_pManager->GetFreeFunc()(flexibleMaxGenerationChildrenCount_, sizeof(int32_t) * parameter->GetChildrenCount());
	}

	if (m_flexibleNextGenerationTime != nullptr)
	{
		m_pManager->GetFreeFunc()(m_flexibleNextGenerationTime, sizeof(float) * parameter->GetChildrenCount());
	}
}

void Instance::GenerateChildrenInRequired(float currentTime)
{
	auto instanceGlobal = this->m_pContainer->GetRootInstance();

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	InstanceGroup* group = m_headGroups;

	for (int32_t i = 0; i < parameter->GetChildrenCount(); i++, group = group->NextUsedByInstance)
	{
		auto node = (EffectNodeImplemented*)parameter->GetChild(i);
		assert(group != NULL);

		while (true)
		{
			// GenerationTimeOffset can be minus value.
			// Minus frame particles is generated simultaniously at frame 0.
			if (maxGenerationChildrenCount[i] > m_generatedChildrenCount[i] &&
				m_nextGenerationTime[i] <= currentTime)
			{
				// Create a particle
				auto newInstance = group->CreateInstance();
				if (newInstance != nullptr)
				{
					Matrix43 rootMatrix;
					rootMatrix.Indentity();

					newInstance->Initialize(this, m_generatedChildrenCount[i], (int32_t)std::max(0.0f, this->m_LivingTime), rootMatrix);
				}

				m_generatedChildrenCount[i]++;

				auto gt = ApplyEq(node->CommonValues.RefEqGenerationTime, node->CommonValues.GenerationTime);
				m_nextGenerationTime[i] += Max(0.0f, gt.getValue(*instanceGlobal));
			}
			else
			{
				break;
			}
		}
	}
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
void Instance::Initialize( Instance* parent, int32_t instanceNumber, int32_t parentTime, const Matrix43& globalMatrix)
{
	assert(this->m_pContainer != nullptr);
	
	// Invalidate matrix
	m_GlobalMatrix43Calculated = false;
	m_ParentMatrix43Calculated = false;

	auto instanceGlobal = this->m_pContainer->GetRootInstance();

	auto parameter = (EffectNodeImplemented*) m_pEffectNode;

	// Extend array
	if (parameter->GetChildrenCount() >= ChildrenMax)
	{
		m_flexibleGeneratedChildrenCount = (int32_t*)(m_pManager->GetMallocFunc()(sizeof(int32_t) * parameter->GetChildrenCount()));
		flexibleMaxGenerationChildrenCount_ = (int32_t*)(m_pManager->GetMallocFunc()(sizeof(int32_t) * parameter->GetChildrenCount()));
		m_flexibleNextGenerationTime = (float*)(m_pManager->GetMallocFunc()(sizeof(float) * parameter->GetChildrenCount()));

		m_generatedChildrenCount = m_flexibleGeneratedChildrenCount;
		maxGenerationChildrenCount = flexibleMaxGenerationChildrenCount_;
		m_nextGenerationTime = m_flexibleNextGenerationTime;
	}

	// 親の設定
	m_pParent = parent;

	// initialize children
	for (int32_t i = 0; i < parameter->GetChildrenCount(); i++)
	{
		auto pNode = (EffectNodeImplemented*) parameter->GetChild(i);

		m_generatedChildrenCount[i] = 0;

		auto gt = ApplyEq(pNode->CommonValues.RefEqGenerationTimeOffset, pNode->CommonValues.GenerationTimeOffset);

		m_nextGenerationTime[i] = gt.getValue(*instanceGlobal);

		if (pNode->CommonValues.RefEqMaxGeneration >= 0)
		{
			auto maxGene = static_cast<float>(pNode->CommonValues.MaxGeneration);
			ApplyEq(maxGene,
					this->m_pEffectNode->m_effect,
					this->m_pContainer->GetRootInstance(),
					pNode->CommonValues.RefEqMaxGeneration,
					maxGene);
			maxGenerationChildrenCount[i] = maxGene;
		}
		else
		{
			maxGenerationChildrenCount[i] = pNode->CommonValues.MaxGeneration;
		}
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
		m_GlobalMatrix43 = globalMatrix;
		m_ParentMatrix43.Indentity();

		// 親の初期化
		m_ParentMatrix43 = GetGlobalMatrix43();

		// Generate zero frame effect
		GenerateChildrenInRequired(0.0f);
		return;
	}
	
	// 親の行列を計算
	m_pParent->CalculateMatrix( 0 );

	// 状態の初期化
	m_State = INSTANCE_STATE_ACTIVE;

	// initialize about a lifetime
	m_LivingTime = 0.0f;
	{
		auto ri = ApplyEq(parameter->CommonValues.RefEqLife, parameter->CommonValues.life);
		m_LivedTime = (float)ri.getValue(*instanceGlobal);
	}

	// SRTの初期化
	m_pParent->GetGlobalMatrix43().GetTranslation( m_GlobalPosition );
	m_GlobalRevisionLocation = Vector3D(0.0f, 0.0f, 0.0f);
	m_GlobalRevisionVelocity = Vector3D(0.0f, 0.0f, 0.0f);
	m_GenerationLocation.Indentity();
	m_GlobalMatrix43 = globalMatrix;
	m_ParentMatrix43.Indentity();

	// 親の初期化
	if( parameter->CommonValues.TranslationBindType == BindType::WhenCreating )
	{
		m_ParentMatrix43.Value[3][0] = m_pParent->m_GlobalMatrix43.Value[3][0];
		m_ParentMatrix43.Value[3][1] = m_pParent->m_GlobalMatrix43.Value[3][1];
		m_ParentMatrix43.Value[3][2] = m_pParent->m_GlobalMatrix43.Value[3][2];
	}

	if( parameter->CommonValues.RotationBindType == BindType::WhenCreating &&
		parameter->CommonValues.ScalingBindType == BindType::WhenCreating )
	{
		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_pParent->m_GlobalMatrix43.Value[m][n];
			}
		}
	}
	else if ( parameter->CommonValues.RotationBindType == BindType::WhenCreating )
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
	else if ( parameter->CommonValues.ScalingBindType == BindType::WhenCreating )
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
	
	// Initialize parent color
	if (parameter->RendererCommon.ColorBindType == BindType::Always)
	{
		ColorParent = m_pParent->ColorInheritance;
	}
	else if (parameter->RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		ColorParent = m_pParent->ColorInheritance;
	}

	// Translation
	if( m_pEffectNode->TranslationType == ParameterTranslationType_Fixed )
	{
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_PVA )
	{
		auto rvl = ApplyEq(m_pEffectNode->TranslationPVA.RefEqP,
						   m_pEffectNode->TranslationPVA.location,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.location = rvl.getValue(*this->m_pContainer->GetRootInstance());

		auto rvv = ApplyEq(m_pEffectNode->TranslationPVA.RefEqV,
						   m_pEffectNode->TranslationPVA.velocity,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.velocity = rvv.getValue(*this->m_pContainer->GetRootInstance());

		auto rva = ApplyEq(m_pEffectNode->TranslationPVA.RefEqA,
						   m_pEffectNode->TranslationPVA.acceleration,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.acceleration = rva.getValue(*this->m_pContainer->GetRootInstance());

	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_Easing )
	{
		auto rvs = ApplyEq(m_pEffectNode->TranslationEasing.RefEqS,
						   m_pEffectNode->TranslationEasing.location.start,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		auto rve = ApplyEq(m_pEffectNode->TranslationEasing.RefEqE,
						   m_pEffectNode->TranslationEasing.location.end,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);

		translation_values.easing.start = rvs.getValue(*this->m_pContainer->GetRootInstance());
		translation_values.easing.end = rve.getValue(*this->m_pContainer->GetRootInstance());
	}
	else if( m_pEffectNode->TranslationType == ParameterTranslationType_FCurve )
	{
		assert( m_pEffectNode->TranslationFCurve != NULL );

		translation_values.fcruve.offset.x = m_pEffectNode->TranslationFCurve->X.GetOffset( *instanceGlobal );
		translation_values.fcruve.offset.y = m_pEffectNode->TranslationFCurve->Y.GetOffset( *instanceGlobal );
		translation_values.fcruve.offset.z = m_pEffectNode->TranslationFCurve->Z.GetOffset( *instanceGlobal );
	}
	
	// Rotation
	if( m_pEffectNode->RotationType == ParameterRotationType_Fixed )
	{
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_PVA )
	{
		auto rvl = ApplyEq(m_pEffectNode->RotationPVA.RefEqP,
						   m_pEffectNode->RotationPVA.rotation,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rvv = ApplyEq(m_pEffectNode->RotationPVA.RefEqV,
						   m_pEffectNode->RotationPVA.velocity,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rva = ApplyEq(m_pEffectNode->RotationPVA.RefEqA,
						   m_pEffectNode->RotationPVA.acceleration,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);

		rotation_values.random.rotation = rvl.getValue(*instanceGlobal);
		rotation_values.random.velocity = rvv.getValue(*instanceGlobal);
		rotation_values.random.acceleration = rva.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_Easing )
	{
		auto rvs = ApplyEq(m_pEffectNode->RotationEasing.RefEqS,
						   m_pEffectNode->RotationEasing.rotation.start,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rve = ApplyEq(m_pEffectNode->RotationEasing.RefEqE,
						   m_pEffectNode->RotationEasing.rotation.end,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);

		rotation_values.easing.start = rvs.getValue(*instanceGlobal);
		rotation_values.easing.end = rve.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisPVA )
	{
		rotation_values.axis.random.rotation = m_pEffectNode->RotationAxisPVA.rotation.getValue(*instanceGlobal);
		rotation_values.axis.random.velocity = m_pEffectNode->RotationAxisPVA.velocity.getValue(*instanceGlobal);
		rotation_values.axis.random.acceleration = m_pEffectNode->RotationAxisPVA.acceleration.getValue(*instanceGlobal);
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisPVA.axis.getValue(*instanceGlobal);
		rotation_values.axis.axis.normalize();
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_AxisEasing )
	{
		rotation_values.axis.easing.start = m_pEffectNode->RotationAxisEasing.easing.start.getValue(*instanceGlobal);
		rotation_values.axis.easing.end = m_pEffectNode->RotationAxisEasing.easing.end.getValue(*instanceGlobal);
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisEasing.axis.getValue(*instanceGlobal);
		rotation_values.axis.axis.normalize();
	}
	else if( m_pEffectNode->RotationType == ParameterRotationType_FCurve )
	{
		assert( m_pEffectNode->RotationFCurve != NULL );

		rotation_values.fcruve.offset.x = m_pEffectNode->RotationFCurve->X.GetOffset( *instanceGlobal );
		rotation_values.fcruve.offset.y = m_pEffectNode->RotationFCurve->Y.GetOffset( *instanceGlobal );
		rotation_values.fcruve.offset.z = m_pEffectNode->RotationFCurve->Z.GetOffset( *instanceGlobal );
	}

	// Scaling
	if( m_pEffectNode->ScalingType == ParameterScalingType_Fixed )
	{
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_PVA )
	{
		auto rvl = ApplyEq(m_pEffectNode->ScalingPVA.RefEqP, m_pEffectNode->ScalingPVA.Position, m_pEffectNode->DynamicFactor.Scale, m_pEffectNode->DynamicFactor.ScaleInv);
		auto rvv = ApplyEq(m_pEffectNode->ScalingPVA.RefEqV,
						   m_pEffectNode->ScalingPVA.Velocity,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rva = ApplyEq(m_pEffectNode->ScalingPVA.RefEqA,
						   m_pEffectNode->ScalingPVA.Acceleration,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);

		scaling_values.random.scale = rvl.getValue(*instanceGlobal);
		scaling_values.random.velocity = rvv.getValue(*instanceGlobal);
		scaling_values.random.acceleration = rva.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_Easing )
	{
		auto rvs = ApplyEq(m_pEffectNode->ScalingEasing.RefEqS,
						   m_pEffectNode->ScalingEasing.Position.start,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rve = ApplyEq(m_pEffectNode->ScalingEasing.RefEqE,
						   m_pEffectNode->ScalingEasing.Position.end,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);

		scaling_values.easing.start = rvs.getValue(*instanceGlobal);
		scaling_values.easing.end = rve.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA )
	{
		scaling_values.single_random.scale = m_pEffectNode->ScalingSinglePVA.Position.getValue(*instanceGlobal);
		scaling_values.single_random.velocity = m_pEffectNode->ScalingSinglePVA.Velocity.getValue(*instanceGlobal);
		scaling_values.single_random.acceleration = m_pEffectNode->ScalingSinglePVA.Acceleration.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing )
	{
		scaling_values.single_easing.start = m_pEffectNode->ScalingSingleEasing.start.getValue(*instanceGlobal);
		scaling_values.single_easing.end = m_pEffectNode->ScalingSingleEasing.end.getValue(*instanceGlobal);
	}
	else if( m_pEffectNode->ScalingType == ParameterScalingType_FCurve )
	{
		assert( m_pEffectNode->ScalingFCurve != NULL );

		scaling_values.fcruve.offset.x = m_pEffectNode->ScalingFCurve->X.GetOffset( *instanceGlobal );
		scaling_values.fcruve.offset.y = m_pEffectNode->ScalingFCurve->Y.GetOffset( *instanceGlobal );
		scaling_values.fcruve.offset.z = m_pEffectNode->ScalingFCurve->Z.GetOffset( *instanceGlobal );
	}

	// Spawning Method
	if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT )
	{
		vector3d p = m_pEffectNode->GenerationLocation.point.location.getValue(*instanceGlobal);
		m_GenerationLocation.Translation( p.x, p.y, p.z );
	}
	else if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_LINE)
	{
		vector3d s = m_pEffectNode->GenerationLocation.line.position_start.getValue(*instanceGlobal);
		vector3d e = m_pEffectNode->GenerationLocation.line.position_end.getValue(*instanceGlobal);
		auto noize = m_pEffectNode->GenerationLocation.line.position_noize.getValue(*instanceGlobal);
		auto division = Max(1, m_pEffectNode->GenerationLocation.line.division);

		Vector3D dir;
		(e - s).setValueToArg(dir);

		if (Vector3D::LengthSq(dir) < 0.001)
		{
			m_GenerationLocation.Translation(0 ,0, 0);
		}
		else
		{
			auto len = Vector3D::Length(dir);
			dir /= len;
		
			int32_t target = 0;
			if (m_pEffectNode->GenerationLocation.line.type == ParameterGenerationLocation::LineType::Order)
			{
				target = instanceNumber % division;
			}
			else if (m_pEffectNode->GenerationLocation.line.type == ParameterGenerationLocation::LineType::Random)
			{
				target = (int32_t)((division) * instanceGlobal->GetRand());
				if (target == division) target -= 1;
			}

			auto d = 0.0f;
			if (division > 1)
			{
				d = (len / (float)(division-1)) * target;
			}

			d += noize;
		
			s.x += dir.X * d;
			s.y += dir.Y * d;
			s.z += dir.Z * d;

			Vector3D xdir;
			Vector3D ydir;
			Vector3D zdir;

			if (fabs(dir.Y) > 0.999f)
			{
				xdir = dir;
				Vector3D::Cross(zdir, xdir, Vector3D(-1, 0, 0));
				Vector3D::Normal(zdir, zdir);
				Vector3D::Cross(ydir, zdir, xdir);
				Vector3D::Normal(ydir, ydir);
			}
			else
			{
				xdir = dir;
				Vector3D::Cross(ydir, Vector3D(0, 0, 1), xdir);
				Vector3D::Normal(ydir, ydir);
				Vector3D::Cross(zdir, xdir, ydir);
				Vector3D::Normal(zdir, zdir);
			}

			if (m_pEffectNode->GenerationLocation.EffectsRotation)
			{
				m_GenerationLocation.Value[0][0] = xdir.X;
				m_GenerationLocation.Value[0][1] = xdir.Y;
				m_GenerationLocation.Value[0][2] = xdir.Z;

				m_GenerationLocation.Value[1][0] = ydir.X;
				m_GenerationLocation.Value[1][1] = ydir.Y;
				m_GenerationLocation.Value[1][2] = ydir.Z;

				m_GenerationLocation.Value[2][0] = zdir.X;
				m_GenerationLocation.Value[2][1] = zdir.Y;
				m_GenerationLocation.Value[2][2] = zdir.Z;
			}
			else
			{
				m_GenerationLocation.Indentity();
			}

			m_GenerationLocation.Value[3][0] = s.x;
			m_GenerationLocation.Value[3][1] = s.y;
			m_GenerationLocation.Value[3][2] = s.z;
		}
	}
	else if( m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE )
	{
		Matrix43 mat_x, mat_y;
		mat_x.RotationX( m_pEffectNode->GenerationLocation.sphere.rotation_x.getValue( *instanceGlobal ) );
		mat_y.RotationY( m_pEffectNode->GenerationLocation.sphere.rotation_y.getValue( *instanceGlobal ) );
		float r = m_pEffectNode->GenerationLocation.sphere.radius.getValue(*instanceGlobal);
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
						instanceGlobal, 
						parentTime,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX )
				{
					emitter = model->GetEmitterFromVertex( 
						instanceNumber,
						parentTime,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX_RANDOM )
				{
					emitter = model->GetEmitterFromVertex( 
						instanceGlobal,
						parentTime,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE )
				{
					emitter = model->GetEmitterFromFace( 
						instanceNumber,
						parentTime,
						m_pManager->GetCoordinateSystem(), 
						((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification() );
				}
				else if( m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE_RANDOM )
				{
					emitter = model->GetEmitterFromFace( 
						instanceGlobal,
						parentTime,
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
		float radius = m_pEffectNode->GenerationLocation.circle.radius.getValue(*instanceGlobal);
		float start = m_pEffectNode->GenerationLocation.circle.angle_start.getValue(*instanceGlobal);
		float end = m_pEffectNode->GenerationLocation.circle.angle_end.getValue(*instanceGlobal);
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
			target = (int32_t)( (div) * instanceGlobal->GetRand() );
			if (target == div) target -= 1;
		}

		float angle = (end - start) * ((float)target / (float)div) + start;

		angle += m_pEffectNode->GenerationLocation.circle.angle_noize.getValue(*instanceGlobal);

		Matrix43 mat;
		if (m_pEffectNode->GenerationLocation.circle.axisDirection == ParameterGenerationLocation::AxisType::X)
		{
			mat.RotationX(angle);
			m_GenerationLocation.Translation(0, 0, radius);
		}
		if (m_pEffectNode->GenerationLocation.circle.axisDirection == ParameterGenerationLocation::AxisType::Y)
		{
			mat.RotationY(angle);
			m_GenerationLocation.Translation(radius, 0, 0);
		}
		if (m_pEffectNode->GenerationLocation.circle.axisDirection == ParameterGenerationLocation::AxisType::Z)
		{
			mat.RotationZ(angle);
			m_GenerationLocation.Translation(0, radius, 0);
		}

		
		Matrix43::Multiple( m_GenerationLocation, m_GenerationLocation, mat );
	}

	if( m_pEffectNode->SoundType == ParameterSoundType_Use )
	{
		soundValues.delay = (int32_t)m_pEffectNode->Sound.Delay.getValue( *instanceGlobal );
	}

	// UV
	if (m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_ANIMATION)
	{
		uvTimeOffset = (int32_t)m_pEffectNode->RendererCommon.UV.Animation.StartFrame.getValue(*instanceGlobal);
		uvTimeOffset *= m_pEffectNode->RendererCommon.UV.Animation.FrameLength;
	}
	
	if (m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_SCROLL)
	{
		auto xy = m_pEffectNode->RendererCommon.UV.Scroll.Position.getValue(*instanceGlobal);
		auto zw = m_pEffectNode->RendererCommon.UV.Scroll.Size.getValue(*instanceGlobal);

		uvAreaOffset.X = xy.x;
		uvAreaOffset.Y = xy.y;
		uvAreaOffset.Width = zw.x;
		uvAreaOffset.Height = zw.y;

		m_pEffectNode->RendererCommon.UV.Scroll.Speed.getValue(*instanceGlobal).setValueToArg(uvScrollSpeed);
	}

	if (m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_FCURVE)
	{
		uvAreaOffset.X = m_pEffectNode->RendererCommon.UV.FCurve.Position->X.GetOffset(*instanceGlobal);
		uvAreaOffset.Y = m_pEffectNode->RendererCommon.UV.FCurve.Position->Y.GetOffset(*instanceGlobal);
		uvAreaOffset.Width = m_pEffectNode->RendererCommon.UV.FCurve.Size->X.GetOffset(*instanceGlobal);
		uvAreaOffset.Height = m_pEffectNode->RendererCommon.UV.FCurve.Size->Y.GetOffset(*instanceGlobal);
	}

	m_pEffectNode->InitializeRenderedInstance(*this, m_pManager);

	// Generate zero frame effect
	GenerateChildrenInRequired(0.0f);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Update( float deltaFrame, bool shown )
{
	assert(this->m_pContainer != nullptr);
	
	// Invalidate matrix
	m_GlobalMatrix43Calculated = false;
	m_ParentMatrix43Calculated = false;

	if (is_time_step_allowed && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
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

	// step time
	// frame 0 - generated time
	// frame 1- now
	if (is_time_step_allowed)
	{
		m_LivingTime += deltaFrame;
	}

	if(shown)
	{
		CalculateMatrix( deltaFrame );
	}
	else if( m_pEffectNode->LocationAbs.type != LocationAbsType::None )
	{
		// If attraction forces are not default, updating is needed in each frame.
		CalculateMatrix( deltaFrame );
	}

	// Get parent color.
	if (m_pParent != NULL)
	{
		if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
		{
			ColorParent = m_pParent->ColorInheritance;
		}
	}

	/* 親の削除処理 */
	if (m_pParent != NULL && m_pParent->GetState() != INSTANCE_STATE_ACTIVE)
	{
		CalculateParentMatrix( deltaFrame );
		m_pParent = nullptr;
	}

	// Create child particles
	if( is_time_step_allowed && (originalTime <= m_LivedTime || !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct) )
	{
		GenerateChildrenInRequired(originalTime + deltaFrame);
	}
	
	// check whether killed?
	bool killed = false;
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		// if pass time
		if( m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
		{
			if( m_LivingTime > m_LivedTime )
			{
				killed = true;
			}
		}

		// if remove parent
		if( m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved )
		{
			if( m_pParent == nullptr || m_pParent->GetState() != INSTANCE_STATE_ACTIVE )
			{
				m_pParent = nullptr;
				killed = true;
			}
		}

		// if children are removed and going not to generate a child
		if( !killed && m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct )
		{
			int maxcreate_count = 0;
			InstanceGroup* group = m_headGroups;

			for (int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance)
			{
				auto child = (EffectNodeImplemented*) m_pEffectNode->GetChild(i);

				if (maxGenerationChildrenCount[i] <= m_generatedChildrenCount[i] && group->GetInstanceCount() == 0)
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
		// if it need to calculate a matrix
		if( m_pEffectNode->GetChildrenCount() > 0)
		{
			// Get parent color.
			if (m_pParent != nullptr)
			{
				if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
				{
					ColorParent = m_pParent->ColorInheritance;
				}
			}
		}

		// Delete this particle with myself.
		Kill();
		return;
	}

	// allow to pass time
	is_time_step_allowed = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::CalculateMatrix( float deltaFrame )
{
	// 計算済なら終了
	if( m_GlobalMatrix43Calculated ) return;
	
	//if( m_sequenceNumber == ((ManagerImplemented*)m_pManager)->GetSequenceNumber() ) return;
	m_sequenceNumber = ((ManagerImplemented*)m_pManager)->GetSequenceNumber();

	assert( m_pEffectNode != NULL );
	assert( m_pContainer != NULL );

	// 親の処理
	if( m_pParent != NULL )
	{
		CalculateParentMatrix( deltaFrame );
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
			localPosition = ApplyEq(m_pEffectNode->TranslationFixed.RefEq,
									m_pEffectNode->TranslationFixed.Position,
									m_pEffectNode->DynamicFactor.Tra,
									m_pEffectNode->DynamicFactor.TraInv);
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
			m_pEffectNode->TranslationEasing.location.setValueToArg(
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
			localAngle = ApplyEq(m_pEffectNode->RotationFixed.RefEq,
								 m_pEffectNode->RotationFixed.Position,
								 m_pEffectNode->DynamicFactor.Rot,
								 m_pEffectNode->DynamicFactor.RotInv);
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
			m_pEffectNode->RotationEasing.rotation.setValueToArg(
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
			localScaling = ApplyEq(m_pEffectNode->ScalingFixed.RefEq,
								   m_pEffectNode->ScalingFixed.Position,
								   m_pEffectNode->DynamicFactor.Scale,
								   m_pEffectNode->DynamicFactor.ScaleInv);
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
			m_pEffectNode->ScalingEasing.Position.setValueToArg(
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

		if( m_pEffectNode->LocationAbs.type != LocationAbsType::None )
		{
			ModifyMatrixFromLocationAbs( deltaFrame );
		}
	}

	m_GlobalMatrix43Calculated = true;
}

void Instance::CalculateParentMatrix( float deltaFrame )
{
	// 計算済なら終了
	if( m_ParentMatrix43Calculated ) return;

	// 親の行列を計算
	m_pParent->CalculateMatrix( deltaFrame );

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
		
		if( (lType == BindType::Always && rType == BindType::Always && sType == BindType::Always) )
		{
			m_ParentMatrix43 = m_pParent->GetGlobalMatrix43();
		}
		else if ( lType == BindType::NotBind_Root && rType == BindType::NotBind_Root && sType == BindType::NotBind_Root )
		{
			m_ParentMatrix43 = rootInstance->GetGlobalMatrix43();
		}
		else
		{
			Vector3D s, t;
			Matrix43 r;

			m_ParentMatrix43.GetSRT( s, r, t );
			
			if( lType == BindType::Always )
			{
				m_pParent->GetGlobalMatrix43().GetTranslation( t );
			}
			else if( lType == BindType::NotBind_Root && rootInstance != NULL )
			{
				rootInstance->GetGlobalMatrix43().GetTranslation( t );
			}
			
			if( rType == BindType::Always )
			{
				m_pParent->GetGlobalMatrix43().GetRotation( r );
			}
			else if( rType == BindType::NotBind_Root && rootInstance != NULL )
			{
				rootInstance->GetGlobalMatrix43().GetRotation( r );
			}
			

			if( sType == BindType::Always )
			{
				m_pParent->GetGlobalMatrix43().GetScale( s );
			}
			else if( sType == BindType::NotBind_Root && rootInstance != NULL )
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

	m_ParentMatrix43Calculated = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::ModifyMatrixFromLocationAbs( float deltaFrame )
{
	InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();

	// Update attraction forces
	if( m_pEffectNode->LocationAbs.type == LocationAbsType::None )
	{	
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsType::Gravity )
	{
		m_GlobalRevisionLocation.X = m_pEffectNode->LocationAbs.gravity.x *
			m_LivingTime * m_LivingTime * 0.5f;
		m_GlobalRevisionLocation.Y = m_pEffectNode->LocationAbs.gravity.y *
			m_LivingTime * m_LivingTime * 0.5f;
		m_GlobalRevisionLocation.Z = m_pEffectNode->LocationAbs.gravity.z *
			m_LivingTime * m_LivingTime * 0.5f;
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsType::AttractiveForce )
	{
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

			if (deltaFrame > 0)
			{
				float eps = 0.0001f;
				m_GlobalRevisionVelocity += targetDirection * force * deltaFrame;
				float currentVelocity = Vector3D::Length(m_GlobalRevisionVelocity) + eps;
				Vector3D currentDirection = m_GlobalRevisionVelocity / currentVelocity;

				m_GlobalRevisionVelocity = (targetDirection * control + currentDirection * (1.0f - control)) * currentVelocity;
				m_GlobalRevisionLocation += m_GlobalRevisionVelocity * deltaFrame;
			}
		}
	}

	Matrix43 MatTraGlobal;
	MatTraGlobal.Translation( m_GlobalRevisionLocation.X, m_GlobalRevisionLocation.Y, m_GlobalRevisionLocation.Z );
	Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTraGlobal );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Draw(Instance* next)
{
	assert( m_pEffectNode != NULL );

	if( !m_pEffectNode->IsRendered ) return;

	if( m_sequenceNumber != ((ManagerImplemented*)m_pManager)->GetSequenceNumber() )
	{
		CalculateMatrix( 0 );
	}

	m_pEffectNode->Rendering(*this, next, m_pManager);
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
	if( m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_DEFAULT )
	{
		return RectF( 0.0f, 0.0f, 1.0f, 1.0f );
	}
	else if( m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_FIXED )
	{
		return RectF(
			m_pEffectNode->RendererCommon.UV.Fixed.Position.x,
			m_pEffectNode->RendererCommon.UV.Fixed.Position.y,
			m_pEffectNode->RendererCommon.UV.Fixed.Position.w,
			m_pEffectNode->RendererCommon.UV.Fixed.Position.h );
	}
	else if( m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_ANIMATION )
	{
		auto time = m_LivingTime + uvTimeOffset;

		int32_t frameNum = (int32_t)(time / m_pEffectNode->RendererCommon.UV.Animation.FrameLength);
		int32_t frameCount = m_pEffectNode->RendererCommon.UV.Animation.FrameCountX * m_pEffectNode->RendererCommon.UV.Animation.FrameCountY;

		if( m_pEffectNode->RendererCommon.UV.Animation.LoopType == m_pEffectNode->RendererCommon.UV.Animation.LOOPTYPE_ONCE )
		{
			if( frameNum >= frameCount )
			{
				frameNum = frameCount - 1;
			}
		}
		else if ( m_pEffectNode->RendererCommon.UV.Animation.LoopType == m_pEffectNode->RendererCommon.UV.Animation.LOOPTYPE_LOOP )
		{
			frameNum %= frameCount;
		}
		else if ( m_pEffectNode->RendererCommon.UV.Animation.LoopType == m_pEffectNode->RendererCommon.UV.Animation.LOOPTYPE_REVERSELOOP )
		{
			bool rev = (frameNum / frameCount) % 2 == 1;
			frameNum %= frameCount;
			if( rev )
			{
				frameNum = frameCount - 1 - frameNum;
			}
		}

		int32_t frameX = frameNum % m_pEffectNode->RendererCommon.UV.Animation.FrameCountX;
		int32_t frameY = frameNum / m_pEffectNode->RendererCommon.UV.Animation.FrameCountX;

		return RectF(
			m_pEffectNode->RendererCommon.UV.Animation.Position.x + m_pEffectNode->RendererCommon.UV.Animation.Position.w * frameX,
			m_pEffectNode->RendererCommon.UV.Animation.Position.y + m_pEffectNode->RendererCommon.UV.Animation.Position.h * frameY,
			m_pEffectNode->RendererCommon.UV.Animation.Position.w,
			m_pEffectNode->RendererCommon.UV.Animation.Position.h );
	}
	else if( m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_SCROLL )
	{
		auto time = (int32_t)m_LivingTime + uvTimeOffset;

		return RectF(
			uvAreaOffset.X + uvScrollSpeed.X * time,
			uvAreaOffset.Y + uvScrollSpeed.Y * time,
			uvAreaOffset.Width,
			uvAreaOffset.Height);
	}
	else if (m_pEffectNode->RendererCommon.UVType == ParameterRendererCommon::UV_FCURVE)
	{
		auto time = (int32_t)m_LivingTime + uvTimeOffset;

		return RectF(
			uvAreaOffset.X + m_pEffectNode->RendererCommon.UV.FCurve.Position->X.GetValue(time),
			uvAreaOffset.Y + m_pEffectNode->RendererCommon.UV.FCurve.Position->Y.GetValue(time),
			uvAreaOffset.Width + m_pEffectNode->RendererCommon.UV.FCurve.Size->X.GetValue(time),
			uvAreaOffset.Height + m_pEffectNode->RendererCommon.UV.FCurve.Size->Y.GetValue(time));
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
