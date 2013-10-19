
#ifndef	__EFFEKSEER_INSTANCE_H__
#define	__EFFEKSEER_INSTANCE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.RectF.h"
#include "Effekseer.Color.h"

#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Effekseer.EffectNodeModel.h"
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
	@brief	エフェクトの実体
*/
class Instance
{
	friend class Manager;
	friend class InstanceContainer;
public:

	// マネージャ
	Manager*	m_pManager;

	// パラメーター
	EffectNode* m_pEffectNode;

	// コンテナ
	InstanceContainer*	m_pContainer;

	// グループの連結リストの先頭
	InstanceGroup*	m_headGroups;

	// 親
	Instance*	m_pParent;

	// 位置
	Vector3D	m_LocalPosition;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d location;
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d	start;
			vector3d	end;
		} easing;

		struct
		{
			vector3d	offset;
		} fcruve;

	} translation_values;	

	// 補正位置
	vector3d	m_globalRevisionLocation;

	union 
	{
		struct
		{
		
		} none;

		struct
		{
			vector3d	velocity;
		} gravity;

	} translation_abs_values;

	// 回転
	Vector3D	m_LocalAngle;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d rotation;
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d start;
			vector3d end;
		} easing;
		
		struct
		{
			float rotation;

			union
			{
				struct
				{
					float rotation;
					float velocity;
					float acceleration;
				} random;

				struct
				{
					float start;
					float end;
				} easing;
			};
		} axis;

		struct
		{
			vector3d offset;
		} fcruve;

	} rotation_values;

	// 拡大縮小
	Vector3D	m_LocalScaling;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d  scale;
			vector3d  velocity;
			vector3d  acceleration;
		} random;

		struct
		{
			vector3d  start;
			vector3d  end;
		} easing;
		
		struct
		{
			float  scale;
			float  velocity;
			float  acceleration;
		} single_random;

		struct
		{
			float  start;
			float  end;
		} single_easing;

		struct
		{
			vector3d offset;
		} fcruve;

	} scaling_values;

	// 描画
	union
	{
		EffectNodeSprite::InstanceValues	sprite;
		EffectNodeRibbon::InstanceValues	ribbon;
		EffectNodeRing::InstanceValues		ring;
		EffectNodeModel::InstanceValues		model;
		EffectNodeTrack::InstanceValues		track;
	} rendererValues;
	
	// 音
	union
	{
		int		delay;
	} soundValues;

	// 状態
	eInstanceState	m_State;

	// 生存時間
	float		m_LivedTime;

	// 生成されてからの時間
	float		m_LivingTime;

	// 生成位置
	Matrix43		m_generation_location;

	// 変換用行列
	Matrix43		m_GlobalMatrix43;

	// 親の変換用行列
	Matrix43		m_ParentMatrix43;

	/* 時間を進めるかどうか? */
	bool			m_stepTime;

	/* 更新番号 */
	uint32_t		m_sequenceNumber;

	// コンストラクタ
	Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer );

	// デストラクタ
	virtual ~Instance();
public:

	/**
		@brief	状態の取得
	*/
	eInstanceState GetState() const;

	/**
		@brief	行列の取得
	*/
	const Matrix43& GetGlobalMatrix43() const;

	/**
		@brief	初期化
	*/
	void Initialize( Instance* parent, int32_t instanceNumber );

	/**
		@brief	更新
	*/
	void Update( float deltaFrame, bool shown );

	/**
		@brief	行列の更新
	*/
	void CalculateMatrix( float deltaFrame );

	/**
		@brief	描画
	*/
	void Draw();

	/**
		@brief	破棄
	*/
	void Kill();

	/**
		@brief	UVの位置取得
	*/
	RectF GetUV() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCE_H__
