#include "CompiledMaterialGenerator.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <fstream>
#include <filesystem>

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
	names[Effekseer::CompiledMaterialPlatformType::WebGPU] = "WebGPU";
	names[Effekseer::CompiledMaterialPlatformType::Switch] = "Switch";
	names[Effekseer::CompiledMaterialPlatformType::PS4] = "PS4";
	names[Effekseer::CompiledMaterialPlatformType::PS5] = "PS5";
	names[Effekseer::CompiledMaterialPlatformType::Switch2] = "Switch2";

	for (auto& name : names)
	{
		auto path = std::filesystem::path(directory);

#ifdef _WIN32
		auto filename = std::string("EffekseerMaterialCompiler");
#elif defined(__APPLE__)
		auto filename = std::string("libEffekseerMaterialCompiler");
#else
		auto filename = std::string("libEffekseerMaterialCompiler");
#endif

		filename += name.second;

#ifdef _WIN32
#ifdef _DEBUG
		filename += ".Debug.dll";
#else
		filename += ".dll";
#endif
#elif defined(__APPLE__)
		filename += ".dylib";
#else
		filename += ".so";
#endif
		path /= filename;

		auto dll = std::make_shared<DynamicLinkLibrary>();
		if (dll->Load(path.string().c_str()))
		{
			dlls_[name.first] = dll;
		}
	}

	return true;
}

bool CompiledMaterialGenerator::Compile(const char* dstPath, const char* srcPath)
{
	auto load = [](const char* path) -> std::vector<uint8_t>
	{
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
		if (createCompiler == nullptr)
		{
			return false;
		}

		auto compiler = Effekseer::RefPtr<Effekseer::MaterialCompiler>(createCompiler());
		if (compiler == nullptr)
		{
			return false;
		}

		std::vector<uint8_t> vsStandardBinary;
		std::vector<uint8_t> psStandardBinary;
		std::vector<uint8_t> vsModelBinary;
		std::vector<uint8_t> psModelBinary;
		std::vector<uint8_t> vsRefractionStandardBinary;
		std::vector<uint8_t> psRefractionStandardBinary;
		std::vector<uint8_t> vsRefractionModelBinary;
		std::vector<uint8_t> psRefractionModelBinary;

		auto binary = Effekseer::RefPtr<Effekseer::CompiledMaterialBinary>(compiler->Compile(&materialFile));
		if (binary == nullptr)
		{
			return false;
		}

		auto store = [&binary](Effekseer::MaterialShaderType type, std::vector<uint8_t>& vs, std::vector<uint8_t>& ps) -> void
		{
			vs.resize(binary->GetVertexShaderSize(type));
			if (!vs.empty())
			{
				memcpy(vs.data(), binary->GetVertexShaderData(type), vs.size());
			}

			ps.resize(binary->GetPixelShaderSize(type));
			if (!ps.empty())
			{
				memcpy(ps.data(), binary->GetPixelShaderData(type), ps.size());
			}
		};

		store(Effekseer::MaterialShaderType::Standard, vsStandardBinary, psStandardBinary);
		store(Effekseer::MaterialShaderType::Model, vsModelBinary, psModelBinary);
		store(Effekseer::MaterialShaderType::Refraction, vsRefractionStandardBinary, psRefractionStandardBinary);
		store(Effekseer::MaterialShaderType::RefractionModel, vsRefractionModelBinary, psRefractionModelBinary);

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
