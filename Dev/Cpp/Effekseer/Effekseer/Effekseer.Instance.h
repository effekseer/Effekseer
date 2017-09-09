
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
	static const int32_t ChildrenMax = 16;

	// マネージャ
	Manager*	m_pManager;

	// パラメーター
	EffectNodeImplemented* m_pEffectNode;

	// コンテナ
	InstanceContainer*	m_pContainer;

	// グループの連結リストの先頭
	InstanceGroup*	m_headGroups;

	// 親
	Instance*	m_pParent;
	
	// グローバル位置
	Vector3D	m_GlobalPosition;
	Vector3D	m_GlobalVelocity;
	
	// グローバル位置補正
	Vector3D	m_GlobalRevisionLocation;
	Vector3D	m_GlobalRevisionVelocity;
	
	// Color for binding
	color		ColorInheritance;

	// Parent color
	color		ColorParent;

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
			vector3d axis;

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

	// The time offset for UV
	int32_t		uvTimeOffset;

	// Scroll, FCurve area for UV
	RectF		uvAreaOffset;

	// Scroll speed for UV
	Vector2D	uvScrollSpeed;

	// The number of generated chiledren. (fixed size)
	int32_t		m_fixedGeneratedChildrenCount[ChildrenMax];

	// The time to generate next child.  (fixed size)
	float		m_fixedNextGenerationTime[ChildrenMax];

	// The number of generated chiledren. (flexible size)
	int32_t*		m_flexibleGeneratedChildrenCount;

	// The time to generate next child.  (flexible size)
	float*		m_flexibleNextGenerationTime;

	// The number of generated chiledren. (actually used)
	int32_t*		m_generatedChildrenCount;

	// The time to generate next child.  (actually used)
	float*			m_nextGenerationTime;

	// Spawning Method matrix
	Matrix43		m_GenerationLocation;

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

private:
	/**
		@brief	行列の更新
	*/
	void CalculateMatrix( float deltaFrame );
	
	/**
		@brief	行列の更新
	*/
	void CalculateParentMatrix();
	
	/**
		@brief	絶対パラメータの反映
	*/
	void ModifyMatrixFromLocationAbs( float deltaFrame );
	
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCE_H__
