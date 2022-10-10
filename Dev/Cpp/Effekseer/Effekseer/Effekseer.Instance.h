
#ifndef __EFFEKSEER_INSTANCE_H__
#define __EFFEKSEER_INSTANCE_H__

#include "Effekseer.Base.h"

#include "SIMD/Mat43f.h"
#include "SIMD/Mat44f.h"
#include "SIMD/Quaternionf.h"
#include "SIMD/Vec2f.h"
#include "SIMD/Vec3f.h"
#include "SIMD/Vec4f.h"

#include "Effekseer.Color.h"
#include "Effekseer.IntrusiveList.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Random.h"
#include "Effekseer.RectF.h"

#include "Effekseer.EffectNodeModel.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeTrack.h"
#include "ForceField/ForceFields.h"

#include "Parameter/AlphaCutoff.h"
#include "Parameter/CustomData.h"
#include "Parameter/Rotation.h"
#include "Parameter/Scaling.h"
#include "Parameter/UV.h"

namespace Effekseer
{

struct InstanceSoundState
{
	int32_t delay;
};

class TimeSeriesMatrix
{
	SIMD::Mat43f previous_ = SIMD::Mat43f::Identity;
	SIMD::Mat43f current_ = SIMD::Mat43f::Identity;
	float previousTime_ = 0;
	float currentTime_ = 0;
	bool isCurrentMatrixSpecified_ = false;

public:
	void Reset(const SIMD::Mat43f& matrix, float time);

	void Step(const SIMD::Mat43f& matrix, float time);

	const SIMD::Mat43f& GetPrevious() const;

	const SIMD::Mat43f& GetCurrent() const;

	SIMD::Mat43f Get(float time) const;
};

class alignas(16) Instance : public IntrusiveList<Instance>::Node
{
	friend class Manager;
	friend class InstanceContainer;

protected:
	//! custom data
	InstanceCustomData customDataValues1;
	InstanceCustomData customDataValues2;

	SIMD::Vec3f prevPosition_;
	SIMD::Vec3f prevGlobalPosition_;

	SIMD::Vec3f parentPosition_;
	SIMD::Vec3f steeringVec_;

public:
	static const int32_t ChildrenMax = 16;

	ManagerImplemented* m_pManager = nullptr;

	EffectNodeImplemented* m_pEffectNode = nullptr;

	InstanceContainer* m_pContainer = nullptr;

	// a group which the instance belongs to
	InstanceGroup* ownGroup_ = nullptr;

	// a head of list in children group
	InstanceGroup* childrenGroups_ = nullptr;

	Instance* m_pParent = nullptr;

	// Random generator
	RandObject m_randObject;

	float spawnDeltaFrame_ = 0.0f;

	LocalForceFieldInstance forceField_;

	// Color for binding
	Color ColorInheritance;

	// Parent color
	Color ColorParent;

	struct
	{
		float maxFollowSpeed;
		float steeringSpeed;
	} followParentParam;

	InstanceTranslationState translation_values;

	RotationState rotation_values;

	ScalingState scaling_values;

	// 描画
	union
	{
		EffectNodeSprite::InstanceValues sprite;
		EffectNodeRibbon::InstanceValues ribbon;
		EffectNodeRing::InstanceValues ring;
		EffectNodeModel::InstanceValues model;
		EffectNodeTrack::InstanceValues track;
	} rendererValues;

	InstanceSoundState soundValues;

	eInstanceState m_State = eInstanceState::INSTANCE_STATE_ACTIVE;

	// 生存時間
	float m_LivedTime = 0;

	// 生成されてからの時間
	float m_LivingTime = 0;

	// 削除されてからの時間
	float m_RemovingTime = 0;

	std::array<InstanceUVState, ParameterRendererCommon::UVParameterNum> uvAnimationData_;

	// Spawning Method matrix
	SIMD::Mat43f m_GenerationLocation;

	// a transform matrix in the world coordinate
	TimeSeriesMatrix globalMatrix_;

	SIMD::Mat43f globalMatrix_rendered;

	// parent's transform matrix
	SIMD::Mat43f m_ParentMatrix;

	// FirstUpdate実行前
	bool m_IsFirstTime;

	// 変換用行列が計算済かどうか
	bool m_GlobalMatrix43Calculated = false;

	// 親の変換用行列が計算済かどうか
	bool m_ParentMatrix43Calculated = false;

	//! whether a time is allowed to pass
	bool is_time_step_allowed = false;

	int32_t m_InstanceNumber = 0;

	uint32_t m_sequenceNumber = 0;

	AlphaCuttoffState alpha_cutoff_values;

	float m_AlphaThreshold = 0.0f;

	Instance(ManagerImplemented* pManager, EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup);

	virtual ~Instance();

	void GenerateChildrenInRequired();

	void UpdateChildrenGroupMatrix();

	InstanceGlobal* GetInstanceGlobal();

public:
	float GetNormalizedLivetime() const
	{
		return Clamp(m_LivingTime / m_LivedTime, 1.0f, 0.0f);
	}

	bool IsFirstTime() const
	{
		return m_IsFirstTime;
	}

	eInstanceState GetState() const;

	bool IsActive() const
	{
		return m_State <= eInstanceState::INSTANCE_STATE_REMOVING;
	}

	const TimeSeriesMatrix& GetGlobalMatrix() const;

	const SIMD::Mat43f& GetRenderedGlobalMatrix() const;

	void ResetGlobalMatrix(const SIMD::Mat43f& mat);

	void UpdateGlobalMatrix(const SIMD::Mat43f& mat);

	void ApplyBaseMatrix(const SIMD::Mat43f& baseMatrix);

	void Initialize(Instance* parent, float spawnDeltaFrame, int32_t instanceNumber);

	void FirstUpdate();

	void Update(float deltaFrame, bool shown);

	void Draw(Instance* next, int32_t index, void* userData);

	void Kill();

	RectF GetUV(const int32_t index) const;

	RectF GetUV(const int32_t index, float livingTime, float livedTime) const;

	//! get custom data
	std::array<float, 4> GetCustomData(int32_t index) const;

	//! get random object
	RandObject& GetRandObject()
	{
		return m_randObject;
	}

	bool AreChildrenActive() const;

	float GetFlipbookIndexAndNextRate() const;

private:
	void UpdateTransform(float deltaFrame);

	void UpdateParentMatrix(float deltaFrame);

	float GetFlipbookIndexAndNextRate(const UVAnimationType& UVType, const UVParameter& UV, const InstanceUVState& data) const;

	float GetUVTime() const;

	EffectNode* GetEffectNode() const
	{
		return m_pEffectNode;
	}

	InstanceContainer* GetContainer() const
	{
		return m_pContainer;
	}

	InstanceGroup* GetOwnGroup() const
	{
		return ownGroup_;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_INSTANCE_H__
