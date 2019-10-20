
#include "EffectPlatform.h"

void EffectPlatform::CreateCheckeredPattern(int width, int height, uint32_t* pixels)
{
	const uint32_t color[2] = {0x00202020, 0x00808080};

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			*pixels++ = color[(x / 20 % 2) ^ (y / 20 % 2)];
		}
	}
}

EffekseerRenderer::Renderer* EffectPlatform::GetRenderer() const { return renderer_; }

void* EffectPlatform::GetNativePtr(int32_t index)
{
#ifdef _WIN32
	if (index == 0)
	{
		return glfwGetWin32Window(window_);
	}

	return (HINSTANCE)GetModuleHandle(0);
#endif

#ifdef __APPLE__
	return glfwGetCocoaWindow(window_);
#endif

#ifdef __linux__
	if (index == 0)
	{
		return glfwGetX11Display();
	}

	return reinterpret_cast<void*>(glfwGetX11Window(window_));
#endif

	return nullptr;
}

EffectPlatform::EffectPlatform(bool isOpenGLMode) : isOpenGLMode_(isOpenGLMode)
{
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

	if (isOpenGLMode)
	{
#if !_WIN32
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	window_ = glfwCreateWindow(1280, 720, "Example glfw", nullptr, nullptr);

	if (window_ == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	if (isOpenGLMode)
	{
		glfwMakeContextCurrent(window_);
	}

	checkeredPattern_.resize(1280 * 720);
	CreateCheckeredPattern(1280, 720, checkeredPattern_.data());
}

EffectPlatform::~EffectPlatform()
{
	for (auto& effect : effects_)
	{
		effect->Release();
	}
	effects_.clear();

	if (renderer_ != nullptr)
	{
		renderer_->Destroy();
		renderer_ = nullptr;
	}

	if (manager_ != nullptr)
	{
		// TODO release causes memory leaks
		// manager_->Release();
		manager_->Destroy();
		manager_ = nullptr;
	}

	if (window_ != nullptr)
	{
		glfwDestroyWindow(window_);
		glfwTerminate();
		window_ = nullptr;
	}
}

void EffectPlatform::Initialize(const EffectPlatformInitializingParameter& param)
{
	if (isInitialized_)
	{
		return;
	}

	InitializeDevice(param);

	manager_ = ::Effekseer::Manager::Create(8000);

	renderer_ = CreateRenderer();

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 10.0f) / 1.0f;
	auto focus = ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f);

	renderer_->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	if (isOpenGLMode_)
	{
		renderer_->SetProjectionMatrix(
			::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, 1280.0f / 720.0f, 1.0f, 50.0f));
	}
	else
	{
		renderer_->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, 1280.0f / 720.0f, 1.0f, 50.0f));
	}

	manager_->SetSpriteRenderer(renderer_->CreateSpriteRenderer());
	manager_->SetRibbonRenderer(renderer_->CreateRibbonRenderer());
	manager_->SetRingRenderer(renderer_->CreateRingRenderer());
	manager_->SetModelRenderer(renderer_->CreateModelRenderer());
	manager_->SetTrackRenderer(renderer_->CreateTrackRenderer());

	manager_->SetCoordinateSystem(::Effekseer::CoordinateSystem::RH);

	manager_->SetTextureLoader(renderer_->CreateTextureLoader());
	manager_->SetModelLoader(renderer_->CreateModelLoader());

	isInitialized_ = true;
}

Effekseer::Handle EffectPlatform::Play(const char16_t* path)
{
	FILE* filePtr = NULL;
#ifdef _WIN32
	_wfopen_s(&filePtr, (const wchar_t*)path, L"rb");
#else
	int8_t path8[256];
	ConvertUtf16ToUtf8(path8, 256, (const int16_t*)path);
	filePtr = fopen((const char*)path8, "rb");
#endif

	if (filePtr == nullptr)
	{
		throw "Failed to load an effect buffer";
	}

	fseek(filePtr, SEEK_END, 0);
	auto size = ftell(filePtr);
	fseek(filePtr, SEEK_SET, 0);

	std::vector<uint8_t> data;
	data.resize(size);
	fread(data.data(), 1, size, filePtr);
	fclose(filePtr);

	auto effect = Effekseer::Effect::Create(manager_, path);
	if (effect == nullptr)
	{
		throw "Failed to load an effect";
	}

	buffers_.push_back(data);
	effects_.push_back(effect);
	auto handle = manager_->Play(effect, Effekseer::Vector3D());
	effectHandles_.push_back(handle);
	return handle;
}

bool EffectPlatform::Update()
{
	if (glfwWindowShouldClose(window_) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	manager_->Update();

	BeginRendering();

	renderer_->BeginRendering();
	manager_->Draw();
	renderer_->EndRendering();

	EndRendering();

	if (isOpenGLMode_)
	{
		glfwSwapBuffers(window_);
	}
	else
	{
		Present();
	}

	return true;
}

void EffectPlatform::StopAllEffects()
{
	if (manager_ != nullptr)
	{
		manager_->StopAllEffects();
	}
}