
#ifndef	__EFFEKSEER_INSTANCECONTAINER_H__
#define	__EFFEKSEER_INSTANCECONTAINER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	
	@note

*/
class InstanceContainer
{
	friend class ManagerImplemented;

private:

	// マネージャ
	Manager*	m_pManager;

	// パラメーター
	EffectNodeImplemented* m_pEffectNode;

	// グローバル
	InstanceGlobal*	m_pGlobal;

	// 子のコンテナ
	InstanceContainer**	m_Children;

	// インスタンスの子の数
	int	m_ChildrenCount;

	// グループの連結リストの先頭
	InstanceGroup*	m_headGroups;

	// グループの連結リストの最後
	InstanceGroup*	m_tailGroups;

	// placement new
	static void* operator new( size_t size, Manager* pManager );

	// placement delete
	static void operator delete( void* p, Manager* pManager );

	// default delete
	static void operator delete( void* p ){}

	// コンストラクタ
	InstanceContainer( Manager* pManager, EffectNode* pEffectNode, InstanceGlobal* pGlobal, int ChildrenCount );

	// デストラクタ
	virtual ~InstanceContainer();

	// 指定した番号にコンテナを設定
	void SetChild( int num, InstanceContainer* pContainter );

	// 無効なグループの破棄
	void RemoveInvalidGroups();

public:
	// 指定した番号のコンテナを取得
	InstanceContainer* GetChild( int num );

	/**
		@brief	グループの作成
	*/
	InstanceGroup* CreateGroup();

	/**
		@brief	グループの先頭取得
	*/
	InstanceGroup* GetFirstGroup() const;

	void Update( bool recursive, float deltaFrame, bool shown );

	void SetBaseMatrix( bool recursive, const Matrix43& mat );

	void RemoveForcibly( bool recursive );

	void Draw( bool recursive );

	void KillAllInstances(  bool recursive );

	InstanceGlobal* GetRootInstance();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCECONTAINER_H__
