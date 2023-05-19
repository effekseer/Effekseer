#include "EffekseerRendererDX11.MaterialLoader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"
#include <d3dcompiler.h>
#include <iostream>

#include "../EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

#undef min

namespace EffekseerRendererDX11
{

MaterialLoader::MaterialLoader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface)
	: graphicsDevice_(graphicsDevice)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

MaterialLoader ::~MaterialLoader()
{
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

			return Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);
		}
	}

	// code file
	{
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			return Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Code);
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
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, 40);

		if (material->IsSimpleVertex)
		{
			auto vl = EffekseerRenderer::GetMaterialSimpleVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

			/*
			const Effekseer::Backend::VertexLayoutElement vlElem[3] = {
				{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
			};

			auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 3).DownCast<Backend::VertexLayout>();
			*/

			shader = Shader::Create(graphicsDevice_,
									graphicsDevice_->CreateShaderFromBinary(
										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									vl,
									"MaterialStandardRenderer");
		}
		else
		{
			/*
			Effekseer::Backend::VertexLayoutElement vlElem[8] = {
				{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atNormal", "NORMAL", 1},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atTangent", "NORMAL", 2},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord2", "TEXCOORD", 1},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 2},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 3},
			};

			auto getFormat = [](int32_t i) -> Effekseer::Backend::VertexLayoutFormat
			{
				if (i == 1)
					return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
				if (i == 2)
					return Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
				if (i == 3)
					return Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
				if (i == 4)
					return Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT;

				assert(0);
				return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
			};

			int32_t offset = 40;
			int count = 6;
			int semanticIndex = 2;
			const char* customData1Name = "atCustomData1";
			const char* customData2Name = "atCustomData2";

			if (materialFile.GetCustomData1Count() > 0)
			{
				vlElem[count].Name = customData1Name;
				vlElem[count].Format = getFormat(materialFile.GetCustomData1Count());
				vlElem[count].SemanticIndex = semanticIndex;
				semanticIndex++;

				count++;
				offset += sizeof(float) * materialFile.GetCustomData1Count();
			}

			if (materialFile.GetCustomData2Count() > 0)
			{
				vlElem[count].Name = customData2Name;
				vlElem[count].Format = getFormat(materialFile.GetCustomData2Count());
				vlElem[count].SemanticIndex = semanticIndex;
				semanticIndex++;

				count++;
				offset += sizeof(float) * materialFile.GetCustomData2Count();
			}

			auto vl = graphicsDevice_->CreateVertexLayout(vlElem, count).DownCast<Backend::VertexLayout>();
			*/
			auto vl = EffekseerRenderer::GetMaterialSpriteVertexLayout(graphicsDevice_, static_cast<int32_t>(materialFile.GetCustomData1Count()), static_cast<int32_t>(materialFile.GetCustomData2Count())).DownCast<Backend::VertexLayout>();

			shader = Shader::Create(graphicsDevice_,
									graphicsDevice_->CreateShaderFromBinary(
										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									vl,
									"MaterialStandardRenderer");

			if (shader == nullptr)
				return false;
		}

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
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, 40);

		auto vl = EffekseerRenderer::GetMaterialModelVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

		/*
		const Effekseer::Backend::VertexLayoutElement vlElem[6] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Position", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Normal", "NORMAL", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Binormal", "NORMAL", 1},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Tangent", "NORMAL", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_TexCoord", "TEXCOORD", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "a_Color", "NORMAL", 3},
		};

		auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 6).DownCast<Backend::VertexLayout>();
		*/

		// compile
		std::string log;

		auto shader = Shader::Create(graphicsDevice_,
									 graphicsDevice_->CreateShaderFromBinary(
										 (uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]),
										 binary->GetVertexShaderSize(shaderTypesModel[st]),
										 (uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]),
										 binary->GetPixelShaderSize(shaderTypesModel[st])),
									 vl,
									 "MaterialStandardModelRenderer");

		if (shader == nullptr)
			return false;

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

		if (!compiled.GetHasValue(::Effekseer::CompiledMaterialPlatformType::DirectX11))
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
		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::DirectX11);

		return LoadAcutually(materialFile, binary);
	}
	else
	{
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerDX11());
		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&materialFile));

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

} // namespace EffekseerRendererDX11