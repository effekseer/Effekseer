
#ifndef	__EFFEKSEER_INSTANCEGLOBAL_H__
#define	__EFFEKSEER_INSTANCEGLOBAL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Color.h"

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
	: public IRandObject
{
	friend class ManagerImplemented;
	friend class Instance;


private:
	/* このエフェクトで使用しているインスタンス数 */
	int			m_instanceCount;
	
	/* 更新されたフレーム数 */
	float		m_updatedFrame;

	InstanceContainer*	m_rootContainer;
	Vector3D			m_targetLocation;

	int64_t				m_seed = 0;

	std::array<float, 4> dynamicInputParameters;

	//! placement new
	static void* operator new(size_t size);

	//! placement delete
	static void operator delete(void* p);

	InstanceGlobal();

	virtual ~InstanceGlobal();

public:

	bool		IsGlobalColorSet = false;
	Color		GlobalColor = Color(255, 255, 255, 255);

	std::array<std::array<float, 4>, 16> dynamicEqResults;

	std::vector<InstanceContainer*>	RenderedInstanceContainers;

	std::array<float, 4> GetDynamicEquationResult(int32_t index);
	void SetSeed(int64_t seed);

	virtual float GetRand() override;

	virtual float GetRand(float min_, float max_) override;

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

	static float Rand(void* userData);

	static float RandSeed(void* userData, float randSeed);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGLOBAL_H__
