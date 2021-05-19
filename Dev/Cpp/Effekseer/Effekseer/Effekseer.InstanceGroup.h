
#ifndef __EFFEKSEER_INSTANCEGROUP_H__
#define __EFFEKSEER_INSTANCEGROUP_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.EffectNodeTrack.h"
#include "Effekseer.IntrusiveList.h"
#include "SIMD/Mat43f.h"
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
	@brief	インスタンスグループ
	@note
	インスタンスコンテナ内でさらにインスタンスをグループ化するクラス
*/
class alignas(32) InstanceGroup
{
	friend class InstanceContainer;
	friend class ManagerImplemented;

private:
	ManagerImplemented* m_manager = nullptr;
	EffectNodeImplemented* m_effectNode = nullptr;
	InstanceContainer* m_container = nullptr;
	InstanceGlobal* m_global = nullptr;
	//int32_t m_time = 0;

	// The number of generated instances.
	int32_t m_generatedCount = 0;

	// The maximum number of instances to generate.
	int32_t m_maxGenerationCount = 0;

	// The time to generate next instance.
	float m_nextGenerationTime = 0.0f;

	SIMD::Mat43f parentMatrix_;
	SIMD::Mat43f parentRotation_;
	SIMD::Vec3f parentTranslation_;
	SIMD::Vec3f parentScale_;

	// インスタンスの実体
	IntrusiveList<Instance> m_instances;
	IntrusiveList<Instance> m_removingInstances;

	InstanceGroup(ManagerImplemented* manager, EffectNodeImplemented* effectNode, InstanceContainer* container, InstanceGlobal* global);

	~InstanceGroup();

	void NotfyEraseInstance();

public:
	/**
		@brief	描画に必要なパラメータ
	*/
	union
	{
		EffectNodeTrack::InstanceGroupValues track;
	} rendererValues;

	void Initialize(RandObject& rand, Instance* parent);

	/**
		@brief	インスタンスの生成
	*/
	Instance* CreateRootInstance();

	void GenerateInstancesInRequirred(float localTime, RandObject& rand, Instance* parent);

	Instance* GetFirst();

	int GetInstanceCount() const;

	void Update(bool shown);

	void SetBaseMatrix(const SIMD::Mat43f& mat);

	void SetParentMatrix(const SIMD::Mat43f& mat);

	void RemoveForcibly();

	void KillAllInstances();

	bool IsActive() const;

	//int32_t GetTime() const
	//{
	//	return m_time;
	//}

	/**
		@brief	グループを生成したインスタンスからの参照が残っているか?
	*/
	bool IsReferencedFromInstance = true;

	/**
		@brief	インスタンスから利用する連結リストの次のオブジェクトへのポインタ
	*/
	InstanceGroup* NextUsedByInstance = nullptr;

	/**
		@brief	コンテナから利用する連結リストの次のオブジェクトへのポインタ
	*/
	InstanceGroup* NextUsedByContainer = nullptr;

	InstanceGlobal* GetRootInstance() const
	{
		return m_global;
	}

	const SIMD::Mat43f& GetParentMatrix() const
	{
		return parentMatrix_;
	}
	const SIMD::Vec3f& GetParentTranslation() const
	{
		return parentTranslation_;
	}
	const SIMD::Mat43f& GetParentRotation() const
	{
		return parentRotation_;
	}
	const SIMD::Vec3f& GetParentScale() const
	{
		return parentScale_;
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_INSTANCEGROUP_H__
