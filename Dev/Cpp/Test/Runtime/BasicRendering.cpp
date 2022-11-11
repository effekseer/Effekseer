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

#include "../TestHelper.h"
#include <iostream>

void BasicRuntimeTestPlatform(EffectPlatformInitializingParameter param, EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	platform->Initialize(param);
	if (param.CoordinateSyatem == Effekseer::CoordinateSystem::LH)
	{
		suffix += ".LH";
	}

	auto singleTest = [&](std::u16string root, std::u16string name, std::u16string ext, std::string savename, int time)
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/" + root + u"/" + name + ext).c_str());

		for (size_t i = 0; i < time; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto singleResourceData_00_Basic_Test = [&](const char16_t* name, const char* savename, int time) -> void
	{
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../ResourceData/samples/00_Basic/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < time; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	{
		auto cameraMat = platform->GetRenderer()->GetCameraMatrix();

		Effekseer::Matrix44 mat;
		if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
		{
			mat.LookAtRH({0, 20, 20}, {0, 20, 0}, {0, 1, 0});
		}
		else
		{
			mat.LookAtLH({0, 20, -20}, {0, 20, 0}, {0, 1, 0});
		}
		platform->GetRenderer()->SetCameraMatrix(mat);

		singleResourceData_00_Basic_Test(u"Simple_Turbulence_Fireworks", "Simple_Turbulence_Fireworks", 180);

		platform->GetRenderer()->SetCameraMatrix(cameraMat);
	}

	{
		auto single10Test = [&](const char16_t* name, const char* savename) -> void
		{
			singleTest(u"10", name, u".efk", savename, 30);
		};
		single10Test(u"SimpleLaser", "SimpleLaser");
		single10Test(u"FCurve_Parameters1", "FCurve_Parameters1");
		single10Test(u"Ribbon_Parameters1", "Ribbon_Parameters1");
		single10Test(u"Ring_Parameters1", "Ring_Parameters1");
		single10Test(u"Track_Parameters1", "Track_Parameters1");
		single10Test(u"Sprite_Parameters1", "Sprite_Parameters1");
		single10Test(u"Distortions1", "Distortions1");
	}

	{
		auto single14Test = [&](const char16_t* name, const char* savename) -> void
		{
			singleTest(u"14", name, u".efk", savename, 30);
		};
		single14Test(u"Model_Parameters1", "Model_Parameters1");
	}

	{
		auto single15Test = [&](const char16_t* name, const char* savename) -> void
		{
			singleTest(u"15", name, u".efkefc", savename, 30);
		};
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
		single15Test(u"SpawnMethodParameter1", "SpawnMethodParameter1");
	}

	{
		auto single16Test = [&](const char16_t* name, const char* savename) -> void
		{
			singleTest(u"16", name, u".efkefc", savename, 30);
		};

		single16Test(u"DrawWithoutInstancing", "DrawWithoutInstancing");
		single16Test(u"AlphaBlendTexture01", "AlphaBlendTexture01");
		single16Test(u"AlphaCutoffEdgeColor01", "AlphaCutoffEdgeColor01");
		single16Test(u"BasicRenderSettings_Emissive", "BasicRenderSettings_Emissive");
		single16Test(u"Curve01", "Curve01");
		single16Test(u"EdgeFallOff01", "EdgeFallOff01");
		single16Test(u"Flip01", "Flip01");
		single16Test(u"Flip_UV_01", "Flip_UV_01");
		single16Test(u"ForceFieldLocal02", "ForceFieldLocal02");
		single16Test(u"ForceFieldLocal03", "ForceFieldLocal03");
		single16Test(u"Material_EffectScale", "Material_EffectScale");
		single16Test(u"TGA01", "TGA01");
		single16Test(u"ProcedualModel01", "ProcedualModel01");
		single16Test(u"ProcedualModel02", "ProcedualModel02");
		single16Test(u"ProcedualModel03", "ProcedualModel03");
		single16Test(u"AlphaCutoffParameter01", "AlphaCutoffParameter01");
		single16Test(u"RotateScale01", "RotateScale01");
		single16Test(u"FollowParent01", "FollowParent01");

		{
			auto cameraMat = platform->GetRenderer()->GetCameraMatrix();

			Effekseer::Matrix44 mat;
			if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
			{
				mat.LookAtRH({0, 0, 10}, {0, 0, 0}, {0, 1, 0});
			}
			else
			{
				mat.LookAtLH({0, 0, -10}, {0, 0, 0}, {0, 1, 0});
			}
			platform->GetRenderer()->SetCameraMatrix(mat);
			platform->GenerateDepth();

			single16Test(u"SoftParticle01", "SoftParticle01_NotFlipped");

			platform->GetRenderer()->SetCameraMatrix(cameraMat);
		}

		{
			auto cameraMat = platform->GetRenderer()->GetCameraMatrix();

			Effekseer::Matrix44 mat;

			if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
			{
				mat.LookAtRH({0, 0, 10}, {0, 0, 0}, {0, 1, 0});
			}
			else
			{
				mat.LookAtLH({0, 0, -10}, {0, 0, 0}, {0, 1, 0});
			}
			platform->GetRenderer()->SetCameraMatrix(mat);
			platform->GetRenderer()->SetBackgroundTextureUVStyle(EffekseerRenderer::UVStyle::VerticalFlipped);
			platform->GenerateDepth();

			single16Test(u"SoftParticle01", "SoftParticle01_Flipped");

			platform->GetRenderer()->SetCameraMatrix(cameraMat);
		}
	}

	{
		auto single17Test = [&](const char16_t* name, const char* savename, int32_t time) -> void
		{
			singleTest(u"17", name, u".efkefc", savename, time);
		};

		single17Test(u"Flip_UV_02", "Flip_UV_02", 57);
		single17Test(u"Gradient1", "Gradient1", 60);
		single17Test(u"KillRules01", "KillRules01", 60);
		single17Test(u"Light1", "Light1", 60);
		single17Test(u"LocalTime", "LocalTime", 60);
		single17Test(u"Noise1", "Noise1", 60);
	}
}

void BasicRuntimeTest()
{
	EffectPlatformInitializingParameter param;
	// param.CoordinateSyatem = Effekseer::CoordinateSystem::LH;

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_DX11");
		platform->Terminate();
	}
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		auto platform = std::make_shared<EffectPlatformVulkan>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_Vulkan");
		platform->Terminate();
	}
#endif

#ifdef _WIN32
	{

#ifdef __EFFEKSEER_BUILD_DX12__
		{
			auto platform = std::make_shared<EffectPlatformDX12>();
			BasicRuntimeTestPlatform(param, platform.get(), "", "_DX12");
			platform->Terminate();
		}
#endif

		{
			auto platform = std::make_shared<EffectPlatformDX9>();
			BasicRuntimeTestPlatform(param, platform.get(), "", "_DX9");
			platform->Terminate();
		}

		{
			auto platform = std::make_shared<EffectPlatformGL>();
			BasicRuntimeTestPlatform(param, platform.get(), "", "_GL");
			platform->Terminate();
		}
	}

#elif defined(__APPLE__)

	{
		auto platform = std::make_shared<EffectPlatformMetal>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_Metal");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_GL");
		platform->Terminate();
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
#endif
#endif
}

TestRegister Runtime_BasicRuntimeTest("Runtime.BasicRuntimeTest", []() -> void
									  { BasicRuntimeTest(); });
