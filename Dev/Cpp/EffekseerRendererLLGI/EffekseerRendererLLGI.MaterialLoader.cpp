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

MaterialLoader::MaterialLoader(Backend::GraphicsDeviceRef graphicsDevice,
							   ::Effekseer::FileInterfaceRef fileInterface,
							   ::Effekseer::CompiledMaterialPlatformType platformType,
							   ::Effekseer::MaterialCompiler* materialCompiler)
	: fileInterface_(fileInterface)
	, platformType_(platformType)
	, materialCompiler_(materialCompiler)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}

	graphicsDevice_ = graphicsDevice;
	ES_SAFE_ADDREF(materialCompiler_);
}

MaterialLoader ::~MaterialLoader()
{
	ES_SAFE_RELEASE(materialCompiler_);
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
	// code file
	{
		auto binaryPath = std::u16string(path) + u"d";
		auto reader = fileInterface_->OpenRead(binaryPath.c_str());

		if (reader != nullptr)
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
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
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
	if (binary == nullptr)
	{
		return nullptr;
	}

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

			std::vector<LLGI::DataStructure> dataVS;
			std::vector<LLGI::DataStructure> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultVS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
				dataVS.emplace_back(ds);
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultPS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
				dataPS.emplace_back(ds);
			}

			auto vl = EffekseerRenderer::GetMaterialSimpleVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

			auto vs_shader_data = Backend::Serialize(dataVS);
			auto ps_shader_data = Backend::Serialize(dataPS);

			shader = Shader::Create(
				graphicsDevice_,
				graphicsDevice_->CreateShaderFromBinary(
					vs_shader_data.data(),
					(int32_t)vs_shader_data.size(),
					ps_shader_data.data(),
					(int32_t)ps_shader_data.size()),
				vl,
				"MaterialStandardRenderer");
		}
		else
		{
			LLGI::CompilerResult resultVS;
			LLGI::CompilerResult resultPS;

			Deserialize((uint8_t*)binary->GetVertexShaderData(shaderTypes[st]), binary->GetVertexShaderSize(shaderTypes[st]), resultVS);
			Deserialize((uint8_t*)binary->GetPixelShaderData(shaderTypes[st]), binary->GetPixelShaderSize(shaderTypes[st]), resultPS);

			std::vector<LLGI::DataStructure> dataVS;
			std::vector<LLGI::DataStructure> dataPS;

			for (size_t i = 0; i < resultVS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultVS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
				dataVS.emplace_back(ds);
			}

			for (size_t i = 0; i < resultPS.Binary.size(); i++)
			{
				LLGI::DataStructure ds;
				ds.Data = resultPS.Binary[i].data();
				ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
				dataPS.emplace_back(ds);
			}

			auto vl = EffekseerRenderer::GetMaterialSpriteVertexLayout(graphicsDevice_, static_cast<int32_t>(materialFile.GetCustomData1Count()), static_cast<int32_t>(materialFile.GetCustomData2Count())).DownCast<Backend::VertexLayout>();

			auto vs_shader_data = Backend::Serialize(dataVS);
			auto ps_shader_data = Backend::Serialize(dataPS);

			shader = Shader::Create(graphicsDevice_,
									graphicsDevice_->CreateShaderFromBinary(
										vs_shader_data.data(),
										(int32_t)vs_shader_data.size(),
										ps_shader_data.data(),
										(int32_t)ps_shader_data.size()),
									vl,
									"MaterialStandardRenderer");
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

		std::vector<LLGI::DataStructure> dataVS;
		std::vector<LLGI::DataStructure> dataPS;

		for (size_t i = 0; i < resultVS.Binary.size(); i++)
		{
			LLGI::DataStructure ds;
			ds.Data = resultVS.Binary[i].data();
			ds.Size = static_cast<int32_t>(resultVS.Binary[i].size());
			dataVS.emplace_back(ds);
		}

		for (size_t i = 0; i < resultPS.Binary.size(); i++)
		{
			LLGI::DataStructure ds;
			ds.Data = resultPS.Binary[i].data();
			ds.Size = static_cast<int32_t>(resultPS.Binary[i].size());
			dataPS.emplace_back(ds);
		}

		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, LLGI_InstanceCount);

		// compile
		std::string log;
		auto vl = EffekseerRenderer::GetMaterialModelVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

		auto vs_shader_data = Backend::Serialize(dataVS);
		auto ps_shader_data = Backend::Serialize(dataPS);

		auto shader = Shader::Create(graphicsDevice_,
									 graphicsDevice_->CreateShaderFromBinary(
										 vs_shader_data.data(),
										 (int32_t)vs_shader_data.size(),
										 ps_shader_data.data(),
										 (int32_t)ps_shader_data.size()),
									 vl,
									 "MaterialStandardModelRenderer");

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
