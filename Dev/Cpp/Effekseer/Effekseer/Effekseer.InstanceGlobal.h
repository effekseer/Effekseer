﻿
#ifndef __EFFEKSEER_INSTANCEGLOBAL_H__
#define __EFFEKSEER_INSTANCEGLOBAL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.Random.h"
#include "SIMD/Mat43f.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Vec3f.h"

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
	SIMD::Vec3f m_targetLocation = SIMD::Vec3f(0.0f, 0.0f, 0.0f);

	RandObject m_randObjects;
	std::array<float, 4> dynamicInputParameters;
	std::array<uint8_t, 4> m_inputTriggerCounts;

	float nextDeltaFrame_ = 0.0f;
	int32_t layer_ = 0;
	void* m_userData = nullptr;

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

	bool IsSpawnDisabled = false;
	int CurrentLevelOfDetails = 0;

	SIMD::Mat44f EffectGlobalMatrix;
	// Used for collision detection by kill rules
	SIMD::Mat44f InvertedEffectGlobalMatrix;

	bool IsGlobalColorSet = false;
	Color GlobalColor = Color(255, 255, 255, 255);

	std::array<std::array<float, 4>, 16> dynamicEqResults;

	std::vector<InstanceContainer*> RenderedInstanceContainers;

	std::array<float, 4> GetDynamicEquationResult(int32_t index);

	const std::array<float, 4>& GetDynamicInputParameters() const
	{
		return dynamicInputParameters;
	}

	uint32_t GetInputTriggerCount(uint32_t index) const;

	void AddInputTriggerCount(uint32_t index);

	RandObject& GetRandObject()
	{
		return m_randObjects;
	}

	void IncInstanceCount();

	void DecInstanceCount();

	int GetInstanceCount();

	float GetUpdatedFrame() const;

	void ResetUpdatedFrame();

	InstanceContainer* GetRootContainer() const;
	void SetRootContainer(InstanceContainer* container);

	const SIMD::Vec3f& GetTargetLocation() const;
	void SetTargetLocation(const Vector3D& location);

	void SetLayer(int32_t layer)
	{
		layer_ = layer;
	}
	int32_t GetLayer() const
	{
		return layer_;
	}
	int32_t GetLayerBits() const
	{
		return 1 << layer_;
	}

	void SetUserData(void* userData)
	{
		m_userData = userData;
	}
	void* GetUserData() const
	{
		return m_userData;
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_INSTANCEGLOBAL_H__
