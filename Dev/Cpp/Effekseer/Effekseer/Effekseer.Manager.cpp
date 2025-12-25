#include "Effekseer.Manager.h"
#include "Effekseer.CurveLoader.h"
#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceChunk.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.MaterialLoader.h"
#include "Effekseer.Resource.h"
#include "Effekseer.Setting.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Geometry/Effekseer.GeometryUtility.h"
#include "Model/Effekseer.ModelLoader.h"
#include "Renderer/Effekseer.GpuParticles.h"
#include "Renderer/Effekseer.GpuTimer.h"
#include "Renderer/Effekseer.ModelRenderer.h"
#include "Renderer/Effekseer.RibbonRenderer.h"
#include "Renderer/Effekseer.RingRenderer.h"
#include "Renderer/Effekseer.SpriteRenderer.h"
#include "Renderer/Effekseer.TrackRenderer.h"
#include "SIMD/Utils.h"
#include "Sound/Effekseer.SoundPlayer.h"
#include "Utils/Effekseer.Profiler.h"
#include <algorithm>
#include <iostream>

namespace Effekseer
{

static int64_t GetTime(void)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

//! TODO should be moved
static std::function<void(LogType, const std::string&)> g_logger;

void SetLogger(const std::function<void(LogType, const std::string&)>& logger)
{
	g_logger = logger;
}

void Log(LogType logType, const std::string& message)
{
	if (g_logger != nullptr)
	{
		g_logger(logType, message);
	}
}

Manager::DrawParameter::DrawParameter()
{
	CameraCullingMask = 1;
}

ManagerRef Manager::Create(int instance_max, bool autoFlip)
{
	return MakeRefPtr<ManagerImplemented>(instance_max, autoFlip);
}

void ManagerImplemented::DrawSet::UpdateLevelOfDetails(const LayerParameter& loadParameter)
{
	SIMD::Mat43f drawSetMatrix = this->GetGlobalMatrix();
	SIMD::Vec3f diff = SIMD::Vec3f(loadParameter.ViewerPosition) - drawSetMatrix.GetTranslation();
	float distanceToViewer = diff.GetLength() + loadParameter.DistanceBias;

	EffectImplemented* effect = (EffectImplemented*)this->ParameterPointer.Get();
	if (effect->LODs.distance3 > 0.0F && distanceToViewer > effect->LODs.distance3)
	{
		GlobalPointer->CurrentLevelOfDetails = 1 << 3;
	}
	else if (effect->LODs.distance2 > 0.0F && distanceToViewer > effect->LODs.distance2)
	{
		GlobalPointer->CurrentLevelOfDetails = 1 << 2;
	}
	else if (effect->LODs.distance1 > 0.0F && distanceToViewer > effect->LODs.distance1)
	{
		GlobalPointer->CurrentLevelOfDetails = 1 << 1;
	}
	else
	{
		GlobalPointer->CurrentLevelOfDetails = 1 << 0;
	}
}

SIMD::Mat43f ManagerImplemented::DrawSet::GetGlobalMatrix() const
{
	return GlobalMatrix;
}

void ManagerImplemented::DrawSet::SetGlobalMatrix(const SIMD::Mat43f& mat)
{
	GlobalMatrix = mat;
	IsParameterChanged = true;

	if (IsPreupdated)
	{
		InstanceContainer* pContainer = InstanceContainerPointer;
		if (pContainer == nullptr)
			return;

		auto firstGroup = pContainer->GetFirstGroup();
		if (firstGroup == nullptr)
			return;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();
		if (pInstance == nullptr)
			return;

		pInstance->UpdateGlobalMatrix(mat);
	}
}

Handle ManagerImplemented::AddDrawSet(const EffectRef& effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer)
{
	Handle Temp = nextHandle_;

	// avoid an overflow
	if (nextHandle_ > std::numeric_limits<int32_t>::max() - 1)
	{
		nextHandle_ = 0;
	}
	nextHandle_++;

	DrawSet drawset(effect, pInstanceContainer, pGlobalPointer);
	drawset.Self = Temp;

	drawSets_[Temp] = drawset;

	return Temp;
}

void ManagerImplemented::StopStoppingEffects()
{
	for (auto& draw_set_it : drawSets_)
	{
		DrawSet& draw_set = draw_set_it.second;
		if (draw_set.IsRemoving)
			continue;
		if (draw_set.GoingToStop)
			continue;

		bool isRemoving = false;

		// Empty
		if (!isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 0)
		{
			isRemoving = true;
		}

		// Root only exists and none plan to create new instances
		if (!isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 1)
		{
			InstanceContainer* pRootContainer = draw_set.InstanceContainerPointer;
			InstanceGroup* group = pRootContainer != nullptr ? pRootContainer->GetFirstGroup() : nullptr;

			if (group)
			{
				Instance* pRootInstance = group->GetFirst();

				if (pRootInstance && pRootInstance->IsActive())
				{
					if (pRootInstance->IsFirstTime() || pRootInstance->AreChildrenActive())
					{
						continue;
					}
				}
			}

			// when a sound is playing.
			if (soundPlayer_ && soundPlayer_->CheckPlayingTag(draw_set.GlobalPointer))
			{
				continue;
			}

			// when gpu particles are found
			if (auto gpuParticleSystem = GetGpuParticleSystem())
			{
				if (gpuParticleSystem->GetParticleCount(draw_set.GlobalPointer) > 0)
				{
					continue;
				}
			}

			isRemoving = true;
		}

		if (isRemoving)
		{
			StopEffect(draw_set_it.first);
		}
	}
}

void ManagerImplemented::GCDrawSet(bool isRemovingManager)
{
	// dispose instance groups
	{
		auto it = removingDrawSets_[1].begin();
		while (it != removingDrawSets_[1].end())
		{
			// HACK for UpdateHandle
			if (it->second.UpdateCountAfterRemoving < 2)
			{
				UpdateInstancesByInstanceGlobal(it->second);
				UpdateHandleInternal(it->second);
				it->second.UpdateCountAfterRemoving++;
			}

			DrawSet& drawset = (*it).second;

			// dispose all instances
			if (drawset.InstanceContainerPointer != nullptr)
			{
				drawset.InstanceContainerPointer->RemoveForcibly(true);
				ReleaseInstanceContainer(drawset.InstanceContainerPointer);
			}

			drawset.ParameterPointer = nullptr;

			if (gpuTimer_ != nullptr)
			{
				gpuTimer_->RemoveTimer(drawset.GlobalPointer);
			}

			ES_SAFE_DELETE(drawset.GlobalPointer);

			it = removingDrawSets_[1].erase(it);
		}
		removingDrawSets_[1].clear();
	}

	// wait next frame to be removed
	{
		auto it = removingDrawSets_[0].begin();
		while (it != removingDrawSets_[0].end())
		{
			// HACK for UpdateHandle
			if (it->second.UpdateCountAfterRemoving < 1)
			{
				UpdateInstancesByInstanceGlobal(it->second);
				UpdateHandleInternal(it->second);
				it->second.UpdateCountAfterRemoving++;
			}

			removingDrawSets_[1][(*it).first] = (*it).second;
			it = removingDrawSets_[0].erase(it);
		}
		removingDrawSets_[0].clear();
	}

	{
		auto it = drawSets_.begin();
		while (it != drawSets_.end())
		{
			DrawSet& draw_set = (*it).second;

			if (draw_set.IsRemoving)
			{
				if ((*it).second.RemovingCallback != nullptr)
				{
					(*it).second.RemovingCallback(this, (*it).first, isRemovingManager);
				}

				removingDrawSets_[0][(*it).first] = (*it).second;
				it = drawSets_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

InstanceContainer* ManagerImplemented::CreateInstanceContainer(
	EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, const SIMD::Mat43f& rootMatrix, Instance* pParent)
{
	auto memory = pooledInstanceContainers_.Pop();

	InstanceContainer* pContainer = new (memory) InstanceContainer(this, pEffectNode, pGlobal);

	for (int i = 0; i < pEffectNode->GetChildrenCount(); i++)
	{
		auto child = CreateInstanceContainer(pEffectNode->GetChild(i), pGlobal, false, Matrix43(), nullptr);
		if (child == nullptr)
		{
			ReleaseInstanceContainer(pContainer);
			return nullptr;
		}

		pContainer->AddChild(child);
	}

	if (isRoot)
	{
		auto group = pContainer->CreateInstanceGroup();
		if (group == nullptr)
		{
			ReleaseInstanceContainer(pContainer);
			return nullptr;
		}

		auto instance = group->CreateRootInstance();
		if (instance == nullptr)
		{
			group->IsReferencedFromInstance = false;
			pContainer->RemoveInvalidGroups();
			ReleaseInstanceContainer(pContainer);
			return nullptr;
		}

		pGlobal->SetRootContainer(pContainer);

		instance->Initialize(nullptr, 1, 0);
		instance->ResetGlobalMatrix(rootMatrix);

		// This group is not generated by an instance, so changed a flag
		group->IsReferencedFromInstance = false;
	}

	return pContainer;
}

void ManagerImplemented::ReleaseInstanceContainer(InstanceContainer* container)
{
	container->~InstanceContainer();
	pooledInstanceContainers_.Push(container);
}

int ManagerImplemented::Rand()
{
	return rand();
}

void ManagerImplemented::ExecuteEvents()
{
	for (auto& ds : drawSets_)
	{
		if (ds.second.GoingToStop)
		{
			InstanceContainer* pContainer = ds.second.InstanceContainerPointer;

			if (pContainer != nullptr)
			{
				pContainer->KillAllInstances(true);
			}

			ds.second.IsRemoving = true;
			if (GetSoundPlayer() != nullptr)
			{
				GetSoundPlayer()->StopTag(ds.second.GlobalPointer);
			}

			if (GetGpuParticleSystem() != nullptr)
			{
				GetGpuParticleSystem()->KillParticles(ds.second.GlobalPointer);
			}
		}

		if (ds.second.GoingToStopRoot && ds.second.AreChildrenOfRootGenerated)
		{
			InstanceContainer* pContainer = ds.second.InstanceContainerPointer;

			if (pContainer != nullptr)
			{
				pContainer->KillAllInstances(false);
			}
		}
	}
}

void ManagerImplemented::StoreSortingDrawSets(const Manager::DrawParameter& drawParameter)
{
	sortedRenderingDrawSets_.clear();

	for (const auto& ds : renderingDrawSets_)
	{
		sortedRenderingDrawSets_.emplace_back(ds);
	}

	if (drawParameter.IsSortingEffectsEnabled)
	{
		std::sort(sortedRenderingDrawSets_.begin(), sortedRenderingDrawSets_.end(), [&](const DrawSet& a, const DrawSet& b) -> bool
				  {
			const auto da = SIMD::Vec3f::Dot(a.GetGlobalMatrix().GetTranslation() - drawParameter.CameraPosition, drawParameter.CameraFrontDirection);
			const auto db = SIMD::Vec3f::Dot(b.GetGlobalMatrix().GetTranslation() - drawParameter.CameraPosition, drawParameter.CameraFrontDirection);
			return da > db; });
	}
}

bool ManagerImplemented::CanDraw(const DrawSet& drawSet, const Manager::DrawParameter& drawParameter, const std::array<Plane, 6>& planes)
{
	if (drawSet.InstanceContainerPointer == nullptr ||
		!drawSet.IsShown)
	{
		return false;
	}

	if ((drawParameter.CameraCullingMask & drawSet.GlobalPointer->GetLayerBits()) == 0)
	{
		return false;
	}

	if (drawParameter.ZNear != drawParameter.ZFar)
	{
		auto effect = (EffectImplemented*)drawSet.ParameterPointer.Get();

		if (effect->Culling.Shape == CullingShape::Sphere)
		{
			Sphare s;
			s.Center = drawSet.CullingPosition;
			s.Radius = drawSet.CullingRadius;
			if (!GeometryUtility::IsContain(planes, s))
			{
				return false;
			}
		}
	}

	return true;
}

ManagerImplemented::ManagerImplemented(int instance_max, bool autoFlip)
	: autoFlip_(autoFlip)
	, nextHandle_(0)
	, instanceMax_(instance_max)
	, setting_(nullptr)
	, sequenceNumber_(0)
	, spriteRenderer_(nullptr)
	, ribbonRenderer_(nullptr)
	, ringRenderer_(nullptr)
	, modelRenderer_(nullptr)
	, trackRenderer_(nullptr)

	, soundPlayer_(nullptr)
	, randFunc_(nullptr)
{
	setting_ = Setting::Create();

	SetRandFunc(Rand);

	renderingDrawSets_.reserve(64);

	int chunk_max = (instanceMax_ + InstanceChunk::InstancesOfChunk - 1) / InstanceChunk::InstancesOfChunk;

	for (auto& chunks : instanceChunks_)
	{
		chunks.reserve(chunk_max);
	}
	std::fill(creatableChunkOffsets_.begin(), creatableChunkOffsets_.end(), 0);

	pooledInstanceChunks_.Init(chunk_max, 16, false);
	pooledInstanceGroup_.Init(instance_max, 32, true);
	pooledInstanceContainers_.Init(instance_max, 32, true);

	setting_->SetEffectLoader(Effect::CreateEffectLoader());
	EffekseerPrintDebug("*** Create : Manager\n");
}

ManagerImplemented::~ManagerImplemented()
{
	if (workerThreads_.size() > 0)
	{
		workerThreads_[0].WaitForComplete();
	}

	StopAllEffects();

	ExecuteEvents();

	for (int i = 0; i < 5; i++)
	{
		GCDrawSet(true);
	}
}

Instance* ManagerImplemented::CreateInstance(EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup)
{
	int32_t generationNumber = pEffectNode->GetGeneration();
	assert(generationNumber < GenerationsMax);

	auto& chunks = instanceChunks_[generationNumber];

	int32_t offset = creatableChunkOffsets_[generationNumber];

	auto it = std::find_if(chunks.begin() + offset, chunks.end(), [](const InstanceChunk* chunk)
						   { return chunk->IsInstanceCreatable(); });

	creatableChunkOffsets_[generationNumber] = (int32_t)std::distance(chunks.begin(), it);

	if (it != chunks.end())
	{
		auto chunk = *it;
		return chunk->CreateInstance(this, pEffectNode, pContainer, pGroup);
	}

	{
		auto memory = pooledInstanceChunks_.Pop();
		if (memory != nullptr)
		{
			auto chunk = new (memory) InstanceChunk();
			chunks.push_back(chunk);
			return chunk->CreateInstance(this, pEffectNode, pContainer, pGroup);
		}
	}

	return nullptr;
}

InstanceGroup* ManagerImplemented::CreateInstanceGroup(EffectNodeImplemented* pEffectNode, InstanceContainer* pContainer, InstanceGlobal* pGlobal)
{
	auto memory = pooledInstanceGroup_.Pop();
	return new (memory) InstanceGroup(this, pEffectNode, pContainer, pGlobal);
}

void ManagerImplemented::ReleaseGroup(InstanceGroup* group)
{
	group->~InstanceGroup();
	pooledInstanceGroup_.Push(group);
}

void ManagerImplemented::LaunchWorkerThreads(uint32_t threadCount)
{
	workerThreads_.resize(threadCount);

	for (auto& worker : workerThreads_)
	{
		worker.Launch();
	}
}

ThreadNativeHandleType ManagerImplemented::GetWorkerThreadHandle(uint32_t threadID)
{
	if (threadID < workerThreads_.size())
	{
		return workerThreads_[threadID].GetThreadHandle();
	}
	return 0;
}

uint32_t ManagerImplemented::GetSequenceNumber() const
{
	return sequenceNumber_;
}

RandFunc ManagerImplemented::GetRandFunc() const
{
	return randFunc_;
}

void ManagerImplemented::SetRandFunc(RandFunc func)
{
	randFunc_ = func;
}

void ManagerImplemented::SetCollisionCallback(CollisionCallback callback)
{
	collisionCallback_ = callback;
}

CollisionCallback ManagerImplemented::GetCollisionCallback() const
{
	return collisionCallback_;
}

CoordinateSystem ManagerImplemented::GetCoordinateSystem() const
{
	return setting_->GetCoordinateSystem();
}

void ManagerImplemented::SetCoordinateSystem(CoordinateSystem coordinateSystem)
{
	setting_->SetCoordinateSystem(coordinateSystem);
}

SpriteRendererRef ManagerImplemented::GetSpriteRenderer()
{
	return spriteRenderer_;
}

void ManagerImplemented::SetSpriteRenderer(SpriteRendererRef renderer)
{
	spriteRenderer_ = renderer;
}

RibbonRendererRef ManagerImplemented::GetRibbonRenderer()
{
	return ribbonRenderer_;
}

void ManagerImplemented::SetRibbonRenderer(RibbonRendererRef renderer)
{
	ribbonRenderer_ = renderer;
}

RingRendererRef ManagerImplemented::GetRingRenderer()
{
	return ringRenderer_;
}

void ManagerImplemented::SetRingRenderer(RingRendererRef renderer)
{
	ringRenderer_ = renderer;
}

ModelRendererRef ManagerImplemented::GetModelRenderer()
{
	return modelRenderer_;
}

void ManagerImplemented::SetModelRenderer(ModelRendererRef renderer)
{
	modelRenderer_ = renderer;
}

TrackRendererRef ManagerImplemented::GetTrackRenderer()
{
	return trackRenderer_;
}

void ManagerImplemented::SetTrackRenderer(TrackRendererRef renderer)
{
	trackRenderer_ = renderer;
}

GpuTimerRef ManagerImplemented::GetGpuTimer()
{
	return gpuTimer_;
}

void ManagerImplemented::SetGpuTimer(GpuTimerRef gpuTimer)
{
	if (gpuTimer_ && gpuParticleSystem_)
	{
		gpuTimer_->RemoveTimer(gpuParticleSystem_.Get());
	}

	gpuTimer_ = gpuTimer;

	if (gpuTimer_ && gpuParticleSystem_)
	{
		gpuTimer_->AddTimer(gpuParticleSystem_.Get());
	}
}

GpuParticleSystemRef ManagerImplemented::GetGpuParticleSystem()
{
	return gpuParticleSystem_;
}

void ManagerImplemented::SetGpuParticleSystem(GpuParticleSystemRef system)
{
	if (gpuTimer_ && gpuParticleSystem_)
	{
		gpuTimer_->RemoveTimer(gpuParticleSystem_.Get());
	}

	gpuParticleSystem_ = system;

	if (gpuTimer_ && gpuParticleSystem_)
	{
		gpuTimer_->AddTimer(gpuParticleSystem_.Get());
	}
}

GpuParticleFactoryRef ManagerImplemented::GetGpuParticleFactory()
{
	return setting_->GetGpuParticleFactory();
}

void ManagerImplemented::SetGpuParticleFactory(GpuParticleFactoryRef factory)
{
	setting_->SetGpuParticleFactory(factory);
}

SoundPlayerRef ManagerImplemented::GetSoundPlayer()
{
	return soundPlayer_;
}

void ManagerImplemented::SetSoundPlayer(SoundPlayerRef soundPlayer)
{
	soundPlayer_ = soundPlayer;
}

const SettingRef& ManagerImplemented::GetSetting() const
{
	return setting_;
}

void ManagerImplemented::SetSetting(const SettingRef& setting)
{
	setting_ = setting;
}

EffectLoaderRef ManagerImplemented::GetEffectLoader()
{
	return setting_->GetEffectLoader();
}

void ManagerImplemented::SetEffectLoader(EffectLoaderRef effectLoader)
{
	setting_->SetEffectLoader(effectLoader);
}

TextureLoaderRef ManagerImplemented::GetTextureLoader()
{
	return setting_->GetTextureLoader();
}

void ManagerImplemented::SetTextureLoader(TextureLoaderRef textureLoader)
{
	setting_->SetTextureLoader(textureLoader);
}

SoundLoaderRef ManagerImplemented::GetSoundLoader()
{
	return setting_->GetSoundLoader();
}

void ManagerImplemented::SetSoundLoader(SoundLoaderRef soundLoader)
{
	setting_->SetSoundLoader(soundLoader);
}

ModelLoaderRef ManagerImplemented::GetModelLoader()
{
	return setting_->GetModelLoader();
}

void ManagerImplemented::SetModelLoader(ModelLoaderRef modelLoader)
{
	setting_->SetModelLoader(modelLoader);
}

MaterialLoaderRef ManagerImplemented::GetMaterialLoader()
{
	return setting_->GetMaterialLoader();
}

void ManagerImplemented::SetMaterialLoader(MaterialLoaderRef loader)
{
	setting_->SetMaterialLoader(loader);
}

CurveLoaderRef ManagerImplemented::GetCurveLoader()
{
	return setting_->GetCurveLoader();
}

void ManagerImplemented::SetCurveLoader(CurveLoaderRef loader)
{
	setting_->SetCurveLoader(loader);
}

void ManagerImplemented::StopEffect(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		drawSet.GoingToStop = true;
		drawSet.IsRemoving = true;
	}
}

void ManagerImplemented::StopAllEffects()
{
	for (auto& it : drawSets_)
	{
		it.second.GoingToStop = true;
		it.second.IsRemoving = true;
	}
}

void ManagerImplemented::StopRoot(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].GoingToStopRoot = true;
	}
}

void ManagerImplemented::StopRoot(const EffectRef& effect)
{
	for (auto& it : drawSets_)
	{
		if (it.second.ParameterPointer == effect)
		{
			it.second.GoingToStopRoot = true;
		}
	}
}

bool ManagerImplemented::Exists(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		// always exists before update
		if (!drawSets_[handle].IsPreupdated)
			return true;

		if (drawSets_[handle].IsRemoving)
			return false;
		return true;
	}
	return false;
}

EffectRef ManagerImplemented::GetEffect(Handle handle)
{
	auto it = drawSets_.find(handle);
	if (it != drawSets_.end())
	{
		return it->second.ParameterPointer;
	}

	return nullptr;
}

int32_t ManagerImplemented::GetInstanceCount(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		return drawSets_[handle].GlobalPointer->GetInstanceCount();
	}
	return 0;
}

int32_t ManagerImplemented::GetTotalInstanceCount() const
{
	int32_t instanceCount = 0;
	for (auto pair : drawSets_)
	{
		const DrawSet& drawSet = pair.second;
		instanceCount += drawSet.GlobalPointer->GetInstanceCount();
	}
	return instanceCount;
}

int32_t ManagerImplemented::GetCurrentLOD(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		return drawSet.GlobalPointer->CurrentLevelOfDetails;
	}

	return 0;
}

const Manager::LayerParameter& ManagerImplemented::GetLayerParameter(int32_t layer) const
{
	if (layer >= 0 && layer < LayerCount)
	{
		return layerParameters_[layer];
	}
	return layerParameters_[0];
}

void ManagerImplemented::SetLayerParameter(int32_t layer, const LayerParameter& layerParameter)
{
	if (layer >= 0 && layer < LayerCount)
	{
		layerParameters_[layer] = layerParameter;
	}
}

Matrix43 ManagerImplemented::GetMatrix(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		return ToStruct(drawSet.GetGlobalMatrix());
	}

	return Matrix43();
}

void ManagerImplemented::SetMatrix(Handle handle, const Matrix43& mat)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		Vector3D t;
		mat.GetSRT(drawSet.Scaling, drawSet.Rotation, t);
		drawSet.SetGlobalMatrix(mat);
	}
}

Vector3D ManagerImplemented::GetLocation(Handle handle)
{
	Vector3D location;

	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		auto mat = drawSet.GetGlobalMatrix();

		location.X = mat.X.GetW();
		location.Y = mat.Y.GetW();
		location.Z = mat.Z.GetW();
	}

	return location;
}

void ManagerImplemented::SetLocation(Handle handle, float x, float y, float z)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		auto mat = drawSet.GetGlobalMatrix();

		mat.X.SetW(x);
		mat.Y.SetW(y);
		mat.Z.SetW(z);

		drawSet.SetGlobalMatrix(mat);
	}
}

void ManagerImplemented::SetLocation(Handle handle, const Vector3D& location)
{
	SetLocation(handle, location.X, location.Y, location.Z);
}

void ManagerImplemented::AddLocation(Handle handle, const Vector3D& location)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];
		auto mat = drawSet.GetGlobalMatrix();
		mat.X.SetW(mat.X.GetW() + location.X);
		mat.Y.SetW(mat.Y.GetW() + location.Y);
		mat.Z.SetW(mat.Z.GetW() + location.Z);
		drawSet.SetGlobalMatrix(mat);
	}
}

void ManagerImplemented::SetRotation(Handle handle, float x, float y, float z)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		auto mat = drawSet.GetGlobalMatrix();

		const auto t = mat.GetTranslation();

		drawSet.Rotation.RotationZXY(z, x, y);

		drawSet.SetGlobalMatrix(SIMD::Mat43f::SRT(drawSet.Scaling, drawSet.Rotation, t));
	}
}

void ManagerImplemented::SetRotation(Handle handle, const Vector3D& axis, float angle)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		auto mat = drawSet.GetGlobalMatrix();
		const auto t = mat.GetTranslation();

		drawSet.Rotation.RotationAxis(axis, angle);
		drawSet.SetGlobalMatrix(SIMD::Mat43f::SRT(drawSet.Scaling, drawSet.Rotation, t));
	}
}

void ManagerImplemented::SetScale(Handle handle, float x, float y, float z)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		auto mat = drawSet.GetGlobalMatrix();
		const auto t = mat.GetTranslation();

		drawSet.Scaling = {x, y, z};
		drawSet.SetGlobalMatrix(SIMD::Mat43f::SRT(drawSet.Scaling, drawSet.Rotation, t));
	}
}

void ManagerImplemented::SetAllColor(Handle handle, Color color)
{
	if (drawSets_.count(handle) > 0)
	{
		auto& drawSet = drawSets_[handle];

		drawSet.GlobalPointer->IsGlobalColorSet = true;
		drawSet.GlobalPointer->GlobalColor = color;
	}
}

void ManagerImplemented::SetTargetLocation(Handle handle, float x, float y, float z)
{
	SetTargetLocation(handle, Vector3D(x, y, z));
}

void ManagerImplemented::SetTargetLocation(Handle handle, const Vector3D& location)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		InstanceGlobal* instanceGlobal = drawSet.GlobalPointer;
		instanceGlobal->SetTargetLocation(location);

		drawSet.IsParameterChanged = true;
	}
}

float ManagerImplemented::GetDynamicInput(Handle handle, int32_t index)
{
	auto it = drawSets_.find(handle);
	if (it != drawSets_.end())
	{
		auto globalPtr = it->second.GlobalPointer;
		if (index < 0 || globalPtr->dynamicInputParameters_.size() <= index)
			return 0.0f;

		return globalPtr->dynamicInputParameters_[index];
	}

	return 0.0f;
}

void ManagerImplemented::SetDynamicInput(Handle handle, int32_t index, float value)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		InstanceGlobal* instanceGlobal = drawSet.GlobalPointer;

		if (index < 0 || (int32_t)instanceGlobal->dynamicInputParameters_.size() <= index)
			return;

		instanceGlobal->dynamicInputParameters_[index] = value;

		drawSet.IsParameterChanged = true;
	}
}

void ManagerImplemented::SendTrigger(Handle handle, int32_t index)
{
	if (drawSets_.count(handle) > 0)
	{
		DrawSet& drawSet = drawSets_[handle];

		drawSet.GlobalPointer->AddInputTriggerCount(index);
	}
}

Matrix43 ManagerImplemented::GetBaseMatrix(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		return ToStruct(drawSets_[handle].BaseMatrix);
	}

	return Matrix43();
}

void ManagerImplemented::SetBaseMatrix(Handle handle, const Matrix43& mat)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].BaseMatrix = mat;
		drawSets_[handle].DoUseBaseMatrix = true;
		drawSets_[handle].IsParameterChanged = true;
	}
}

void ManagerImplemented::SetRemovingCallback(Handle handle, EffectInstanceRemovingCallback callback)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].RemovingCallback = callback;
	}
}

bool ManagerImplemented::GetShown(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		return drawSets_[handle].IsShown;
	}

	return false;
}

void ManagerImplemented::SetShown(Handle handle, bool shown)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].IsShown = shown;
	}
}

bool ManagerImplemented::GetPaused(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		return drawSets_[handle].IsPaused;
	}

	return false;
}

void ManagerImplemented::SetPaused(Handle handle, bool paused)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].IsPaused = paused;
	}
}

void ManagerImplemented::SetSpawnDisabled(Handle handle, bool spawnDisabled)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].GlobalPointer->IsSpawnDisabled = spawnDisabled;
	}
}

bool ManagerImplemented::GetSpawnDisabled(Handle handle)
{
	if (drawSets_.count(handle) > 0)
	{
		return drawSets_[handle].GlobalPointer->IsSpawnDisabled;
	}
	return false;
}

void ManagerImplemented::SetPausedToAllEffects(bool paused)
{
	std::map<Handle, DrawSet>::iterator it = drawSets_.begin();
	while (it != drawSets_.end())
	{
		(*it).second.IsPaused = paused;
		++it;
	}
}

int32_t ManagerImplemented::GetLayer(Handle handle)
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		return it->second.GlobalPointer->GetLayer();
	}

	return 0;
}

void ManagerImplemented::SetLayer(Handle handle, int32_t layer)
{
	if (layer >= 0 && layer < LayerCount)
	{
		auto it = drawSets_.find(handle);

		if (it != drawSets_.end())
		{
			return it->second.GlobalPointer->SetLayer(layer);
		}
	}
}

int64_t ManagerImplemented::GetGroupMask(Handle handle) const
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		return it->second.GroupMask;
	}

	return 0;
}

void ManagerImplemented::SetGroupMask(Handle handle, int64_t groupmask)
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		it->second.GroupMask = groupmask;
	}
}

float ManagerImplemented::GetSpeed(Handle handle) const
{
	auto it = drawSets_.find(handle);
	if (it == drawSets_.end())
		return 0.0f;
	return it->second.Speed;
}

void ManagerImplemented::SetSpeed(Handle handle, float speed)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].Speed = speed;
		drawSets_[handle].IsParameterChanged = true;
	}
}

void ManagerImplemented::SetRandomSeed(Handle handle, int32_t seed)
{
	if (drawSets_.count(handle) > 0)
	{
		auto& drawSet = drawSets_[handle];
		auto pGlobal = drawSet.GlobalPointer;
		pGlobal->GetRandObject().SetSeed(seed);
	}
}

void ManagerImplemented::SetTimeScaleByGroup(int64_t groupmask, float timeScale)
{
	for (auto& it : drawSets_)
	{
		if ((it.second.GroupMask & groupmask) != 0)
		{
			it.second.TimeScale = timeScale;
		}
	}
}

void ManagerImplemented::SetTimeScaleByHandle(Handle handle, float timeScale)
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		it->second.TimeScale = timeScale;
	}
}

void ManagerImplemented::SetAutoDrawing(Handle handle, bool autoDraw)
{
	if (drawSets_.count(handle) > 0)
	{
		drawSets_[handle].IsAutoDrawing = autoDraw;
	}
}

void* ManagerImplemented::GetUserData(Handle handle)
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		return it->second.GlobalPointer->GetUserData();
	}

	return nullptr;
}

void ManagerImplemented::SetUserData(Handle handle, void* userData)
{
	auto it = drawSets_.find(handle);

	if (it != drawSets_.end())
	{
		it->second.GlobalPointer->SetUserData(userData);
	}
}

void ManagerImplemented::Flip()
{
	PROFILER_BLOCK("Manager::Flip", profiler::colors::Red);

	if (!autoFlip_)
	{
		renderingMutex_.lock();
	}

	// execute preupdate
	for (auto& drawSet : drawSets_)
	{
		Preupdate(drawSet.second);
	}

	StopStoppingEffects();

	ExecuteEvents();

	GCDrawSet(false);

	renderingDrawSets_.clear();
	renderingDrawSetMaps_.clear();

	{
		for (auto& it : drawSets_)
		{
			DrawSet& ds = it.second;
			EffectImplemented* effect = (EffectImplemented*)ds.ParameterPointer.Get();

			if (ds.InstanceContainerPointer == nullptr)
			{
				continue;
			}

			if (ds.IsParameterChanged)
			{
				Vector3D location;

				auto mat = ds.GetGlobalMatrix();

				location.X = mat.X.GetW();
				location.Y = mat.Y.GetW();
				location.Z = mat.Z.GetW();

				if (effect->Culling.Shape == CullingShape::Sphere)
				{
					float radius = effect->Culling.Sphere.Radius;
					SIMD::Vec3f culling_pos = {0, 0, 0};

					{
						SIMD::Vec3f s = mat.GetScale();
						radius *= s.GetLength();
						culling_pos = SIMD::Vec3f::Transform(SIMD::Vec3f(effect->Culling.Location), mat);
					}

					if (ds.DoUseBaseMatrix)
					{
						SIMD::Vec3f s = ds.BaseMatrix.GetScale();
						radius *= s.GetLength();
						culling_pos = SIMD::Vec3f::Transform(culling_pos, ds.BaseMatrix);
					}

					ds.CullingPosition = SIMD::ToStruct(culling_pos);
					ds.CullingRadius = radius;
				}

				ds.GlobalPointer->EffectGlobalMatrix = mat;
				if (ds.DoUseBaseMatrix)
				{
					ds.GlobalPointer->EffectGlobalMatrix *= ds.BaseMatrix;
				}
				// would be nice to have Invert function for SIMD matrix
				Matrix44 inverted;
				Matrix44::Inverse(inverted, ToStruct(SIMD::Mat44f(mat)));
				ds.GlobalPointer->InvertedEffectGlobalMatrix = inverted;

				ds.IsParameterChanged = false;
			}

			renderingDrawSets_.push_back(ds);
			renderingDrawSetMaps_[it.first] = it.second;
		}
	}

	if (!autoFlip_)
	{
		renderingMutex_.unlock();
	}
}

void ManagerImplemented::Update(float deltaFrame)
{
	UpdateParameter parameter;
	parameter.DeltaFrame = deltaFrame;
	parameter.UpdateInterval = 0.0f;
	Update(parameter);
}

void ManagerImplemented::Update(const UpdateParameter& parameter)
{
	PROFILER_BLOCK("Manager::Update", profiler::colors::Red);

	// start to measure time
	int64_t beginTime = ::Effekseer::GetTime();

	// Hack for GC
	for (size_t i = 0; i < removingDrawSets_.size(); i++)
	{
		for (auto& ds : removingDrawSets_[i])
		{
			ds.second.UpdateCountAfterRemoving++;
		}
	}

	// add frames
	float maximumDeltaFrame = 0;

	for (auto& drawSet : drawSets_)
	{
		float df = drawSet.second.IsPaused ? 0 : parameter.DeltaFrame * drawSet.second.Speed * drawSet.second.TimeScale;
		drawSet.second.NextUpdateFrame += df;

		maximumDeltaFrame = std::max(maximumDeltaFrame, drawSet.second.NextUpdateFrame);
	}

	int times = 0;

	if (parameter.UpdateInterval != 0)
	{
		times = static_cast<int>(maximumDeltaFrame / parameter.UpdateInterval);
	}

	// Update once at least
	if (times == 0)
	{
		times = 1;
	}

	nextComputeCount_ = times;

	BeginUpdate();

	if (workerThreads_.size() == 0)
	{
		DoUpdate(parameter, times);
	}
	else
	{
		workerThreads_[0].WaitForComplete();
		// Process on worker thread
		workerThreads_[0].RunAsync([this, parameter, times]()
								   { DoUpdate(parameter, times); });

		if (parameter.SyncUpdate)
		{
			workerThreads_[0].WaitForComplete();
		}
	}

	// end to measure time
	updateTime_ = (int)(Effekseer::GetTime() - beginTime);

	EndUpdate();

	ExecuteSounds();

	if (gpuTimer_)
	{
		gpuTimer_->UpdateResults();
	}
}

void ManagerImplemented::DoUpdate(const UpdateParameter& parameter, int times)
{
	PROFILER_BLOCK("Manager::DoUpdate", profiler::colors::Red);

	for (int32_t t = 0; t < times; t++)
	{
		// specify delta frames
		for (auto& drawSet : drawSets_)
		{
			if (drawSet.second.NextUpdateFrame >= parameter.UpdateInterval)
			{
				float idf = 0;

				if (parameter.UpdateInterval > 0)
				{
					idf = parameter.UpdateInterval;
				}
				else
				{
					idf = drawSet.second.NextUpdateFrame;
				}

				drawSet.second.NextUpdateFrame -= idf;
				drawSet.second.GlobalPointer->BeginDeltaFrame(idf);
			}
			else
			{
				drawSet.second.GlobalPointer->BeginDeltaFrame(0);
			}
		}

		for (auto& chunks : instanceChunks_)
		{
			// wakeup threads and wait to complete threads are hevery, so multithread the updates if you have a large number of instances.
			const size_t multithreadingChunkThreshold = 4;

			if (workerThreads_.size() >= 2 && chunks.size() >= multithreadingChunkThreshold)
			{
				const uint32_t chunkStep = (uint32_t)workerThreads_.size();

				for (uint32_t threadID = 1; threadID < (uint32_t)workerThreads_.size(); threadID++)
				{
					const uint32_t chunkOffset = threadID;
					// Process on worker thread
					PROFILER_BLOCK("DoUpdate::RunAsyncGroup", profiler::colors::Red100);
					workerThreads_[threadID].RunAsync([this, &chunks, chunkOffset, chunkStep]()
													  {
						PROFILER_BLOCK("DoUpdate::RunAsync", profiler::colors::Red200);
						for (size_t i = chunkOffset; i < chunks.size(); i += chunkStep)
						{
							chunks[i]->UpdateInstances();
						} });
				}

				// Process on this thread
				{
					PROFILER_BLOCK("DoUpdate::RunAsync(Main)", profiler::colors::Red300);
					for (size_t i = 0; i < chunks.size(); i += chunkStep)
					{
						chunks[i]->UpdateInstances();
					}
				}

				// Wait for all worker threads completion
				for (uint32_t threadID = 1; threadID < (uint32_t)workerThreads_.size(); threadID++)
				{
					PROFILER_BLOCK("DoUpdate::WaitForComplete", profiler::colors::Red400);
					workerThreads_[threadID].WaitForComplete();
				}
			}
			else
			{
				PROFILER_BLOCK("DoUpdate::RunAsync(Single)", profiler::colors::Red300);
				for (auto chunk : chunks)
				{
					chunk->UpdateInstances();
				}
			}

			{
				PROFILER_BLOCK("DoUpdate::GenerateChildrenInRequired", profiler::colors::Red500);
				for (auto chunk : chunks)
				{
					chunk->GenerateChildrenInRequired();
				}
			}
		}

		{
			PROFILER_BLOCK("DoUpdate::UpdateHandleInternal", profiler::colors::Red600);
			for (auto& drawSet : drawSets_)
			{
				UpdateHandleInternal(drawSet.second);
			}
		}

		for (auto& drawSet : drawSets_)
		{
			drawSet.second.AreChildrenOfRootGenerated = true;
		}
	}
}

void ManagerImplemented::BeginUpdate()
{
	renderingMutex_.lock();
	isLockedWithRenderingMutex_ = true;

	if (autoFlip_)
	{
		Flip();
	}

	sequenceNumber_++;
}

void ManagerImplemented::EndUpdate()
{
	for (auto& chunks : instanceChunks_)
	{
		auto first = chunks.begin();
		auto last = chunks.end();
		while (first != last)
		{
			auto it = std::find_if(first, last, [](const InstanceChunk* chunk)
								   { return chunk->GetAliveCount() == 0; });
			if (it != last)
			{
				(*it)->~InstanceChunk();
				pooledInstanceChunks_.Push(*it);
				if (it != last - 1)
					*it = *(last - 1);
				last--;
			}
			first = it;
		}
		chunks.erase(last, chunks.end());
	}
	std::fill(creatableChunkOffsets_.begin(), creatableChunkOffsets_.end(), 0);

	renderingMutex_.unlock();
	isLockedWithRenderingMutex_ = false;
}

void ManagerImplemented::UpdateHandle(Handle handle, float deltaFrame)
{
	{
		auto it = drawSets_.find(handle);
		if (it != drawSets_.end())
		{
			DrawSet& drawSet = it->second;

			{
				float df = drawSet.IsPaused ? 0 : deltaFrame * drawSet.Speed * drawSet.TimeScale;
				drawSet.NextUpdateFrame += df;

				drawSet.GlobalPointer->BeginDeltaFrame(drawSet.NextUpdateFrame);
				drawSet.NextUpdateFrame = 0.0f;
			}

			UpdateInstancesByInstanceGlobal(drawSet);

			UpdateHandleInternal(drawSet);

			drawSet.AreChildrenOfRootGenerated = true;
		}
	}

	ExecuteSounds();
}

void ManagerImplemented::UpdateHandleToMoveToFrame(Handle handle, float frame)
{
	auto it = drawSets_.find(handle);
	if (it == drawSets_.end())
	{
		return;
	}

	DrawSet& drawSet = it->second;

	if (frame < drawSet.GlobalPointer->GetUpdatedFrame())
	{
		StopWithoutRemoveDrawSet(drawSet);
		ResetAndPlayWithDataSet(drawSet, frame);
	}
	else
	{
		auto updatedFrame = drawSet.GlobalPointer->GetUpdatedFrame();
		for (int i = 0; i < frame - updatedFrame; i++)
		{
			UpdateHandle(handle, 1);
		}
	}
}

void ManagerImplemented::UpdateInstancesByInstanceGlobal(const DrawSet& drawSet)
{
	for (auto& chunks : instanceChunks_)
	{
		for (auto chunk : chunks)
		{
			chunk->UpdateInstancesByInstanceGlobal(drawSet.GlobalPointer);
		}

		for (auto chunk : chunks)
		{
			chunk->GenerateChildrenInRequiredByInstanceGlobal(drawSet.GlobalPointer);
		}
	}
}

void ManagerImplemented::UpdateHandleInternal(DrawSet& drawSet)
{
	// evaluate LOD
	drawSet.UpdateLevelOfDetails(layerParameters_[drawSet.GlobalPointer->GetLayer()]);

	// calculate dynamic parameters
	auto e = static_cast<EffectImplemented*>(drawSet.ParameterPointer.Get());
	assert(e != nullptr);
	assert(drawSet.GlobalPointer->dynamicEqResults.size() >= e->dynamicEquation.size());

	std::array<float, 1> globals;
	globals[0] = drawSet.GlobalPointer->GetUpdatedFrame() / 60.0f;

	for (size_t i = 0; i < e->dynamicEquation.size(); i++)
	{
		if (e->dynamicEquation[i].GetRunningPhase() != InternalScript::RunningPhaseType::Global)
			continue;

		drawSet.GlobalPointer->dynamicEqResults[i] = e->dynamicEquation[i].Execute(drawSet.GlobalPointer->dynamicInputParameters_,
																				   globals,
																				   std::array<float, 5>(),
																				   RandCallback::Rand,
																				   RandCallback::RandSeed,
																				   drawSet.GlobalPointer);
	}

	Preupdate(drawSet);

	if (drawSet.InstanceContainerPointer != nullptr)
	{
		drawSet.InstanceContainerPointer->Update(true, drawSet.IsShown);

		if (drawSet.DoUseBaseMatrix)
		{
			drawSet.InstanceContainerPointer->ApplyBaseMatrix(true, drawSet.BaseMatrix);
		}
	}

	if (drawSet.GlobalPointer->IsUsingGpuParticles)
	{
		if (auto gpuParticleSystem = GetGpuParticleSystem())
		{
			gpuParticleSystem->SetDeltaTime(drawSet.GlobalPointer, drawSet.GlobalPointer->GetNextDeltaFrame());
		}
	}

	drawSet.GlobalPointer->EndDeltaFrame();
}

void ManagerImplemented::Preupdate(DrawSet& drawSet)
{
	if (drawSet.IsPreupdated)
		return;

	// Create an instance through a container
	auto pContainer =
		CreateInstanceContainer(drawSet.ParameterPointer->GetRoot(), drawSet.GlobalPointer, true, drawSet.GetGlobalMatrix(), nullptr);

	drawSet.InstanceContainerPointer = pContainer;
	drawSet.IsPreupdated = true;

	if (drawSet.InstanceContainerPointer == nullptr)
	{
		drawSet.IsRemoving = true;
		return;
	}

	drawSet.InstanceContainerPointer->GetFirstGroup()->GetFirst()->FirstUpdate();

	for (int32_t frame = 0; frame < drawSet.StartFrame; frame++)
	{
		drawSet.GlobalPointer->BeginDeltaFrame(1.0f);

		UpdateInstancesByInstanceGlobal(drawSet);

		UpdateHandleInternal(drawSet);
	}
}

bool ManagerImplemented::IsClippedWithDepth(DrawSet& drawSet, InstanceContainer* container, const Manager::DrawParameter& drawParameter)
{
	// don't use this parameter
	if (container->effectNode_->DepthValues.DepthParameter.DepthClipping > std::numeric_limits<float>::max() / 10)
		return false;

	SIMD::Vec3f pos = drawSet.GetGlobalMatrix().GetTranslation();
	auto distance = SIMD::Vec3f::Dot(pos - SIMD::Vec3f(drawParameter.CameraPosition), SIMD::Vec3f(drawParameter.CameraFrontDirection));
	if (container->effectNode_->DepthValues.DepthParameter.DepthClipping < distance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ManagerImplemented::StopWithoutRemoveDrawSet(DrawSet& drawSet)
{
	drawSet.InstanceContainerPointer->KillAllInstances(true);
	drawSet.InstanceContainerPointer->RemoveForcibly(true);
	ReleaseInstanceContainer(drawSet.InstanceContainerPointer);
	drawSet.InstanceContainerPointer = nullptr;
}

void ManagerImplemented::ResetAndPlayWithDataSet(DrawSet& drawSet, float frame)
{
	assert(drawSet.ParameterPointer != nullptr);
	auto effect = drawSet.ParameterPointer->GetRoot();
	assert(effect != nullptr);

	auto pGlobal = drawSet.GlobalPointer;
	auto e = static_cast<EffectImplemented*>(drawSet.ParameterPointer.Get());

	// reallocate
	pGlobal->GetRandObject().SetSeed(drawSet.RandomSeed);

	pGlobal->RenderedInstanceContainers.resize(e->renderingNodesCount);
	for (size_t i = 0; i < pGlobal->RenderedInstanceContainers.size(); i++)
	{
		pGlobal->RenderedInstanceContainers[i] = nullptr;
	}

	drawSet.IsPreupdated = false;
	drawSet.IsParameterChanged = true;
	drawSet.StartFrame = 0;
	drawSet.GoingToStop = false;
	drawSet.GoingToStopRoot = false;
	drawSet.IsRemoving = false;
	pGlobal->ResetUpdatedFrame();

	// Create an instance through a container
	// drawSet.InstanceContainerPointer = CreateInstanceContainer(e->GetRoot(), drawSet.GlobalPointer, true, drawSet.GlobalMatrix, nullptr);

	auto isShown = drawSet.IsShown;
	drawSet.IsShown = false;

	// skip
	Preupdate(drawSet);

	for (float f = 0; f < frame - 1; f += 1.0f)
	{
		drawSet.GlobalPointer->BeginDeltaFrame(1.0f);

		UpdateInstancesByInstanceGlobal(drawSet);
		UpdateHandleInternal(drawSet);
		// drawSet.InstanceContainerPointer->Update(true, false);
		drawSet.GlobalPointer->EndDeltaFrame();
	}

	drawSet.IsShown = isShown;

	drawSet.GlobalPointer->BeginDeltaFrame(1.0f);

	UpdateInstancesByInstanceGlobal(drawSet);
	UpdateHandleInternal(drawSet);

	// drawSet.InstanceContainerPointer->Update(true, drawSet.IsShown);
	drawSet.GlobalPointer->EndDeltaFrame();
}

void ManagerImplemented::Compute()
{
	ScopedGpuStage gpuPass(gpuTimer_, GpuStage::Compute);

	if (auto gpuParticleSystem = GetGpuParticleSystem())
	{
		ScopedGpuTime gpuTime(gpuTimer_, gpuParticleSystem.Get());

		GpuParticleSystem::Context context{};
		context.CoordinateReversed = GetCoordinateSystem() != CoordinateSystem::RH;

		for (int i = 0; i < nextComputeCount_; i++)
		{
			gpuParticleSystem->ComputeFrame(context);
		}
	}
}

void ManagerImplemented::Draw(const Manager::DrawParameter& drawParameter)
{
	PROFILER_BLOCK("Manager::Draw", profiler::colors::Blue);

	if (workerThreads_.size() > 0)
	{
		workerThreads_[0].WaitForComplete();
	}

	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	// start to record a time
	int64_t beginTime = ::Effekseer::GetTime();

	ScopedGpuStage gpuPass(gpuTimer_, GpuStage::Draw);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	const auto render = [this, &drawParameter, &cullingPlanes](DrawSet& drawSet) -> void
	{
		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		if (drawSet.IsAutoDrawing)
		{
			ScopedGpuTime gpuTime(gpuTimer_, drawSet.GlobalPointer);

			if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
			{
				for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
				{
					if (IsClippedWithDepth(drawSet, c, drawParameter))
						continue;

					c->Draw(false);
				}
			}
			else
			{
				drawSet.InstanceContainerPointer->Draw(true);
			}
		}
	};

	if (drawParameter.IsSortingEffectsEnabled)
	{
		StoreSortingDrawSets(drawParameter);

		for (auto& drawSet : sortedRenderingDrawSets_)
		{
			render(drawSet);
		}
	}
	else
	{
		for (size_t i = 0; i < renderingDrawSets_.size(); i++)
		{
			render(renderingDrawSets_[i]);
		}
	}

	if (auto gpuParticleSystem = GetGpuParticleSystem())
	{
		ScopedGpuTime gpuTime(gpuTimer_, gpuParticleSystem.Get());

		GpuParticleSystem::Context context{};
		context.CoordinateReversed = GetCoordinateSystem() != CoordinateSystem::RH;
		gpuParticleSystem->RenderFrame(context);
	}

	// calculate a time
	drawTime_ = (int)(Effekseer::GetTime() - beginTime);
}

void ManagerImplemented::DrawBack(const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	// start to record a time
	int64_t beginTime = ::Effekseer::GetTime();

	ScopedGpuStage gpuPass(gpuTimer_, GpuStage::DrawBack);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	const auto render = [this, &drawParameter, &cullingPlanes](DrawSet& drawSet) -> void
	{
		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		if (drawSet.IsAutoDrawing)
		{
			ScopedGpuTime gpuTime(gpuTimer_, drawSet.GlobalPointer);

			auto e = (EffectImplemented*)drawSet.ParameterPointer.Get();
			for (int32_t j = 0; j < e->renderingNodesThreshold; j++)
			{
				if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
					continue;

				drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
			}
		}
	};

	if (drawParameter.IsSortingEffectsEnabled)
	{
		StoreSortingDrawSets(drawParameter);

		for (auto& drawSet : sortedRenderingDrawSets_)
		{
			render(drawSet);
		}
	}
	else
	{
		for (size_t i = 0; i < renderingDrawSets_.size(); i++)
		{
			render(renderingDrawSets_[i]);
		}
	}

	// calculate a time
	drawTime_ = (int)(Effekseer::GetTime() - beginTime);
}

void ManagerImplemented::DrawFront(const Manager::DrawParameter& drawParameter)
{
	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	// start to record a time
	int64_t beginTime = ::Effekseer::GetTime();

	ScopedGpuStage gpuPass(gpuTimer_, GpuStage::DrawFront);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	const auto render = [this, &drawParameter, &cullingPlanes](DrawSet& drawSet) -> void
	{
		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		if (drawSet.IsAutoDrawing)
		{
			ScopedGpuTime gpuTime(gpuTimer_, drawSet.GlobalPointer);

			if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
			{
				auto e = (EffectImplemented*)drawSet.ParameterPointer.Get();
				for (size_t j = e->renderingNodesThreshold; j < drawSet.GlobalPointer->RenderedInstanceContainers.size(); j++)
				{
					if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[j], drawParameter))
						continue;

					drawSet.GlobalPointer->RenderedInstanceContainers[j]->Draw(false);
				}
			}
			else
			{
				drawSet.InstanceContainerPointer->Draw(true);
			}
		}
	};

	if (drawParameter.IsSortingEffectsEnabled)
	{
		StoreSortingDrawSets(drawParameter);

		for (auto& drawSet : sortedRenderingDrawSets_)
		{
			render(drawSet);
		}
	}
	else
	{
		for (size_t i = 0; i < renderingDrawSets_.size(); i++)
		{
			render(renderingDrawSets_[i]);
		}
	}

	if (auto gpuParticleSystem = GetGpuParticleSystem())
	{
		ScopedGpuTime gpuTime(gpuTimer_, gpuParticleSystem.Get());

		GpuParticleSystem::Context context{};
		context.CoordinateReversed = GetCoordinateSystem() != CoordinateSystem::RH;
		gpuParticleSystem->RenderFrame(context);
	}

	// calculate a time
	drawTime_ = (int)(Effekseer::GetTime() - beginTime);
}

Handle ManagerImplemented::Play(const EffectRef& effect, float x, float y, float z)
{
	return Play(effect, Vector3D(x, y, z), 0);
}

Handle ManagerImplemented::Play(const EffectRef& effect, const Vector3D& position, int32_t startFrame)
{
	if (effect == nullptr)
		return -1;

	auto e = effect->GetImplemented();

	// Create root
	InstanceGlobal* pGlobal = new InstanceGlobal();
	if (gpuTimer_ != nullptr)
	{
		gpuTimer_->AddTimer(pGlobal);
	}

	int32_t randomSeed = 0;
	if (e->m_defaultRandomSeed >= 0)
	{
		randomSeed = e->m_defaultRandomSeed;
	}
	else
	{
		randomSeed = GetRandFunc()();
	}

	pGlobal->GetRandObject().SetSeed(randomSeed);

	pGlobal->dynamicInputParameters_ = e->defaultDynamicInputs;

	pGlobal->RenderedInstanceContainers.resize(e->renderingNodesCount);
	for (size_t i = 0; i < pGlobal->RenderedInstanceContainers.size(); i++)
	{
		pGlobal->RenderedInstanceContainers[i] = nullptr;
	}

	// create a dateSet without an instance
	// an instance is created in Preupdate because effects need to show instances without update(0 frame)
	Handle handle = AddDrawSet(effect, nullptr, pGlobal);

	auto& drawSet = drawSets_[handle];

	drawSet.SetGlobalMatrix(SIMD::Mat43f::Translation(position));
	drawSet.StartFrame = startFrame;
	drawSet.RandomSeed = randomSeed;

	return handle;
}

Handle ManagerImplemented::Play(const Manager::PlayParameter& parameter)
{
	if (parameter.Effect == nullptr)
	{
		return -1;
	}

	auto handle = Play(parameter.Effect, parameter.Position, parameter.StartFrame);
	if (handle < 0)
	{
		return handle;
	}

	auto it = drawSets_.find(handle);
	if (it == drawSets_.end())
	{
		return handle;
	}

	auto& drawSet = it->second;
	drawSet.Scaling = parameter.Scale;
	drawSet.Rotation.RotationZXY(parameter.Rotation.Z, parameter.Rotation.X, parameter.Rotation.Y);
	drawSet.SetGlobalMatrix(SIMD::Mat43f::SRT(SIMD::Vec3f(drawSet.Scaling), drawSet.Rotation, SIMD::Vec3f(parameter.Position)));
	drawSet.GlobalPointer->SetExternalModels(parameter.ExternalModels);

	return handle;
}

int ManagerImplemented::GetCameraCullingMaskToShowAllEffects()
{
	int mask = 0;

	for (auto& ds : drawSets_)
	{
		auto layerBits = ds.second.GlobalPointer->GetLayerBits();
		mask |= layerBits;
	}

	return mask;
}

void ManagerImplemented::DrawHandle(Handle handle, const Manager::DrawParameter& drawParameter)
{
	if (workerThreads_.size() > 0)
	{
		workerThreads_[0].WaitForComplete();
	}

	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	auto it = renderingDrawSetMaps_.find(handle);
	if (it != renderingDrawSetMaps_.end())
	{
		DrawSet& drawSet = it->second;

		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
		{
			for (auto& c : drawSet.GlobalPointer->RenderedInstanceContainers)
			{
				if (IsClippedWithDepth(drawSet, c, drawParameter))
					continue;

				c->Draw(false);
			}
		}
		else
		{
			drawSet.InstanceContainerPointer->Draw(true);
		}
	}
}

void ManagerImplemented::DrawHandleBack(Handle handle, const Manager::DrawParameter& drawParameter)
{
	if (workerThreads_.size() > 0)
	{
		workerThreads_[0].WaitForComplete();
	}

	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	std::map<Handle, DrawSet>::iterator it = renderingDrawSetMaps_.find(handle);
	if (it != renderingDrawSetMaps_.end())
	{
		DrawSet& drawSet = it->second;
		auto e = (EffectImplemented*)drawSet.ParameterPointer.Get();

		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		for (int32_t i = 0; i < e->renderingNodesThreshold; i++)
		{
			if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
				continue;

			drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
		}
	}
}

void ManagerImplemented::DrawHandleFront(Handle handle, const Manager::DrawParameter& drawParameter)
{
	if (workerThreads_.size() > 0)
	{
		workerThreads_[0].WaitForComplete();
	}

	std::lock_guard<std::recursive_mutex> lock(renderingMutex_);

	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	std::map<Handle, DrawSet>::iterator it = renderingDrawSetMaps_.find(handle);
	if (it != renderingDrawSetMaps_.end())
	{
		DrawSet& drawSet = it->second;
		auto e = (EffectImplemented*)drawSet.ParameterPointer.Get();

		if (!CanDraw(drawSet, drawParameter, cullingPlanes))
		{
			return;
		}

		if (drawSet.GlobalPointer->RenderedInstanceContainers.size() > 0)
		{
			for (size_t i = e->renderingNodesThreshold; i < drawSet.GlobalPointer->RenderedInstanceContainers.size(); i++)
			{
				if (IsClippedWithDepth(drawSet, drawSet.GlobalPointer->RenderedInstanceContainers[i], drawParameter))
					continue;

				drawSet.GlobalPointer->RenderedInstanceContainers[i]->Draw(false);
			}
		}
		else
		{
			drawSet.InstanceContainerPointer->Draw(true);
		}
	}
}

bool ManagerImplemented::GetIsCulled(Handle handle, const Manager::DrawParameter& drawParameter)
{
	const auto cullingPlanes = GeometryUtility::CalculateFrustumPlanes(drawParameter.ViewProjectionMatrix, drawParameter.ZNear, drawParameter.ZFar, GetSetting()->GetCoordinateSystem());

	if (drawSets_.count(handle) > 0)
	{
		return !CanDraw(drawSets_[handle], drawParameter, cullingPlanes);
	}

	return true;
}

int ManagerImplemented::GetUpdateTime() const
{
	return updateTime_;
};

int ManagerImplemented::GetDrawTime() const
{
	return drawTime_;
}

int32_t ManagerImplemented::GetGpuTime() const
{
	if (gpuTimer_ != nullptr)
	{
		int32_t timeCount = 0;
		for (auto& kv : drawSets_)
		{
			timeCount += gpuTimer_->GetResult(kv.second.GlobalPointer);
		}

		if (gpuParticleSystem_)
		{
			timeCount += gpuTimer_->GetResult(gpuParticleSystem_.Get());
		}
		return timeCount;
	}
	return 0;
}

int32_t ManagerImplemented::GetGpuTime(Handle handle) const
{
	if (gpuTimer_ != nullptr)
	{
		auto it = drawSets_.find(handle);
		if (it != drawSets_.end())
		{
			return gpuTimer_->GetResult(it->second.GlobalPointer);
		}
	}
	return 0;
}

int32_t ManagerImplemented::GetRestInstancesCount() const
{
	return pooledInstanceChunks_.GetRestInstance() * InstanceChunk::InstancesOfChunk;
}

void ManagerImplemented::BeginReloadEffect(const EffectRef& effect, bool doLockThread)
{
	if (doLockThread)
	{
		renderingMutex_.lock();
		isLockedWithRenderingMutex_ = true;
	}

	for (auto& it : drawSets_)
	{
		if (it.second.ParameterPointer != effect)
			continue;

		if (it.second.InstanceContainerPointer == nullptr)
		{
			continue;
		}

		// dispose instances
		StopWithoutRemoveDrawSet(it.second);
	}
}

void ManagerImplemented::EndReloadEffect(const EffectRef& effect, bool doLockThread)
{
	for (auto& it : drawSets_)
	{
		auto& ds = it.second;

		if (ds.ParameterPointer != effect)
			continue;

		if (it.second.InstanceContainerPointer != nullptr)
		{
			continue;
		}

		ResetAndPlayWithDataSet(ds, ds.GlobalPointer->GetUpdatedFrame());
	}

	Flip();

	if (doLockThread)
	{
		renderingMutex_.unlock();
		isLockedWithRenderingMutex_ = false;
	}
}

void ManagerImplemented::LockRendering()
{
	renderingMutex_.lock();
}

void ManagerImplemented::UnlockRendering()
{
	renderingMutex_.unlock();
}

void ManagerImplemented::RequestToPlaySound(Instance* instance, const EffectNodeImplemented* node)
{
	if (node->Sound.WaveId >= 0)
	{
		InstanceGlobal* instanceGlobal = instance->GetInstanceGlobal();
		IRandObject& rand = instance->GetRandObject();

		SoundPlayer::InstanceParameter parameter;
		parameter.Data = node->GetEffect()->GetWave(node->Sound.WaveId);
		parameter.Volume = node->Sound.Volume.getValue(rand);
		parameter.Pitch = node->Sound.Pitch.getValue(rand);
		parameter.Pan = node->Sound.Pan.getValue(rand);

		parameter.Mode3D = (node->Sound.PanType == ParameterSoundPanType_3D);
		parameter.Position = ToStruct(instance->GetGlobalMatrix().GetCurrent().GetTranslation());
		parameter.Distance = node->Sound.Distance;
		parameter.UserData = instanceGlobal->GetUserData();

		std::lock_guard<std::mutex> lock(soundMutex_);
		requestedSounds_.emplace(static_cast<SoundTag>(instanceGlobal), parameter);
	}
}

void ManagerImplemented::ExecuteSounds()
{
	if (requestedSounds_.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> lock(soundMutex_);

	auto player = GetSoundPlayer();
	if (player != nullptr)
	{
		while (!requestedSounds_.empty())
		{
			auto sound = requestedSounds_.back();
			player->Play(sound.first, sound.second);
			requestedSounds_.pop();
		}
	}
	else
	{
		while (!requestedSounds_.empty())
		{
			requestedSounds_.pop();
		}
	}
}

} // namespace Effekseer
