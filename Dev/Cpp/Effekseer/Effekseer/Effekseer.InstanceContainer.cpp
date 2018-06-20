

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"

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
void* InstanceContainer::operator new(size_t size, Manager* pManager)
{
	return pManager->GetMallocFunc()(size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::operator delete(void* p, Manager* pManager)
{
	pManager->GetFreeFunc()(p, sizeof(InstanceContainer));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer::InstanceContainer(Manager* pManager, EffectNode* pEffectNode, InstanceGlobal* pGlobal, int ChildrenCount)
	: m_pManager(pManager)
	, m_pEffectNode((EffectNodeImplemented*)pEffectNode)
	, m_pGlobal(pGlobal)
	, m_Children(NULL)
	, m_ChildrenCount(ChildrenCount)
	, m_headGroups(NULL)
	, m_tailGroups(NULL)

{
	auto en = (EffectNodeImplemented*)pEffectNode;
	if (en->RenderingPriority >= 0)
	{
		pGlobal->RenderedInstanceContainers[en->RenderingPriority] = this;
	}

	m_Children = (InstanceContainer**)m_pManager->GetMallocFunc()(sizeof(InstanceContainer*) * m_ChildrenCount);
	for (int i = 0; i < m_ChildrenCount; i++)
	{
		m_Children[i] = NULL;
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

	for (int i = 0; i < m_ChildrenCount; i++)
	{
		if (m_Children[i] != NULL)
		{
			m_Children[i]->~InstanceContainer();
			InstanceContainer::operator delete(m_Children[i], m_pManager);
			m_Children[i] = NULL;
		}
	}
	m_pManager->GetFreeFunc()((void*)m_Children, sizeof(InstanceContainer*) * m_ChildrenCount);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* InstanceContainer::GetChild(int num)
{
	return m_Children[num];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::SetChild(int num, InstanceContainer* pContainter)
{
	m_Children[num] = pContainter;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::RemoveInvalidGroups()
{
	/* 最後に存在する有効なグループ */
	InstanceGroup* tailGroup = NULL;

	for (InstanceGroup* group = m_headGroups; group != NULL; )
	{
		if (!group->IsReferencedFromInstance && group->GetInstanceCount() == 0 && group->GetRemovingInstanceCount() == 0)
		{
			InstanceGroup* next = group->NextUsedByContainer;

			delete group;

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
InstanceGroup* InstanceContainer::CreateGroup()
{
	InstanceGroup* group = new InstanceGroup(m_pManager, m_pEffectNode, this, m_pGlobal);

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
void InstanceContainer::Update(bool recursive, float deltaFrame, bool shown)
{
	// 更新
	for (InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByContainer)
	{
		group->Update(deltaFrame, shown);
	}

	// 破棄
	RemoveInvalidGroups();

	if (recursive)
	{
		for (int i = 0; i < m_ChildrenCount; i++)
		{
			m_Children[i]->Update(recursive, deltaFrame, shown);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::SetBaseMatrix(bool recursive, const Matrix43& mat)
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
		for (int i = 0; i < m_ChildrenCount; i++)
		{
			m_Children[i]->SetBaseMatrix(recursive, mat);
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
		for (int i = 0; i < m_ChildrenCount; i++)
		{
			m_Children[i]->RemoveForcibly(recursive);
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
		for (int i = 0; i < m_ChildrenCount; i++)
		{
			m_Children[i]->Draw(recursive);
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
		for (int i = 0; i < m_ChildrenCount; i++)
		{
			m_Children[i]->KillAllInstances(recursive);
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

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------