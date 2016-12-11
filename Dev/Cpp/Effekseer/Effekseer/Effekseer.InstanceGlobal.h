
#ifndef	__EFFEKSEER_INSTANCEGLOBAL_H__
#define	__EFFEKSEER_INSTANCEGLOBAL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Vector3D.h"

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
class InstanceGlobal
{
	friend class ManagerImplemented;

private:
	/* このエフェクトで使用しているインスタンス数 */
	int			m_instanceCount;
	
	/* 更新されたフレーム数 */
	float		m_updatedFrame;

	InstanceContainer*	m_rootContainer;
	Vector3D			m_targetLocation;

	InstanceGlobal();

	virtual ~InstanceGlobal();

public:
	void IncInstanceCount();

	void DecInstanceCount();

	void AddUpdatedFrame( float frame );

	/**
		@brief	全てのインスタンス数を取得
	*/
	int GetInstanceCount();

	/**
		@brief	更新されたフレーム数を取得する。
	*/
	float GetUpdatedFrame();

	InstanceContainer* GetRootContainer() const;
	void SetRootContainer( InstanceContainer* container );

	const Vector3D& GetTargetLocation() const;
	void SetTargetLocation( const Vector3D& location );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGLOBAL_H__
