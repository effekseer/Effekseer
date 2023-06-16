
#ifndef __EFFEKSEER_MANAGER_IMPLEMENTED_H__
#define __EFFEKSEER_MANAGER_IMPLEMENTED_H__

#include "Effekseer.Base.h"
#include "Effekseer.InstanceChunk.h"
#include "Effekseer.IntrusiveList.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"
#include "Effekseer.WorkerThread.h"
#include "Geometry/GeometryUtility.h"
#include "Utils/Effekseer.CustomAllocator.h"
#include "Utils/InstancePool.h"

namespace Effekseer
{

class ManagerImplemented : public Manager, public ReferenceObject
{
	friend class Effect;
	friend class EffectNode;
	friend class InstanceContainer;
	friend class InstanceGroup;

public:
	class alignas(32) DrawSet
	{
	public:
		EffectRef ParameterPointer;
		InstanceContainer* InstanceContainerPointer;
		InstanceGlobal* GlobalPointer;
		int RandomSeed = 0;
		bool IsPaused;
		bool IsShown;
		bool IsAutoDrawing;
		bool IsRemoving;
		bool IsParameterChanged;
		bool DoUseBaseMatrix;
		bool GoingToStop;
		bool GoingToStopRoot;
		bool AreChildrenOfRootGenerated = false;
		EffectInstanceRemovingCallback RemovingCallback;

		Matrix43 Rotation;
		Vector3D Scaling = {1.f, 1.f, 1.f};

		SIMD::Mat43f BaseMatrix;

		float Speed;

		Handle Self;

		bool IsPreupdated = false;
		int32_t StartFrame = 0;

		//! a time (by 1/60) to progress an effect when Update is called
		float NextUpdateFrame = 0.0f;

		//! Rate of scale in relation to manager's time
		float TimeScale = 1.0f;

		//! HACK for GC (Instances must be updated after removing) If you use UpdateHandle, updating instance which is contained removing
		//! effects is not called. It makes update called forcibly.
		int32_t UpdateCountAfterRemoving = 0;

		//! a bit mask for group
		int64_t GroupMask = 0;

		Vector3D CullingPosition{};
		float CullingRadius{};

		DrawSet(const EffectRef& effect, InstanceContainer* pContainer, InstanceGlobal* pGlobal)
			: ParameterPointer(effect)
			, InstanceContainerPointer(pContainer)
			, GlobalPointer(pGlobal)
			, IsPaused(false)
			, IsShown(true)
			, IsAutoDrawing(true)
			, IsRemoving(false)
			, IsParameterChanged(false)
			, DoUseBaseMatrix(false)
			, GoingToStop(false)
			, GoingToStopRoot(false)
			, RemovingCallback(nullptr)
			, Speed(1.0f)
			, Self(-1)
		{
			Rotation.Indentity();
		}

		DrawSet()
			: ParameterPointer(nullptr)
			, InstanceContainerPointer(nullptr)
			, GlobalPointer(nullptr)
			, IsPaused(false)
			, IsShown(true)
			, IsRemoving(false)
			, IsParameterChanged(false)
			, DoUseBaseMatrix(false)
			, RemovingCallback(nullptr)
			, Speed(1.0f)
			, Self(-1)
		{
			Rotation.Indentity();
		}

		SIMD::Mat43f GetGlobalMatrix() const;

		void SetGlobalMatrix(const SIMD::Mat43f& mat);

		void UpdateLevelOfDetails(const LayerParameter& loadParameter);

	private:
		SIMD::Mat43f GlobalMatrix;
	};

private:
	CustomVector<WorkerThread> m_WorkerThreads;

	//! whether does rendering and update handle flipped automatically
	bool m_autoFlip = true;

	//! next handle
	Handle m_NextHandle = 0;

	// 確保済みインスタンス数
	int m_instance_max;

	/**
		@note
		An user can specify only the maximum number of instance.
		But the number of instance container is larger than one of instance.
	*/
	InstancePool<InstanceChunk> pooledInstanceChunks_;
	InstancePool<InstanceGroup> pooledInstanceGroup_;
	InstancePool<InstanceContainer> pooledInstanceContainers_;

	// instance chunks by generations
	// 世代ごとのインスタンスチャンク
	static const size_t GenerationsMax = 20;
	std::array<std::vector<InstanceChunk*>, GenerationsMax> instanceChunks_;
	std::array<int32_t, GenerationsMax> creatableChunkOffsets_;

	// playing objects
	CustomAlignedMap<Handle, DrawSet> m_DrawSets;

	//! objects which are waiting to be disposed
	std::array<CustomAlignedMap<Handle, DrawSet>, 2> m_RemovingDrawSets;

	//! objects on rendering
	CustomAlignedVector<DrawSet> m_renderingDrawSets;

	//! objects on rendering temporaly (sorted)
	CustomAlignedVector<DrawSet> sortedRenderingDrawSets_;

	//! objects on rendering
	CustomAlignedMap<Handle, DrawSet> m_renderingDrawSetMaps;

	// mutex for rendering
	std::recursive_mutex m_renderingMutex;
	bool m_isLockedWithRenderingMutex = false;

	SettingRef m_setting;

	int m_updateTime;
	int m_drawTime;

	uint32_t m_sequenceNumber;

	SpriteRendererRef m_spriteRenderer;

	RibbonRendererRef m_ribbonRenderer;

	RingRendererRef m_ringRenderer;

	ModelRendererRef m_modelRenderer;

	TrackRendererRef m_trackRenderer;

	GPUTimerRef m_gpuTimer;

	SoundPlayerRef m_soundPlayer;

	RandFunc m_randFunc;

	std::array<LayerParameter, LayerCount> m_layerParameters;

	std::queue<std::pair<SoundTag, SoundPlayer::InstanceParameter>> m_requestedSounds;
	std::mutex m_soundMutex;

	Handle AddDrawSet(const EffectRef& effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer);

	void StopStoppingEffects();

	//! GC Draw sets
	void GCDrawSet(bool isRemovingManager);

	static int Rand();

	void ExecuteEvents();

	void ExecuteSounds();

	void StoreSortingDrawSets(const Manager::DrawParameter& drawParameter);

	static bool CanDraw(const DrawSet& drawSet, const Manager::DrawParameter& drawParameter, const std::array<Plane, 6>& planes);

public:
	ManagerImplemented(int instance_max, bool autoFlip);

	virtual ~ManagerImplemented();

	Instance* CreateInstance(EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup);

	InstanceGroup* CreateInstanceGroup(EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGlobal* pGlobal);
	void ReleaseGroup(InstanceGroup* group);

	InstanceContainer*
	CreateInstanceContainer(EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, const SIMD::Mat43f& rootMatrix, Instance* pParent);
	void ReleaseInstanceContainer(InstanceContainer* container);

	void LaunchWorkerThreads(uint32_t threadCount) override;

	ThreadNativeHandleType GetWorkerThreadHandle(uint32_t threadID) override;

	uint32_t GetSequenceNumber() const;

	RandFunc GetRandFunc() const override;

	void SetRandFunc(RandFunc func) override;

	CoordinateSystem GetCoordinateSystem() const override;

	void SetCoordinateSystem(CoordinateSystem coordinateSystem) override;

	SpriteRendererRef GetSpriteRenderer() override;

	void SetSpriteRenderer(SpriteRendererRef renderer) override;

	RibbonRendererRef GetRibbonRenderer() override;

	void SetRibbonRenderer(RibbonRendererRef renderer) override;

	RingRendererRef GetRingRenderer() override;

	void SetRingRenderer(RingRendererRef renderer) override;

	ModelRendererRef GetModelRenderer() override;

	void SetModelRenderer(ModelRendererRef renderer) override;

	TrackRendererRef GetTrackRenderer() override;

	void SetTrackRenderer(TrackRendererRef renderer) override;

	GPUTimerRef GetGPUTimer() override;

	void SetGPUTimer(GPUTimerRef gpuTimer) override;

	const SettingRef& GetSetting() const override;

	void SetSetting(const SettingRef& setting) override;

	EffectLoaderRef GetEffectLoader() override;

	void SetEffectLoader(EffectLoaderRef effectLoader) override;

	TextureLoaderRef GetTextureLoader() override;

	void SetTextureLoader(TextureLoaderRef textureLoader) override;

	SoundPlayerRef GetSoundPlayer() override;

	void SetSoundPlayer(SoundPlayerRef soundPlayer) override;

	SoundLoaderRef GetSoundLoader() override;

	void SetSoundLoader(SoundLoaderRef soundLoader) override;

	ModelLoaderRef GetModelLoader() override;

	void SetModelLoader(ModelLoaderRef modelLoader) override;

	MaterialLoaderRef GetMaterialLoader() override;

	void SetMaterialLoader(MaterialLoaderRef loader) override;

	CurveLoaderRef GetCurveLoader() override;

	void SetCurveLoader(CurveLoaderRef loader) override;

	void StopEffect(Handle handle) override;

	void StopAllEffects() override;

	void StopRoot(Handle handle) override;

	void StopRoot(const EffectRef& effect) override;

	bool Exists(Handle handle) override;

	int32_t GetInstanceCount(Handle handle) override;

	int32_t GetTotalInstanceCount() const override;

	int GetCurrentLOD(Handle handle) override;

	const LayerParameter& GetLayerParameter(int32_t layer) const override;

	void SetLayerParameter(int32_t layer, const LayerParameter& layerParameter) override;

	Matrix43 GetMatrix(Handle handle) override;

	void SetMatrix(Handle handle, const Matrix43& mat) override;

	Vector3D GetLocation(Handle handle) override;
	void SetLocation(Handle handle, float x, float y, float z) override;
	void SetLocation(Handle handle, const Vector3D& location) override;
	void AddLocation(Handle handle, const Vector3D& location) override;

	void SetRotation(Handle handle, float x, float y, float z) override;

	void SetRotation(Handle handle, const Vector3D& axis, float angle) override;

	void SetScale(Handle handle, float x, float y, float z) override;

	void SetAllColor(Handle handle, Color color) override;

	void SetTargetLocation(Handle handle, float x, float y, float z) override;
	void SetTargetLocation(Handle handle, const Vector3D& location) override;

	float GetDynamicInput(Handle handle, int32_t index) override;

	void SetDynamicInput(Handle handle, int32_t index, float value) override;

	void SendTrigger(Handle handle, int32_t index) override;

	Matrix43 GetBaseMatrix(Handle handle) override;

	void SetBaseMatrix(Handle handle, const Matrix43& mat) override;

	void SetRemovingCallback(Handle handle, EffectInstanceRemovingCallback callback) override;

	bool GetShown(Handle handle) override;

	void SetShown(Handle handle, bool shown) override;

	bool GetPaused(Handle handle) override;

	void SetPaused(Handle handle, bool paused) override;

	void SetPausedToAllEffects(bool paused) override;

	void SetSpawnDisabled(Handle handle, bool spawnDisabled) override;

	bool GetSpawnDisabled(Handle handle) override;

	int32_t GetLayer(Handle handle) override;

	void SetLayer(Handle handle, int32_t layer) override;

	int64_t GetGroupMask(Handle handle) const override;

	void SetGroupMask(Handle handle, int64_t groupmask) override;

	float GetSpeed(Handle handle) const override;

	void SetSpeed(Handle handle, float speed) override;

	void SetTimeScaleByGroup(int64_t groupmask, float timeScale) override;

	void SetTimeScaleByHandle(Handle handle, float timeScale) override;

	void SetAutoDrawing(Handle handle, bool autoDraw) override;

	void* GetUserData(Handle handle) override;

	void SetUserData(Handle handle, void* userData) override;

	void Flip() override;

	void Update(float deltaFrame) override;

	void Update(const UpdateParameter& parameter) override;

	void DoUpdate(const UpdateParameter& parameter, int times);

	void BeginUpdate() override;

	void EndUpdate() override;

	void UpdateHandle(Handle handle, float deltaFrame = 1.0f) override;

	void UpdateHandleToMoveToFrame(Handle handle, float frame) override;

	void SetRandomSeed(Handle handle, int32_t seed) override;

private:
	void UpdateInstancesByInstanceGlobal(const DrawSet& drawSet);

	//! update draw sets
	void UpdateHandleInternal(DrawSet& drawSet);

	void Preupdate(DrawSet& drawSet);

	//! whether container is disabled while rendering because of a distance between the effect and a camera
	bool IsClippedWithDepth(DrawSet& drawSet, InstanceContainer* container, const Manager::DrawParameter& drawParameter);

	void StopWithoutRemoveDrawSet(DrawSet& drawSet);

	void ResetAndPlayWithDataSet(DrawSet& drawSet, float frame);

public:
	void Draw(const Manager::DrawParameter& drawParameter) override;

	void DrawBack(const Manager::DrawParameter& drawParameter) override;

	void DrawFront(const Manager::DrawParameter& drawParameter) override;

	void DrawHandle(Handle handle, const Manager::DrawParameter& drawParameter) override;

	void DrawHandleBack(Handle handle, const Manager::DrawParameter& drawParameter) override;

	void DrawHandleFront(Handle handle, const Manager::DrawParameter& drawParameter) override;

	bool GetIsCulled(Handle handle, const Manager::DrawParameter& drawParameter) override;

	Handle Play(const EffectRef& effect, float x, float y, float z) override;

	Handle Play(const EffectRef& effect, const Vector3D& position, int32_t startFrame) override;

	int GetCameraCullingMaskToShowAllEffects() override;

	int GetUpdateTime() const override;

	int GetDrawTime() const override;

	int32_t GetGPUTime() const override;

	int32_t GetGPUTime(Handle handle) const override;

	int32_t GetRestInstancesCount() const override;

	void BeginReloadEffect(const EffectRef& effect, bool doLockThread);

	void EndReloadEffect(const EffectRef& effect, bool doLockThread);

	const CustomAlignedMap<Handle, DrawSet>& GetPlayingDrawSets() const
	{
		return m_DrawSets;
	}

	virtual int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ReferenceObject::Release();
	}

	void LockRendering() override;

	void UnlockRendering() override;

	void RequestToPlaySound(Instance* instance, const EffectNodeImplemented* node);

	ManagerImplemented* GetImplemented() override
	{
		return this;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_MANAGER_IMPLEMENTED_H__
