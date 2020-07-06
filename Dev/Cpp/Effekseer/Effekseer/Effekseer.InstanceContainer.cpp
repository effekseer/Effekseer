

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"

#include "Renderer/Effekseer.SpriteRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer::InstanceContainer(ManagerImplemented* pManager, EffectNode* pEffectNode, InstanceGlobal* pGlobal)
	: m_pManager(pManager)
	, m_pEffectNode((EffectNodeImplemented*)pEffectNode)
	, m_pGlobal(pGlobal)
	, m_headGroups(NULL)
	, m_tailGroups(NULL)

{
	auto en = (EffectNodeImplemented*)pEffectNode;
	if (en->RenderingPriority >= 0)
	{
		pGlobal->RenderedInstanceContainers[en->RenderingPriority] = this;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer::~InstanceContainer()
{
	RemoveForcibly(false);

	assert(m_headGroups == NULL);
	assert(m_tailGroups == NULL);

	for (auto child : m_Children)
	{
		m_pManager->ReleaseInstanceContainer(child);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::AddChild(InstanceContainer* pContainter)
{
	m_Children.push_back(pContainter);
}

InstanceContainer* InstanceContainer::GetChild(int index)
{
	assert(index < static_cast<int32_t>(m_Children.size()));

	auto it = m_Children.begin();
	for (int i = 0; i < index; i++)
	{
		it++;
	}
	return *it;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::RemoveInvalidGroups()
{
	/* 最後に存在する有効なグループ */
	InstanceGroup* tailGroup = NULL;

	for (InstanceGroup* group = m_headGroups; group != NULL;)
	{
		if (!group->IsReferencedFromInstance && group->GetInstanceCount() == 0)
		{
			InstanceGroup* next = group->NextUsedByContainer;
			m_pManager->ReleaseGroup(group);

			if (m_headGroups == group)
			{
				m_headGroups = next;
			}
			group = next;

			if (tailGroup != NULL)
			{
				tailGroup->NextUsedByContainer = next;
			}
		}
		else
		{
			tailGroup = group;
			group = group->NextUsedByContainer;
		}
	}

	m_tailGroups = tailGroup;

	assert(m_tailGroups == NULL || m_tailGroups->NextUsedByContainer == NULL);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup* InstanceContainer::CreateInstanceGroup()
{
	InstanceGroup* group = m_pManager->CreateInstanceGroup(m_pEffectNode, this, m_pGlobal);
	if (group == nullptr)
	{
		return nullptr;
	}

	if (m_tailGroups != NULL)
	{
		m_tailGroups->NextUsedByContainer = group;
		m_tailGroups = group;
	}
	else
	{
		assert(m_headGroups == NULL);
		m_headGroups = group;
		m_tailGroups = group;
	}

	m_pEffectNode->InitializeRenderedInstanceGroup(*group, m_pManager);

	return group;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup* InstanceContainer::GetFirstGroup() const
{
	return m_headGroups;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::Update(bool recursive, bool shown)
{
	// 更新
	for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
	{
		group->Update(shown);
	}

	// 破棄
	RemoveInvalidGroups();

	if (recursive)
	{
		for (auto child : m_Children)
		{
			child->Update(recursive, shown);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::SetBaseMatrix(bool recursive, const Mat43f& mat)
{
	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
		{
			group->SetBaseMatrix(mat);
		}
	}

	if (recursive)
	{
		for (auto child : m_Children)
		{
			child->SetBaseMatrix(recursive, mat);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::RemoveForcibly(bool recursive)
{
	KillAllInstances(false);

	for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
	{
		group->RemoveForcibly();
	}
	RemoveInvalidGroups();

	if (recursive)
	{
		for (auto child : m_Children)
		{
			child->RemoveForcibly(recursive);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::Draw(bool recursive)
{
	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_NONE)
	{
		/* 個数計測 */
		int32_t count = 0;
		{
			for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
			{
				for (auto instance : group->m_instances)
				{
					if (instance->m_State == INSTANCE_STATE_ACTIVE)
					{
						count++;
					}
				}
			}
		}

		if (count > 0)
		{
			/* 描画 */
			m_pEffectNode->BeginRendering(count, m_pManager);

			for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
			{
				m_pEffectNode->BeginRenderingGroup(group, m_pManager);

				if (m_pEffectNode->RenderingOrder == RenderingOrder_FirstCreatedInstanceIsFirst)
				{
					auto it = group->m_instances.begin();

					while (it != group->m_instances.end())
					{
						if ((*it)->m_State == INSTANCE_STATE_ACTIVE)
						{
							auto it_temp = it;
							it_temp++;

							if (it_temp != group->m_instances.end())
							{
								(*it)->Draw((*it_temp));
							}
							else
							{
								(*it)->Draw(nullptr);
							}
						}

						it++;
					}
				}
				else
				{
					auto it = group->m_instances.rbegin();

					while (it != group->m_instances.rend())
					{
						if ((*it)->m_State == INSTANCE_STATE_ACTIVE)
						{
							auto it_temp = it;
							it_temp++;

							if (it_temp != group->m_instances.rend())
							{
								(*it)->Draw((*it_temp));
							}
							else
							{
								(*it)->Draw(nullptr);
							}
						}
						it++;
					}
				}

				m_pEffectNode->EndRenderingGroup(group, m_pManager);
			}

			m_pEffectNode->EndRendering(m_pManager);
		}
	}

	if (recursive)
	{
		for (auto child : m_Children)
		{
			child->Draw(recursive);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::KillAllInstances(bool recursive)
{
	for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
	{
		group->KillAllInstances();
	}

	if (recursive)
	{
		for (auto child : m_Children)
		{
			child->KillAllInstances(recursive);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGlobal* InstanceContainer::GetRootInstance()
{
	return m_pGlobal;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
