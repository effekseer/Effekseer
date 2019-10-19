
#ifndef	__EFFEKSEER_INSTANCECHUNK_H__
#define	__EFFEKSEER_INSTANCECHUNK_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <array>
#include "Effekseer.Base.h"
#include "Effekseer.Instance.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	インスタンス共通部分
	@note
	生成されたインスタンスの全てから参照できる部分
*/
class alignas(32) InstanceChunk
{
public:
	static const int32_t InstancesOfChunk = 16;

	InstanceChunk();
	
	~InstanceChunk();

	void UpdateInstances( float deltaFrame );

	Instance* CreateInstance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup );

	int32_t GetAliveCount() const { return m_aliveCount; }

	bool IsInstanceCreatable() const { return m_aliveCount < InstancesOfChunk; }

private:
	std::array<uint8_t[sizeof(Instance)], InstancesOfChunk> m_instances;
	std::array<bool, InstancesOfChunk> m_instancesAlive;
	int32_t m_aliveCount = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCECHUNK_H__
