
#ifndef	__EFFEKSEER_INSTANCEGROUP_H__
#define	__EFFEKSEER_INSTANCEGROUP_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.IntrusiveList.h"
#include "Effekseer.EffectNodeTrack.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	インスタンスグループ
	@note
	インスタンスコンテナ内でさらにインスタンスをグループ化するクラス
*/
class InstanceGroup
{
friend class InstanceContainer;
friend class ManagerImplemented;

private:
	ManagerImplemented*		m_manager;
	EffectNodeImplemented*	m_effectNode;
	InstanceContainer*	m_container;
	InstanceGlobal*		m_global;
	int32_t				m_time;

	Matrix43			m_ParentMatrix;
	Matrix43			m_ParentRotation;
	Vector3D			m_ParentTranslation;
	Vector3D			m_ParentScale;

	// インスタンスの実体
	IntrusiveList<Instance> m_instances;
	IntrusiveList<Instance> m_removingInstances;

	InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global );

	~InstanceGroup();

public:

	/** 
		@brief	描画に必要なパラメータ
	*/
	union
	{
		EffectNodeTrack::InstanceGroupValues		track;
	} rendererValues;

	/**
		@brief	インスタンスの生成
	*/
	Instance* CreateInstance();

	Instance* GetFirst();

	int GetInstanceCount() const;

	void Update( float deltaFrame, bool shown );

	void SetBaseMatrix( const Matrix43& mat );

	void SetParentMatrix( const Matrix43& mat );

	void RemoveForcibly();

	void KillAllInstances();

	int32_t GetTime() const { return m_time; }

	/**
		@brief	グループを生成したインスタンスからの参照が残っているか?
	*/
	bool IsReferencedFromInstance;

	/**
		@brief	インスタンスから利用する連結リストの次のオブジェクトへのポインタ
	*/
	InstanceGroup*	NextUsedByInstance;

	/**
		@brief	コンテナから利用する連結リストの次のオブジェクトへのポインタ
	*/
	InstanceGroup*	NextUsedByContainer;

	InstanceGlobal* GetRootInstance() const { return m_global; }

	const Matrix43& GetParentMatrix() const { return m_ParentMatrix; }
	const Vector3D& GetParentTranslation() const { return m_ParentTranslation; }
	const Matrix43& GetParentRotation() const { return m_ParentRotation; }
	const Vector3D& GetParentScale() const { return m_ParentScale; }
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGROUP_H__
