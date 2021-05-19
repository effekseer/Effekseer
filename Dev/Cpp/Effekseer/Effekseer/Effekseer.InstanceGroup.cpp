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

	auto gt = ApplyEq(m_effectNode->GetEffect(), m_global, parent, &rand, 
		m_effectNode->CommonValues.RefEqGenerationTimeOffset, m_effectNode->CommonValues.GenerationTimeOffset);

	m_nextGenerationTime = gt.getValue(rand);

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::GenerateInstancesInRequirred(float localTime, RandObject& rand, Instance* parent)
{
	while (true)
	{
		// GenerationTimeOffset can be minus value.
		// Minus frame particles is generated simultaniously at frame 0.
		if (m_maxGenerationCount > m_generatedCount && localTime >= m_nextGenerationTime)
		{
			// Create a particle
			auto instance = m_manager->CreateInstance(m_effectNode, m_container, this);
			if (instance != nullptr)
			{
				m_instances.push_back(instance);
				m_global->IncInstanceCount();

				instance->Initialize(parent, m_generatedCount, SIMD::Mat43f::Identity);
			}

			m_generatedCount++;

			auto gt = ApplyEq(m_effectNode->GetEffect(), m_global, parent, &rand, m_effectNode->CommonValues.RefEqGenerationTime, m_effectNode->CommonValues.GenerationTime);
			m_nextGenerationTime += Max(0.0f, gt.getValue(rand));
		}
		else
		{
			break;
		}
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

		if (instance->m_State != INSTANCE_STATE_ACTIVE)
		{
			it = m_instances.erase(it);
			NotfyEraseInstance();
		}
		else
		{
			it++;
		}
	}

	//m_time++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::SetBaseMatrix(const SIMD::Mat43f& mat)
{
	for (auto instance : m_instances)
	{
		if (instance->m_State == INSTANCE_STATE_ACTIVE)
		{
			instance->m_GlobalMatrix43 *= mat;
			assert(instance->m_GlobalMatrix43.IsValid());
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

		if (instance->GetState() == INSTANCE_STATE_ACTIVE)
		{
			instance->Kill();
		}
	}
}

bool InstanceGroup::IsActive() const
{
	return GetInstanceCount() > 0 || m_generatedCount < m_maxGenerationCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------