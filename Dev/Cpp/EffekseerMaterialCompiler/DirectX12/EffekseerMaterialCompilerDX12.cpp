#include "EffekseerMaterialCompilerDX12.h"
#include <iostream>

#include "../3rdParty/LLGI/src/DX12/LLGI.CompilerDX12.h"

#undef min

#pragma comment(lib, "d3dcompiler.lib")

namespace Effekseer
{

void Serialize(std::vector<uint8_t>& dst, const LLGI::CompilerResult& result) { 
	
	uint32_t binarySize = 0;

	binarySize += sizeof(uint32_t);

	for (size_t i = 0; i < result.Binary.size(); i++)
	{
		binarySize += sizeof(uint32_t);
		binarySize += result.Binary[i].size();
	}

	dst.resize(binarySize);

	uint32_t offset = 0;
	uint32_t count = result.Binary.size(); 
	
	memcpy(dst.data() + offset, &count, sizeof(int32_t));
	offset += sizeof(int32_t);

	for (size_t i = 0; i < result.Binary.size(); i++)
	{
		uint32_t size = result.Binary[i].size();

		memcpy(dst.data() + offset, &size, sizeof(int32_t));
		offset += sizeof(int32_t);

		memcpy(dst.data() + offset, result.Binary[i].data(), result.Binary[i].size());
		offset += result.Binary[i].size();
	}
}

namespace DX12
{

#define _DIRECTX12 1
#include "../HLSL/HLSL.h"

std::string Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

std::string GetType(int32_t i)
{
	if (i == 1)
		return "float";
	if (i == 2)
		return "float2";
	if (i == 3)
		return "float3";
	if (i == 4)
		return "float4";
	if (i == 16)
		return "float4x4";
	assert(0);
	return "";
}

std::string GetElement(int32_t i)
{
	if (i == 1)
		return ".x";
	if (i == 2)
		return ".xy";
	if (i == 3)
		return ".xyz";
	if (i == 4)
		return ".xyzw";
	assert(0);
	return "";
}

void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name, int32_t registerId)
{
	maincode << GetType(type) << " " << name << " : register(c" << registerId << ");" << std::endl;
}

void ExportTexture(std::ostringstream& maincode, const char* name, int32_t registerId)
{
	maincode << "Texture2D " << name << "_texture : register(t" << registerId << ");" << std::endl;
	maincode << "SamplerState " << name << "_sampler : register(s" << registerId << ");" << std::endl;
}

int32_t ExportHeader(std::ostringstream& maincode, Material* material, int stage, bool isSprite, int instanceCount)
{
	auto cind = 0;

	maincode << material_common_define;

	if (stage == 0)
	{
		if (isSprite)
		{
			if (material->GetIsSimpleVertex())
			{
				maincode << material_sprite_vs_pre_simple;
			}
			else
			{
				maincode << material_sprite_vs_pre;
			}
			cind = 10;
		}
		else
		{
			maincode << model_vs_pre;
			cind = 6 + instanceCount * 6;
		}
	}
	else
	{
		maincode << g_material_ps_pre;
		cind = 2;
	}

	return cind;
}

void ExportMain(
	std::ostringstream& maincode, Material* material, int stage, bool isSprite, MaterialShaderType shaderType, const std::string& baseCode)
{
	if (stage == 0)
	{
		if (isSprite)
		{
			if (material->GetIsSimpleVertex())
			{
				maincode << material_sprite_vs_suf1_simple;
			}
			else
			{
				maincode << material_sprite_vs_suf1;
			}
		}
		else
		{
			maincode << model_vs_suf1;
		}

		if (material->GetCustomData1Count() > 0)
		{
			if (isSprite)
			{
				maincode << GetType(material->GetCustomData1Count()) + " customData1 = Input.CustomData1;\n";
			}
			else
			{
				maincode << GetType(material->GetCustomData1Count()) + " customData1 = customData1_[Input.Index.x];\n";
			}
			maincode << "Output.CustomData1 = customData1" + GetElement(material->GetCustomData1Count()) + ";\n";
		}

		if (material->GetCustomData2Count() > 0)
		{
			if (isSprite)
			{
				maincode << GetType(material->GetCustomData2Count()) + " customData2 = Input.CustomData2;\n";
			}
			else
			{
				maincode << GetType(material->GetCustomData2Count()) + " customData2 = customData2_[Input.Index.x];\n";
			}
			maincode << "Output.CustomData2 = customData2" + GetElement(material->GetCustomData2Count()) + ";\n";
		}
	}
	else
	{
		maincode << g_material_ps_suf1;

		if (material->GetCustomData1Count() > 0)
		{
			maincode << GetType(material->GetCustomData1Count()) + " customData1 = Input.CustomData1;\n";
		}

		if (material->GetCustomData2Count() > 0)
		{
			maincode << GetType(material->GetCustomData2Count()) + " customData2 = Input.CustomData2;\n";
		}
	}

	maincode << baseCode;

	if (stage == 0)
	{
		if (isSprite)
		{
			maincode << material_sprite_vs_suf2;
		}
		else
		{
			maincode << model_vs_suf2;
		}
	}
	else
	{
		if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
		{
			maincode << g_material_ps_suf2_refraction;
		}
		else
		{
			if (material->GetShadingModel() == Effekseer::ShadingModelType::Lit)
			{
				maincode << g_material_ps_suf2_lit;
			}
			else if (material->GetShadingModel() == Effekseer::ShadingModelType::Unlit)
			{
				maincode << g_material_ps_suf2_unlit;
			}
			else
			{
				assert(0);
			}
		}
	}
}

ShaderData
GenerateShader(Material* material, MaterialShaderType shaderType, int32_t maximumTextureCount, int32_t pixelShaderTextureSlotOffset, int32_t instanceCount)
{
	ShaderData shaderData;

	bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
	bool isRefrection =
		material->GetHasRefraction() && (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

	for (int stage = 0; stage < 2; stage++)
	{
		std::ostringstream maincode;

		auto cind = ExportHeader(maincode, material, stage, isSprite, instanceCount);

		if (stage == 1)
		{
			ExportUniform(maincode, 4, "mUVInversedBack", 0);
			ExportUniform(maincode, 4, "predefined_uniform", 1);
		}

		if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Lit && stage == 1)
		{
			ExportUniform(maincode, 4, "cameraPosition", cind);
			cind++;
			ExportUniform(maincode, 4, "lightDirection", cind);
			cind++;
			ExportUniform(maincode, 4, "lightColor", cind);
			cind++;
			ExportUniform(maincode, 4, "lightAmbientColor", cind);
			cind++;

			maincode << "#define _MATERIAL_LIT_ 1" << std::endl;
		}
		else if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (isRefrection && stage == 1)
		{
			ExportUniform(maincode, 16, "cameraMat", cind);
			cind += 4;
		}

		if (!isSprite && stage == 0)
		{
			if (material->GetCustomData1Count() > 0)
			{
				maincode << "float4 customData1_[" << instanceCount << "]"
						 << " : register(c" << cind << ");" << std::endl;
				cind += instanceCount;
			}
			if (material->GetCustomData2Count() > 0)
			{
				maincode << "float4 customData2_[" << instanceCount << "]"
						 << " : register(c" << cind << ");" << std::endl;
				cind += instanceCount;
			}
		}

		for (size_t i = 0; i < material->GetUniformCount(); i++)
		{
			ExportUniform(maincode, 4, material->GetUniformName(i), cind);
			cind++;
		}

		// finish constant buffer
		maincode << "};" << std::endl;

		int32_t textureSlotOffset = 0;

		if (stage == 1)
		{
			textureSlotOffset = pixelShaderTextureSlotOffset;
		}

		int32_t actualTextureCount = std::min(maximumTextureCount, material->GetTextureCount());

		for (size_t i = 0; i < actualTextureCount; i++)
		{
			ExportTexture(maincode, material->GetTextureName(i), i + textureSlotOffset);
		}

		// background
		for (size_t i = material->GetTextureCount(); i < material->GetTextureCount() + 1; i++)
		{
			ExportTexture(maincode, "background", i + textureSlotOffset);
		}

		auto baseCode = std::string(material->GetGenericCode());
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "float2");
		baseCode = Replace(baseCode, "$F3$", "float3");
		baseCode = Replace(baseCode, "$F4$", "float4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$UV$", "uv");
		baseCode = Replace(baseCode, "$MOD", "fmod");

		// replace textures
		for (size_t i = 0; i < actualTextureCount; i++)
		{
			std::string keyP = "$TEX_P" + std::to_string(material->GetTextureIndex(i)) + "$";
			std::string keyS = "$TEX_S" + std::to_string(material->GetTextureIndex(i)) + "$";

			if (stage == 0)
			{
				baseCode = Replace(baseCode,
								   keyP,
								   std::string(material->GetTextureName(i)) + "_texture.SampleLevel(" + material->GetTextureName(i) +
									   "_sampler,GetUV(");
				baseCode = Replace(baseCode, keyS, "),0)");
			}
			else
			{
				baseCode = Replace(baseCode,
								   keyP,
								   std::string(material->GetTextureName(i)) + "_texture.Sample(" + material->GetTextureName(i) +
									   "_sampler,GetUV(");
				baseCode = Replace(baseCode, keyS, "))");
			}
		}

		// invalid texture
		for (size_t i = actualTextureCount; i < material->GetTextureCount(); i++)
		{
			auto textureIndex = material->GetTextureIndex(i);
			auto textureName = std::string(material->GetTextureName(i));

			std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
			std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

			baseCode = Replace(baseCode, keyP, "float4(");
			baseCode = Replace(baseCode, keyS, ",0.0,1.0)");
		}

		if (stage == 0)
		{
			maincode << material_common_vs_functions;
		}

		ExportMain(maincode, material, stage, isSprite, shaderType, baseCode);

		if (stage == 0)
		{
			shaderData.CodeVS = maincode.str();
		}
		else
		{
			shaderData.CodePS = maincode.str();
		}
	}

	// custom data
	int32_t inputSlot = 2;
	int32_t outputSlot = 7;
	if (material->GetCustomData1Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS,
						"//$C_IN1$",
						GetType(material->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(inputSlot) + ";");
		}

		shaderData.CodeVS =
			Replace(shaderData.CodeVS,
					"//$C_OUT1$",
					GetType(material->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(outputSlot) + ";");
		shaderData.CodePS =
			Replace(shaderData.CodePS,
					"//$C_PIN1$",
					GetType(material->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(outputSlot) + ";");

		inputSlot++;
		outputSlot++;
	}

	if (material->GetCustomData2Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS,
						"//$C_IN2$",
						GetType(material->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(inputSlot) + ";");
		}
		shaderData.CodeVS =
			Replace(shaderData.CodeVS,
					"//$C_OUT2$",
					GetType(material->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(outputSlot) + ";");
		shaderData.CodePS =
			Replace(shaderData.CodePS,
					"//$C_PIN2$",
					GetType(material->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(outputSlot) + ";");
	}

	return shaderData;
}

} // namespace DX12

} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryDX12 : public CompiledMaterialBinary, ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryDX12() {}

	virtual ~CompiledMaterialBinaryDX12() {}

	void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		vertexShaders_.at(static_cast<int>(type)) = data;
	}

	void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data) { pixelShaders_.at(static_cast<int>(type)) = data; }

	const uint8_t* GetVertexShaderData(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetVertexShaderSize(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).size(); }

	const uint8_t* GetPixelShaderData(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetPixelShaderSize(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).size(); }

	int AddRef() override { return ReferenceObject::AddRef(); }

	int Release() override { return ReferenceObject::Release(); }

	int GetRef() override { return ReferenceObject::GetRef(); }
};

CompiledMaterialBinary* MaterialCompilerDX12::Compile(Material* material, int32_t maximumTextureCount)
{
	// to use options
	// auto compiler = LLGI::CreateSharedPtr(LLGI::CreateCompiler(LLGI::DeviceType::DirectX12));
	auto compiler = LLGI::CreateSharedPtr(new LLGI::CompilerDX12(LLGI::CompilerDX12Option::ColumnMajor));

	auto binary = new CompiledMaterialBinaryDX12();

	auto convertToVectorVS = [compiler](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		LLGI::CompilerResult result;
		compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Vertex);

		if (result.Binary.size() > 0)
		{
			Serialize(ret, result);
		}
		else
		{
			std::cout << "VertexShader Compile Error" << std::endl;
			std::cout << result.Message << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto convertToVectorPS = [compiler](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		LLGI::CompilerResult result;
		compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Pixel);

		if (result.Binary.size() > 0)
		{
			Serialize(ret, result);
		}
		else
		{
			std::cout << "PixelShader Compile Error" << std::endl;
			std::cout << result.Message << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto saveBinary = [&material, &binary, &convertToVectorVS, &convertToVectorPS, &maximumTextureCount](MaterialShaderType type) {
		auto shader = DX12::GenerateShader(material, type, maximumTextureCount, 8, 1);
		binary->SetVertexShaderData(type, convertToVectorVS(shader.CodeVS));
		binary->SetPixelShaderData(type, convertToVectorPS(shader.CodePS));
	};

	if (material->GetHasRefraction())
	{
		saveBinary(MaterialShaderType::Refraction);
		saveBinary(MaterialShaderType::RefractionModel);
	}

	saveBinary(MaterialShaderType::Standard);
	saveBinary(MaterialShaderType::Model);

	return binary;
}

CompiledMaterialBinary* MaterialCompilerDX12::Compile(Material* material) { return Compile(material, Effekseer::UserTextureSlotMax); }

} // namespace Effekseer

#ifdef __SHARED_OBJECT__

extern "C"
{

#ifdef _WIN32
#define EFK_EXPORT __declspec(dllexport)
#else
#define EFK_EXPORT
#endif

	EFK_EXPORT Effekseer::MaterialCompiler* EFK_STDCALL CreateCompiler() { return new Effekseer::MaterialCompilerDX12(); }
}
#endif