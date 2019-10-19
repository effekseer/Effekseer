

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.CustomAllocator.h"
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global )
	: m_manager		( (ManagerImplemented*)manager )
	, m_effectNode((EffectNodeImplemented*) effectNode)
	, m_container	( container )
	, m_global		( global )
	, m_time		( 0 )
	, IsReferencedFromInstance	( true )
	, NextUsedByInstance	( NULL )
	, NextUsedByContainer	( NULL )
{
	m_ParentMatrix.Indentity();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::~InstanceGroup()
{
	RemoveForcibly();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::CreateInstance()
{
	Instance* instance = NULL;

	instance = m_manager->CreateInstance( m_effectNode, m_container, this );
	
	if( instance )
	{
		m_instances.push_back( instance );
		m_global->IncInstanceCount();
	}
	return instance;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::GetFirst()
{
	if( m_instances.size() > 0 )
	{
		return m_instances.front();
	}
	return NULL;
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
void InstanceGroup::Update( float deltaFrame, bool shown )
{
	for( auto it = m_instances.begin(); it != m_instances.end(); )
	{
		auto instance = *it;
	
		if( instance->m_State != INSTANCE_STATE_ACTIVE )
		{
			it = m_instances.erase( it );
			m_global->DecInstanceCount();
		}
		else
		{
			it++;
		}
	}

	m_time++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::SetBaseMatrix( const Matrix43& mat )
{
	for (auto instance : m_instances)
	{
		if (instance->m_State == INSTANCE_STATE_ACTIVE)
		{
			Matrix43::Multiple(instance->m_GlobalMatrix43, instance->m_GlobalMatrix43, mat);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::SetParentMatrix( const Matrix43& mat )
{
	BindType tType = m_effectNode->CommonValues.TranslationBindType;
	BindType rType = m_effectNode->CommonValues.RotationBindType;
	BindType sType = m_effectNode->CommonValues.ScalingBindType;

	auto rootGroup = m_global->GetRootContainer()->GetFirstGroup();

	if( tType == BindType::Always && rType == BindType::Always && sType == BindType::Always )
	{
		m_ParentMatrix = mat;
	}
	else if( tType == BindType::NotBind_Root && rType == BindType::NotBind_Root && sType == BindType::NotBind_Root )
	{
		m_ParentMatrix = rootGroup->GetParentMatrix();
	}
	else if( tType == BindType::WhenCreating && rType == BindType::WhenCreating && sType == BindType::WhenCreating )
	{
		// 何もしない
	}
	else
	{
		Vector3D s, t;
		Matrix43 r;
		mat.GetSRT( s, r, t );
		
		if( tType == BindType::Always )
		{
			m_ParentTranslation = t;
		}
		else if( tType == BindType::NotBind_Root )
		{
			m_ParentTranslation = rootGroup->GetParentTranslation();
		}

		if( rType == BindType::Always )
		{
			m_ParentRotation = r;
		}
		else if( rType == BindType::NotBind_Root )
		{
			m_ParentRotation = rootGroup->GetParentRotation();
		}

		if( sType == BindType::Always )
		{
			m_ParentScale = s;
		}
		else if( sType == BindType::NotBind_Root )
		{
			m_ParentScale = rootGroup->GetParentScale();
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

		if (instance->GetState() == INSTANCE_STATE_ACTIVE)
		{
			instance->Kill();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------