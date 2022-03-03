#include "EffekseerMaterialCompilerDX11.h"
#include "../DirectX/ShaderGenerator.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")

#undef min

namespace Effekseer
{
namespace DX11
{

static ID3DBlob* CompileVertexShader(const char* vertexShaderText,
									 const char* vertexShaderFileName,
									 const std::vector<D3D_SHADER_MACRO>& macro,
									 std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : nullptr,
					nullptr,
					"main",
					"vs_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != nullptr)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		ES_SAFE_RELEASE(shader);
		return nullptr;
	}

	return shader;
}

static ID3DBlob* CompilePixelShader(const char* vertexShaderText,
									const char* vertexShaderFileName,
									const std::vector<D3D_SHADER_MACRO>& macro,
									std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : nullptr,
					nullptr,
					"main",
					"ps_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != nullptr)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		ES_SAFE_RELEASE(shader);
		return nullptr;
	}

	return shader;
}

#define _DIRECTX11 1
#include "../HLSL/HLSL.h"

} // namespace DX11

} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryDX11 : public CompiledMaterialBinary, public ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryDX11()
	{
	}

	virtual ~CompiledMaterialBinaryDX11()
	{
	}

	void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		vertexShaders_.at(static_cast<int>(type)) = data;
	}

	void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		pixelShaders_.at(static_cast<int>(type)) = data;
	}

	const uint8_t* GetVertexShaderData(MaterialShaderType type) const override
	{
		return vertexShaders_.at(static_cast<int>(type)).data();
	}

	int32_t GetVertexShaderSize(MaterialShaderType type) const override
	{
		return static_cast<int32_t>(vertexShaders_.at(static_cast<int>(type)).size());
	}

	const uint8_t* GetPixelShaderData(MaterialShaderType type) const override
	{
		return pixelShaders_.at(static_cast<int>(type)).data();
	}

	int32_t GetPixelShaderSize(MaterialShaderType type) const override
	{
		return static_cast<int32_t>(pixelShaders_.at(static_cast<int>(type)).size());
	}

	int AddRef() override
	{
		return ReferenceObject::AddRef();
	}

	int Release() override
	{
		return ReferenceObject::Release();
	}

	int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
};

CompiledMaterialBinary* MaterialCompilerDX11::Compile(MaterialFile* materialFile, int32_t maximumTextureCount)
{
	auto binary = new CompiledMaterialBinaryDX11();

	auto convertToVectorVS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto blob = DX11::CompileVertexShader(str.c_str(), "VS", macros, log);

		if (blob != nullptr)
		{
			ret.resize(blob->GetBufferSize());
			memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
			blob->Release();
		}
		else
		{
			std::cout << "VertexShader Compile Error" << std::endl;
			std::cout << log << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto convertToVectorPS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto blob = DX11::CompilePixelShader(str.c_str(), "PS", macros, log);

		if (blob != nullptr)
		{
			ret.resize(blob->GetBufferSize());
			memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
			blob->Release();
		}
		else
		{
			std::cout << "PixelShader Compile Error" << std::endl;
			std::cout << log << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto saveBinary = [&materialFile, &binary, &convertToVectorVS, &convertToVectorPS, &maximumTextureCount](MaterialShaderType type) -> bool {
		auto generator = DirectX::ShaderGenerator(DX11::material_common_define,
												  DX11::material_common_vs_functions,
												  DX11::material_sprite_vs_pre,
												  DX11::material_sprite_vs_pre_simple,
												  DX11::model_vs_pre,
												  DX11::material_sprite_vs_suf1,
												  DX11::material_sprite_vs_suf1_simple,
												  DX11::model_vs_suf1,
												  DX11::material_sprite_vs_suf2,
												  DX11::model_vs_suf2,
												  DX11::g_material_ps_pre,
												  DX11::g_material_ps_suf1,
												  DX11::g_material_ps_suf2_lit,
												  DX11::g_material_ps_suf2_unlit,
												  DX11::g_material_ps_suf2_refraction,
												  DirectX::ShaderGeneratorTarget::DirectX11);

		auto shader = generator.GenerateShader(materialFile, type, maximumTextureCount, 0, 40);

		auto vsBuffer = convertToVectorVS(shader.CodeVS);
		auto psBuffer = convertToVectorPS(shader.CodePS);
		if (vsBuffer.size() == 0 || psBuffer.size() == 0)
		{
			return false;
		}

		binary->SetVertexShaderData(type, vsBuffer);
		binary->SetPixelShaderData(type, psBuffer);
		return true;
	};

	if (materialFile->GetHasRefraction())
	{
		if (!saveBinary(MaterialShaderType::Refraction) ||
			!saveBinary(MaterialShaderType::RefractionModel))
		{
			return nullptr;
		}
	}

	if (!saveBinary(MaterialShaderType::Standard) ||
		!saveBinary(MaterialShaderType::Model))
	{
		return nullptr;
	}

	return binary;
}

CompiledMaterialBinary* MaterialCompilerDX11::Compile(MaterialFile* materialFile)
{
	return Compile(materialFile, Effekseer::UserTextureSlotMax);
}

} // namespace Effekseer

#ifdef __SHARED_OBJECT__

extern "C"
{

#ifdef _WIN32
#define EFK_EXPORT __declspec(dllexport)
#else
#define EFK_EXPORT
#endif

	EFK_EXPORT Effekseer::MaterialCompiler* EFK_STDCALL CreateCompiler()
	{
		return new Effekseer::MaterialCompilerDX11();
	}
}
#endif