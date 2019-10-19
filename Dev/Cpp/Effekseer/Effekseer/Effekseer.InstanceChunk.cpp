

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.InstanceChunk.h"
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceChunk::InstanceChunk()
{
	std::fill( m_instancesAlive.begin(), m_instancesAlive.end(), false );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceChunk::~InstanceChunk()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceChunk::UpdateInstances( float deltaFrame )
{
	for( int32_t i = 0; i < InstancesOfChunk; i++ )
	{
		if( m_instancesAlive[i] )
		{
			Instance* instance = reinterpret_cast<Instance*>( m_instances[i] );

			if( instance->m_State == INSTANCE_STATE_ACTIVE )
			{
				instance->Update( deltaFrame, true );
			}
			else if( instance->m_State == INSTANCE_STATE_REMOVING )
			{
				// 削除中処理
				instance->m_State = INSTANCE_STATE_REMOVED;
			}
			else if( instance->m_State == INSTANCE_STATE_REMOVED )
			{
				instance->~Instance();
				m_instancesAlive[i] = false;
				m_aliveCount--;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceChunk::CreateInstance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup )
{
	for( int32_t i = 0; i < InstancesOfChunk; i++ )
	{
		if( !m_instancesAlive[i] )
		{
			m_instancesAlive[i] = true;
			m_aliveCount++;
			return new( m_instances[i] ) Instance( pManager, pEffectNode, pContainer, pGroup );
		}
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------