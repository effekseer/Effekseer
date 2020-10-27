
#ifndef __EFFEKSEER_INSTANCEGLOBAL_H__
#define __EFFEKSEER_INSTANCEGLOBAL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
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
	: public IRandObject
{
	friend class ManagerImplemented;
	friend class Instance;

private:
	/* このエフェクトで使用しているインスタンス数 */
	int m_instanceCount;

	/* 更新されたフレーム数 */
	float m_updatedFrame;

	InstanceContainer* m_rootContainer;
	Vec3f m_targetLocation = Vec3f(0.0f, 0.0f, 0.0f);

	int64_t m_seed = 0;

	std::array<float, 4> dynamicInputParameters;

	//! placement new
	static void* operator new(size_t size);

	//! placement delete
	static void operator delete(void* p);

	InstanceGlobal();

	virtual ~InstanceGlobal();

public:
	//! A delta time for next update
	float NextDeltaFrame = 0.0f;

	void BeginDeltaFrame(float frame);

	void EndDeltaFrame();

	bool IsGlobalColorSet = false;
	Color GlobalColor = Color(255, 255, 255, 255);

	std::array<std::array<float, 4>, 16> dynamicEqResults;

	std::vector<InstanceContainer*> RenderedInstanceContainers;

	std::array<float, 4> GetDynamicEquationResult(int32_t index);
	void SetSeed(int64_t seed);

	virtual float GetRand() override;

	virtual float GetRand(float min_, float max_) override;

	void IncInstanceCount();

	void DecInstanceCount();

	/**
		@brief	全てのインスタンス数を取得
	*/
	int GetInstanceCount();

	/**
		@brief	更新されたフレーム数を取得する。
	*/
	float GetUpdatedFrame();

	void ResetUpdatedFrame();

	InstanceContainer* GetRootContainer() const;
	void SetRootContainer(InstanceContainer* container);

	const Vec3f& GetTargetLocation() const;
	void SetTargetLocation(const Vector3D& location);

	static float Rand(void* userData);

	static float RandSeed(void* userData, float randSeed);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_INSTANCEGLOBAL_H__
