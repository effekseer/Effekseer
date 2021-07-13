#include "EffekseerRendererLLGI.MaterialLoader.h"
#include "EffekseerRendererLLGI.ModelRenderer.h"
#include "EffekseerRendererLLGI.Shader.h"
#include <iostream>
#include <string>

#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

#undef min

namespace EffekseerRendererLLGI
{

static const int LLGI_InstanceCount = 40;

void MaterialLoader::Deserialize(uint8_t* data, uint32_t datasize, LLGI::CompilerResult& result)
{
	if (datasize < 4)
		return;

	uint32_t count = 0;
	uint32_t offset = 0;

	memcpy(&count, data + offset, sizeof(int32_t));
	offset += sizeof(uint32_t);

	result.Binary.resize(count);

	for (uint32_t i = 0; i < count; i++)
	{
		uint32_t size = 0;
		memcpy(&size, data + offset, sizeof(int32_t));
		offset += sizeof(uint32_t);

		result.Binary[i].resize(size);

		memcpy(result.Binary[i].data(), data + offset, size);
		offset += size;
	}
}

MaterialLoader::MaterialLoader(Backend::GraphicsDevice* graphicsDevice,
							   ::Effekseer::FileInterface* fileInterface,
							   ::Effekseer::CompiledMaterialPlatformType platformType,
							   ::Effekseer::MaterialCompiler* materialCompiler)
	: fileInterface_(fileInterface)
	, platformType_(platformType)
	, materialCompiler_(materialCompiler)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}

	graphicsDevice_ = graphicsDevice;
	ES_SAFE_ADDREF(graphicsDevice_);
	ES_SAFE_ADDREF(materialCompiler_);
}

MaterialLoader ::~MaterialLoader()
{
	ES_SAFE_RELEASE(materialCompiler_);
	ES_SAFE_RELEASE(graphicsDevice_);
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
	// code file
	{
		auto binaryPath = std::u16string(path) + u"d";
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(binaryPath.c_str()));

		if (reader.get() != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);

			if (material != nullptr)
			{
				return material;
			}
		}
	}

	// code file
	if (materialCompiler_ != nullptr)
	{
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(path));

		if (reader.get() != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Code);

			return material;
		}
	}

	return nullptr;
}

::Effekseer::MaterialRef MaterialLoader::LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary)
{
	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
	material->IsRefractionRequired = materialFile.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (materialFile.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		Shader* shader = nullptr;
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, LLGI_InstanceCount);

		if (material->IsSimpleVertex)
		{
			LLGI::CompilerResult resultVS;
			LLGI::CompilerResult resultPS;

			Deserialize((uint8_t*)binary->GetVertexShaderData(shaderTypes[st]), binary->GetVertexShaderSize(shaderTypes[st]), resultVS);
			Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypes[st]), binary->GetPixelShaderSize(shaderTypes[st]), resultPS);

			std::array<LLGI::DataStructure, 4> dataVS;
			std::array<LLGI::DataStructure, 4> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				dataVS[i].Data = resultVS.Binary[i].data();
				dataVS[i].Size = static_cast<int32_t>(resultVS.Binary[i].size());
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				dataPS[i].Data = resultPS.Binary[i].data();
				dataPS[i].Size = static_cast<int32_t>(resultPS.Binary[i].size());
			}

			// Pos(3) Color(1) UV(2)
			std::vector<VertexLayout> layouts;
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "POSITION", 0});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 0});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 0});

			shader = Shader::Create(graphicsDevice_,
									dataVS.data(),
									(int32_t)resultVS.Binary.size(),
									dataPS.data(),
									(int32_t)resultPS.Binary.size(),
									"MaterialStandardRenderer",
									layouts,
									true);
		}
		else
		{
			LLGI::CompilerResult resultVS;
			LLGI::CompilerResult resultPS;

			Deserialize((uint8_t*)binary->GetVertexShaderData(shaderTypes[st]), binary->GetVertexShaderSize(shaderTypes[st]), resultVS);
			Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypes[st]), binary->GetPixelShaderSize(shaderTypes[st]), resultPS);

			std::array<LLGI::DataStructure, 4> dataVS;
			std::array<LLGI::DataStructure, 4> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				dataVS[i].Data = resultVS.Binary[i].data();
				dataVS[i].Size = static_cast<int32_t>(resultVS.Binary[i].size());
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				dataPS[i].Data = resultPS.Binary[i].data();
				dataPS[i].Size = static_cast<int32_t>(resultPS.Binary[i].size());
			}

			// Pos(3) Color(1) Normal(1) Tangent(1) UV(2) UV(2)
			std::vector<VertexLayout> layouts;
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "POSITION", 0});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 0});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 1});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 2});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 0});
			layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 1});

			int32_t offset = 40;
			int count = 6;
			int index = 2;

			auto getFormat = [](int32_t i) -> LLGI::VertexLayoutFormat {
				if (i == 2)
					return LLGI::VertexLayoutFormat::R32G32_FLOAT;
				if (i == 3)
					return LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
				if (i == 4)
					return LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT;

				assert(false);
				return LLGI::VertexLayoutFormat::R32_FLOAT;
			};
			if (materialFile.GetCustomData1Count() > 0)
			{
				layouts.push_back(VertexLayout{getFormat(materialFile.GetCustomData1Count()), "TEXCOORD", index});

				index++;
				count++;
				offset += sizeof(float) * materialFile.GetCustomData1Count();
			}

			if (materialFile.GetCustomData2Count() > 0)
			{
				layouts.push_back(VertexLayout{getFormat(materialFile.GetCustomData2Count()), "TEXCOORD", index});

				index++;
				count++;

				offset += sizeof(float) * materialFile.GetCustomData2Count();
			}

			shader = Shader::Create(graphicsDevice_,
									dataVS.data(),
									(int32_t)resultVS.Binary.size(),
									dataPS.data(),
									(int32_t)resultPS.Binary.size(),
									"MaterialStandardRenderer",
									layouts,
									true);
		}

		if (shader == nullptr)
			return nullptr;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetPixelConstantBufferSize(pixelUniformSize);

		material->TextureCount = materialFile.GetTextureCount();
		material->UniformCount = materialFile.GetUniformCount();

		if (st == 0)
		{
			material->UserPtr = shader;
		}
		else
		{
			material->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		LLGI::CompilerResult resultVS;
		LLGI::CompilerResult resultPS;

		Deserialize(
			(uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]), binary->GetVertexShaderSize(shaderTypesModel[st]), resultVS);
		Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]), binary->GetPixelShaderSize(shaderTypesModel[st]), resultPS);

		std::array<LLGI::DataStructure, 4> dataVS;
		std::array<LLGI::DataStructure, 4> dataPS;

		for (size_t i = 0; i < resultVS.Binary.size(); i++)
		{
			dataVS[i].Data = resultVS.Binary[i].data();
			dataVS[i].Size = static_cast<int32_t>(resultVS.Binary[i].size());
		}

		for (size_t i = 0; i < resultPS.Binary.size(); i++)
		{
			dataPS[i].Data = resultPS.Binary[i].data();
			dataPS[i].Size = static_cast<int32_t>(resultPS.Binary[i].size());
		}

		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, LLGI_InstanceCount);

		std::vector<VertexLayout> layouts;
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "POSITION", 0});
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 0});
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 1});
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32B32_FLOAT, "NORMAL", 2});
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R32G32_FLOAT, "TEXCOORD", 0});
		layouts.push_back(VertexLayout{LLGI::VertexLayoutFormat::R8G8B8A8_UNORM, "NORMAL", 3});

		// compile
		std::string log;

		auto shader = Shader::Create(graphicsDevice_,
									 dataVS.data(),
									 (int32_t)resultVS.Binary.size(),
									 dataPS.data(),
									 (int32_t)resultPS.Binary.size(),
									 "MaterialStandardModelRenderer",
									 layouts,
									 true);
		if (shader == nullptr)
			return nullptr;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetPixelConstantBufferSize(pixelUniformSize);

		if (st == 0)
		{
			material->ModelUserPtr = shader;
		}
		else
		{
			material->RefractionModelUserPtr = shader;
		}
	}

	material->CustomData1 = materialFile.GetCustomData1Count();
	material->CustomData2 = materialFile.GetCustomData2Count();
	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
	material->UniformCount = materialFile.GetUniformCount();
	material->ShadingModel = materialFile.GetShadingModel();

	for (int32_t i = 0; i < material->TextureCount; i++)
	{
		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
	}

	return material;
}

::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
{
	if (fileType == Effekseer::MaterialFileType::Compiled)
	{
		auto compiled = Effekseer::CompiledMaterial();
		if (!compiled.Load(static_cast<const uint8_t*>(data), size))
		{
			return nullptr;
		}

		if (!compiled.GetHasValue(platformType_))
		{
			return nullptr;
		}

		// compiled
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size())))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto binary = compiled.GetBinary(platformType_);

		return LoadAcutually(materialFile, binary);
	}
	else
	{
		if (materialCompiler_ == nullptr)
		{
			return nullptr;
		}

		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto binary = ::Effekseer::CreateUniqueReference(materialCompiler_->Compile(&materialFile));

		return LoadAcutually(materialFile, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialRef data)
{
	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(refractionShader);
	ES_SAFE_DELETE(refractionModelShader);

	data->UserPtr = nullptr;
	data->ModelUserPtr = nullptr;
	data->RefractionUserPtr = nullptr;
	data->RefractionModelUserPtr = nullptr;
}

} // namespace EffekseerRendererLLGI
