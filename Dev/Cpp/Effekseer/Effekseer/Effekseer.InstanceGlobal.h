
#ifndef __EFFEKSEER_INSTANCEGLOBAL_H__
#define __EFFEKSEER_INSTANCEGLOBAL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.Random.h"
#include "SIMD/Effekseer.Mat43f.h"
#include "SIMD/Effekseer.Vec3f.h"

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
	friend class Instance;

private:
	/* このエフェクトで使用しているインスタンス数 */
	int m_instanceCount;

	/* 更新されたフレーム数 */
	float m_updatedFrame;

	InstanceContainer* m_rootContainer;
	Vec3f m_targetLocation;

	RandObject m_randObjects;
	std::array<float, 4> dynamicInputParameters;

	float nextDeltaFrame_ = 0.0f;

	//! placement new
	static void* operator new(size_t size);

	//! placement delete
	static void operator delete(void* p);

	InstanceGlobal();

	virtual ~InstanceGlobal();

public:
	//! A delta time for next update
	float GetNextDeltaFrame() const;

	void BeginDeltaFrame(float frame);

	void EndDeltaFrame();

	bool IsGlobalColorSet = false;
	Color GlobalColor = Color(255, 255, 255, 255);

	std::array<std::array<float, 4>, 16> dynamicEqResults;

	std::vector<InstanceContainer*> RenderedInstanceContainers;

	std::array<float, 4> GetDynamicEquationResult(int32_t index);

	RandObject& GetRandObject()
	{
		return m_randObjects;
	}

	void IncInstanceCount();

	void DecInstanceCount();

	int GetInstanceCount();

	float GetUpdatedFrame();

	void ResetUpdatedFrame();

	InstanceContainer* GetRootContainer() const;
	void SetRootContainer(InstanceContainer* container);

	const Vec3f& GetTargetLocation() const;
	void SetTargetLocation(const Vector3D& location);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_INSTANCEGLOBAL_H__
