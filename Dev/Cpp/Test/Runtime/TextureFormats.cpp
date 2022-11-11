#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#include <Runtime/EffectPlatformGL.h>
#elif defined(__APPLE__)
#include <Runtime/EffectPlatformGL.h>
#include <Runtime/EffectPlatformMetal.h>
#else
#include <Runtime/EffectPlatformGL.h>
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../TestHelper.h"
#include <iostream>

void TextureFormatsPlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);

	auto single15nowebglTest = [&](const char16_t* name, const char* savename) -> void
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/15_NoWebGL/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto single16Test = [&](const char16_t* name, const char* savename) -> void
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	single15nowebglTest(u"dds", "dds");
	single16Test(u"TGA01", "TGA01");
}

void TextureFormatsTest()
{

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		TextureFormatsPlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)
	//#ifdef __EFFEKSEER_BUILD_VULKAN__
	//	{
	//		auto platform = std::make_shared<EffectPlatformVulkan>();
	//		BasicRuntimeTestPlatform(platform.get(), "", "_Vulkan");
	//		platform->Terminate();
	//	}
	//#endif

#ifdef _WIN32
	{

#ifdef __EFFEKSEER_BUILD_DX12__
		//	{
		//		auto platform = std::make_shared<EffectPlatformDX12>();
		//		TextureFormatsPlatform(platform.get(), "", "_DX12");
		//		platform->Terminate();
		//	}
#endif

		{
			auto platform = std::make_shared<EffectPlatformDX9>();
			TextureFormatsPlatform(platform.get(), "", "_DX9");
			platform->Terminate();
		}

		{
			auto platform = std::make_shared<EffectPlatformGL>();
			TextureFormatsPlatform(platform.get(), "", "_GL");
			platform->Terminate();
		}
	}

#elif defined(__APPLE__)

	//{
	//	auto platform = std::make_shared<EffectPlatformMetal>();
	//	BasicRuntimeTestPlatform(platform.get(), "", "_Metal");
	//	platform->Terminate();
	//}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		TextureFormatsPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		TextureFormatsPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
#endif
#endif
}

TestRegister Runtime_TextureFormatsTest("Runtime.TextureFormatsTest", []() -> void
										{ TextureFormatsTest(); });
