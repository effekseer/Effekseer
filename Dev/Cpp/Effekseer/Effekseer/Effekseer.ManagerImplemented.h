
#ifndef __EFFEKSEER_MANAGER_IMPLEMENTED_H__
#define __EFFEKSEER_MANAGER_IMPLEMENTED_H__

#include "Culling/Culling3D.h"
#include "Effekseer.Base.h"
#include "Effekseer.InstanceChunk.h"
#include "Effekseer.IntrusiveList.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.Matrix44.h"
#include "Utils/Effekseer.CustomAllocator.h"

namespace Effekseer
{

class ManagerImplemented : public Manager, public ReferenceObject
{
	friend class Effect;
	friend class EffectNode;
	friend class InstanceContainer;
	friend class InstanceGroup;

private:
	class alignas(32) DrawSet
	{
	public:
		Effect* ParameterPointer;
		InstanceContainer* InstanceContainerPointer;
		InstanceGlobal* GlobalPointer;
		Culling3D::Object* CullingObjectPointer;
		bool IsPaused;
		bool IsShown;
		bool IsAutoDrawing;
		bool IsRemoving;
		bool IsParameterChanged;
		bool DoUseBaseMatrix;
		bool GoingToStop;
		bool GoingToStopRoot;
		int RandomSeed = 0;
		EffectInstanceRemovingCallback RemovingCallback;

		Mat43f BaseMatrix;
		Mat43f GlobalMatrix;

		float Speed;

		Handle Self;

		bool IsPreupdated = false;
		int32_t StartFrame = 0;

		int32_t Layer = 0;

		//! HACK for GC (Instances must be updated after removing) If you use UpdateHandle, updating instance which is contained removing
		//! effects is not called. It makes update called forcibly.
		int32_t UpdateCountAfterRemoving = 0;

		DrawSet(Effect* effect, InstanceContainer* pContainer, InstanceGlobal* pGlobal)
			: ParameterPointer(effect)
			, InstanceContainerPointer(pContainer)
			, GlobalPointer(pGlobal)
			, CullingObjectPointer(NULL)
			, IsPaused(false)
			, IsShown(true)
			, IsAutoDrawing(true)
			, IsRemoving(false)
			, IsParameterChanged(false)
			, DoUseBaseMatrix(false)
			, GoingToStop(false)
			, GoingToStopRoot(false)
			, RemovingCallback(NULL)
			, Speed(1.0f)
			, Self(-1)
		{
		}

		DrawSet()
			: ParameterPointer(NULL)
			, InstanceContainerPointer(NULL)
			, GlobalPointer(NULL)
			, CullingObjectPointer(NULL)
			, IsPaused(false)
			, IsShown(true)
			, IsRemoving(false)
			, IsParameterChanged(false)
			, DoUseBaseMatrix(false)
			, RemovingCallback(NULL)
			, Speed(1.0f)
			, Self(-1)
		{
		}

		Mat43f* GetEnabledGlobalMatrix();

		void CopyMatrixFromInstanceToRoot();
	};

	struct CullingParameter
	{
		float SizeX;
		float SizeY;
		float SizeZ;
		int32_t LayerCount;

		CullingParameter()
		{
			SizeX = 0.0f;
			SizeY = 0.0f;
			SizeZ = 0.0f;
			LayerCount = 0;
		}

	} cullingCurrent, cullingNext;

private:
	//! whether does rendering and update handle flipped automatically
	bool m_autoFlip = true;

	//! next handle
	Handle m_NextHandle = 0;

	// 確保済みインスタンス数
	int m_instance_max;

	// buffers which is allocated while initializing
	// 初期化中に確保されたバッファ
	CustomAlignedVector<InstanceChunk> reservedChunksBuffer_;
	CustomAlignedVector<uint8_t> reservedGroupBuffer_;
	CustomAlignedVector<uint8_t> reservedContainerBuffer_;

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

	// playing objects
	CustomAlignedMap<Handle, DrawSet> m_DrawSets;

	//! objects which are waiting to be disposed
	std::array<CustomAlignedMap<Handle, DrawSet>, 2> m_RemovingDrawSets;

	//! objects on rendering
	CustomAlignedVector<DrawSet> m_renderingDrawSets;

	//! objects on rendering
	CustomAlignedMap<Handle, DrawSet> m_renderingDrawSetMaps;

	// mutex for rendering
	std::mutex m_renderingMutex;
	bool m_isLockedWithRenderingMutex = false;

	Setting* m_setting;

	int m_updateTime;
	int m_drawTime;

	uint32_t m_sequenceNumber;

	Culling3D::World* m_cullingWorld;

	std::vector<DrawSet*> m_culledObjects;
	std::set<Handle> m_culledObjectSets;
	bool m_culled;

	SpriteRenderer* m_spriteRenderer;

	RibbonRenderer* m_ribbonRenderer;

	RingRenderer* m_ringRenderer;

	ModelRenderer* m_modelRenderer;

	TrackRenderer* m_trackRenderer;

	SoundPlayer* m_soundPlayer;

	MallocFunc m_MallocFunc;

	FreeFunc m_FreeFunc;

	RandFunc m_randFunc;

	int m_randMax;

	Handle AddDrawSet(Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer);

	void StopStoppingEffects();

	//! GC Draw sets
	void GCDrawSet(bool isRemovingManager);

	static void* EFK_STDCALL Malloc(unsigned int size);

	static void EFK_STDCALL Free(void* p, unsigned int size);

	static int EFK_STDCALL Rand();

	void ExecuteEvents();

public:
	ManagerImplemented(int instance_max, bool autoFlip);

	virtual ~ManagerImplemented();

	Instance* CreateInstance(EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup);

	InstanceGroup* CreateInstanceGroup(EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGlobal* pGlobal);
	void ReleaseGroup(InstanceGroup* group);

	InstanceContainer*
	CreateInstanceContainer(EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, const Mat43f& rootMatrix, Instance* pParent);
	void ReleaseInstanceContainer(InstanceContainer* container);

	void Destroy() override;

	uint32_t GetSequenceNumber() const;

	MallocFunc GetMallocFunc() const override;

	void SetMallocFunc(MallocFunc func) override;

	FreeFunc GetFreeFunc() const override;

	void SetFreeFunc(FreeFunc func) override;

	RandFunc GetRandFunc() const override;

	void SetRandFunc(RandFunc func) override;

	int GetRandMax() const override;

	void SetRandMax(int max_) override;

	CoordinateSystem GetCoordinateSystem() const override;

	void SetCoordinateSystem(CoordinateSystem coordinateSystem) override;

	SpriteRenderer* GetSpriteRenderer() override;

	void SetSpriteRenderer(SpriteRenderer* renderer) override;

	RibbonRenderer* GetRibbonRenderer() override;

	void SetRibbonRenderer(RibbonRenderer* renderer) override;

	RingRenderer* GetRingRenderer() override;

	void SetRingRenderer(RingRenderer* renderer) override;

	ModelRenderer* GetModelRenderer() override;

	void SetModelRenderer(ModelRenderer* renderer) override;

	TrackRenderer* GetTrackRenderer() override;

	void SetTrackRenderer(TrackRenderer* renderer) override;

	Setting* GetSetting() override;

	void SetSetting(Setting* setting) override;

	EffectLoader* GetEffectLoader() override;

	void SetEffectLoader(EffectLoader* effectLoader) override;

	TextureLoader* GetTextureLoader() override;

	void SetTextureLoader(TextureLoader* textureLoader) override;

	SoundPlayer* GetSoundPlayer() override;

	void SetSoundPlayer(SoundPlayer* soundPlayer) override;

	SoundLoader* GetSoundLoader() override;

	void SetSoundLoader(SoundLoader* soundLoader) override;

	ModelLoader* GetModelLoader() override;

	void SetModelLoader(ModelLoader* modelLoader) override;

	MaterialLoader* GetMaterialLoader() override;

	void SetMaterialLoader(MaterialLoader* loader) override;

	void StopEffect(Handle handle) override;

	void StopAllEffects() override;

	void StopRoot(Handle handle) override;

	void StopRoot(Effect* effect) override;

	bool Exists(Handle handle) override;

	int32_t GetInstanceCount(Handle handle) override;

	int32_t GetTotalInstanceCount() const override;

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

	Matrix43 GetBaseMatrix(Handle handle) override;

	void SetBaseMatrix(Handle handle, const Matrix43& mat) override;

	void SetRemovingCallback(Handle handle, EffectInstanceRemovingCallback callback) override;

	bool GetShown(Handle handle) override;

	void SetShown(Handle handle, bool shown) override;

	bool GetPaused(Handle handle) override;

	void SetPaused(Handle handle, bool paused) override;

	void SetPausedToAllEffects(bool paused) override;

	int GetLayer(Handle handle) override;

	void SetLayer(Handle handle, int32_t layer) override;

	float GetSpeed(Handle handle) const override;

	void SetSpeed(Handle handle, float speed) override;

	void SetAutoDrawing(Handle handle, bool autoDraw) override;

	void Flip() override;

	void Update(float deltaFrame) override;

	void BeginUpdate() override;

	void EndUpdate() override;

	void UpdateHandle(Handle handle, float deltaFrame = 1.0f) override;

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

	Handle Play(Effect* effect, float x, float y, float z) override;

	Handle Play(Effect* effect, const Vector3D& position, int32_t startFrame) override;

	int GetCameraCullingMaskToShowAllEffects() override;

	int GetUpdateTime() const override;

	int GetDrawTime() const override;

	int32_t GetRestInstancesCount() const override;

	void BeginReloadEffect(Effect* effect, bool doLockThread);

	void EndReloadEffect(Effect* effect, bool doLockThread);

	void CreateCullingWorld(float xsize, float ysize, float zsize, int32_t layerCount) override;

	void CalcCulling(const Matrix44& cameraProjMat, bool isOpenGL) override;

	void RessignCulling() override;

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
};

} // namespace Effekseer

#endif // __EFFEKSEER_MANAGER_IMPLEMENTED_H__
