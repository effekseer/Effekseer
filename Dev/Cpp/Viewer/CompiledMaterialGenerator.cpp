#include "CompiledMaterialGenerator.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <fstream>

typedef Effekseer::MaterialCompiler*(EFK_STDCALL* CreateCompilerFunc)();

CompiledMaterialGenerator::CompiledMaterialGenerator()
{
}

CompiledMaterialGenerator::~CompiledMaterialGenerator()
{
}

bool CompiledMaterialGenerator::Initialize(const char* directory)
{

	std::map<Effekseer::CompiledMaterialPlatformType, std::string> names;

	names[Effekseer::CompiledMaterialPlatformType::DirectX9] = "DX9";
	names[Effekseer::CompiledMaterialPlatformType::DirectX11] = "DX11";
	names[Effekseer::CompiledMaterialPlatformType::DirectX12] = "DX12";
	names[Effekseer::CompiledMaterialPlatformType::Metal] = "Metal";
	names[Effekseer::CompiledMaterialPlatformType::Vulkan] = "VK";
	names[Effekseer::CompiledMaterialPlatformType::OpenGL] = "GL";
	names[Effekseer::CompiledMaterialPlatformType::Switch] = "Switch";
	names[Effekseer::CompiledMaterialPlatformType::PS4] = "PS4";
	names[Effekseer::CompiledMaterialPlatformType::PS5] = "PS5";

	for (auto& name : names)
	{
		std::string path = directory;

#ifdef _WIN32
		path += "EffekseerMaterialCompiler";
#elif defined(__APPLE__)
		path += "libEffekseerMaterialCompiler";
#else
		path += "libEffekseerMaterialCompiler";
#endif

		path += name.second;

#ifdef _WIN32
#ifdef _DEBUG
		path += ".Debug.dll";
#else
		path += ".dll";
#endif
#elif defined(__APPLE__)
		path += ".dylib";
#else
		path += ".so";
#endif

		auto dll = std::make_shared<DynamicLinkLibrary>();
		if (dll->Load(path.c_str()))
		{
			dlls_[name.first] = dll;
		}
	}

	return true;
}

bool CompiledMaterialGenerator::Compile(const char* dstPath, const char* srcPath)
{
	auto load = [](const char* path) -> std::vector<uint8_t> {
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::vector<uint8_t>();
		}

		file.seekg(0, std::fstream::end);
		auto eofPos = file.tellg();
		file.clear();
		file.seekg(0, std::fstream::beg);
		auto begPos = file.tellg();
		auto size = eofPos - begPos;

		std::vector<uint8_t> data;
		data.resize(size);
		file.read((char*)data.data(), size);
		file.close();

		return data;
	};

	auto srcData = load(srcPath);
	auto dstData = load(dstPath);

	Effekseer::MaterialFile materialFile;
	materialFile.Load(srcData.data(), static_cast<int32_t>(srcData.size()));

	Effekseer::CompiledMaterial cm;

	if (dstData.size() > 0)
	{
		cm.Load(dstData.data(), static_cast<int32_t>(dstData.size()));
	}

	if (cm.GUID != materialFile.GetGUID())
	{
		cm = Effekseer::CompiledMaterial();
	}

	for (auto dll : dlls_)
	{
		if (cm.GetHasValue(dll.first))
		{
			continue;
		}

#if defined(_WIN32) && !defined(_WIN64)
		auto createCompiler = dll.second->GetProc<CreateCompilerFunc>("_CreateCompiler@0");
#else
		auto createCompiler = dll.second->GetProc<CreateCompilerFunc>("CreateCompiler");
#endif
		auto compiler = Effekseer::RefPtr<Effekseer::MaterialCompiler>(createCompiler());

		std::vector<uint8_t> vsStandardBinary;
		std::vector<uint8_t> psStandardBinary;
		std::vector<uint8_t> vsModelBinary;
		std::vector<uint8_t> psModelBinary;
		std::vector<uint8_t> vsRefractionStandardBinary;
		std::vector<uint8_t> psRefractionStandardBinary;
		std::vector<uint8_t> vsRefractionModelBinary;
		std::vector<uint8_t> psRefractionModelBinary;

		auto compile_and_store =
			[&compiler, &materialFile](Effekseer::MaterialShaderType type, std::vector<uint8_t>& vs, std::vector<uint8_t>& ps) -> bool {
			auto binary = Effekseer::RefPtr<Effekseer::CompiledMaterialBinary>(compiler->Compile(&materialFile));

			if (binary != nullptr)
			{
				vs.resize(binary->GetVertexShaderSize(type));
				memcpy(vs.data(), binary->GetVertexShaderData(type), binary->GetVertexShaderSize(type));

				ps.resize(binary->GetPixelShaderSize(type));
				memcpy(ps.data(), binary->GetPixelShaderData(type), binary->GetPixelShaderSize(type));
			}

			return binary != nullptr;
		};

		if (!compile_and_store(Effekseer::MaterialShaderType::Standard, vsStandardBinary, psStandardBinary) ||
			!compile_and_store(Effekseer::MaterialShaderType::Model, vsModelBinary, psModelBinary) ||
			!compile_and_store(Effekseer::MaterialShaderType::Refraction, vsRefractionStandardBinary, psRefractionStandardBinary) ||
			!compile_and_store(Effekseer::MaterialShaderType::RefractionModel, vsRefractionModelBinary, psRefractionModelBinary))
		{
			return false;
		}

		if (vsStandardBinary.size() > 0 && psStandardBinary.size() > 0 && vsModelBinary.size() > 0 && psModelBinary.size() > 0)
		{
			cm.UpdateData(vsStandardBinary,
						  psStandardBinary,
						  vsModelBinary,
						  psModelBinary,
						  vsRefractionStandardBinary,
						  psRefractionStandardBinary,
						  vsRefractionModelBinary,
						  psRefractionModelBinary,
						  dll.first);
		}
	}

	cm.Save(dstData, materialFile.GetGUID(), srcData);

	std::ofstream file(dstPath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (file.bad())
		return false;

	file.write((char*)dstData.data(), dstData.size());

	file.close();

	return true;
}
