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
	: manager_(manager)
	, effectNode_(effectNode)
	, container_(container)
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

	auto gt = ApplyEq(effectNode_->GetEffect(), global_, parent, &rand, effectNode_->CommonValues.Generation.RefEqOffset, effectNode_->CommonValues.Generation.Offset);

	generationOffsetTime_ = gt.getValue(rand);
	nextGenerationTime_ = generationOffsetTime_;

	if (effectNode_->CommonValues.RefEqMaxGeneration >= 0)
	{
		auto maxGene = static_cast<float>(effectNode_->CommonValues.MaxGeneration);
		ApplyEq(maxGene, effectNode_->GetEffect(), global_, parent, &rand, effectNode_->CommonValues.RefEqMaxGeneration, maxGene);
		maxGenerationCount_ = static_cast<int32_t>(maxGene);
	}
	else
	{
		maxGenerationCount_ = effectNode_->CommonValues.MaxGeneration;
	}

	if (effectNode_->CommonValues.Generation.Type == GenerationTiming::Trigger || effectNode_->CommonValues.Generation.TriggerToStart.type == TriggerType::None)
	{
		generationState_ = GenerationState::Generating;
	}
}

Instance* InstanceGroup::CreateRootInstance()
{
	auto instance = manager_->CreateInstance(effectNode_, container_, this);
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
	const auto generationType = effectNode_->CommonValues.Generation.Type;

	if (generationState_ == GenerationState::Ended)
	{
		return;
	}

	if (generationState_ == GenerationState::BeforeStart)
	{
		if (IsTriggerActivated(effectNode_->CommonValues.Generation.TriggerToStart, global_, parent))
		{
			generationState_ = GenerationState::Generating;
			nextGenerationTime_ = generationOffsetTime_ + localTime;
		}
	}
	if (generationState_ == GenerationState::Generating && generationType == GenerationTiming::Continuous)
	{
		if (IsTriggerActivated(effectNode_->CommonValues.Generation.TriggerToStop, global_, parent))
		{
			generationState_ = GenerationState::Ended;
		}
	}

	const bool isSpawnRestrictedByLOD = (global_->CurrentLevelOfDetails & effectNode_->LODsParam.MatchingLODs) == 0 && !effectNode_->CanSpawnWithNonMatchingLOD();
	const bool canSpawn = !global_->IsSpawnDisabled && !isSpawnRestrictedByLOD;

	if (generationType == GenerationTiming::Trigger)
	{
		const bool canGenerateNow = generationState_ == GenerationState::Generating && localTime >= generationOffsetTime_;
		if (canGenerateNow && IsTriggerActivated(effectNode_->CommonValues.Generation.TriggerToGenerate, global_, parent))
		{
			auto triggerCount = ApplyEq(effectNode_->GetEffect(), global_, parent, &rand, effectNode_->CommonValues.Generation.RefEqBurst, effectNode_->CommonValues.Generation.Burst);
			int32_t requestCount = static_cast<int32_t>(triggerCount.getValue(rand));
			requestCount = Max(requestCount, 0);

			while (requestCount > 0 && maxGenerationCount_ > generatedCount_)
			{
				if (canSpawn)
				{
					auto instance = manager_->CreateInstance(effectNode_, container_, this);
					if (instance != nullptr)
					{
						instances_.push_back(instance);
						global_->IncInstanceCount();

						instance->Initialize(parent, localTime, generatedCount_);
					}
				}

				generatedCount_++;
				requestCount--;
			}

			if (generatedCount_ >= maxGenerationCount_)
			{
				generationState_ = GenerationState::Ended;
			}
		}

		return;
	}

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
			auto instance = manager_->CreateInstance(effectNode_, container_, this);
			if (instance != nullptr)
			{
				instances_.push_back(instance);
				global_->IncInstanceCount();

				instance->Initialize(parent, nextGenerationTime_, generatedCount_);
			}

			generatedCount_++;
		}

		auto gt = ApplyEq(effectNode_->GetEffect(), global_, parent, &rand, effectNode_->CommonValues.Generation.RefEqInterval, effectNode_->CommonValues.Generation.Interval);
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
	TranslationParentBindType tType = effectNode_->CommonValues.TranslationBindType;
	BindType rType = effectNode_->CommonValues.RotationBindType;
	BindType sType = effectNode_->CommonValues.ScalingBindType;

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
