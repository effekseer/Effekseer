
#ifndef	__EFFEKSEER_MANAGER_IMPLEMENTED_H__
#define	__EFFEKSEER_MANAGER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"
#include "Culling/Culling3D.h"
#include "Utils/Effekseer.CustomAllocator.h"
#include "Effekseer.IntrusiveList.h"
#include "Effekseer.InstanceChunk.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief エフェクト管理クラス(実装)
*/
class ManagerImplemented
	: public Manager
	, public ReferenceObject
{
	friend class Effect;
	friend class EffectNode;
	friend class InstanceContainer;
	friend class InstanceGroup;

private:

	/**
		@brief	再生オブジェクトの組み合わせ
	*/
	class DrawSet
	{
	public:
		Effect*			ParameterPointer;
		InstanceContainer*	InstanceContainerPointer;
		InstanceGlobal*		GlobalPointer;
		Culling3D::Object*	CullingObjectPointer;
		bool				IsPaused;
		bool				IsShown;
		bool				IsAutoDrawing;
		bool				IsRemoving;
		bool				IsParameterChanged;
		bool				DoUseBaseMatrix;
		bool				GoingToStop;
		bool				GoingToStopRoot;
		EffectInstanceRemovingCallback	RemovingCallback;
		
		Mat43f				BaseMatrix;
		Mat43f				GlobalMatrix;

		float				Speed;

		Handle				Self;

		bool				IsPreupdated = false;
		int32_t				StartFrame = 0;

		int32_t Layer = 0;

		DrawSet( Effect* effect, InstanceContainer* pContainer, InstanceGlobal* pGlobal )
			: ParameterPointer			( effect )
			, InstanceContainerPointer	( pContainer )
			, GlobalPointer				( pGlobal )
			, CullingObjectPointer		( NULL )
			, IsPaused					( false )
			, IsShown					( true )
			, IsAutoDrawing				( true )
			, IsRemoving				( false )
			, IsParameterChanged		( false )
			, DoUseBaseMatrix			( false )
			, GoingToStop				( false )
			, GoingToStopRoot			( false )
			, RemovingCallback			( NULL )
			, Speed						( 1.0f )
			, Self						( -1 )
		{
		
		}

		DrawSet()
			: ParameterPointer			( NULL )
			, InstanceContainerPointer	( NULL )
			, GlobalPointer				( NULL )
			, CullingObjectPointer		( NULL )
			, IsPaused					( false )
			, IsShown					( true )
			, IsRemoving				( false )
			, IsParameterChanged		( false )
			, DoUseBaseMatrix			( false )
			, RemovingCallback			(NULL)
			, Speed						( 1.0f )
			, Self						( -1 )
		{
		
		}

		Mat43f* GetEnabledGlobalMatrix();

		void CopyMatrixFromInstanceToRoot();
	};

	struct CullingParameter
	{
		float		SizeX;
		float		SizeY;
		float		SizeZ;
		int32_t		LayerCount;

		CullingParameter()
		{
			SizeX = 0.0f;
			SizeY = 0.0f;
			SizeZ = 0.0f;
			LayerCount = 0;
		}

	} cullingCurrent, cullingNext;

private:
	/* 自動データ入れ替えフラグ */
	bool m_autoFlip;

	// 次のHandle
	Handle		m_NextHandle;

	// 確保済みインスタンス数
	int m_instance_max;

	// buffers which is allocated while initializing
	// 初期化中に確保されたバッファ
	std::unique_ptr<InstanceChunk[]> reservedChunksBuffer_;
	std::unique_ptr<uint8_t[]> reservedGroupBuffer_;
	std::unique_ptr<uint8_t[]> reservedContainerBuffer_;

	// pooled instances. Thease are not used and waiting to be used.
	// プールされたインスタンス。使用されておらず、使用されてるのを待っている。
	std::queue<InstanceChunk*> pooledChunks_;
	std::queue<InstanceGroup*> pooledGroups_;
	std::queue<InstanceContainer*> pooledContainers_;

	// instance chunks by generations
	// 世代ごとのインスタンスチャンク
	static const size_t GenerationsMax = 20;
	std::array<std::vector<InstanceChunk*>, GenerationsMax> instanceChunks_;
	std::array<int32_t, GenerationsMax> creatableChunkOffsets_;

	// 再生中オブジェクトの組み合わせ集合体
	std::map<Handle,DrawSet>	m_DrawSets;

	// 破棄待ちオブジェクト
	std::map<Handle,DrawSet>	m_RemovingDrawSets[2];

	//! objects on rendering
	CustomVector<DrawSet> m_renderingDrawSets;

	//! objects on rendering
	CustomMap<Handle,DrawSet> m_renderingDrawSetMaps;

	// mutex for rendering
	std::mutex					m_renderingMutex;
	bool						m_isLockedWithRenderingMutex = false;

	/* 設定インスタンス */
	Setting*					m_setting;

	int							m_updateTime;
	int							m_drawTime;

	/* 更新回数カウント */
	uint32_t					m_sequenceNumber;
	
	/* カリング */
	Culling3D::World*			m_cullingWorld;

	/* カリング */
	std::vector<DrawSet*>	m_culledObjects;
	std::set<Handle>		m_culledObjectSets;
	bool					m_culled;

	/* スプライト描画機能用インスタンス */
	SpriteRenderer*				m_spriteRenderer;

	/* リボン描画機能用インスタンス */
	RibbonRenderer*				m_ribbonRenderer;

	/* リング描画機能用インスタンス */
	RingRenderer*				m_ringRenderer;

	/* モデル描画機能用インスタンス */
	ModelRenderer*				m_modelRenderer;

	/* トラック描画機能用インスタンス */
	TrackRenderer*				m_trackRenderer;

	/* サウンド再生用インスタンス */
	SoundPlayer*				m_soundPlayer;

	// メモリ確保関数
	MallocFunc	m_MallocFunc;

	// メモリ破棄関数
	FreeFunc	m_FreeFunc;

	// ランダム関数
	RandFunc	m_randFunc;

	// ランダム関数最大値
	int			m_randMax;

	// 描画オブジェクト追加
	Handle AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer );

	// 描画オブジェクト破棄処理
	void GCDrawSet( bool isRemovingManager );

	// メモリ確保関数
	static void* EFK_STDCALL Malloc( unsigned int size );

	// メモリ破棄関数
	static void EFK_STDCALL Free( void* p, unsigned int size );

	// ランダム関数
	static int EFK_STDCALL Rand();

	// 破棄等のイベントを実際に実行
	void ExecuteEvents();
public:

	// コンストラクタ
	ManagerImplemented( int instance_max, bool autoFlip );

	// デストラクタ
	virtual ~ManagerImplemented();

	/* Root以外のインスタンスバッファ生成(Flip,Update,終了時からのみ呼ばれる) */
	Instance* CreateInstance( EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup );

	InstanceGroup* CreateInstanceGroup( EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGlobal* pGlobal );
	void ReleaseGroup( InstanceGroup* group );

	InstanceContainer* CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, const Mat43f& rootMatrix, Instance* pParent );
	void ReleaseInstanceContainer( InstanceContainer* container );

	/**
		@brief マネージャー破棄
		@note
		このマネージャーから生成されたエフェクトは全て強制的に破棄されます。
	*/
	void Destroy() override;

	/**
		@brief	更新番号を取得する。
	*/
	uint32_t GetSequenceNumber() const;

	MallocFunc GetMallocFunc() const override;

	void SetMallocFunc( MallocFunc func ) override;

	FreeFunc GetFreeFunc() const override;

	void SetFreeFunc( FreeFunc func ) override;

	RandFunc GetRandFunc() const override;

	void SetRandFunc( RandFunc func ) override;

	/**
		@brief	ランダム最大値取得
	*/
	int GetRandMax() const override;

	/**
		@brief	ランダム関数設定
	*/
	void SetRandMax( int max_ ) override;

	/**
		@brief	座標系を取得する。
	*/
	CoordinateSystem GetCoordinateSystem() const override;

	/**
		@brief	座標系を設定する。
	*/
	void SetCoordinateSystem( CoordinateSystem coordinateSystem ) override;

	/**
		@brief	スプライト描画機能取得
	*/
	SpriteRenderer* GetSpriteRenderer() override;

	/**
		@brief	スプライト描画機能設定
	*/
	void SetSpriteRenderer( SpriteRenderer* renderer ) override;

	/**
		@brief	リボン描画機能取得
	*/
	RibbonRenderer* GetRibbonRenderer() override;

	/**
		@brief	リボン描画機能設定
	*/
	void SetRibbonRenderer( RibbonRenderer* renderer ) override;

	/**
		@brief	リング描画機能取得
	*/
	RingRenderer* GetRingRenderer() override;

	/**
		@brief	リング描画機能設定
	*/
	void SetRingRenderer( RingRenderer* renderer ) override;

	/**
		@brief	モデル描画機能取得
	*/
	ModelRenderer* GetModelRenderer() override;

	/**
		@brief	モデル描画機能設定
	*/
	void SetModelRenderer( ModelRenderer* renderer ) override;

	/**
		@brief	軌跡描画機能取得
	*/
	TrackRenderer* GetTrackRenderer() override;

	/**
		@brief	軌跡描画機能設定
	*/
	void SetTrackRenderer( TrackRenderer* renderer ) override;

	/**
		@brief	設定クラスを取得する。
	*/
	Setting* GetSetting() override;

	/**
		@brief	設定クラスを設定する。
		@param	setting	[in]	設定
	*/
	void SetSetting(Setting* setting) override;

	/**
		@brief	エフェクト読込クラスを取得する。
	*/
	EffectLoader* GetEffectLoader() override;

	/**
		@brief	エフェクト読込クラスを設定する。
	*/
	void SetEffectLoader( EffectLoader* effectLoader ) override;

	/**
		@brief	テクスチャ読込クラスを取得する。
	*/
	TextureLoader* GetTextureLoader() override;

	/**
		@brief	テクスチャ読込クラスを設定する。
	*/
	void SetTextureLoader( TextureLoader* textureLoader ) override;
	
	/**
		@brief	サウンド再生取得
	*/
	SoundPlayer* GetSoundPlayer() override;

	/**
		@brief	サウンド再生機能設定
	*/
	void SetSoundPlayer( SoundPlayer* soundPlayer ) override;
	
	/**
		@brief	サウンド再生取得
	*/
	SoundLoader* GetSoundLoader() override;

	/**
		@brief	サウンド再生機能設定
	*/
	void SetSoundLoader( SoundLoader* soundLoader ) override;

	/**
		@brief	モデル読込クラスを取得する。
	*/
	ModelLoader* GetModelLoader() override;

	/**
		@brief	モデル読込クラスを設定する。
	*/
	void SetModelLoader( ModelLoader* modelLoader ) override;
	
	MaterialLoader* GetMaterialLoader() override;

	void SetMaterialLoader(MaterialLoader* loader) override;

	/**
		@brief	エフェクト停止
	*/
	void StopEffect( Handle handle ) override;

	/**
		@brief	全てのエフェクト停止
	*/
	void StopAllEffects() override;

	/**
		@brief	エフェクト停止
	*/
	void StopRoot( Handle handle ) override;

	/**
		@brief	エフェクト停止
	*/
	void StopRoot( Effect* effect ) override;

	/**
		@brief	エフェクトのインスタンスが存在しているか取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	存在してるか?
	*/
	bool Exists( Handle handle ) override;

	int32_t GetInstanceCount( Handle handle ) override;

	int32_t GetTotalInstanceCount() const override;

	/**
		@brief	エフェクトのインスタンスに設定されている行列を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	行列
	*/
	Matrix43 GetMatrix( Handle handle ) override;

	/**
		@brief	エフェクトのインスタンスに変換行列を設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	mat		[in]	変換行列
	*/
	void SetMatrix( Handle handle, const Matrix43& mat ) override;

	Vector3D GetLocation( Handle handle ) override;
	void SetLocation( Handle handle, float x, float y, float z ) override;
	void SetLocation( Handle handle, const Vector3D& location ) override;
	void AddLocation( Handle handle, const Vector3D& location ) override;

	/**
		@brief	エフェクトのインスタンスの回転角度を指定する。(ラジアン)
	*/
	void SetRotation( Handle handle, float x, float y, float z ) override;

	/**
		@brief	エフェクトのインスタンスの任意軸周りの反時計周りの回転角度を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	axis	[in]	軸
		@param	angle	[in]	角度(ラジアン)
	*/
	void SetRotation( Handle handle, const Vector3D& axis, float angle ) override;

	/**
		@brief	エフェクトのインスタンスの拡大率を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	x		[in]	X方向拡大率
		@param	y		[in]	Y方向拡大率
		@param	z		[in]	Z方向拡大率
	*/
	void SetScale( Handle handle, float x, float y, float z ) override;

	void SetAllColor(Handle handle, Color color) override;

	// エフェクトのターゲット位置を指定する。
	void SetTargetLocation( Handle handle, float x, float y, float z ) override;
	void SetTargetLocation( Handle handle, const Vector3D& location ) override;

	float GetDynamicInput(Handle handle, int32_t index) override;

	void SetDynamicInput(Handle handle, int32_t index, float value) override;

	Matrix43 GetBaseMatrix( Handle handle ) override;

	void SetBaseMatrix( Handle handle, const Matrix43& mat ) override;

	/**
		@brief	エフェクトのインスタンスに廃棄時のコールバックを設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	callback	[in]	コールバック
	*/
	void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback ) override;

	bool GetShown(Handle handle) override;

	void SetShown( Handle handle, bool shown ) override;
	
	bool GetPaused(Handle handle) override;

	void SetPaused( Handle handle, bool paused ) override;

	void SetPausedToAllEffects(bool paused) override;

	int GetLayer(Handle handle) override;

	void SetLayer(Handle handle, int32_t layer) override;

	float GetSpeed(Handle handle) const override;

	void SetSpeed( Handle handle, float speed ) override;
	
	void SetAutoDrawing( Handle handle, bool autoDraw ) override;
	
	void Flip() override;

	void Update( float deltaFrame ) override;

	void BeginUpdate() override;

	void EndUpdate() override;

	void UpdateHandle( Handle handle, float deltaFrame = 1.0f ) override;

private:

	void UpdateInstancesByInstanceGlobal(const DrawSet& drawSet);

	//! update draw sets
	void UpdateHandleInternal(DrawSet& drawSet);

	void Preupdate(DrawSet& drawSet);

	//! whether container is disabled while rendering because of a distance between the effect and a camera
	bool IsClippedWithDepth(DrawSet& drawSet, InstanceContainer* container, const Manager::DrawParameter& drawParameter);
 public:

	void Draw(const Manager::DrawParameter& drawParameter) override;
	
	void DrawBack(const Manager::DrawParameter& drawParameter) override;

	void DrawFront(const Manager::DrawParameter& drawParameter) override;

	void DrawHandle(Handle handle, const Manager::DrawParameter& drawParameter) override;

	void DrawHandleBack(Handle handle, const Manager::DrawParameter& drawParameter) override;

	void DrawHandleFront(Handle handle, const Manager::DrawParameter& drawParameter) override;

	Handle Play( Effect* effect, float x, float y, float z ) override;

	Handle Play(Effect* effect, const Vector3D& position, int32_t startFrame) override;
	
	int GetCameraCullingMaskToShowAllEffects() override;

	int GetUpdateTime() const override;
	
	int GetDrawTime() const override;

	int32_t GetRestInstancesCount() const override;

	/**
		@brief	start reload
	*/
	void BeginReloadEffect( Effect* effect, bool doLockThread );

	/**
		@brief	end reload
	*/
	void EndReloadEffect( Effect* effect, bool doLockThread);

	/**
		@brief	エフェクトをカリングし描画負荷を減らすための空間を生成する。
		@param	xsize	X方向幅
		@param	ysize	Y方向幅
		@param	zsize	Z方向幅
		@param	layerCount	層数(大きいほどカリングの効率は上がるがメモリも大量に使用する)
	*/
	void CreateCullingWorld( float xsize, float ysize, float zsize, int32_t layerCount) override;

	/**
		@brief	カリングを行い、カリングされたオブジェクトのみを描画するようにする。
		@param	cameraProjMat	カメラプロジェクション行列
		@param	isOpenGL		OpenGLによる描画か?
	*/
	void CalcCulling(const Matrix44& cameraProjMat, bool isOpenGL) override;

	/**
		@brief	現在存在するエフェクトのハンドルからカリングの空間を配置しなおす。
	*/
	void RessignCulling() override;

	virtual int GetRef() override { return ReferenceObject::GetRef(); }
	virtual int AddRef() override { return ReferenceObject::AddRef(); }
	virtual int Release() override { return ReferenceObject::Release(); }
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MANAGER_IMPLEMENTED_H__
