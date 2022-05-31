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
	, m_global(global)
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
	m_global->DecInstanceCount();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Initialize(RandObject& rand, Instance* parent)
{
	m_generatedCount = 0;

	auto gt = ApplyEq(m_effectNode->GetEffect(), m_global, parent, &rand, m_effectNode->CommonValues.RefEqGenerationTimeOffset, m_effectNode->CommonValues.GenerationTimeOffset);

	m_generationOffsetTime = gt.getValue(rand);
	m_nextGenerationTime = m_generationOffsetTime;

	if (m_effectNode->CommonValues.RefEqMaxGeneration >= 0)
	{
		auto maxGene = static_cast<float>(m_effectNode->CommonValues.MaxGeneration);
		ApplyEq(maxGene, m_effectNode->GetEffect(), m_global, parent, &rand, m_effectNode->CommonValues.RefEqMaxGeneration, maxGene);
		m_maxGenerationCount = static_cast<int32_t>(maxGene);
	}
	else
	{
		m_maxGenerationCount = m_effectNode->CommonValues.MaxGeneration;
	}

	if (m_effectNode->TriggerParam.ToStartGeneration.type == TriggerType::None)
	{
		m_generationState = GenerationState::Generating;
	}
}

Instance* InstanceGroup::CreateRootInstance()
{
	auto instance = m_manager->CreateInstance(m_effectNode, m_container, this);
	if (instance != nullptr)
	{
		m_instances.push_back(instance);
		m_global->IncInstanceCount();
		return instance;
	}
	return nullptr;
}

void InstanceGroup::GenerateInstancesIfRequired(float localTime, RandObject& rand, Instance* parent)
{
	if (m_generationState == GenerationState::BeforeStart &&
		m_effectNode->TriggerParam.ToStartGeneration.type != TriggerType::None)
	{
		if (m_global->GetInputTriggerCount(m_effectNode->TriggerParam.ToStartGeneration.index) > 0)
		{
			m_generationState = GenerationState::Generating;
			m_nextGenerationTime = m_generationOffsetTime + localTime;
		}
	}
	if (m_generationState == GenerationState::Generating &&
		m_effectNode->TriggerParam.ToStopGeneration.type != TriggerType::None)
	{
		if (m_global->GetInputTriggerCount(m_effectNode->TriggerParam.ToStopGeneration.index) > 0)
		{
			m_generationState = GenerationState::Ended;
		}
	}

	const bool isSpawnRestrictedByLOD = (m_global->CurrentLevelOfDetails & m_effectNode->LODsParam.MatchingLODs) == 0 && !m_effectNode->CanSpawnWithNonMatchingLOD();
	const bool canSpawn = !m_global->IsSpawnDisabled && !isSpawnRestrictedByLOD;

	// GenerationTimeOffset can be minus value.
	// Minus frame particles is generated simultaniously at frame 0.
	while (m_generationState == GenerationState::Generating &&
		   m_maxGenerationCount > m_generatedCount &&
		   localTime >= m_nextGenerationTime)
	{
		// Disabled spawn only prevents instance generation but spawn rate should not be affected once spawn is enabled again
		if (canSpawn)
		{
			// Create a particle
			auto instance = m_manager->CreateInstance(m_effectNode, m_container, this);
			if (instance != nullptr)
			{
				m_instances.push_back(instance);
				m_global->IncInstanceCount();

				instance->Initialize(parent, m_nextGenerationTime, m_generatedCount);
			}

			m_generatedCount++;
		}

		auto gt = ApplyEq(m_effectNode->GetEffect(), m_global, parent, &rand, m_effectNode->CommonValues.RefEqGenerationTime, m_effectNode->CommonValues.GenerationTime);
		m_nextGenerationTime += Max(0.0f, gt.getValue(rand));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::GetFirst()
{
	if (m_instances.size() > 0)
	{
		return m_instances.front();
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGroup::GetInstanceCount() const
{
	return (int32_t)m_instances.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Update(bool shown)
{
	for (auto it = m_instances.begin(); it != m_instances.end();)
	{
		auto instance = *it;

		if (!instance->IsActive())
		{
			it = m_instances.erase(it);
			NotfyEraseInstance();
		}
		else
		{
			it++;
		}
	}

	time_ += m_global->GetNextDeltaFrame();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::ApplyBaseMatrix(const SIMD::Mat43f& mat)
{
	for (auto instance : m_instances)
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

	auto rootGroup = m_global->GetRootContainer()->GetFirstGroup();

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
	while (!m_instances.empty())
	{
		auto instance = m_instances.front();
		m_instances.pop_front();
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
		   (m_generationState != GenerationState::Ended &&
			m_generatedCount < m_maxGenerationCount);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------