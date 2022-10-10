#include "EffekseerMaterialCompilerVulkan.h"
#include <iostream>

#include "../3rdParty/LLGI/src/Vulkan/LLGI.CompilerVulkan.h"

#include "../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.h"

namespace Effekseer
{

static void Serialize(std::vector<uint8_t>& dst, const LLGI::CompilerResult& result)
{

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


} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryVulkan : public CompiledMaterialBinary, ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryVulkan() {}

	virtual ~CompiledMaterialBinaryVulkan() {}

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

CompiledMaterialBinary* MaterialCompilerVulkan::Compile(MaterialFile* material, int32_t maximumUniformCount, int32_t maximumTextureCount)
{
	// to use options
	auto compiler = LLGI::CreateSharedPtr(new LLGI::CompilerVulkan());

	auto binary = new CompiledMaterialBinaryVulkan();

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

	auto saveBinary = [&material, &binary, &convertToVectorVS, &convertToVectorPS, &maximumUniformCount, &maximumTextureCount](MaterialShaderType type) {
		
		GLSL::ShaderGenerator generator;
		auto shader = generator.GenerateShader(material, type, maximumUniformCount, maximumTextureCount, true, true, true, true, 1, true, true, 40);

		//auto shader = Vulkan::GenerateShader(material, type, maximumTextureCount);
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

CompiledMaterialBinary* MaterialCompilerVulkan::Compile(MaterialFile* material)
{
	return Compile(material, Effekseer::UserUniformSlotMax, Effekseer::UserTextureSlotMax);
}

} // namespace Effekseer
