
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
#include "ForceField/Effekseer.ForceFields.h"

#include "Parameter/Effekseer.AlphaCutoff.h"
#include "Parameter/Effekseer.Collisions.h"
#include "Parameter/Effekseer.CustomData.h"
#include "Parameter/Effekseer.Rotation.h"
#include "Parameter/Effekseer.Scaling.h"
#include "Parameter/Effekseer.UV.h"

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

	SIMD::Vec3f prevLocalVelocity_;

	SIMD::Vec3f parentPosition_;

	SIMD::Vec3f steering_vec_;

	SIMD::Vec3f location_modify_global_;
	SIMD::Vec3f velocity_modify_global_;

	SIMD::Vec3f globalDirection_;

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

	InstanceTranslationState translation_state_;

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
	float livedTime_ = 0;

	// 生成されてからの時間
	float livingTime_ = 0;

	// 削除されてからの時間
	float removingTime_ = 0;

	std::array<InstanceUVState, ParameterRendererCommon::UVParameterNum> uvAnimationData_;

	// Spawning Method matrix
	SIMD::Mat43f generationLocation_;

	// a transform matrix in the world coordinate
	TimeSeriesMatrix globalMatrix_;

	SIMD::Mat43f globalMatrix_rendered;

	// parent's transform matrix
	SIMD::Mat43f parentMatrix_;

	// FirstUpdate実行前
	bool isFirstTime_;

	// 変換用行列が計算済かどうか
	bool m_GlobalMatrix43Calculated = false;

	// 親の変換用行列が計算済かどうか
	bool m_ParentMatrix43Calculated = false;

	//! whether a time is allowed to pass
	bool is_time_step_allowed = false;

	int32_t m_InstanceNumber = 0;

	uint32_t sequenceNumber_ = 0;

	AlphaCuttoffState alphaCutoffValues_;

	float alphaThreshold_ = 0.0f;

	CollisionsState collisionState_;

	bool isUVFlippedH_ = false;

	int32_t gpuEmitterID_ = -1;

	Instance(ManagerImplemented* pManager, EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup);

	virtual ~Instance();

	void GenerateChildrenIfRequired();

	void UpdateChildrenGroupMatrix();

	InstanceGlobal* GetInstanceGlobal();

public:
	float GetNormalizedLivetime() const
	{
		return Clamp(livingTime_ / livedTime_, 1.0f, 0.0f);
	}

	bool IsFirstTime() const
	{
		return isFirstTime_;
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

	void SetUVFlippedH(bool isFlipped)
	{
		isUVFlippedH_ = isFlipped;
	}

	bool IsUVFlippedH() const
	{
		return isUVFlippedH_;
	}

	bool AreChildrenActive() const;

	bool HasCollidedThisFrame() const;

	float GetFlipbookIndexAndNextRate() const;

	SIMD::Vec3f GetGlobalDirection() const;

private:
	void UpdateTransform(float deltaFrame);

	void UpdateParentMatrix(float deltaFrame);

	float GetFlipbookIndexAndNextRate(const UVAnimationType& UVType, const UVParameter& UV, const InstanceUVState& data) const;

	float GetUVTime() const;

	RectF ApplyUVHorizontalFlip(RectF uv) const;

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
