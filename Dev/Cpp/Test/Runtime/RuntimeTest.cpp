#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include "EffectPlatformDX12.h"
#endif

#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#include "EffectPlatformGL.h"
#elif defined(__APPLE__)
#include "EffectPlatformGL.h"
#else
#include "EffectPlatformGL.h"
#endif

#include "../TestHelper.h"

void BasicRuntimeTestPlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);

	auto single10Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/" + name + u".efk").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto single14Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/14/" + name + u".efk").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto single15Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/15/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	single10Test(u"SimpleLaser", "SimpleLaser");
	single10Test(u"FCurve_Parameters1", "FCurve_Parameters1");
	single10Test(u"Ribbon_Parameters1", "Ribbon_Parameters1");
	single10Test(u"Ring_Parameters1", "Ring_Parameters1");
	single10Test(u"Track_Parameters1", "Track_Parameters1");
	single10Test(u"Sprite_Parameters1", "Sprite_Parameters1");
	single10Test(u"Distortions1", "Distortions1");

	single14Test(u"Model_Parameters1", "Model_Parameters1");
	single15Test(u"Material_Refraction", "Material_Refraction");
}

void BasicRuntimeDeviceLostTest()
{
#ifdef _WIN32
	auto platform = std::make_shared<EffectPlatformDX9>();

	srand(0);

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost1.png");

	platform->SetFullscreen(true);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost2.png");

	platform->SetFullscreen(false);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost3.png");

	platform->Terminate();
#endif
}

void UpdateHandleTest()
{
	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_0.png");

		platform->Terminate();
	}

	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_1.png");

		platform->Terminate();
	}
}

void BasicRuntimeTest()
{
#ifdef _WIN32
#ifdef __EFFEKSEER_BUILD_DX12__
	{
		auto platform = std::make_shared<EffectPlatformDX12>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX12");
		platform->Terminate();
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformDX9>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX9");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}

#elif defined(__APPLE__)
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#else
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
}