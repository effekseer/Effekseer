

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
	: manager_(pManager)
	, effectNode_((EffectNodeImplemented*)pEffectNode)
	, global_(pGlobal)
	, headGroups_(nullptr)
	, tailGroups_(nullptr)

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

	assert(headGroups_ == nullptr);
	assert(tailGroups_ == nullptr);

	for (auto child : children_)
	{
		manager_->ReleaseInstanceContainer(child);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::AddChild(InstanceContainer* pContainter)
{
	children_.push_back(pContainter);
}

InstanceContainer* InstanceContainer::GetChild(int index)
{
	assert(index < static_cast<int32_t>(children_.size()));

	auto it = children_.begin();
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
	InstanceGroup* tailGroup = nullptr;

	for (InstanceGroup* group = headGroups_; group != nullptr;)
	{
		if (!group->IsReferencedFromInstance && group->GetInstanceCount() == 0)
		{
			InstanceGroup* next = group->NextUsedByContainer;
			manager_->ReleaseGroup(group);

			if (headGroups_ == group)
			{
				headGroups_ = next;
			}
			group = next;

			if (tailGroup != nullptr)
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

	tailGroups_ = tailGroup;

	assert(tailGroups_ == nullptr || tailGroups_->NextUsedByContainer == nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup* InstanceContainer::CreateInstanceGroup()
{
	InstanceGroup* group = manager_->CreateInstanceGroup(effectNode_, this, global_);
	if (group == nullptr)
	{
		return nullptr;
	}

	if (tailGroups_ != nullptr)
	{
		tailGroups_->NextUsedByContainer = group;
		tailGroups_ = group;
	}
	else
	{
		assert(headGroups_ == nullptr);
		headGroups_ = group;
		tailGroups_ = group;
	}

	effectNode_->InitializeRenderedInstanceGroup(*group, manager_);

	return group;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup* InstanceContainer::GetFirstGroup() const
{
	return headGroups_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::Update(bool recursive, bool shown)
{
	// 更新
	for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
	{
		group->Update(shown);
	}

	// 破棄
	RemoveInvalidGroups();

	if (recursive)
	{
		for (auto child : children_)
		{
			child->Update(recursive, shown);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::ApplyBaseMatrix(bool recursive, const SIMD::Mat43f& mat)
{
	if (effectNode_->GetType() != EffectNodeType::Root)
	{
		for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
		{
			group->ApplyBaseMatrix(mat);
		}
	}

	if (recursive)
	{
		for (auto child : children_)
		{
			child->ApplyBaseMatrix(recursive, mat);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceContainer::RemoveForcibly(bool recursive)
{
	KillAllInstances(false);

	for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
	{
		group->RemoveForcibly();
	}
	RemoveInvalidGroups();

	if (recursive)
	{
		for (auto child : children_)
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
	if (effectNode_->GetType() != EffectNodeType::Root && effectNode_->GetType() != EffectNodeType::NoneType)
	{
		/* 個数計測 */
		int32_t count = 0;
		{
			for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
			{
				for (auto instance : group->instances_)
				{
					if (instance->IsActive())
					{
						count++;
					}
				}
			}
		}

		if ((count > 0 && effectNode_->IsRendered && (global_->CurrentLevelOfDetails & effectNode_->LODsParam.MatchingLODs) > 0) || effectNode_->CanDrawWithNonMatchingLOD())
		{
			void* userData = global_->GetUserData();

			effectNode_->BeginRendering(count, manager_, global_, userData);

			for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
			{
				effectNode_->BeginRenderingGroup(group, manager_, userData);

				if (effectNode_->RenderingOrder == RenderingOrder_FirstCreatedInstanceIsFirst)
				{
					auto it = group->instances_.begin();
					int32_t index = 0;
					while (it != group->instances_.end())
					{
						if ((*it)->IsActive())
						{
							auto it_temp = it;
							it_temp++;

							if (it_temp != group->instances_.end())
							{
								(*it)->Draw((*it_temp), index, userData);
							}
							else
							{
								(*it)->Draw(nullptr, index, userData);
							}

							index++;
						}

						it++;
					}
				}
				else
				{
					auto it = group->instances_.rbegin();
					int32_t index = 0;
					while (it != group->instances_.rend())
					{
						if ((*it)->IsActive())
						{
							auto it_temp = it;
							it_temp++;

							if (it_temp != group->instances_.rend())
							{
								(*it)->Draw((*it_temp), index, userData);
							}
							else
							{
								(*it)->Draw(nullptr, index, userData);
							}

							index++;
						}
						it++;
					}
				}

				effectNode_->EndRenderingGroup(group, manager_, userData);
			}

			effectNode_->EndRendering(manager_, userData);
		}
	}

	if (recursive)
	{
		for (auto child : children_)
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
	for (InstanceGroup* group = headGroups_; group != nullptr; group = group->NextUsedByContainer)
	{
		group->KillAllInstances();
	}

	if (recursive)
	{
		for (auto child : children_)
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
	return global_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
