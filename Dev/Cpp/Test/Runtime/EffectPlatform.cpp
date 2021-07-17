
#include "EffectPlatform.h"
#include <assert.h>

void EffectPlatform::CreateCheckeredPattern(int width, int height, uint32_t* pixels)
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

	if (isBackgroundFlipped_)
	{
		for (size_t y = 0; y < initParam_.WindowSize[1] / 2; y++)
		{
			for (size_t x = 0; x < initParam_.WindowSize[0]; x++)
			{
				std::swap(checkeredPattern_[x + y * initParam_.WindowSize[0]], checkeredPattern_[x + (initParam_.WindowSize[1] - 1 - y) * initParam_.WindowSize[0]]);
			}
		}
	}
}

EffekseerRenderer::RendererRef EffectPlatform::GetRenderer() const
{
	return renderer_;
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
	CreateCheckeredPattern(initParam_.WindowSize[0], initParam_.WindowSize[1], checkeredPattern_.data());

	InitializeWindow();

	InitializeDevice(param);

	manager_ = ::Effekseer::Manager::Create(param.InstanceCount);

	renderer_ = CreateRenderer();

	if (renderer_ != nullptr)
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

		manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
		manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
		manager_->SetRingRenderer(renderer_->CreateRingRenderer());
		manager_->SetModelRenderer(renderer_->CreateModelRenderer());
		manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());

		manager_->SetTextureLoader(renderer_->CreateTextureLoader());
		manager_->SetModelLoader(renderer_->CreateModelLoader());
		manager_->SetMaterialLoader(renderer_->CreateMaterialLoader());
	}

	manager_->SetCoordinateSystem(::Effekseer::CoordinateSystem::RH);

	if (param.IsCullingCreated)
	{
		manager_->CreateCullingWorld(100.0f, 100.0f, 100.0f, 6);
	}

	// support multithread in 1.6
	manager_->LaunchWorkerThreads(4);

	isInitialized_ = true;
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

bool EffectPlatform::Update()
{
	if (!DoEvent())
		return false;

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
		manager_->Update();
	}

	BeginRendering();

	if (renderer_ != nullptr)
	{
		renderer_->SetTime(time_);
		renderer_->BeginRendering();
		manager_->Draw();
		renderer_->EndRendering();
	}

	EndRendering();

	Present();

	time_ += 1.0f / 60.0f;

	return true;
}

bool EffectPlatform::Draw()
{
	if (!DoEvent())
		return false;

	BeginRendering();

	if (renderer_ != nullptr)
	{
		renderer_->SetTime(time_);
		renderer_->BeginRendering();
		manager_->Draw();
		renderer_->EndRendering();
	}

	EndRendering();

	Present();

	return true;
}

void EffectPlatform::StopAllEffects()
{
	if (manager_ != nullptr)
	{
		manager_->StopAllEffects();
	}
}