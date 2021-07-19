#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include "EffectPlatformDX12.h"
#endif

#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#include "EffectPlatformGL.h"
#elif defined(__APPLE__)
#include "EffectPlatformGL.h"
#include "EffectPlatformMetal.h"
#else
#include "EffectPlatformGL.h"
#endif

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include "EffectPlatformVulkan.h"
#endif

#include "../Effekseer/Effekseer/Effekseer.Base.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"
#include "../TestHelper.h"
#include <iostream>

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

	single10Test(u"SimpleLaser", "SimpleLaser");
	single10Test(u"FCurve_Parameters1", "FCurve_Parameters1");
	single10Test(u"Ribbon_Parameters1", "Ribbon_Parameters1");
	single10Test(u"Ring_Parameters1", "Ring_Parameters1");
	single10Test(u"Track_Parameters1", "Track_Parameters1");
	single10Test(u"Sprite_Parameters1", "Sprite_Parameters1");
	single10Test(u"Distortions1", "Distortions1");

	single14Test(u"Model_Parameters1", "Model_Parameters1");
	single15Test(u"Lighing_Parameters1", "Lighing_Parameters1");
	single15Test(u"DynamicParameter1", "DynamicParameter1");
	single15Test(u"Material_Sampler1", "Material_Sampler1");
	single15Test(u"Material_Refraction", "Material_Refraction");
	single15Test(u"Material_WorldPositionOffset", "Material_WorldPositionOffset");
	single15Test(u"BasicRenderSettings_Blend", "BasicRenderSettings_Blend");
	single15Test(u"BasicRenderSettings_Inherit_Color", "BasicRenderSettings_Inherit_Color");
	single15Test(u"ForceFieldLocal_Turbulence1", "ForceFieldLocal_Turbulence1");
	single15Test(u"ForceFieldLocal_Old", "ForceFieldLocal_Old");
	single15Test(u"Material_FresnelRotatorPolarCoords", "Material_FresnelRotatorPolarCoords");

	single15Test(u"Update_Easing", "Update_Easing");
	single15Test(u"Update_MultiModel", "Update_MultiModel");

	single15Test(u"Material_UV1", "Material_UV1");
	single15Test(u"Material_UV2", "Material_UV2");

	{
		single16Test(u"AlphaBlendTexture01", "AlphaBlendTexture01");
		single16Test(u"AlphaCutoffEdgeColor01", "AlphaCutoffEdgeColor01");
		single16Test(u"BasicRenderSettings_Emissive", "BasicRenderSettings_Emissive");
		single16Test(u"Curve01", "Curve01");
		single16Test(u"EdgeFallOff01", "EdgeFallOff01");
		single16Test(u"Flip01", "Flip01");
		single16Test(u"ForceFieldLocal02", "ForceFieldLocal02");
		single16Test(u"Material_EffectScale", "Material_EffectScale");
		single16Test(u"TGA01", "TGA01");
		single16Test(u"ProcedualModel01", "ProcedualModel01");
		single16Test(u"ProcedualModel02", "ProcedualModel02");
		single16Test(u"ProcedualModel03", "ProcedualModel03");
	}

	{
		auto cameraMat = platform->GetRenderer()->GetCameraMatrix();

		Effekseer::Matrix44 mat;
		mat.LookAtRH({0, 0, 10}, {0, 0, 0}, {0, 1, 0});
		platform->GetRenderer()->SetCameraMatrix(mat);
		platform->GenerateDepth();

		single16Test(u"SoftParticle01", "SoftParticle01_NotFlipped");

		platform->GetRenderer()->SetCameraMatrix(cameraMat);
	}

	{
		auto cameraMat = platform->GetRenderer()->GetCameraMatrix();

		Effekseer::Matrix44 mat;
		mat.LookAtRH({0, 0, 10}, {0, 0, 0}, {0, 1, 0});
		platform->GetRenderer()->SetCameraMatrix(mat);
		platform->GetRenderer()->SetBackgroundTextureUVStyle(EffekseerRenderer::UVStyle::VerticalFlipped);
		platform->GenerateDepth();

		single16Test(u"SoftParticle01", "SoftParticle01_Flipped");

		platform->GetRenderer()->SetCameraMatrix(cameraMat);
	}
}

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

void BasicRuntimeTest()
{

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		BasicRuntimeTestPlatform(platform.get(), "", "_Vulkan");
		platform->Terminate();
	}
#endif

#ifdef _WIN32
	{

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
			auto platform = std::make_shared<EffectPlatformGL>();
			BasicRuntimeTestPlatform(platform.get(), "", "_GL");
			platform->Terminate();
		}
	}

#elif defined(__APPLE__)

	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		BasicRuntimeTestPlatform(platform.get(), "", "_Metal");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
#endif
#endif
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

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Runtime_StringAndPathHelperTest("Runtime.StringAndPathHelperTest", []() -> void { StringAndPathHelperTest(); });

TestRegister Runtime_CustomAllocatorTest("Runtime.CustomAllocatorTest", []() -> void { CustomAllocatorTest(); });

TestRegister Runtime_InstanceDisposeTest("Runtime.InstanceDisposeTest", []() -> void { InstanceDisposeTest(); });

TestRegister Runtime_ReloadTest("Runtime.ReloadTest", []() -> void { ReloadTest(); });

TestRegister Runtime_UpdateToMoveTest("Runtime.UpdateToMoveTest", []() -> void { UpdateToMoveTest(); });

TestRegister Runtime_MassPlayTest("Runtime.MassPlayTest", []() -> void { MassPlayTest(); });

TestRegister Runtime_PlaybackSpeedTest("Runtime.PlaybackSpeedTest", []() -> void { PlaybackSpeedTest(); });

TestRegister Runtime_StartingFrameTest("Runtime.StartingFrameTest", []() -> void { StartingFrameTest(); });

TestRegister Runtime_UpdateHandleTest("Runtime.UpdateHandleTest", []() -> void { UpdateHandleTest(); });

TestRegister Runtime_BasicRuntimeTest("Runtime.BasicRuntimeTest", []() -> void { BasicRuntimeTest(); });

TestRegister Runtime_DX11DefferedContextTest("Runtime.DX11DefferedContextTest", []() -> void { DX11DefferedContextTest(); });

TestRegister Runtime_BasicRuntimeDeviceLostTest("Runtime.BasicRuntimeDeviceLostTest", []() -> void { BasicRuntimeDeviceLostTest(); });

TestRegister Runtime_ProceduralModelCacheTest("Runtime.ProceduralModelCacheTest", []() -> void { ProceduralModelCacheTest(); });

TestRegister Runtime_CullingTest("Runtime.CullingTest", []() -> void { CullingTest(); });

#endif