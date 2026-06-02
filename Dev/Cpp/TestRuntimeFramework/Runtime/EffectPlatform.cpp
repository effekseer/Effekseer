
#include "EffectPlatform.h"
#include <assert.h>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace
{

int64_t GetTimeMicroseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

template <class T>
void FlipRows(std::vector<T>& pixels, int32_t width, int32_t height)
{
	for (int32_t y = 0; y < height / 2; y++)
	{
		for (int32_t x = 0; x < width; x++)
		{
			std::swap(pixels[x + y * width], pixels[x + (height - 1 - y) * width]);
		}
	}
}

} // namespace

void EffectPlatform::CreateBackgroundPattern(int width, int height, uint32_t* pixels)
{
	if (initParam_.BackgroundPattern == BackgroundPatternType::NonPeriodicGradient)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				const auto r = static_cast<uint32_t>(24 + (x * 83) / width + (y * 29) / height);
				const auto g = static_cast<uint32_t>(40 + (x * 37) / width + (y * 71) / height);
				const auto b = static_cast<uint32_t>(32 + (x * 19) / width + (y * 47) / height);
				*pixels++ = 0xFF000000 | (b << 16) | (g << 8) | r;
			}
		}
	}
	else
	{
		{
			const uint32_t color[2] = {0xFF204020, 0xFF80A080};

			for (int y = 0; y < height / 2; y++)
			{
				for (int x = 0; x < width; x++)
				{
					*pixels++ = color[(x / 20 % 2) ^ (y / 20 % 2)];
				}
			}
		}

		{
			const uint32_t color[2] = {0xFF402020, 0xFFA08080};

			for (int y = height / 2; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					*pixels++ = color[(x / 20 % 2) ^ (y / 20 % 2)];
				}
			}
		}
	}

	if (isBackgroundFlipped_)
	{
		FlipRows(checkeredPattern_, initParam_.WindowSize[0], initParam_.WindowSize[1]);
	}
}

EffekseerRenderer::RendererRef EffectPlatform::GetRenderer() const
{
	return renderer_;
}

std::array<EffectPlatform::GroundPlaneVertex, 4> EffectPlatform::CreateGroundPlaneVertices() const
{
	return Effekseer::ToolRuntime::CreateGroundPlaneClipVertices(
		renderer_->GetCameraMatrix(),
		renderer_->GetProjectionMatrix(),
		Effekseer::ToolRuntime::CreateDefaultSoftParticleGround());
}

std::array<uint16_t, 6> EffectPlatform::CreateGroundPlaneIndices() const
{
	return Effekseer::ToolRuntime::CreateGroundPlaneIndices16();
}

std::array<std::array<float, 4>, 4> EffectPlatform::CreateGroundViewProjectionColumns() const
{
	return Effekseer::ToolRuntime::CreateViewProjectionColumns(renderer_->GetCameraMatrix(), renderer_->GetProjectionMatrix());
}

EffekseerRenderer::DepthReconstructionParameter EffectPlatform::CreateGroundDepthReconstructionParameter(float depthBufferScale, float depthBufferOffset) const
{
	return Effekseer::ToolRuntime::CreateDepthReconstructionParameter(renderer_->GetProjectionMatrix(), depthBufferScale, depthBufferOffset);
}

EffectPlatform::EffectPlatform()
{
}

EffectPlatform::~EffectPlatform()
{
	if (isInitialized_ && !isTerminated_)
	{
		assert(0);
	}
}
void EffectPlatform::Initialize(const EffectPlatformInitializingParameter& param)
{
	if (isInitialized_)
	{
		return;
	}

	initParam_ = param;

	checkeredPattern_.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1]);
	CreateBackgroundPattern(initParam_.WindowSize[0], initParam_.WindowSize[1], checkeredPattern_.data());

	InitializeWindow();

	InitializeDevice(param);

	manager_ = ::Effekseer::Manager::Create(param.InstanceCount);

	renderer_ = CreateRenderer();

	if (renderer_ != nullptr)
	{
		if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
		{
			auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 10.0f) / 2.0f;
			auto focus = ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f);

			renderer_->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

			if (isOpenGLMode_)
			{
				renderer_->SetProjectionMatrix(
					::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, (float)initParam_.WindowSize[0] / (float)initParam_.WindowSize[1], 1.0f, 50.0f));
			}
			else
			{
				renderer_->SetProjectionMatrix(
					::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)initParam_.WindowSize[0] / (float)initParam_.WindowSize[1], 1.0f, 50.0f));
			}
		}
		else
		{
			auto position = ::Effekseer::Vector3D(10.0f, 5.0f, -10.0f) / 2.0f;
			auto focus = ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f);

			renderer_->SetCameraMatrix(::Effekseer::Matrix44().LookAtLH(position, focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

			if (isOpenGLMode_)
			{
				renderer_->SetProjectionMatrix(
					::Effekseer::Matrix44().PerspectiveFovLH_OpenGL(90.0f / 180.0f * 3.14f, (float)initParam_.WindowSize[0] / (float)initParam_.WindowSize[1], 1.0f, 50.0f));
			}
			else
			{
				renderer_->SetProjectionMatrix(
					::Effekseer::Matrix44().PerspectiveFovLH(90.0f / 180.0f * 3.14f, (float)initParam_.WindowSize[0] / (float)initParam_.WindowSize[1], 1.0f, 50.0f));
			}

			auto lightDirection = renderer_->GetLightDirection();
			lightDirection.Z = -lightDirection.Z;
			renderer_->SetLightDirection(lightDirection);
		}

		manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
		manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
		manager_->SetRingRenderer(renderer_->CreateRingRenderer());
		manager_->SetModelRenderer(renderer_->CreateModelRenderer());
		manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());
		manager_->SetGpuParticleSystem(renderer_->CreateGpuParticleSystem());
		manager_->SetGpuParticleFactory(renderer_->CreateGpuParticleFactory());

		manager_->SetTextureLoader(renderer_->CreateTextureLoader());
		manager_->SetModelLoader(renderer_->CreateModelLoader());
		manager_->SetMaterialLoader(renderer_->CreateMaterialLoader());
	}

	manager_->SetCoordinateSystem(param.CoordinateSyatem);

#if !defined(__EMSCRIPTEN__) || defined(EFFEKSEER_WEBGPU_BROWSER_ENABLE_WORKER_THREADS)
	// support multithread in 1.6
	manager_->LaunchWorkerThreads(4);
#endif

	isInitialized_ = true;
}

void EffectPlatform::ResetBackgroundPattern()
{
	isGroundDepthEnabled_ = false;
	checkeredPattern_.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1]);
	CreateBackgroundPattern(initParam_.WindowSize[0], initParam_.WindowSize[1], checkeredPattern_.data());
	UpdateBackgroundTexture();
}

void EffectPlatform::GenerateGroundDepth()
{
	isGroundDepthEnabled_ = true;
	if (renderer_ == nullptr)
	{
		return;
	}

	const auto width = initParam_.WindowSize[0];
	const auto height = initParam_.WindowSize[1];
	const auto groundDepth = Effekseer::ToolRuntime::CreateGroundDepthRenderData(
		width,
		height,
		renderer_->GetCameraMatrix(),
		renderer_->GetProjectionMatrix(),
		renderer_->GetCameraPosition(),
		Effekseer::ToolRuntime::CreateDefaultSoftParticleGround(),
		isOpenGLMode_,
		isBackgroundFlipped_);
	checkeredPattern_ = groundDepth.BackgroundPixels;

	Effekseer::Backend::TextureParameter texParam;
	Effekseer::CustomVector<uint8_t> initialData;
	initialData.resize(groundDepth.DepthTextureInitialData.size());
	texParam.Format = Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT;
	texParam.Size = {width, height, 1};
	if (!groundDepth.DepthTextureInitialData.empty())
	{
		memcpy(initialData.data(), groundDepth.DepthTextureInitialData.data(), groundDepth.DepthTextureInitialData.size());
	}

	auto depth = GetRenderer()->GetGraphicsDevice()->CreateTexture(texParam, initialData);

	GetRenderer()->SetDepth(depth, CreateGroundDepthReconstructionParameter());

	UpdateBackgroundTexture();
}

void EffectPlatform::Terminate()
{
	PreDestroyDevice();

	effects_.clear();

	manager_.Reset();
	renderer_.Reset();

	DestroyDevice();

	isTerminated_ = true;
}

Effekseer::Handle EffectPlatform::Play(const char16_t* path, Effekseer::Vector3D position, int32_t startFrame)
{
	// reset time
	time_ = 0;

	char path8[256];
	Effekseer::ConvertUtf16ToUtf8(path8, 256, path);

	FILE* filePtr = NULL;
#ifdef _WIN32
	_wfopen_s(&filePtr, (const wchar_t*)path, L"rb");
#else
	filePtr = fopen((const char*)path8, "rb");
#endif

	if (filePtr == nullptr)
	{
		printf("Failed to load %s./n", path8);
		assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	auto size = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	std::vector<uint8_t> data;
	data.resize(size);
	fread(data.data(), 1, size, filePtr);
	fclose(filePtr);

	auto effect = Effekseer::Effect::Create(manager_, path);
	if (effect == nullptr)
	{
		assert(0);
	}

	buffers_.push_back(data);
	effects_.push_back(effect);
	auto handle = manager_->Play(effect, position, startFrame);
	effectHandles_.push_back(handle);
	return handle;
}

bool EffectPlatform::Update(EffectPlatformMeasuredTime* measuredTime)
{
	const auto totalBeginTime = GetTimeMicroseconds();

	if (!DoEvent())
		return false;

	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = renderer_ != nullptr ? renderer_->GetCameraPosition() : Effekseer::Vector3D(0.0, 0.0, 0.0);
	manager_->SetLayerParameter(0, layerParameter);

	const auto updateBeginTime = GetTimeMicroseconds();

	if (this->initParam_.IsUpdatedByHandle)
	{
		manager_->BeginUpdate();

		for (auto h : effectHandles_)
		{
			manager_->UpdateHandle(h);
		}

		manager_->EndUpdate();
	}
	else
	{
		Effekseer::Manager::UpdateParameter updateParameter;
		updateParameter.UpdateInterval = 0.0;
		manager_->Update(updateParameter);
	}

	if (measuredTime != nullptr)
	{
		measuredTime->ManagerUpdate = static_cast<int32_t>(GetTimeMicroseconds() - updateBeginTime);
	}

	const auto computeBeginTime = GetTimeMicroseconds();

	BeginCompute();
	manager_->Compute();
	EndCompute();

	if (measuredTime != nullptr)
	{
		measuredTime->Compute = static_cast<int32_t>(GetTimeMicroseconds() - computeBeginTime);
	}

	const auto platformBeginRenderingBeginTime = GetTimeMicroseconds();
	BeginRendering();

	if (measuredTime != nullptr)
	{
		measuredTime->PlatformBeginRendering = static_cast<int32_t>(GetTimeMicroseconds() - platformBeginRenderingBeginTime);
	}

	if (renderer_ != nullptr)
	{
		Effekseer::Manager::DrawParameter param;
		param.ViewProjectionMatrix = renderer_->GetCameraProjectionMatrix();
		param.ZNear = 0.0f;
		param.ZFar = 1.0f;

		renderer_->SetTime(time_);
		const auto rendererBeginTime = GetTimeMicroseconds();
		renderer_->BeginRendering();

		if (measuredTime != nullptr)
		{
			measuredTime->RendererBegin = static_cast<int32_t>(GetTimeMicroseconds() - rendererBeginTime);
		}

		const auto drawBeginTime = GetTimeMicroseconds();
		manager_->Draw(param);

		if (measuredTime != nullptr)
		{
			measuredTime->ManagerDraw = static_cast<int32_t>(GetTimeMicroseconds() - drawBeginTime);
			const auto drawTimeBreakdown = manager_->GetDrawTimeBreakdown();
			measuredTime->ManagerDrawWorkerThreadWait = drawTimeBreakdown.WorkerThreadWait;
			measuredTime->ManagerDrawMutexLock = drawTimeBreakdown.MutexLock;
			measuredTime->ManagerDrawCulling = drawTimeBreakdown.Culling;
			measuredTime->ManagerDrawSorting = drawTimeBreakdown.Sorting;
			measuredTime->ManagerDrawDrawSets = drawTimeBreakdown.DrawSets;
			measuredTime->ManagerDrawGpuParticles = drawTimeBreakdown.GpuParticles;
			measuredTime->ManagerDrawTotal = drawTimeBreakdown.Total;
		}

		const auto rendererEndTime = GetTimeMicroseconds();
		renderer_->EndRendering();

		if (measuredTime != nullptr)
		{
			measuredTime->RendererEnd = static_cast<int32_t>(GetTimeMicroseconds() - rendererEndTime);
		}
	}

	const auto platformEndRenderingBeginTime = GetTimeMicroseconds();
	EndRendering();

	if (measuredTime != nullptr)
	{
		const auto beforePresentTime = GetTimeMicroseconds();
		measuredTime->PlatformEndRendering = static_cast<int32_t>(beforePresentTime - platformEndRenderingBeginTime);
		measuredTime->FrameWithoutPresent = static_cast<int32_t>(beforePresentTime - totalBeginTime);
	}

	const auto presentBeginTime = GetTimeMicroseconds();
	Present();

	time_ += 1.0f / 60.0f;

	if (measuredTime != nullptr)
	{
		const auto totalEndTime = GetTimeMicroseconds();
		measuredTime->Present = static_cast<int32_t>(totalEndTime - presentBeginTime);
		measuredTime->Frame = static_cast<int32_t>(totalEndTime - totalBeginTime);
	}

	return true;
}

bool EffectPlatform::Draw()
{
	if (!DoEvent())
		return false;

	BeginRendering();

	if (renderer_ != nullptr)
	{
		Effekseer::Manager::DrawParameter param;
		param.ViewProjectionMatrix = renderer_->GetCameraProjectionMatrix();
		param.ZNear = 0.0f;
		param.ZFar = 1.0f;

		renderer_->SetTime(time_);
		renderer_->BeginRendering();
		manager_->Draw(param);
		renderer_->EndRendering();
	}

	EndRendering();

	Present();

	return true;
}

bool EffectPlatform::BeginFrame()
{
	return DoEvent();
}

void EffectPlatform::StopAllEffects()
{
	if (manager_ != nullptr)
	{
		manager_->StopAllEffects();
	}
}

void EffectPlatform::ClearLoadedEffects()
{
	effectHandles_.clear();
	effects_.clear();
	buffers_.clear();
}
