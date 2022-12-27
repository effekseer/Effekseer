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

void TimeSeriesMatrix::Reset(const SIMD::Mat43f& matrix, float time)
{
	previous_ = matrix;
	current_ = matrix;
	previousTime_ = 0.0f;
	currentTime_ = 0.0f;
	isCurrentMatrixSpecified_ = true;
}

void TimeSeriesMatrix::Step(const SIMD::Mat43f& matrix, float time)
{
	if (isCurrentMatrixSpecified_)
	{
		previous_ = current_;
	}
	else
	{
		previous_ = matrix;
	}

	current_ = matrix;
	previousTime_ = currentTime_;
	currentTime_ = time;
	isCurrentMatrixSpecified_ = true;
}

const SIMD::Mat43f& TimeSeriesMatrix::GetPrevious() const
{
	return previous_;
}

const SIMD::Mat43f& TimeSeriesMatrix::GetCurrent() const
{
	return current_;
}

SIMD::Mat43f TimeSeriesMatrix::Get(float time) const
{
	if (time >= currentTime_)
	{
		return GetCurrent();
	}
	else if (time <= previousTime_)
	{
		return GetPrevious();
	}

	SIMD::Vec3f s_previous;
	SIMD::Mat43f r_previous;
	SIMD::Vec3f t_previous;

	previous_.GetSRT(s_previous, r_previous, t_previous);

	SIMD::Vec3f s_current;
	SIMD::Mat43f r_current;
	SIMD::Vec3f t_current;

	current_.GetSRT(s_current, r_current, t_current);

	const auto q_previous = SIMD::Quaternionf::FromMatrix(r_previous);
	const auto q_current = SIMD::Quaternionf::FromMatrix(r_current);

	const auto alpha = (time - previousTime_) / (currentTime_ - previousTime_);

	const auto t = t_current * alpha + t_previous * (1.0f - alpha);
	const auto s = s_current * alpha + s_previous * (1.0f - alpha);
	const auto q = SIMD::Quaternionf::Slerp(q_previous, q_current, alpha);

	return SIMD::Mat43f::SRT(s, q.ToMatrix(), t);
}

Instance::Instance(ManagerImplemented* pManager, EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup)
	: m_pManager(pManager)
	, m_pEffectNode(pEffectNode)
	, m_pContainer(pContainer)
	, ownGroup_(pGroup)
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
	assert(m_State != eInstanceState::INSTANCE_STATE_ACTIVE);
}

void Instance::GenerateChildrenInRequired()
{
	if (m_State == eInstanceState::INSTANCE_STATE_DISPOSING)
	{
		return;
	}

	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->GenerateInstancesIfRequired(m_LivingTime, m_randObject, this);
	}
}

void Instance::UpdateChildrenGroupMatrix()
{
	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->SetParentMatrix(globalMatrix_.GetCurrent());
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

const TimeSeriesMatrix& Instance::GetGlobalMatrix() const
{
	return globalMatrix_;
}

const SIMD::Mat43f& Instance::GetRenderedGlobalMatrix() const
{
	return globalMatrix_rendered;
}

void Instance::ResetGlobalMatrix(const SIMD::Mat43f& mat)
{
	if (globalMatrix_.GetCurrent() == mat)
	{
		return;
	}

	m_sequenceNumber = m_pManager->GetSequenceNumber();
	globalMatrix_.Reset(mat, m_LivingTime);
	UpdateChildrenGroupMatrix();
	m_GlobalMatrix43Calculated = true;
}

void Instance::UpdateGlobalMatrix(const SIMD::Mat43f& mat)
{
	if (globalMatrix_.GetCurrent() == mat)
	{
		return;
	}

	m_sequenceNumber = m_pManager->GetSequenceNumber();
	globalMatrix_.Step(mat, m_LivingTime);
	UpdateChildrenGroupMatrix();
	m_GlobalMatrix43Calculated = true;
}

void Instance::ApplyBaseMatrix(const SIMD::Mat43f& baseMatrix)
{
	globalMatrix_rendered = globalMatrix_.GetCurrent() * baseMatrix;
	assert(globalMatrix_rendered.IsValid());
}

void Instance::Initialize(Instance* parent, float spawnDeltaFrame, int32_t instanceNumber)
{
	assert(this->m_pContainer != nullptr);

	// Initialize a state
	m_State = eInstanceState::INSTANCE_STATE_ACTIVE;

	// Initialize paramaters about a parent
	m_pParent = parent;
	m_ParentMatrix = SIMD::Mat43f::Identity;
	m_LivingTime = 0.0f;
	m_LivedTime = FLT_MAX;
	m_RemovingTime = 0.0f;

	spawnDeltaFrame_ = spawnDeltaFrame;

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

		return;
	}

	const int32_t parentTime = (int32_t)std::max(0.0f, this->m_pParent->m_LivingTime);

	{
		auto ri = ApplyEq(effect, instanceGlobal, m_pParent, &rand, parameter->CommonValues.RefEqLife, parameter->CommonValues.life);
		m_LivedTime = (float)ri.getValue(rand);
	}

	// initialize SRT

	// calculate parent matrixt to get matrix
	m_pParent->UpdateTransform(0);

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
		if ((parameter->CommonValues.TranslationBindType == TranslationParentBindType::Always &&
			 parameter->CommonValues.RotationBindType == BindType::Always &&
			 parameter->CommonValues.ScalingBindType == BindType::Always) ||
			!parameter->IsParticleSpawnedWithDecimal())
		{
			m_ParentMatrix = m_pParent->GetGlobalMatrix().GetCurrent();
		}
		else
		{
			m_ParentMatrix = m_pParent->GetGlobalMatrix().Get(spawnDeltaFrame_);
		}

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

	RotationFunctions::InitRotation(rotation_values, m_pEffectNode->RotationParam, rand, effect, instanceGlobal, m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);
	ScalingFunctions::InitScaling(scaling_values, m_pEffectNode->ScalingParam, rand, effect, instanceGlobal, m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);

	// Spawning Method
	const auto magnification = ((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification();
	m_GenerationLocation = m_pEffectNode->GenerationLocation.GenerateGenerationPosition(
		*(m_pEffectNode->GetEffect()),
		m_InstanceNumber,
		parentTime,
		magnification,
		m_pManager->GetCoordinateSystem(),
		rand);

	if (m_pEffectNode->Sound.SoundType == ParameterSoundType_Use)
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
		prevPosition_ = SIMD::Vec3f::Transform(prevPosition_, m_GenerationLocation);
	}
	else
	{
		prevPosition_ += m_GenerationLocation.GetTranslation();
	}

	prevGlobalPosition_ = SIMD::Vec3f::Transform(prevPosition_, m_ParentMatrix);

	m_pEffectNode->InitializeRenderedInstance(*this, *ownGroup_, m_pManager);
}

void Instance::Update(float deltaFrame, bool shown)
{
	assert(this->m_pContainer != nullptr);

	if (IsFirstTime())
	{
		FirstUpdate();
	}

	const auto isParentRemoving = m_pParent != nullptr && !m_pParent->IsActive();

	bool isParentSequenceChanged = false;
	if (m_pParent != nullptr)
	{
		isParentSequenceChanged = m_pParent->m_sequenceNumber >= m_sequenceNumber;
	}

	const bool isUpdateRequired = deltaFrame != 0.0f || m_pEffectNode->RotationParam.RotationType == ParameterRotationType::ParameterRotationType_RotateToViewpoint;

	if (m_GlobalMatrix43Calculated && (m_ParentMatrix43Calculated || m_pParent == nullptr) && !isUpdateRequired && !isParentRemoving && !isParentSequenceChanged)
	{
		return;
	}

	// Invalidate matrix
	m_GlobalMatrix43Calculated = false;
	m_ParentMatrix43Calculated = false;

	if (is_time_step_allowed && m_pEffectNode->GetType() != EffectNodeType::Root)
	{
		if (m_pEffectNode->Sound.SoundType == ParameterSoundType_Use && deltaFrame > 0)
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

	UpdateTransform(deltaFrame);

	// Get parent color.
	if (m_pParent != nullptr)
	{
		if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
		{
			ColorParent = m_pParent->ColorInheritance;
		}
	}

	if (isParentRemoving)
	{
		UpdateParentMatrix(deltaFrame);
		m_pParent = nullptr;
	}

	UpdateChildrenGroupMatrix();

	if (m_pEffectNode->m_effect->GetVersion() >= 1600)
	{
		auto effect = this->m_pEffectNode->m_effect;
		auto instanceGlobal = this->m_pContainer->GetRootInstance();
		auto& rand = m_randObject;

		m_AlphaThreshold = AlphaCutoffFunctions::CalcAlphaThreshold(rand, m_pParent, m_pEffectNode->AlphaCutoff, alpha_cutoff_values, effect, instanceGlobal, m_LivingTime, m_LivedTime);
	}

	if (m_State == eInstanceState::INSTANCE_STATE_ACTIVE)
	{
		// check whether killed?
		bool removed = false;

		if (m_pEffectNode->GetType() != EffectNodeType::Root)
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
				if (m_pParent == nullptr || m_pParent->GetState() != eInstanceState::INSTANCE_STATE_ACTIVE)
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

			// checking kill rules
			if (!removed && m_pEffectNode->KillParam.Type != KillType::None)
			{
				SIMD::Vec3f localPosition{};
				if (m_pEffectNode->KillParam.IsScaleAndRotationApplied)
				{
					SIMD::Mat44f invertedGlobalMatrix = this->GetInstanceGlobal()->InvertedEffectGlobalMatrix;
					localPosition = SIMD::Vec3f::Transform(this->prevGlobalPosition_, invertedGlobalMatrix);
				}
				else
				{
					SIMD::Mat44f globalMatrix = this->GetInstanceGlobal()->EffectGlobalMatrix;
					localPosition = this->prevGlobalPosition_ - globalMatrix.GetTranslation();
				}

				if (m_pEffectNode->KillParam.Type == KillType::Box)
				{
					localPosition = localPosition - m_pEffectNode->KillParam.Box.Center;
					localPosition = SIMD::Vec3f::Abs(localPosition);
					SIMD::Vec3f size = m_pEffectNode->KillParam.Box.Size;
					bool isWithin = localPosition.GetX() <= size.GetX() && localPosition.GetY() <= size.GetY() && localPosition.GetZ() <= size.GetZ();

					if (isWithin && m_pEffectNode->KillParam.Box.IsKillInside > 0)
					{
						removed = true;
					}
					else if (!isWithin && m_pEffectNode->KillParam.Box.IsKillInside == 0)
					{
						removed = true;
					}
				}
				else if (m_pEffectNode->KillParam.Type == KillType::Plane)
				{
					SIMD::Vec3f planeNormal = m_pEffectNode->KillParam.Plane.PlaneAxis;
					SIMD::Vec3f planePosition = planeNormal * m_pEffectNode->KillParam.Plane.PlaneOffset;
					float planeW = -SIMD::Vec3f::Dot(planePosition, planeNormal);
					float factor = SIMD::Vec3f::Dot(localPosition, planeNormal) + planeW;
					if (factor > 0.0F)
					{
						removed = true;
					}
				}
				else if (m_pEffectNode->KillParam.Type == KillType::Sphere)
				{
					SIMD::Vec3f delta = localPosition - m_pEffectNode->KillParam.Sphere.Center;
					float distance = delta.GetSquaredLength();
					float radius = m_pEffectNode->KillParam.Sphere.Radius;
					bool isWithin = distance <= (radius * radius);
					if (isWithin && m_pEffectNode->KillParam.Sphere.IsKillInside > 0)
					{
						removed = true;
					}
					else if (!isWithin && m_pEffectNode->KillParam.Sphere.IsKillInside == 0)
					{
						removed = true;
					}
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
				m_State = eInstanceState::INSTANCE_STATE_REMOVING;
			}
			else
			{
				// Delete this particle with myself.
				Kill();
				return;
			}
		}
	}
	else if (m_State == eInstanceState::INSTANCE_STATE_REMOVING)
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

void Instance::UpdateTransform(float deltaFrame)
{
	// 計算済なら終了
	if (m_GlobalMatrix43Calculated)
		return;

	assert(m_pEffectNode != nullptr);
	assert(m_pContainer != nullptr);

	if (m_pEffectNode->GetType() != EffectNodeType::Root)
	{
		m_sequenceNumber = ((ManagerImplemented*)m_pManager)->GetSequenceNumber();
		const auto coordinateSystem = m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem();

		if (m_pParent != nullptr)
		{
			UpdateParentMatrix(deltaFrame);
		}

		SIMD::Vec3f localPosition = prevPosition_;
		SIMD::Vec3f localVelocity;

		if (m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
			m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
		{
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

			localVelocity = steeringVec_ * deltaFrame * m_pEffectNode->m_effect->GetMaginification();
		}
		else
		{
			localVelocity = m_pEffectNode->TranslationParam.CalculateTranslationState(translation_values, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, coordinateSystem, m_pEffectNode->DynamicFactor);

			if (m_pEffectNode->GenerationLocation.EffectsRotation)
			{
				// TODO : check rotation(It seems has bugs and it can optimize it)
				localVelocity = SIMD::Vec3f::Transform(localVelocity, m_GenerationLocation.Get3x3SubMatrix());
			}
		}

		localPosition += localVelocity;

		auto matRot = RotationFunctions::CalculateRotation(rotation_values, m_pEffectNode->RotationParam, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor, m_pManager->GetLayerParameter(GetInstanceGlobal()->GetLayer()).ViewerPosition);
		auto scaling = ScalingFunctions::UpdateScaling(scaling_values, m_pEffectNode->ScalingParam, m_randObject, m_pEffectNode->GetEffect(), m_pContainer->GetRootInstance(), m_LivingTime, m_LivedTime, m_pParent, m_pEffectNode->DynamicFactor);

		// update local fields
		if (m_pEffectNode->LocalForceField.HasValue)
		{
			forceField_.ExternalVelocity = localVelocity;
			localPosition += forceField_.Update(m_pEffectNode->LocalForceField, localPosition, m_pEffectNode->GetEffect()->GetMaginification(), deltaFrame, m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem());
		}

		prevPosition_ = localPosition;

		/* 描画部分の更新 */
		m_pEffectNode->UpdateRenderedInstance(*this, *ownGroup_, m_pManager);

		// Update matrix
		SIMD::Mat43f calcMat;
		if (m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			matRot = matRot * m_GenerationLocation.Get3x3SubMatrix();
			calcMat = SIMD::Mat43f::SRT(scaling, matRot, localPosition);
			assert(calcMat.IsValid());
		}
		else
		{
			calcMat = SIMD::Mat43f::SRT(scaling, matRot, localPosition);
			assert(globalMatrix_.GetCurrent().IsValid());
		}

		if (m_pEffectNode->TranslationParam.TranslationType != ParameterTranslationType_ViewOffset)
		{
			calcMat = calcMat * m_ParentMatrix;
			assert(calcMat.IsValid());
		}

		if (m_pEffectNode->LocalForceField.IsGlobalEnabled)
		{
			InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();
			forceField_.UpdateGlobal(m_pEffectNode->LocalForceField, prevGlobalPosition_, m_pEffectNode->GetEffect()->GetMaginification(), instanceGlobal->GetTargetLocation(), deltaFrame, m_pEffectNode->GetEffect()->GetSetting()->GetCoordinateSystem());
			SIMD::Mat43f MatTraGlobal = SIMD::Mat43f::Translation(forceField_.GlobalModifyLocation);
			calcMat *= MatTraGlobal;
		}

		globalMatrix_.Step(calcMat, m_LivingTime);

		prevGlobalPosition_ = globalMatrix_.GetCurrent().GetTranslation();

		globalMatrix_rendered = calcMat;
	}

	m_GlobalMatrix43Calculated = true;
}

void Instance::UpdateParentMatrix(float deltaFrame)
{
	// 計算済なら終了
	if (m_ParentMatrix43Calculated)
		return;

	// 親の行列を計算
	m_pParent->UpdateTransform(deltaFrame);

	parentPosition_ = m_pParent->GetGlobalMatrix().GetCurrent().GetTranslation();

	if (m_pEffectNode->GetType() != EffectNodeType::Root)
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

float Instance::GetFlipbookIndexAndNextRate(const UVAnimationType& UVType, const UVParameter& UV, const InstanceUVState& data) const
{
	if (UVType == UVAnimationType::Animation)
	{
		auto time = GetUVTime();

		// 経過時間を取得
		if (m_pEffectNode->GetType() == EffectNodeType::Ribbon ||
			m_pEffectNode->GetType() == EffectNodeType::Track)
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

void Instance::Draw(Instance* next, int32_t index, void* userData)
{
	assert(m_pEffectNode != nullptr);

	if (!m_pEffectNode->IsRendered)
		return;

	if ((GetInstanceGlobal()->CurrentLevelOfDetails & m_pEffectNode->LODsParam.MatchingLODs) == 0 && !m_pEffectNode->CanDrawWithNonMatchingLOD())
		return;

	if (m_sequenceNumber != ((ManagerImplemented*)m_pManager)->GetSequenceNumber())
	{
		UpdateTransform(0);
	}

	m_pEffectNode->Rendering(*this, next, index, m_pManager, userData);
}

void Instance::Kill()
{
	if (IsActive())
	{
		for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
		{
			group->IsReferencedFromInstance = false;
		}

		m_State = eInstanceState::INSTANCE_STATE_REMOVED;
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

RectF Instance::GetUV(const int32_t index, float livingTime, float livedTime) const
{
	return UVFunctions::GetUV(
		uvAnimationData_[index],
		m_pEffectNode->RendererCommon.UVs[index],
		livingTime,
		livedTime);
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
