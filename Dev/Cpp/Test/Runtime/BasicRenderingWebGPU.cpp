#ifdef __EFFEKSEER_BUILD_WEBGPU__

#include "../../Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../../EffekseerRendererWebGPU/EffekseerRendererWebGPU/EffekseerMaterialCompilerWebGPU.h"
#include "BasicRendering.h"
#include "../TestHelper.h"
#include <Runtime/EffectPlatformWebGPU.h>

#ifdef _WIN32
#include <Runtime/EffectPlatformDX11.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <filesystem>

namespace
{

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

} // namespace

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
