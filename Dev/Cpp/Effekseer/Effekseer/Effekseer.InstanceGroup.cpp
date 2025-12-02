#include "Effekseer.InstanceGroup.h"

#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Utils/Effekseer.CustomAllocator.h"
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::InstanceGroup(ManagerImplemented* manager, EffectNodeImplemented* effectNode, InstanceContainer* container, InstanceGlobal* global)
	: m_manager(manager)
	, m_effectNode(effectNode)
	, m_container(container)
	, global_(global)
{
	parentMatrix_ = SIMD::Mat43f::Identity;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::~InstanceGroup()
{
	RemoveForcibly();
}

void InstanceGroup::NotfyEraseInstance()
{
	global_->DecInstanceCount();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Initialize(RandObject& rand, Instance* parent)
{
	generatedCount_ = 0;

	auto gt = ApplyEq(m_effectNode->GetEffect(), global_, parent, &rand, m_effectNode->CommonValues.RefEqGenerationTimeOffset, m_effectNode->CommonValues.GenerationTimeOffset);

	generationOffsetTime_ = gt.getValue(rand);
	nextGenerationTime_ = generationOffsetTime_;

	if (m_effectNode->CommonValues.RefEqMaxGeneration >= 0)
	{
		auto maxGene = static_cast<float>(m_effectNode->CommonValues.MaxGeneration);
		ApplyEq(maxGene, m_effectNode->GetEffect(), global_, parent, &rand, m_effectNode->CommonValues.RefEqMaxGeneration, maxGene);
		maxGenerationCount_ = static_cast<int32_t>(maxGene);
	}
	else
	{
		maxGenerationCount_ = m_effectNode->CommonValues.MaxGeneration;
	}

	if (m_effectNode->TriggerParam.ToStartGeneration.type == TriggerType::None)
	{
		generationState_ = GenerationState::Generating;
	}
}

Instance* InstanceGroup::CreateRootInstance()
{
	auto instance = m_manager->CreateInstance(m_effectNode, m_container, this);
	if (instance != nullptr)
	{
		instances_.push_back(instance);
		global_->IncInstanceCount();
		return instance;
	}
	return nullptr;
}

void InstanceGroup::GenerateInstancesIfRequired(float localTime, RandObject& rand, Instance* parent)
{
	if (generationState_ == GenerationState::BeforeStart)
	{
		if (IsTriggerActivated(m_effectNode->TriggerParam.ToStartGeneration, global_, parent))
		{
			generationState_ = GenerationState::Generating;
			nextGenerationTime_ = generationOffsetTime_ + localTime;
		}
	}
	if (generationState_ == GenerationState::Generating)
	{
		if (IsTriggerActivated(m_effectNode->TriggerParam.ToStopGeneration, global_, parent))
		{
			generationState_ = GenerationState::Ended;
		}
	}

	const bool isSpawnRestrictedByLOD = (global_->CurrentLevelOfDetails & m_effectNode->LODsParam.MatchingLODs) == 0 && !m_effectNode->CanSpawnWithNonMatchingLOD();
	const bool canSpawn = !global_->IsSpawnDisabled && !isSpawnRestrictedByLOD;

	// GenerationTimeOffset can be minus value.
	// Minus frame particles is generated simultaniously at frame 0.
	while (generationState_ == GenerationState::Generating &&
		   maxGenerationCount_ > generatedCount_ &&
		   localTime >= nextGenerationTime_)
	{
		// Disabled spawn only prevents instance generation but spawn rate should not be affected once spawn is enabled again
		if (canSpawn)
		{
			// Create a particle
			auto instance = m_manager->CreateInstance(m_effectNode, m_container, this);
			if (instance != nullptr)
			{
				instances_.push_back(instance);
				global_->IncInstanceCount();

				instance->Initialize(parent, nextGenerationTime_, generatedCount_);
			}

			generatedCount_++;
		}

		auto gt = ApplyEq(m_effectNode->GetEffect(), global_, parent, &rand, m_effectNode->CommonValues.RefEqGenerationTime, m_effectNode->CommonValues.GenerationTime);
		nextGenerationTime_ += Max(0.0f, gt.getValue(rand));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::GetFirst()
{
	if (instances_.size() > 0)
	{
		return instances_.front();
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGroup::GetInstanceCount() const
{
	return (int32_t)instances_.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Update(bool shown)
{
	for (auto it = instances_.begin(); it != instances_.end();)
	{
		auto instance = *it;

		if (!instance->IsActive())
		{
			it = instances_.erase(it);
			NotfyEraseInstance();
		}
		else
		{
			it++;
		}
	}

	time_ += global_->GetNextDeltaFrame();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::ApplyBaseMatrix(const SIMD::Mat43f& mat)
{
	for (auto instance : instances_)
	{
		if (instance->IsActive())
		{
			instance->ApplyBaseMatrix(mat);
		}
	}
}

void InstanceGroup::SetParentMatrix(const SIMD::Mat43f& mat)
{
	TranslationParentBindType tType = m_effectNode->CommonValues.TranslationBindType;
	BindType rType = m_effectNode->CommonValues.RotationBindType;
	BindType sType = m_effectNode->CommonValues.ScalingBindType;

	auto rootGroup = global_->GetRootContainer()->GetFirstGroup();

	if (tType == BindType::Always && rType == BindType::Always && sType == BindType::Always)
	{
		parentMatrix_ = mat;
	}
	else if (tType == BindType::NotBind_Root && rType == BindType::NotBind_Root && sType == BindType::NotBind_Root)
	{
		parentMatrix_ = rootGroup->GetParentMatrix();
	}
	else if ((tType == BindType::WhenCreating || tType == TranslationParentBindType::WhenCreating_FollowParent) && rType == BindType::WhenCreating && sType == BindType::WhenCreating)
	{
		// don't do anything
	}
	else
	{
		SIMD::Vec3f s, t;
		SIMD::Mat43f r;
		mat.GetSRT(s, r, t);

		if (tType == BindType::Always)
		{
			parentTranslation_ = t;
		}
		else if (tType == BindType::NotBind_Root)
		{
			parentTranslation_ = rootGroup->GetParentTranslation();
		}
		else if (tType == BindType::NotBind)
		{
			parentTranslation_ = SIMD::Vec3f(0.0f, 0.0f, 0.0f);
		}
		else if (tType == TranslationParentBindType::NotBind_FollowParent)
		{
			parentTranslation_ = SIMD::Vec3f(0.0f, 0.0f, 0.0f);
		}

		if (rType == BindType::Always)
		{
			parentRotation_ = r;
		}
		else if (rType == BindType::NotBind_Root)
		{
			parentRotation_ = rootGroup->GetParentRotation();
		}
		else if (rType == BindType::NotBind)
		{
			parentRotation_ = SIMD::Mat43f::Identity;
		}

		if (sType == BindType::Always)
		{
			parentScale_ = s;
		}
		else if (sType == BindType::NotBind_Root)
		{
			parentScale_ = rootGroup->GetParentScale();
		}
		else if (sType == BindType::NotBind)
		{
			parentScale_ = SIMD::Vec3f(1.0f, 1.0f, 1.0f);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::RemoveForcibly()
{
	KillAllInstances();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::KillAllInstances()
{
	while (!instances_.empty())
	{
		auto instance = instances_.front();
		instances_.pop_front();
		NotfyEraseInstance();

		if (instance->IsActive())
		{
			instance->Kill();
		}
	}
}

bool InstanceGroup::IsActive() const
{
	return GetInstanceCount() > 0 ||
		   (generationState_ != GenerationState::Ended &&
			generatedCount_ < maxGenerationCount_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
