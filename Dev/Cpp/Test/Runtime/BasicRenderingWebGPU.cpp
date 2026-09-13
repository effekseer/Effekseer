#ifdef __EFFEKSEER_BUILD_WEBGPU__

#include "../../Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../../EffekseerRendererWebGPU/EffekseerRendererWebGPU/EffekseerMaterialCompilerWebGPU.h"
#include "../TestHelper.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <filesystem>

namespace
{

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

TestRegister Runtime_WebGPUCompiledMaterialTest("Runtime.WebGPUCompiledMaterialTest", []() -> void
												{ WebGPUCompiledMaterialTest(); });

#endif
