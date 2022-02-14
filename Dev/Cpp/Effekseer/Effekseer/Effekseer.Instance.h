﻿
#ifndef __EFFEKSEER_INSTANCE_H__
#define __EFFEKSEER_INSTANCE_H__

#include "Effekseer.Base.h"

#include "SIMD/Mat43f.h"
#include "SIMD/Mat44f.h"
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

#include "Parameter/UV.h"

namespace Effekseer
{

struct InstanceCustomData
{
	union
	{
		struct
		{
			SIMD::Vec2f start;
			SIMD::Vec2f end;
		} easing;

		struct
		{
			SIMD::Vec2f value;
		} random;

		struct
		{
			SIMD::Vec2f offset;
		} fcruve;

		struct
		{
			std::array<float, 4> offset;
		} fcurveColor;
	};
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

	// マネージャ
	ManagerImplemented* m_pManager;

	// パラメーター
	EffectNodeImplemented* m_pEffectNode;

	// コンテナ
	InstanceContainer* m_pContainer;

	// a group which the instance belongs to
	// 自分が所属するグループ
	InstanceGroup* ownGroup_;

	// a head of list in children group
	// 子グループの連結リストの先頭
	InstanceGroup* childrenGroups_;

	// 親
	Instance* m_pParent;

	// Random generator
	RandObject m_randObject;

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

	union
	{
		struct
		{
			SIMD::Vec3f rotation;
		} fixed;

		struct
		{
			SIMD::Vec3f rotation;
			SIMD::Vec3f velocity;
			SIMD::Vec3f acceleration;
		} random;

		InstanceEasing<SIMD::Vec3f> easing;
		/*
		struct
		{
			SIMD::Vec3f start;
			SIMD::Vec3f end;
		} easing;
		*/

		struct
		{
			float rotation;
			SIMD::Vec3f axis;

			union
			{
				struct
				{
					float rotation;
					float velocity;
					float acceleration;
				} random;

				InstanceEasing<float> easing;
			};
		} axis;

		struct
		{
			SIMD::Vec3f offset;
		} fcruve;

	} rotation_values;

	union
	{
		struct
		{
			SIMD::Vec3f scale;
		} fixed;

		struct
		{
			SIMD::Vec3f scale;
			SIMD::Vec3f velocity;
			SIMD::Vec3f acceleration;
		} random;

		InstanceEasing<SIMD::Vec3f> easing;

		/*
		struct
		{
			SIMD::Vec3f start;
			SIMD::Vec3f end;
		} easing;
		*/

		struct
		{
			float scale;
			float velocity;
			float acceleration;
		} single_random;

		InstanceEasing<float> single_easing;

		struct
		{
			SIMD::Vec3f offset;
		} fcruve;

		struct
		{
			float offset;
		} single_fcruve;

	} scaling_values;

	// 描画
	union
	{
		EffectNodeSprite::InstanceValues sprite;
		EffectNodeRibbon::InstanceValues ribbon;
		EffectNodeRing::InstanceValues ring;
		EffectNodeModel::InstanceValues model;
		EffectNodeTrack::InstanceValues track;
	} rendererValues;

	// 音
	union
	{
		int32_t delay;
	} soundValues;

	// 状態
	eInstanceState m_State;

	// 生存時間
	float m_LivedTime;

	// 生成されてからの時間
	float m_LivingTime;

	// 削除されてからの時間
	float m_RemovingTime;

	std::array<UVAnimationInstanceData, ParameterRendererCommon::UVParameterNum> uvAnimationData_;

	// Spawning Method matrix
	SIMD::Mat43f m_GenerationLocation;

	// a transform matrix in the world coordinate
	SIMD::Mat43f m_GlobalMatrix43;

	// parent's transform matrix
	SIMD::Mat43f m_ParentMatrix;

	// FirstUpdate実行前
	bool m_IsFirstTime;

	// 変換用行列が計算済かどうか
	bool m_GlobalMatrix43Calculated;

	// 親の変換用行列が計算済かどうか
	bool m_ParentMatrix43Calculated;

	//! whether a time is allowed to pass
	bool is_time_step_allowed;

	int32_t m_InstanceNumber;

	/* 更新番号 */
	uint32_t m_sequenceNumber;

	//float flipbookIndexAndNextRate_ = 0;

	union
	{
		struct
		{
		} fixed;

		struct
		{
			float begin_threshold;
			int32_t transition_frame;
			float no2_threshold;
			float no3_threshold;
			int32_t transition_frame2;
			float end_threshold;
		} four_point_interpolation;

		InstanceEasing<float> easing;

		struct
		{
			float offset;
		} fcurve;

	} alpha_cutoff_values;

	float m_AlphaThreshold;

	// コンストラクタ
	Instance(ManagerImplemented* pManager, EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup);

	// デストラクタ
	virtual ~Instance();

	void GenerateChildrenInRequired();

	void UpdateChildrenGroupMatrix();

	InstanceGlobal* GetInstanceGlobal();

public:
	bool IsFirstTime() const
	{
		return m_IsFirstTime;
	}

	/**
		@brief	状態の取得
	*/
	eInstanceState GetState() const;

	/**
		@brief	アクティブ状態の判定
	*/
	bool IsActive() const
	{
		return m_State <= INSTANCE_STATE_REMOVING;
	}

	/**
		@brief	行列の取得
	*/
	const SIMD::Mat43f& GetGlobalMatrix43() const;

	/**
		@brief	初期化
	*/
	void Initialize(Instance* parent, int32_t instanceNumber, const SIMD::Mat43f& globalMatrix);

	/**
		@brief	初回の更新
	*/
	void FirstUpdate();

	/**
		@brief	更新
	*/
	void Update(float deltaFrame, bool shown);

	/**
		@brief	Draw instance
	*/
	void Draw(Instance* next, void* userData);

	/**
		@brief	破棄
	*/
	void Kill();

	/**
		@brief	UVの位置取得
	*/
	RectF GetUV(const int32_t index) const;

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
	/**
		@brief	行列の更新
	*/
	void CalculateMatrix(float deltaFrame);

	/**
		@brief	行列の更新
	*/
	void CalculateParentMatrix(float deltaFrame);

	void ApplyDynamicParameterToFixedRotation();

	void ApplyDynamicParameterToFixedScaling();

	float GetFlipbookIndexAndNextRate(const UVAnimationType& UVType, const UVParameter& UV, const UVAnimationInstanceData& data) const;

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_INSTANCE_H__
