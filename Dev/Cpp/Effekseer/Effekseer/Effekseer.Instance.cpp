#include "Effekseer.Instance.h"
#include "Effekseer.Curve.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.Setting.h"
#include "Model/Model.h"

namespace Effekseer
{

namespace
{

void ApplyDynamicParameterToFixedRotation(RotationState& rotation_values,
										  const RotationParameter& rotationParam,
										  RandObject& rand,
										  const Effect* effect,
										  const InstanceGlobal* instanceGlobal,
										  const Instance* m_pParent,
										  const DynamicFactorParameter& dynamicFactor)
{
	if (rotationParam.RotationFixed.RefEq >= 0)
	{
		rotation_values.fixed.rotation = ApplyEq(effect,
												 instanceGlobal,
												 m_pParent,
												 &rand,
												 rotationParam.RotationFixed.RefEq,
												 rotationParam.RotationFixed.Position,
												 dynamicFactor.Rot,
												 dynamicFactor.RotInv);
	}
}

void InitRotation(RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
{
	// Rotation
	if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
	{
		rotation_values.fixed.rotation = rotationParam.RotationFixed.Position;
		ApplyDynamicParameterToFixedRotation(rotation_values, rotationParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   rotationParam.RotationPVA.RefEqP,
						   rotationParam.RotationPVA.rotation,
						   dynamicFactor.Rot,
						   dynamicFactor.RotInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   rotationParam.RotationPVA.RefEqV,
						   rotationParam.RotationPVA.velocity,
						   dynamicFactor.Rot,
						   dynamicFactor.RotInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   rotationParam.RotationPVA.RefEqA,
						   rotationParam.RotationPVA.acceleration,
						   dynamicFactor.Rot,
						   dynamicFactor.RotInv);

		rotation_values.random.rotation = rvl.getValue(rand);
		rotation_values.random.velocity = rvv.getValue(rand);
		rotation_values.random.acceleration = rva.getValue(rand);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
	{
		rotationParam.RotationEasing.Init(rotation_values.easing, effect, instanceGlobal, m_pParent, &rand, dynamicFactor.Rot, dynamicFactor.RotInv);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
	{
		rotation_values.axis.random.rotation = rotationParam.RotationAxisPVA.rotation.getValue(rand);
		rotation_values.axis.random.velocity = rotationParam.RotationAxisPVA.velocity.getValue(rand);
		rotation_values.axis.random.acceleration = rotationParam.RotationAxisPVA.acceleration.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
		rotation_values.axis.axis = rotationParam.RotationAxisPVA.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		rotation_values.axis.easing.start = rotationParam.RotationAxisEasing.easing.start.getValue(rand);
		rotation_values.axis.easing.end = rotationParam.RotationAxisEasing.easing.end.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
		rotation_values.axis.axis = rotationParam.RotationAxisEasing.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		assert(rotationParam.RotationFCurve != nullptr);

		rotation_values.fcruve.offset = rotationParam.RotationFCurve->GetOffsets(rand);
	}
}

void CalculateRotation(SIMD::Vec3f& localAngle, RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
{

	/* 回転の更新(時間から直接求めれるよう対応済み) */
	if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_None)
	{
		localAngle = {0, 0, 0};
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
	{
		ApplyDynamicParameterToFixedRotation(rotation_values, rotationParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
		localAngle = rotation_values.fixed.rotation;
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
	{
		/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
		localAngle = rotation_values.random.rotation + (rotation_values.random.velocity * m_LivingTime) +
					 (rotation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
	{
		localAngle = rotationParam.RotationEasing.GetValue(rotation_values.easing, m_LivingTime / m_LivedTime);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
	{
		rotation_values.axis.rotation = rotation_values.axis.random.rotation + rotation_values.axis.random.velocity * m_LivingTime +
										rotation_values.axis.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		rotation_values.axis.rotation = rotationParam.RotationAxisEasing.easing.GetValue(rotation_values.axis.easing, m_LivingTime / m_LivedTime);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		assert(rotationParam.RotationFCurve != nullptr);
		auto fcurve = rotationParam.RotationFCurve->GetValues(m_LivingTime, m_LivedTime);
		localAngle = fcurve + rotation_values.fcruve.offset;
	}
}

void ApplyDynamicParameterToFixedScaling(ScalingState& scaling_values,
										 const ScalingParameter& scalingParam,
										 RandObject& rand,
										 const Effect* effect,
										 const InstanceGlobal* instanceGlobal,
										 const Instance* m_pParent,
										 const DynamicFactorParameter& dynamicFactor)
{
	if (scalingParam.ScalingFixed.RefEq >= 0)
	{
		scaling_values.fixed.scale = ApplyEq(effect,
											 instanceGlobal,
											 m_pParent,
											 &rand,
											 scalingParam.ScalingFixed.RefEq,
											 scalingParam.ScalingFixed.Position,
											 dynamicFactor.Scale,
											 dynamicFactor.ScaleInv);
	}
}

void InitScaling(ScalingState& scaling_values, const ScalingParameter& scalingParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
{
	if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
	{
		scaling_values.fixed.scale = scalingParam.ScalingFixed.Position;
		ApplyDynamicParameterToFixedScaling(scaling_values, scalingParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   scalingParam.ScalingPVA.RefEqP,
						   scalingParam.ScalingPVA.Position,
						   dynamicFactor.Scale,
						   dynamicFactor.ScaleInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   scalingParam.ScalingPVA.RefEqV,
						   scalingParam.ScalingPVA.Velocity,
						   dynamicFactor.Scale,
						   dynamicFactor.ScaleInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   scalingParam.ScalingPVA.RefEqA,
						   scalingParam.ScalingPVA.Acceleration,
						   dynamicFactor.Scale,
						   dynamicFactor.ScaleInv);

		scaling_values.random.scale = rvl.getValue(rand);
		scaling_values.random.velocity = rvv.getValue(rand);
		scaling_values.random.acceleration = rva.getValue(rand);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
	{
		scalingParam.ScalingEasing.Init(scaling_values.easing, effect, instanceGlobal, m_pParent, &rand, dynamicFactor.Scale, dynamicFactor.ScaleInv);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
	{
		scaling_values.single_random.scale = scalingParam.ScalingSinglePVA.Position.getValue(rand);
		scaling_values.single_random.velocity = scalingParam.ScalingSinglePVA.Velocity.getValue(rand);
		scaling_values.single_random.acceleration = scalingParam.ScalingSinglePVA.Acceleration.getValue(rand);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
	{
		scalingParam.ScalingSingleEasing.Init(scaling_values.single_easing, effect, instanceGlobal, m_pParent, &rand);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
	{
		assert(scalingParam.ScalingFCurve != nullptr);

		scaling_values.fcruve.offset = scalingParam.ScalingFCurve->GetOffsets(rand);
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
	{
		assert(scalingParam.ScalingSingleFCurve != nullptr);

		scaling_values.single_fcruve.offset = scalingParam.ScalingSingleFCurve->S.GetOffset(rand);
	}
}

void UpdateScaling(SIMD::Vec3f& localScaling, ScalingState& scaling_values, const ScalingParameter& scalingParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
{
	/* 拡大の更新(時間から直接求めれるよう対応済み) */
	if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_None)
	{
		localScaling = {1.0f, 1.0f, 1.0f};
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
	{
		ApplyDynamicParameterToFixedScaling(scaling_values, scalingParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);

		localScaling = scaling_values.fixed.scale;
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
	{
		/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
		localScaling = scaling_values.random.scale + (scaling_values.random.velocity * m_LivingTime) +
					   (scaling_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
	{
		localScaling = scalingParam.ScalingEasing.GetValue(scaling_values.easing, m_LivingTime / m_LivedTime);
		/*
			localScaling = m_pEffectNode->ScalingEasing.Position.getValue(
				scaling_values.easing.start, scaling_values.easing.end, m_LivingTime / m_LivedTime);
			*/
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
	{
		float s = scaling_values.single_random.scale + scaling_values.single_random.velocity * m_LivingTime +
				  scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
		localScaling = {s, s, s};
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
	{
		float s = scalingParam.ScalingSingleEasing.GetValue(scaling_values.single_easing, m_LivingTime / m_LivedTime);
		localScaling = {s, s, s};
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
	{
		assert(scalingParam.ScalingFCurve != nullptr);
		auto fcurve = scalingParam.ScalingFCurve->GetValues(m_LivingTime, m_LivedTime);
		localScaling = fcurve + scaling_values.fcruve.offset;
	}
	else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
	{
		assert(scalingParam.ScalingSingleFCurve != nullptr);
		auto s = scalingParam.ScalingSingleFCurve->GetValues(m_LivingTime, m_LivedTime) + scaling_values.single_fcruve.offset;
		localScaling = {s, s, s};
	}
}

} // namespace

Instance::Instance(ManagerImplemented* pManager, EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup)
	: m_pManager(pManager)
	, m_pEffectNode(pEffectNode)
	, m_pContainer(pContainer)
	, ownGroup_(pGroup)
	, childrenGroups_(nullptr)
	, m_pParent(nullptr)
	, m_State(INSTANCE_STATE_ACTIVE)
	, m_LivedTime(0)
	, m_LivingTime(0)
	, m_GlobalMatrix43Calculated(false)
	, m_ParentMatrix43Calculated(false)
	, is_time_step_allowed(false)
	, m_sequenceNumber(0)
{
	ColorInheritance = Color(255, 255, 255, 255);
	ColorParent = Color(255, 255, 255, 255);

	InstanceGroup* group = nullptr;

	for (int i = 0; i < m_pEffectNode->GetChildrenCount(); i++)
	{
		InstanceContainer* childContainer = m_pContainer->GetChild(i);

		auto allocated = childContainer->CreateInstanceGroup();

		// Lack of memory
		if (allocated == nullptr)
		{
			break;
		}

		if (group != nullptr)
		{
			group->NextUsedByInstance = allocated;
			group = allocated;
		}
		else
		{
			group = allocated;
			childrenGroups_ = group;
		}
	}

	for (auto& data : uvAnimationData_)
	{
		data.uvTimeOffset = 0;
	}
}

Instance::~Instance()
{
	assert(m_State != INSTANCE_STATE_ACTIVE);
}

void Instance::GenerateChildrenInRequired()
{
	if (m_State == INSTANCE_STATE_DISPOSING)
	{
		return;
	}

	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->GenerateInstancesInRequirred(m_LivingTime, m_randObject, this);
	}
}

void Instance::UpdateChildrenGroupMatrix()
{
	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->SetParentMatrix(m_GlobalMatrix43);
	}
}

InstanceGlobal* Instance::GetInstanceGlobal()
{
	return m_pContainer->GetRootInstance();
}

eInstanceState Instance::GetState() const
{
	return m_State;
}

const SIMD::Mat43f& Instance::GetGlobalMatrix43() const
{
	return m_GlobalMatrix43;
}

void Instance::Initialize(Instance* parent, int32_t instanceNumber, const SIMD::Mat43f& globalMatrix)
{
	assert(this->m_pContainer != nullptr);

	// Initialize a state
	m_State = INSTANCE_STATE_ACTIVE;

	// Initialize paramaters about a parent
	m_pParent = parent;
	m_ParentMatrix = SIMD::Mat43f::Identity;

	m_GlobalMatrix43 = globalMatrix;
	assert(m_GlobalMatrix43.IsValid());

	m_LivingTime = 0.0f;
	m_LivedTime = FLT_MAX;
	m_RemovingTime = 0.0f;

	m_InstanceNumber = instanceNumber;

	m_IsFirstTime = true;

	auto instanceGlobal = this->m_pContainer->GetRootInstance();

	// Set random seed from InstanceGlobal's randomizer
	m_randObject.SetSeed(instanceGlobal->GetRandObject().GetRandInt());

	prevPosition_ = SIMD::Vec3f(0, 0, 0);
}

void Instance::FirstUpdate()
{
	m_IsFirstTime = false;
	assert(this->m_pContainer != nullptr);

	auto effect = this->m_pEffectNode->m_effect;
	auto instanceGlobal = this->m_pContainer->GetRootInstance();
	auto& rand = m_randObject;

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	// initialize children
	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->Initialize(rand, this);
	}

	if (m_pParent == nullptr)
	{
		// initialize SRT
		m_GenerationLocation = SIMD::Mat43f::Identity;

		// initialize Parent
		m_ParentMatrix = SIMD::Mat43f::Identity;

		// Generate zero frame effect

		// for new children
		// UpdateChildrenGroupMatrix();
		//
		// GenerateChildrenInRequired(0.0f);

		return;
	}

	const int32_t parentTime = (int32_t)std::max(0.0f, this->m_pParent->m_LivingTime);

	{
		auto ri = ApplyEq(effect, instanceGlobal, m_pParent, &rand, parameter->CommonValues.RefEqLife, parameter->CommonValues.life);
		m_LivedTime = (float)ri.getValue(rand);
	}

	// initialize SRT

	// calculate parent matrixt to get matrix
	m_pParent->CalculateMatrix(0);

	const SIMD::Mat43f& parentMatrix = m_pParent->GetGlobalMatrix43();
	forceField_.Reset();
	m_GenerationLocation = SIMD::Mat43f::Identity;

	if (parameter->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating ||
		parameter->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent ||
		parameter->CommonValues.RotationBindType == BindType::WhenCreating ||
		parameter->CommonValues.ScalingBindType == BindType::WhenCreating ||
		(parameter->CommonValues.TranslationBindType == TranslationParentBindType::Always &&
		 parameter->CommonValues.RotationBindType == BindType::Always &&
		 parameter->CommonValues.ScalingBindType == BindType::Always))
	{
		m_ParentMatrix = parentMatrix;
		assert(m_ParentMatrix.IsValid());
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

	steeringVec_ = SIMD::Vec3f(0, 0, 0);

	if (m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
		m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
	{
		followParentParam.maxFollowSpeed = m_pEffectNode->SteeringBehaviorParam.MaxFollowSpeed.getValue(rand);
		followParentParam.steeringSpeed = m_pEffectNode->SteeringBehaviorParam.SteeringSpeed.getValue(rand) / 100.0f;
	}

	m_pEffectNode->TranslationParam.InitializeTranslationState(translation_values, prevPosition_, steeringVec_, rand, effect, instanceGlobal, m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);

	InitRotation(rotation_values, m_pEffectNode->RotationParam, rand, effect, instanceGlobal, m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);
	InitScaling(scaling_values, m_pEffectNode->ScalingParam, rand, effect, instanceGlobal, m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);

	/*
	// Rotation
	if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
	{
		rotation_values.fixed.rotation = m_pEffectNode->RotationParam.RotationFixed.Position;
		ApplyDynamicParameterToFixedRotation();
	}
	else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->RotationParam.RotationPVA.RefEqP,
						   m_pEffectNode->RotationParam.RotationPVA.rotation,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->RotationParam.RotationPVA.RefEqV,
						   m_pEffectNode->RotationParam.RotationPVA.velocity,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->RotationParam.RotationPVA.RefEqA,
						   m_pEffectNode->RotationParam.RotationPVA.acceleration,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);

		rotation_values.random.rotation = rvl.getValue(rand);
		rotation_values.random.velocity = rvv.getValue(rand);
		rotation_values.random.acceleration = rva.getValue(rand);
	}
	else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
	{
		m_pEffectNode->RotationParam.RotationEasing.Init(rotation_values.easing, effect, instanceGlobal, m_pParent, &rand, m_pEffectNode->DynamicFactor.Rot, m_pEffectNode->DynamicFactor.RotInv);
	}
	else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
	{
		rotation_values.axis.random.rotation = m_pEffectNode->RotationParam.RotationAxisPVA.rotation.getValue(rand);
		rotation_values.axis.random.velocity = m_pEffectNode->RotationParam.RotationAxisPVA.velocity.getValue(rand);
		rotation_values.axis.random.acceleration = m_pEffectNode->RotationParam.RotationAxisPVA.acceleration.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
		rotation_values.axis.axis = m_pEffectNode->RotationParam.RotationAxisPVA.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		rotation_values.axis.easing.start = m_pEffectNode->RotationParam.RotationAxisEasing.easing.start.getValue(rand);
		rotation_values.axis.easing.end = m_pEffectNode->RotationParam.RotationAxisEasing.easing.end.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
		rotation_values.axis.axis = m_pEffectNode->RotationParam.RotationAxisEasing.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		assert(m_pEffectNode->RotationParam.RotationFCurve != nullptr);

		rotation_values.fcruve.offset = m_pEffectNode->RotationParam.RotationFCurve->GetOffsets(rand);
	}

	// Scaling
	if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
	{
		scaling_values.fixed.scale = m_pEffectNode->ScalingParam.ScalingFixed.Position;
		ApplyDynamicParameterToFixedScaling();
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->ScalingParam.ScalingPVA.RefEqP,
						   m_pEffectNode->ScalingParam.ScalingPVA.Position,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->ScalingParam.ScalingPVA.RefEqV,
						   m_pEffectNode->ScalingParam.ScalingPVA.Velocity,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   m_pParent,
						   &rand,
						   m_pEffectNode->ScalingParam.ScalingPVA.RefEqA,
						   m_pEffectNode->ScalingParam.ScalingPVA.Acceleration,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);

		scaling_values.random.scale = rvl.getValue(rand);
		scaling_values.random.velocity = rvv.getValue(rand);
		scaling_values.random.acceleration = rva.getValue(rand);
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
	{
		m_pEffectNode->ScalingParam.ScalingEasing.Init(scaling_values.easing, effect, instanceGlobal, m_pParent, &rand, m_pEffectNode->DynamicFactor.Scale, m_pEffectNode->DynamicFactor.ScaleInv);
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
	{
		scaling_values.single_random.scale = m_pEffectNode->ScalingParam.ScalingSinglePVA.Position.getValue(rand);
		scaling_values.single_random.velocity = m_pEffectNode->ScalingParam.ScalingSinglePVA.Velocity.getValue(rand);
		scaling_values.single_random.acceleration = m_pEffectNode->ScalingParam.ScalingSinglePVA.Acceleration.getValue(rand);
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
	{
		m_pEffectNode->ScalingParam.ScalingSingleEasing.Init(scaling_values.single_easing, effect, instanceGlobal, m_pParent, &rand);
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
	{
		assert(m_pEffectNode->ScalingParam.ScalingFCurve != nullptr);

		scaling_values.fcruve.offset = m_pEffectNode->ScalingParam.ScalingFCurve->GetOffsets(rand);
	}
	else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
	{
		assert(m_pEffectNode->ScalingParam.ScalingSingleFCurve != nullptr);

		scaling_values.single_fcruve.offset = m_pEffectNode->ScalingParam.ScalingSingleFCurve->S.GetOffset(rand);
	}
	*/

	// Spawning Method
	const auto magnification = ((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification();
	m_GenerationLocation = m_pEffectNode->GenerationLocation.GenerateGenerationPosition(
		*(m_pEffectNode->GetEffect()),
		m_InstanceNumber,
		parentTime,
		magnification,
		m_pManager->GetCoordinateSystem(),
		rand);

	if (m_pEffectNode->SoundType == ParameterSoundType_Use)
	{
		soundValues.delay = (int32_t)m_pEffectNode->Sound.Delay.getValue(rand);
	}

	// UV
	for (int32_t i = 0; i < ParameterRendererCommon::UVParameterNum; i++)
	{
		UVFunctions::InitUVState(uvAnimationData_[i], rand, m_pEffectNode->RendererCommon.UVs[i]);
	}

	m_AlphaThreshold = AlphaCutoffFunctions::InitAlphaThreshold(alpha_cutoff_values, rand, m_pEffectNode->AlphaCutoff, m_pParent, effect, instanceGlobal);

	// CustomData
	for (int32_t index = 0; index < 2; index++)
	{
		ParameterCustomData* parameterCustomData = nullptr;
		InstanceCustomData* instanceCustomData = nullptr;

		if (index == 0)
		{
			parameterCustomData = &m_pEffectNode->RendererCommon.CustomData1;
			instanceCustomData = &customDataValues1;
		}
		else if (index == 1)
		{
			parameterCustomData = &m_pEffectNode->RendererCommon.CustomData2;
			instanceCustomData = &customDataValues2;
		}

		CustomDataFunctions::InitCustomData(*instanceCustomData, rand, parameterCustomData);
	}

	if (m_pEffectNode->GenerationLocation.EffectsRotation)
	{
		auto tempPrevPosition = prevPosition_;
		tempPrevPosition += m_GenerationLocation.GetTranslation();
		prevGlobalPosition_ = SIMD::Vec3f::Transform(tempPrevPosition, m_ParentMatrix);
	}
	else
	{
		prevPosition_ += m_GenerationLocation.GetTranslation();
		prevGlobalPosition_ = SIMD::Vec3f::Transform(prevPosition_, m_ParentMatrix);
	}

	m_pEffectNode->InitializeRenderedInstance(*this, *ownGroup_, m_pManager);
}

void Instance::Update(float deltaFrame, bool shown)
{
	assert(this->m_pContainer != nullptr);

	if (IsFirstTime())
	{
		FirstUpdate();
	}

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

			if (living_time <= (float)soundValues.delay && (float)soundValues.delay < living_time_p)
			{
				m_pManager->RequestToPlaySound(this, m_pEffectNode);
			}
		}
	}

	// step time
	// frame 0 - generated time
	// frame 1- now
	if (is_time_step_allowed)
	{
		m_LivingTime += deltaFrame;
	}

	if (shown)
	{
		CalculateMatrix(deltaFrame);
	}
	else if (m_pEffectNode->LocalForceField.HasValue)
	{
		// If attraction forces are not default, updating is needed in each frame.
		CalculateMatrix(deltaFrame);
	}

	// Get parent color.
	if (m_pParent != nullptr)
	{
		if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
		{
			ColorParent = m_pParent->ColorInheritance;
		}
	}

	/* 親の削除処理 */
	if (m_pParent != nullptr && !m_pParent->IsActive())
	{
		CalculateParentMatrix(deltaFrame);
		m_pParent = nullptr;
	}

	// Create child particles
	// if( !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
	//{
	//	GenerateChildrenInRequired(m_LivingTime);
	//}

	UpdateChildrenGroupMatrix();

	if (m_pEffectNode->m_effect->GetVersion() >= 1600)
	{
		auto effect = this->m_pEffectNode->m_effect;
		auto instanceGlobal = this->m_pContainer->GetRootInstance();
		auto& rand = m_randObject;

		m_AlphaThreshold = AlphaCutoffFunctions::CalcAlphaThreshold(rand, m_pParent, m_pEffectNode->AlphaCutoff, alpha_cutoff_values, effect, instanceGlobal, m_LivingTime, m_LivedTime);
	}

	if (m_State == INSTANCE_STATE_ACTIVE)
	{
		// check whether killed?
		bool removed = false;

		if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
		{
			// if pass time
			if (m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct)
			{
				if (m_LivingTime > m_LivedTime)
				{
					removed = true;
				}
			}

			// if remove parent
			if (!removed && m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved)
			{
				if (m_pParent == nullptr || m_pParent->GetState() != INSTANCE_STATE_ACTIVE)
				{
					m_pParent = nullptr;
					removed = true;
				}
			}

			// if children are removed and going not to generate a child
			if (!removed && m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct)
			{
				removed = !AreChildrenActive();
			}

			// remove by trigger
			if (!removed && m_pEffectNode->TriggerParam.ToRemove.type != TriggerType::None)
			{
				if (GetInstanceGlobal()->GetInputTriggerCount(m_pEffectNode->TriggerParam.ToRemove.index) > 0)
				{
					removed = true;
				}
			}
		}

		if (removed)
		{
			// if it need to calculate a matrix
			if (m_pEffectNode->GetChildrenCount() > 0)
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

			if (m_pEffectNode->RendererCommon.FadeOutType == ParameterRendererCommon::FADEOUT_AFTER_REMOVED)
			{
				m_State = INSTANCE_STATE_REMOVING;
			}
			else
			{
				// Delete this particle with myself.
				Kill();
				return;
			}
		}
	}
	else if (m_State == INSTANCE_STATE_REMOVING)
	{
		m_RemovingTime += deltaFrame;

		if (m_RemovingTime > m_pEffectNode->RendererCommon.FadeOut.Frame)
		{
			Kill();
			return;
		}
	}

	// allow to pass time
	is_time_step_allowed = true;
}

bool Instance::AreChildrenActive() const
{
	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		if (group->IsActive())
		{
			return true;
		}
	}
	return false;
}

float Instance::GetFlipbookIndexAndNextRate() const
{
	auto& CommonValue = this->m_pEffectNode->RendererCommon;
	return GetFlipbookIndexAndNextRate(CommonValue.UVs[0].Type, CommonValue.UVs[0], uvAnimationData_[0]);
}

void Instance::CalculateMatrix(float deltaFrame)
{
	// 計算済なら終了
	if (m_GlobalMatrix43Calculated)
		return;

	// if( m_sequenceNumber == ((ManagerImplemented*)m_pManager)->GetSequenceNumber() ) return;
	m_sequenceNumber = ((ManagerImplemented*)m_pManager)->GetSequenceNumber();
	const auto coordinateSystem = m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem();

	assert(m_pEffectNode != nullptr);
	assert(m_pContainer != nullptr);

	// 親の処理
	if (m_pParent != nullptr)
	{
		CalculateParentMatrix(deltaFrame);
	}

	/* 更新処理 */
	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		SIMD::Vec3f localPosition{};
		SIMD::Vec3f localAngle;
		SIMD::Vec3f localScaling;

		m_pEffectNode->TranslationParam.CalculateTranslationState(translation_values, localPosition, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, coordinateSystem, m_pEffectNode->DynamicFactor);

		if (!m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			localPosition += m_GenerationLocation.GetTranslation();
		}

		if (m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
			m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
		{
			localPosition = prevPosition_;

			SIMD::Vec3f worldPos = SIMD::Vec3f::Transform(localPosition, m_ParentMatrix);
			SIMD::Vec3f toTarget = parentPosition_ - worldPos;

			if (toTarget.GetLength() > followParentParam.maxFollowSpeed)
			{
				toTarget = toTarget.Normalize();
				toTarget *= followParentParam.maxFollowSpeed;
			}

			SIMD::Vec3f vSteering = toTarget - steeringVec_;
			vSteering *= followParentParam.steeringSpeed;

			steeringVec_ += vSteering * deltaFrame;

			if (steeringVec_.GetLength() > followParentParam.maxFollowSpeed)
			{
				steeringVec_ = steeringVec_.Normalize();
				steeringVec_ *= followParentParam.maxFollowSpeed;
			}

			SIMD::Vec3f followVelocity = steeringVec_ * deltaFrame * m_pEffectNode->m_effect->GetMaginification();
			localPosition += followVelocity;
		}

		// Velocitty
		SIMD::Vec3f localVelocity = SIMD::Vec3f(0, 0, 0);
		if (m_pEffectNode->LocalForceField.HasValue)
		{
			localVelocity = localPosition - prevPosition_;
		}

		prevPosition_ = localPosition;

		CalculateRotation(localAngle, rotation_values, m_pEffectNode->RotationParam, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);
		UpdateScaling(localScaling, scaling_values, m_pEffectNode->ScalingParam, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);

		/*
		if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_None)
		{
			localAngle = {0, 0, 0};
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
		{
			ApplyDynamicParameterToFixedRotation();

			localAngle = rotation_values.fixed.rotation;
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
		{
			localAngle = rotation_values.random.rotation + (rotation_values.random.velocity * m_LivingTime) +
						 (rotation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
		{
			localAngle = m_pEffectNode->RotationParam.RotationEasing.GetValue(rotation_values.easing, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
		{
			rotation_values.axis.rotation = rotation_values.axis.random.rotation + rotation_values.axis.random.velocity * m_LivingTime +
											rotation_values.axis.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f);
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
		{
			rotation_values.axis.rotation = m_pEffectNode->RotationParam.RotationAxisEasing.easing.GetValue(rotation_values.axis.easing, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
		{
			assert(m_pEffectNode->RotationParam.RotationFCurve != nullptr);
			auto fcurve = m_pEffectNode->RotationParam.RotationFCurve->GetValues(m_LivingTime, m_LivedTime);
			localAngle = fcurve + rotation_values.fcruve.offset;
		}

		if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_None)
		{
			localScaling = {1.0f, 1.0f, 1.0f};
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
		{
			ApplyDynamicParameterToFixedScaling();

			localScaling = scaling_values.fixed.scale;
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
		{
			localScaling = scaling_values.random.scale + (scaling_values.random.velocity * m_LivingTime) +
						   (scaling_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
		{
			localScaling = m_pEffectNode->ScalingParam.ScalingEasing.GetValue(scaling_values.easing, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
		{
			float s = scaling_values.single_random.scale + scaling_values.single_random.velocity * m_LivingTime +
					  scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
			localScaling = {s, s, s};
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
		{
			float s = m_pEffectNode->ScalingParam.ScalingSingleEasing.GetValue(scaling_values.single_easing, m_LivingTime / m_LivedTime);
			localScaling = {s, s, s};
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
		{
			assert(m_pEffectNode->ScalingParam.ScalingFCurve != nullptr);
			auto fcurve = m_pEffectNode->ScalingParam.ScalingFCurve->GetValues(m_LivingTime, m_LivedTime);
			localScaling = fcurve + scaling_values.fcruve.offset;
		}
		else if (m_pEffectNode->ScalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
		{
			assert(m_pEffectNode->ScalingParam.ScalingSingleFCurve != nullptr);
			auto s = m_pEffectNode->ScalingParam.ScalingSingleFCurve->GetValues(m_LivingTime, m_LivedTime) + scaling_values.single_fcruve.offset;
			localScaling = {s, s, s};
		}
		*/

		// update local fields
		SIMD::Vec3f currentLocalPosition;

		if (m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			// the center of force field depends Spawn method
			// It should be used a result of past frame
			auto location = SIMD::Mat43f::Translation(localPosition);
			location *= m_GenerationLocation;

			localVelocity = SIMD::Vec3f::Transform(localVelocity, m_GenerationLocation.GetRotation());
			currentLocalPosition = location.GetTranslation();
		}
		else
		{
			currentLocalPosition = localPosition;
		}

		if (m_pEffectNode->LocalForceField.HasValue)
		{
			currentLocalPosition += forceField_.ModifyLocation;
			forceField_.ExternalVelocity = localVelocity;
			forceField_.Update(m_pEffectNode->LocalForceField, currentLocalPosition, m_pEffectNode->GetEffect()->GetMaginification(), deltaFrame, m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem());
		}

		/* 描画部分の更新 */
		m_pEffectNode->UpdateRenderedInstance(*this, *ownGroup_, m_pManager);

		// 回転行列の作成
		SIMD::Mat43f MatRot;
		if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed || m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA ||
			m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing || m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
		{
			MatRot = SIMD::Mat43f::RotationZXY(localAngle.GetZ(), localAngle.GetX(), localAngle.GetY());
		}
		else if (m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA ||
				 m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
		{
			SIMD::Vec3f axis = rotation_values.axis.axis;

			MatRot = SIMD::Mat43f::RotationAxis(axis, rotation_values.axis.rotation);
		}
		else
		{
			MatRot = SIMD::Mat43f::Identity;
		}

		// Update matrix
		if (m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			m_GlobalMatrix43 = SIMD::Mat43f::SRT(localScaling, MatRot, localPosition);
			assert(m_GlobalMatrix43.IsValid());

			m_GlobalMatrix43 *= m_GenerationLocation;
			assert(m_GlobalMatrix43.IsValid());

			m_GlobalMatrix43 *= SIMD::Mat43f::Translation(forceField_.ModifyLocation);
		}
		else
		{
			localPosition += forceField_.ModifyLocation;

			m_GlobalMatrix43 = SIMD::Mat43f::SRT(localScaling, MatRot, localPosition);
			assert(m_GlobalMatrix43.IsValid());
		}

		if (m_pEffectNode->TranslationParam.TranslationType != ParameterTranslationType_ViewOffset)
		{
			m_GlobalMatrix43 *= m_ParentMatrix;
			assert(m_GlobalMatrix43.IsValid());
		}

		if (m_pEffectNode->LocalForceField.IsGlobalEnabled)
		{
			InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();
			forceField_.UpdateGlobal(m_pEffectNode->LocalForceField, prevGlobalPosition_, m_pEffectNode->GetEffect()->GetMaginification(), instanceGlobal->GetTargetLocation(), deltaFrame, m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem());
			SIMD::Mat43f MatTraGlobal = SIMD::Mat43f::Translation(forceField_.GlobalModifyLocation);
			m_GlobalMatrix43 *= MatTraGlobal;
		}

		prevGlobalPosition_ = m_GlobalMatrix43.GetTranslation();
	}

	m_GlobalMatrix43Calculated = true;
}

void Instance::CalculateParentMatrix(float deltaFrame)
{
	// 計算済なら終了
	if (m_ParentMatrix43Calculated)
		return;

	// 親の行列を計算
	m_pParent->CalculateMatrix(deltaFrame);

	parentPosition_ = m_pParent->GetGlobalMatrix43().GetTranslation();

	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		TranslationParentBindType tType = m_pEffectNode->CommonValues.TranslationBindType;
		BindType rType = m_pEffectNode->CommonValues.RotationBindType;
		BindType sType = m_pEffectNode->CommonValues.ScalingBindType;

		if ((tType == BindType::WhenCreating || tType == TranslationParentBindType::WhenCreating_FollowParent) && rType == BindType::WhenCreating && sType == BindType::WhenCreating)
		{
			// do not do anything
		}
		else if (tType == BindType::Always && rType == BindType::Always && sType == BindType::Always)
		{
			m_ParentMatrix = ownGroup_->GetParentMatrix();
			assert(m_ParentMatrix.IsValid());
		}
		else
		{
			SIMD::Vec3f s, t;
			SIMD::Mat43f r;

			if (tType == BindType::WhenCreating || tType == TranslationParentBindType::WhenCreating_FollowParent)
				t = m_ParentMatrix.GetTranslation();
			else
				t = ownGroup_->GetParentTranslation();

			if (rType == BindType::WhenCreating)
				r = m_ParentMatrix.GetRotation();
			else
				r = ownGroup_->GetParentRotation();

			if (sType == BindType::WhenCreating)
				s = m_ParentMatrix.GetScale();
			else
				s = ownGroup_->GetParentScale();

			m_ParentMatrix = SIMD::Mat43f::SRT(s, r, t);
			assert(m_ParentMatrix.IsValid());
		}
	}

	m_ParentMatrix43Calculated = true;
}

void Instance::ApplyDynamicParameterToFixedRotation()
{
	if (m_pEffectNode->RotationParam.RotationFixed.RefEq >= 0)
	{
		rotation_values.fixed.rotation = ApplyEq(m_pEffectNode->GetEffect(),
												 m_pContainer->GetRootInstance(),
												 m_pParent,
												 &m_randObject,
												 m_pEffectNode->RotationParam.RotationFixed.RefEq,
												 m_pEffectNode->RotationParam.RotationFixed.Position,
												 m_pEffectNode->DynamicFactor.Rot,
												 m_pEffectNode->DynamicFactor.RotInv);
	}
}

void Instance::ApplyDynamicParameterToFixedScaling()
{
	if (m_pEffectNode->ScalingParam.ScalingFixed.RefEq >= 0)
	{
		scaling_values.fixed.scale = ApplyEq(m_pEffectNode->GetEffect(),
											 m_pContainer->GetRootInstance(),
											 m_pParent,
											 &m_randObject,
											 m_pEffectNode->ScalingParam.ScalingFixed.RefEq,
											 m_pEffectNode->ScalingParam.ScalingFixed.Position,
											 m_pEffectNode->DynamicFactor.Scale,
											 m_pEffectNode->DynamicFactor.ScaleInv);
	}
}

float Instance::GetFlipbookIndexAndNextRate(const UVAnimationType& UVType, const UVParameter& UV, const InstanceUVState& data) const
{
	if (UVType == UVAnimationType::Animation)
	{
		auto time = GetUVTime();

		// 経過時間を取得
		if (m_pEffectNode->GetType() == eEffectNodeType::EFFECT_NODE_TYPE_RIBBON ||
			m_pEffectNode->GetType() == eEffectNodeType::EFFECT_NODE_TYPE_TRACK)
		{
			// is GetFirstGroup bug?
			auto baseInstance = this->GetContainer()->GetFirstGroup()->GetFirst();
			if (baseInstance != nullptr)
			{
				time = baseInstance->GetUVTime();
			}
		}

		float fFrameNum = time / (float)UV.Animation.FrameLength;
		int32_t frameNum = (int32_t)fFrameNum;
		int32_t frameCount = UV.Animation.FrameCountX * UV.Animation.FrameCountY;

		if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_ONCE)
		{
			if (frameNum >= frameCount)
			{
				frameNum = frameCount - 1;
			}
		}
		else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_LOOP)
		{
			frameNum %= frameCount;
		}
		else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_REVERSELOOP)
		{
			bool rev = (frameNum / frameCount) % 2 == 1;
			frameNum %= frameCount;
			if (rev)
			{
				frameNum = frameCount - 1 - frameNum;
			}
		}

		float flipbookIndexAndNextRate = 0;
		flipbookIndexAndNextRate = static_cast<float>(frameNum);
		flipbookIndexAndNextRate += fFrameNum - static_cast<float>(frameNum);
		return flipbookIndexAndNextRate;
	}

	return 0;
}

float Instance::GetUVTime() const
{
	return m_LivingTime + uvAnimationData_[0].uvTimeOffset;
}

void Instance::Draw(Instance* next, void* userData)
{
	assert(m_pEffectNode != nullptr);

	if (!m_pEffectNode->IsRendered)
		return;

	if (m_sequenceNumber != ((ManagerImplemented*)m_pManager)->GetSequenceNumber())
	{
		CalculateMatrix(0);
	}

	m_pEffectNode->Rendering(*this, next, m_pManager, userData);
}

void Instance::Kill()
{
	if (IsActive())
	{
		for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
		{
			group->IsReferencedFromInstance = false;
		}

		m_State = INSTANCE_STATE_REMOVED;
	}
}

RectF Instance::GetUV(const int32_t index) const
{
	return UVFunctions::GetUV(
		uvAnimationData_[index],
		m_pEffectNode->RendererCommon.UVs[index],
		m_LivingTime,
		m_LivedTime);
}

std::array<float, 4> Instance::GetCustomData(int32_t index) const
{
	assert(0 <= index && index < 2);

	ParameterCustomData* parameterCustomData = nullptr;
	const InstanceCustomData* instanceCustomData = nullptr;

	if (index == 0)
	{
		parameterCustomData = &m_pEffectNode->RendererCommon.CustomData1;
		instanceCustomData = &customDataValues1;
	}
	else if (index == 1)
	{
		parameterCustomData = &m_pEffectNode->RendererCommon.CustomData2;
		instanceCustomData = &customDataValues2;
	}
	else
	{
		return std::array<float, 4>{0.0f, 0.0f, 0, 0};
	}

	return CustomDataFunctions::GetCustomData(parameterCustomData, instanceCustomData, this->m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime);
}

} // namespace Effekseer
