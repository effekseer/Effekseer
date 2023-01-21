#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#elif defined(__APPLE__)
#include <Runtime/EffectPlatformMetal.h>
#else
#endif

#include <Runtime/EffectPlatformGL.h>

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"
#include "../TestHelper.h"
#include <iostream>

void BasicRuntimeDeviceLostTest()
{
#ifdef _WIN32
	auto platform = std::make_shared<EffectPlatformDX9>();

	srand(0);

	EffectPlatformInitializingParameter param;
	// To make fullscreen enabled
	param.WindowSize = {640, 480};
	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/14/Model_Parameters1.efk").c_str());
	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel01.efkefc").c_str());

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

void StartingFrameTest()
{
	srand(0);
#ifdef _WIN32
	auto platform = std::make_shared<EffectPlatformDX11>();
#else
	auto platform = std::make_shared<EffectPlatformGL>();
#endif

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str(), Effekseer::Vector3D(), 30);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("StartingFrame_0.png");

	platform->Terminate();
}

void UpdateHandleTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Memory/DestroyWhenNoMoreChildren.efkefc").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_0.png");

		platform->StopAllEffects();

		for (size_t i = 0; i < 5; i++)
		{
			platform->Update();
		}

		assert(platform->GetManager()->GetRestInstancesCount() == 8000);

		platform->Terminate();
	}

	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Memory/DestroyWhenNoMoreChildren.efkefc").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_1.png");

		platform->StopAllEffects();

		for (size_t i = 0; i < 5; i++)
		{
			platform->Update();
		}

		assert(platform->GetManager()->GetRestInstancesCount() == 8000);

		platform->Terminate();
	}

	// Check memory leak
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		auto handle = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

		for (size_t i = 0; i < 10; i++)
		{
			platform->Update();
		}

		platform->GetManager()->StopEffect(handle);

		for (size_t i = 0; i < 10; i++)
		{
			platform->Update();
		}

		assert(platform->GetManager()->GetRestInstancesCount() == 8000);

		platform->Terminate();
	}
}

void PlaybackSpeedTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetSpeed(h, 0.5f);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackSpeed_0.png");

		platform->Terminate();
	}

	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetSpeed(h, 0.5f);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackSpeed_1.png");

		platform->Terminate();
	}
}

void PlaybackRandomSeedTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetRandomSeed(h, 1);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackRandomSeed_0.png");

		platform->Terminate();
	}

	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetRandomSeed(h, 2);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackRandomSeed_1.png");

		platform->Terminate();
	}
}

void MassPlayTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.InstanceCount = 1;

		platform->Initialize(param);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
			platform->Update();
		}

		platform->Terminate();
	}

	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.InstanceCount = 10;

		platform->Initialize(param);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
			platform->Update();
		}

		platform->Terminate();
	}

	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		param.InstanceCount = 100;
		param.IsUpdatedByHandle = true;
		platform->Initialize(param);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Play(
				(GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Memory/DestroyWhenNoMoreChildren.efkefc").c_str());
			platform->Update();
		}

		platform->Terminate();
	}
}

void ReloadTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto handle = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}

		auto restCount1 = platform->GetManager()->GetInstanceCount(handle);
		platform->TakeScreenshot("Reload_0.png");

		auto manager = platform->GetManager();

		auto effectData = LoadFile((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

		platform->GetEffects()[0]->Reload(&manager, 1, effectData.data(), static_cast<int32_t>(effectData.size()));

		//platform->Update();

		platform->Draw();

		auto restCount2 = platform->GetManager()->GetInstanceCount(handle);
		platform->TakeScreenshot("Reload_1.png");

		assert(restCount1 == restCount2);

		platform->Terminate();
	}
}

void UpdateToMoveTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif

		EffectPlatformInitializingParameter param;
		platform->Initialize(param);

		auto handle = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

		//
		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}

		platform->TakeScreenshot("UpdateToMove_0.png");

		for (size_t i = 0; i < 10; i++)
		{
			platform->Update();
		}

		//
		platform->GetManager()->BeginUpdate();

		platform->GetManager()->UpdateHandleToMoveToFrame(handle, 19);

		platform->GetManager()->EndUpdate();

		platform->Update();

		platform->TakeScreenshot("UpdateToMove_1.png");

		//
		platform->GetManager()->BeginUpdate();

		platform->GetManager()->UpdateHandleToMoveToFrame(handle, 10);
		platform->GetManager()->UpdateHandleToMoveToFrame(handle, 19);

		platform->GetManager()->EndUpdate();

		platform->Update();

		platform->TakeScreenshot("UpdateToMove_2.png");

		platform->Terminate();
	}
}

void InstanceDisposeTestPlatform(EffectPlatform* platform)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);

	{
		auto effect = Effekseer::Effect::Create(
			platform->GetManager(), (GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/14/Model_Parameters1.efk").c_str());
	}
}

void InstanceDisposeTest()
{

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		InstanceDisposeTestPlatform(platform.get());
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)

#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		InstanceDisposeTestPlatform(platform.get());
		platform->Terminate();
	}
#endif

#ifdef _WIN32
#ifdef __EFFEKSEER_BUILD_DX12__
	{
		auto platform = std::make_shared<EffectPlatformDX12>();
		InstanceDisposeTestPlatform(platform.get());
		platform->Terminate();
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformDX9>();
		InstanceDisposeTestPlatform(platform.get());
		platform->Terminate();
	}

#elif defined(__APPLE__)
	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		InstanceDisposeTestPlatform(platform.get());
		platform->Terminate();
	}
#endif

	auto platform = std::make_shared<EffectPlatformGL>();
	InstanceDisposeTestPlatform(platform.get());
	platform->Terminate();
#endif
}

void CustomAllocatorTest()
{
	Effekseer::CustomVector<int> v;
	v.push_back(10);

	Effekseer::CustomList<int> l;
	l.push_back(10);

	Effekseer::CustomSet<int> s;
	s.insert(10);

	Effekseer::CustomMap<int, int> m;
	m[1] = 10;
}

void StringAndPathHelperTest()
{

	if (Effekseer::StringHelper::To<char16_t>("hoge") != std::u16string(u"hoge"))
		throw "";

	if (Effekseer::StringHelper::To<char32_t>("hoge") != std::u32string(U"hoge"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"/a/../b/c")) != std::u16string(u"/b/c"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"a/../b/c")) != std::u16string(u"b/c"))
		throw "";

	if (Effekseer::PathHelper::Normalize(std::u16string(u"../b/c")) != std::u16string(u"../b/c"))
		throw "";

	if (Effekseer::PathHelper::Absolute(std::u16string(u"d/c"), std::u16string(u"/a/b/c")) != std::u16string(u"/a/b/d/c"))
		throw "";

	if (Effekseer::PathHelper::Absolute(std::u16string(u"../d/c"), std::u16string(u"/a/b/c")) != std::u16string(u"/a/d/c"))
		throw "";

	if (Effekseer::PathHelper::Relative(std::u16string(u"/a/b/e"), std::u16string(u"/a/b/c")) != std::u16string(u"e"))
		throw "";
}

void DX11DefferedContextTest()
{
#ifdef _WIN32
	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformDX11>(true);
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetSpeed(h, 0.5f);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}

		platform->Terminate();
	}
#endif
}

void ProceduralModelCacheTest()
{
#ifdef _WIN32
	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformDX11>(true);
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel01.efkefc").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel01.efkefc").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel02.efkefc").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel02.efkefc").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}

		platform->Terminate();
	}
#endif
}

void CullingTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Culling/Sphere.efkefc").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			if (i == 10)
			{
				platform->GetManager()->StopRoot(h);
			}
			platform->Update();
			platform->GetManager()->AddLocation(h, {0.0f, 0.1f, 0.0f});
		}

		platform->Terminate();
	}
}

void RenderLimitTest()
{
	auto test = [](EffectPlatform* platform) {
		EffectPlatformInitializingParameter param;
		param.SpriteCount = 10;
		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/ProcedualModel01.efkefc").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ring_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Track_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Sprite_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());

		for (size_t i = 0; i < 60; i++)
		{
			platform->Update();
		}

		platform->Terminate();
	};

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		test(platform.get());
	}
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		test(platform.get());
	}
#endif

#ifdef _WIN32
	{

#ifdef __EFFEKSEER_BUILD_DX12__
		{
			auto platform = std::make_shared<EffectPlatformDX12>();
			test(platform.get());
		}
#endif

		{
			auto platform = std::make_shared<EffectPlatformDX9>();
			test(platform.get());
		}

		{
			auto platform = std::make_shared<EffectPlatformGL>();
			test(platform.get());
		}
	}

#elif defined(__APPLE__)

	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		test(platform.get());
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		test(platform.get());
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		test(platform.get());
	}
#endif
#endif
#endif
}

void SRGBLinearTestPlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);
	platform->GetRenderer()->SetMaintainGammaColorInLinearColorSpace(true);

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

	auto single16Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/16/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	single10Test(u"SimpleLaser", "SL_SimpleLaser");
	single10Test(u"Sprite_Parameters1", "SL_Sprite_Parameters1");
	single10Test(u"Distortions1", "SL_Distortions1");

	single14Test(u"Model_Parameters1", "SL_Model_Parameters1");
	single15Test(u"Lighing_Parameters1", "SL_Lighing_Parameters1");
	single15Test(u"Material_Sampler1", "SL_Material_Sampler1");
	single15Test(u"Material_Refraction", "SL_Material_Refraction");
	single15Test(u"Material_WorldPositionOffset", "SL_Material_WorldPositionOffset");

	single15Test(u"Material_UV1", "SL_Material_UV1");

	{
		single16Test(u"AlphaBlendTexture01", "SL_AlphaBlendTexture01");
		single16Test(u"AlphaCutoffEdgeColor01", "SL_AlphaCutoffEdgeColor01");
	}
}

void SRGBLinearTest()
{

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		SRGBLinearTestPlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		SRGBLinearTestPlatform(platform.get(), "", "_Vulkan");
		platform->Terminate();
	}
#endif

#ifdef _WIN32
	{

#ifdef __EFFEKSEER_BUILD_DX12__
		{
			auto platform = std::make_shared<EffectPlatformDX12>();
			SRGBLinearTestPlatform(platform.get(), "", "_DX12");
			platform->Terminate();
		}
#endif

		{
			auto platform = std::make_shared<EffectPlatformDX9>();
			SRGBLinearTestPlatform(platform.get(), "", "_DX9");
			platform->Terminate();
		}

		{
			auto platform = std::make_shared<EffectPlatformGL>();
			SRGBLinearTestPlatform(platform.get(), "", "_GL");
			platform->Terminate();
		}
	}

#elif defined(__APPLE__)

	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		SRGBLinearTestPlatform(platform.get(), "", "_Metal");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		SRGBLinearTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		SRGBLinearTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
#endif
#endif
}

void LODsTest()
{
	{
		srand(0);
#ifdef _WIN32
		auto platform = std::make_shared<EffectPlatformDX11>();
#else
		auto platform = std::make_shared<EffectPlatformGL>();
#endif
		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/LODs/SimpleLODs.efkefc").c_str());

		auto setDistanceToEffect = [platform, h](double distance) {
			auto cameraFrontDirection = platform->GetRenderer()->GetCameraFrontDirection();
			auto cameraPosition = platform->GetRenderer()->GetCameraPosition();
			platform->GetManager()->SetLocation(h, cameraPosition - cameraFrontDirection * distance);
		};

		platform->Update();

		setDistanceToEffect(1.0); // Level 0 starts from distance 0
		platform->Update();
		platform->Update();
		platform->TakeScreenshot("LOD_Level_0.png");
		assert(platform->GetManager()->GetCurrentLOD(h) == 0b0001);

		setDistanceToEffect(6.0); // Level 1 starts from distance 4
		platform->Update();
		platform->Update();
		platform->TakeScreenshot("LOD_Level_1.png");
		assert(platform->GetManager()->GetCurrentLOD(h) == 0b0010);

		setDistanceToEffect(10.0); // Level 2 starts from distance 8
		platform->Update();
		platform->Update();
		platform->TakeScreenshot("LOD_Level_2.png");
		assert(platform->GetManager()->GetCurrentLOD(h) == 0b0100);

		setDistanceToEffect(14.0); // Level 3 starts from distance 12
		platform->Update();
		platform->Update();
		platform->TakeScreenshot("LOD_Level_3.png");
		assert(platform->GetManager()->GetCurrentLOD(h) == 0b1000);

		platform->Terminate();
	}
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Runtime_StringAndPathHelperTest("Runtime.StringAndPathHelperTest", []() -> void { StringAndPathHelperTest(); });

TestRegister Runtime_CustomAllocatorTest("Runtime.CustomAllocatorTest", []() -> void { CustomAllocatorTest(); });

TestRegister Runtime_InstanceDisposeTest("Runtime.InstanceDisposeTest", []() -> void { InstanceDisposeTest(); });

TestRegister Runtime_ReloadTest("Runtime.ReloadTest", []() -> void { ReloadTest(); });

TestRegister Runtime_UpdateToMoveTest("Runtime.UpdateToMoveTest", []() -> void { UpdateToMoveTest(); });

TestRegister Runtime_MassPlayTest("Runtime.MassPlayTest", []() -> void { MassPlayTest(); });

TestRegister Runtime_PlaybackSpeedTest("Runtime.PlaybackSpeedTest", []() -> void { PlaybackSpeedTest(); });

TestRegister Runtime_PlaybackRandomSeedTest("Runtime.PlaybackRandomSeedTest", []() -> void { PlaybackRandomSeedTest(); });

TestRegister Runtime_StartingFrameTest("Runtime.StartingFrameTest", []() -> void { StartingFrameTest(); });

TestRegister Runtime_UpdateHandleTest("Runtime.UpdateHandleTest", []() -> void { UpdateHandleTest(); });

TestRegister Runtime_DX11DefferedContextTest("Runtime.DX11DefferedContextTest", []() -> void { DX11DefferedContextTest(); });

TestRegister Runtime_BasicRuntimeDeviceLostTest("Runtime.BasicRuntimeDeviceLostTest", []() -> void { BasicRuntimeDeviceLostTest(); });

TestRegister Runtime_ProceduralModelCacheTest("Runtime.ProceduralModelCacheTest", []() -> void { ProceduralModelCacheTest(); });

TestRegister Runtime_CullingTest("Runtime.CullingTest", []() -> void { CullingTest(); });

TestRegister Runtime_RenderLimitTest("Runtime.RenderLimitTest", []() -> void { RenderLimitTest(); });

TestRegister Runtime_SRGBLinearTest("Runtime.SRGBLinearTest", []() -> void { SRGBLinearTest(); });

TestRegister Runtime_LODsTest("Runtime.LODsTest", []() -> void { LODsTest(); });
#endif