#if defined(__EFFEKSEER_BUILD_WEBGPU__) && defined(__EMSCRIPTEN__)

#include <Effekseer.h>
#include <Runtime/EffectPlatformWebGPU.h>
#include <emscripten.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

namespace
{

struct BrowserPlayerOptions
{
	std::string EffectPath = "/TestData/Effects/10/SimpleLaser.efk";
	int32_t Width = 640;
	int32_t Height = 360;
	bool Loop = true;
	int32_t LoopFrame = 180;
	Effekseer::Vector3D Position = Effekseer::Vector3D();
};

class BrowserPlayer
{
	BrowserPlayerOptions options_;
	std::shared_ptr<EffectPlatformWebGPU> platform_;
	int32_t frame_ = 0;

	void Play()
	{
		std::array<char16_t, 512> path{};
		Effekseer::ConvertUtf8ToUtf16(path.data(), static_cast<int32_t>(path.size()), options_.EffectPath.c_str());
		platform_->Play(path.data(), options_.Position);
		frame_ = 0;
	}

public:
	explicit BrowserPlayer(BrowserPlayerOptions options)
		: options_(std::move(options))
	{
	}

	void Initialize()
	{
		EffectPlatformInitializingParameter param;
		param.VSync = false;
		param.WindowSize = {options_.Width, options_.Height};

		platform_ = std::make_shared<EffectPlatformWebGPU>();
		platform_->Initialize(param);
		Play();
	}

	void Update()
	{
		if (options_.Loop && options_.LoopFrame > 0 && frame_ >= options_.LoopFrame)
		{
			platform_->StopAllEffects();
			Play();
		}

		if (!platform_->Update())
		{
			emscripten_cancel_main_loop();
			return;
		}

		frame_++;
	}
};

std::unique_ptr<BrowserPlayer> g_player;

void Tick()
{
	g_player->Update();
}

bool StartsWith(const char* text, const char* prefix)
{
	return std::strncmp(text, prefix, std::strlen(prefix)) == 0;
}

BrowserPlayerOptions ParseOptions(int argc, char* argv[])
{
	BrowserPlayerOptions options;
	for (int i = 1; i < argc; i++)
	{
		const char* arg = argv[i];
		if (StartsWith(arg, "--effect="))
		{
			options.EffectPath = arg + std::strlen("--effect=");
		}
		else if (StartsWith(arg, "--width="))
		{
			options.Width = std::atoi(arg + std::strlen("--width="));
		}
		else if (StartsWith(arg, "--height="))
		{
			options.Height = std::atoi(arg + std::strlen("--height="));
		}
		else if (StartsWith(arg, "--loop="))
		{
			options.Loop = std::atoi(arg + std::strlen("--loop=")) != 0;
		}
		else if (StartsWith(arg, "--loop-frame="))
		{
			options.LoopFrame = std::atoi(arg + std::strlen("--loop-frame="));
		}
		else if (StartsWith(arg, "--x="))
		{
			options.Position.X = static_cast<float>(std::atof(arg + std::strlen("--x=")));
		}
		else if (StartsWith(arg, "--y="))
		{
			options.Position.Y = static_cast<float>(std::atof(arg + std::strlen("--y=")));
		}
		else if (StartsWith(arg, "--z="))
		{
			options.Position.Z = static_cast<float>(std::atof(arg + std::strlen("--z=")));
		}
	}

	if (options.Width <= 0)
	{
		options.Width = 640;
	}
	if (options.Height <= 0)
	{
		options.Height = 360;
	}
	return options;
}

} // namespace

int main(int argc, char* argv[])
{
	Effekseer::SetLogger([](Effekseer::LogType, const std::string& message) -> void { std::cout << message << std::endl; });

	auto options = ParseOptions(argc, argv);
	std::cout << "EFFEKSEER_WEBGPU_PLAYER effect=" << options.EffectPath << std::endl;

	g_player = std::make_unique<BrowserPlayer>(options);
	g_player->Initialize();
	emscripten_set_main_loop(Tick, 0, true);
	return 0;
}

#endif
