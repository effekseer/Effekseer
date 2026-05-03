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

#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include "../../EffekseerRendererWebGPU/EffekseerRendererWebGPU/EffekseerMaterialCompilerWebGPU.h"
#include <Runtime/EffectPlatformWebGPU.h>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#endif

#include "../../Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../TestHelper.h"
#include <filesystem>
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

#ifdef __EFFEKSEER_BUILD_WEBGPU__
	{
		auto platform = std::make_shared<EffectPlatformWebGPU>();
		BasicRuntimeTestPlatform(param, platform.get(), "", "_WebGPU");
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

#ifdef __EFFEKSEER_BUILD_WEBGPU__
void BasicRuntimeTestWebGPU()
{
	EffectPlatformInitializingParameter param;

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	BasicRuntimeTestPlatform(param, platform.get(), "", "_WebGPU");
	platform->Terminate();
}

void WebGPUInitializeTest()
{
	EffectPlatformInitializingParameter param;

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);
	platform->Terminate();
}

void WebGPUUpdateNoEffectTest()
{
	EffectPlatformInitializingParameter param;

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);
	platform->Update();
	platform->Terminate();
}

void WebGPUPlaySimpleTest()
{
	EffectPlatformInitializingParameter param;

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);
	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
	platform->Terminate();
}

void WebGPUUpdateSimpleTest()
{
	EffectPlatformInitializingParameter param;

	auto platform = std::make_shared<EffectPlatformWebGPU>();
	platform->Initialize(param);
	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
	platform->Update();
	platform->Terminate();
}

void WebGPUModelColorTest()
{
	EffectPlatformInitializingParameter param;

	auto run = [&](EffectPlatform* platform, const char* suffix) -> void {
		srand(0);
		platform->Initialize(param);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/14/Model_Parameters1.efk").c_str());
		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string("Model_Parameters1") + suffix + ".png").c_str());
		platform->Terminate();
	};

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		run(platform.get(), "_DX11_ColorCheck");
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformWebGPU>();
		run(platform.get(), "_WebGPU_ColorCheck");
	}
}

void WebGPUScreenshotSmokeTest()
{
	EffectPlatformInitializingParameter param;

	auto run = [&](EffectPlatform* platform, const char* suffix) -> void {
		platform->Initialize(param);

		auto singleTest = [&](const char16_t* directory, const char16_t* name, const char16_t* ext, const char* savename) -> void {
			srand(0);
			platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/" + directory + u"/" + name + ext).c_str());
			for (size_t i = 0; i < 30; i++)
			{
				platform->Update();
			}
			platform->TakeScreenshot((std::string(savename) + suffix + ".png").c_str());
			platform->StopAllEffects();
		};

		singleTest(u"10", u"SimpleLaser", u".efk", "Smoke_SimpleLaser");
		singleTest(u"10", u"Ribbon_Parameters1", u".efk", "Smoke_Ribbon_Parameters1");
		singleTest(u"10", u"Ring_Parameters1", u".efk", "Smoke_Ring_Parameters1");
		singleTest(u"10", u"Track_Parameters1", u".efk", "Smoke_Track_Parameters1");
		singleTest(u"10", u"Sprite_Parameters1", u".efk", "Smoke_Sprite_Parameters1");
		singleTest(u"10", u"Distortions1", u".efk", "Smoke_Distortions1");
		singleTest(u"14", u"Model_Parameters1", u".efk", "Smoke_Model_Parameters1");
		singleTest(u"15", u"Lighing_Parameters1", u".efkefc", "Smoke_Lighing_Parameters1");
		singleTest(u"15", u"DynamicParameter1", u".efkefc", "Smoke_DynamicParameter1");
		singleTest(u"15", u"BasicRenderSettings_Blend", u".efkefc", "Smoke_BasicRenderSettings_Blend");
		singleTest(u"15", u"Material_Sampler1", u".efkefc", "Smoke_Material_Sampler1");
		singleTest(u"15", u"Material_UV1", u".efkefc", "Smoke_Material_UV1");
		singleTest(u"15", u"Material_CustomData1", u".efkefc", "Smoke_Material_CustomData1");
		singleTest(u"15", u"Material_CustomDataMax", u".efkefc", "Smoke_Material_CustomDataMax");

		platform->Terminate();
	};

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		run(platform.get(), "_DX11_Smoke");
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformWebGPU>();
		run(platform.get(), "_WebGPU_Smoke");
	}
}

void WebGPUMaterialUVTest()
{
	EffectPlatformInitializingParameter param;

	auto run = [&](EffectPlatform* platform, const char* suffix) -> void {
		platform->Initialize(param);

		auto singleTest = [&](const char16_t* name, const char* savename) -> void {
			srand(0);
			platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/15/" + name + u".efkefc").c_str());
			for (size_t i = 0; i < 30; i++)
			{
				platform->Update();
			}
			platform->TakeScreenshot((std::string(savename) + suffix + ".png").c_str());
			platform->StopAllEffects();
		};

		singleTest(u"Material_UV1", "Material_UV1");
		singleTest(u"Material_UV2", "Material_UV2");

		platform->Terminate();
	};

#ifdef _WIN32
	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		run(platform.get(), "_DX11_UVCheck");
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformWebGPU>();
		run(platform.get(), "_WebGPU_UVCheck");
	}
}

void WebGPUCompiledMaterialTest()
{
	auto validate = [](Effekseer::MaterialCompiler* compiler, const char16_t* name) -> void
	{
		auto srcData = LoadFile((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Materials/" + name + u".efkmat").c_str());
		EXPECT_TRUE(!srcData.empty());

		Effekseer::MaterialFile materialFile;
		EXPECT_TRUE(materialFile.Load(srcData.data(), static_cast<int32_t>(srcData.size())));

		auto binary = Effekseer::RefPtr<Effekseer::CompiledMaterialBinary>(compiler->Compile(&materialFile));
		EXPECT_TRUE(binary != nullptr);
		EXPECT_TRUE(binary->GetVertexShaderSize(Effekseer::MaterialShaderType::Standard) > 0);
		EXPECT_TRUE(binary->GetPixelShaderSize(Effekseer::MaterialShaderType::Standard) > 0);
		EXPECT_TRUE(binary->GetVertexShaderSize(Effekseer::MaterialShaderType::Model) > 0);
		EXPECT_TRUE(binary->GetPixelShaderSize(Effekseer::MaterialShaderType::Model) > 0);

		auto copyShader = [&binary](Effekseer::MaterialShaderType type, bool isVertex) -> std::vector<uint8_t>
		{
			const auto size = isVertex ? binary->GetVertexShaderSize(type) : binary->GetPixelShaderSize(type);
			const auto data = isVertex ? binary->GetVertexShaderData(type) : binary->GetPixelShaderData(type);
			if (size <= 0)
			{
				return {};
			}
			return std::vector<uint8_t>(data, data + size);
		};

		Effekseer::CompiledMaterial compiled;
		compiled.UpdateData(
			copyShader(Effekseer::MaterialShaderType::Standard, true),
			copyShader(Effekseer::MaterialShaderType::Standard, false),
			copyShader(Effekseer::MaterialShaderType::Model, true),
			copyShader(Effekseer::MaterialShaderType::Model, false),
			copyShader(Effekseer::MaterialShaderType::Refraction, true),
			copyShader(Effekseer::MaterialShaderType::Refraction, false),
			copyShader(Effekseer::MaterialShaderType::RefractionModel, true),
			copyShader(Effekseer::MaterialShaderType::RefractionModel, false),
			Effekseer::CompiledMaterialPlatformType::WebGPU);

		std::vector<uint8_t> saved;
		compiled.Save(saved, materialFile.GetGUID(), srcData);
		EXPECT_TRUE(!saved.empty());

		Effekseer::CompiledMaterial loaded;
		EXPECT_TRUE(loaded.Load(saved.data(), static_cast<int32_t>(saved.size())));
		EXPECT_TRUE(loaded.GetHasValue(Effekseer::CompiledMaterialPlatformType::WebGPU));

		auto loadedBinary = loaded.GetBinary(Effekseer::CompiledMaterialPlatformType::WebGPU);
		EXPECT_TRUE(loadedBinary != nullptr);
		EXPECT_TRUE(loadedBinary->GetVertexShaderSize(Effekseer::MaterialShaderType::Standard) ==
					binary->GetVertexShaderSize(Effekseer::MaterialShaderType::Standard));
		EXPECT_TRUE(loadedBinary->GetPixelShaderSize(Effekseer::MaterialShaderType::Model) ==
					binary->GetPixelShaderSize(Effekseer::MaterialShaderType::Model));
	};

	auto validateAll = [&validate](Effekseer::MaterialCompiler* compiler) -> void
	{
		validate(compiler, u"Sampler1");
		validate(compiler, u"UV1");
		validate(compiler, u"UV2");
	};

	Effekseer::MaterialCompilerWebGPU compiler;
	validateAll(&compiler);

#ifdef _WIN32
#ifdef _DEBUG
	const auto dllName = L"EffekseerMaterialCompilerWebGPU.Debug.dll";
#else
	const auto dllName = L"EffekseerMaterialCompilerWebGPU.dll";
#endif
	const auto dllPath = std::filesystem::path(GetDirectoryPathAsU16(__FILE__)) / L"../../../../Dev/release/tools" / dllName;
	if (std::filesystem::exists(dllPath))
	{
		auto library = ::LoadLibraryW(dllPath.c_str());
		EXPECT_TRUE(library != nullptr);

		using CreateCompilerFunc = Effekseer::MaterialCompiler*(EFK_STDCALL*)();
		auto createCompiler = reinterpret_cast<CreateCompilerFunc>(::GetProcAddress(library, "CreateCompiler"));
		EXPECT_TRUE(createCompiler != nullptr);

		{
			auto dllCompiler = Effekseer::RefPtr<Effekseer::MaterialCompiler>(createCompiler());
			EXPECT_TRUE(dllCompiler != nullptr);
			validateAll(dllCompiler.Get());
		}

		::FreeLibrary(library);
	}
#endif
}
#endif

TestRegister Runtime_BasicRuntimeTest("Runtime.BasicRuntimeTest", []() -> void
									  { BasicRuntimeTest(); });

#ifdef __EFFEKSEER_BUILD_WEBGPU__
TestRegister Runtime_BasicRuntimeTestWebGPU("Runtime.BasicRuntimeTestWebGPU", []() -> void
											{ BasicRuntimeTestWebGPU(); });
TestRegister Runtime_WebGPUInitializeTest("Runtime.WebGPUInitializeTest", []() -> void
										  { WebGPUInitializeTest(); });
TestRegister Runtime_WebGPUUpdateNoEffectTest("Runtime.WebGPUUpdateNoEffectTest", []() -> void
											  { WebGPUUpdateNoEffectTest(); });
TestRegister Runtime_WebGPUPlaySimpleTest("Runtime.WebGPUPlaySimpleTest", []() -> void
										  { WebGPUPlaySimpleTest(); });
TestRegister Runtime_WebGPUUpdateSimpleTest("Runtime.WebGPUUpdateSimpleTest", []() -> void
											{ WebGPUUpdateSimpleTest(); });
TestRegister Runtime_WebGPUModelColorTest("Runtime.WebGPUModelColorTest", []() -> void
										  { WebGPUModelColorTest(); });
TestRegister Runtime_WebGPUScreenshotSmokeTest("Runtime.WebGPUScreenshotSmokeTest", []() -> void
											  { WebGPUScreenshotSmokeTest(); });
TestRegister Runtime_WebGPUMaterialUVTest("Runtime.WebGPUMaterialUVTest", []() -> void
										  { WebGPUMaterialUVTest(); });
TestRegister Runtime_WebGPUCompiledMaterialTest("Runtime.WebGPUCompiledMaterialTest", []() -> void
												{ WebGPUCompiledMaterialTest(); });
#endif
