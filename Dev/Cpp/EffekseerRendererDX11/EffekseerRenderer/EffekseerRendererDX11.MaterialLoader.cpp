﻿#include "EffekseerRendererDX11.MaterialLoader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"
#include <d3dcompiler.h>
#include <iostream>

#include "../EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

#undef min

namespace EffekseerRendererDX11
{

MaterialLoader::MaterialLoader(const RendererImplementedRef& renderer, ::Effekseer::FileInterface* fileInterface)
	: renderer_(renderer)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
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
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(binaryPath.c_str()));

		if (reader.get() != nullptr)
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
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(path));

		if (reader.get() != nullptr)
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
			// Pos(3) Color(1) UV(2)
			D3D11_INPUT_ELEMENT_DESC decl[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			shader = Shader::Create(renderer_.Get(),
									renderer_->GetGraphicsDevice()->CreateShaderFromBinary(
										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									"MaterialStandardRenderer",
									decl,
									ARRAYSIZE(decl),
									true);
		}
		else
		{
			// Pos(3) Color(1) Normal(1) Tangent(1) UV(2) UV(2)
			D3D11_INPUT_ELEMENT_DESC decl[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 2, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			int32_t offset = 40;
			int count = 6;
			int index = 2;

			auto getFormat = [](int32_t i) -> DXGI_FORMAT {
				if (i == 1)
					return DXGI_FORMAT_R32_FLOAT;
				if (i == 2)
					return DXGI_FORMAT_R32G32_FLOAT;
				if (i == 3)
					return DXGI_FORMAT_R32G32B32_FLOAT;
				if (i == 4)
					return DXGI_FORMAT_R32G32B32A32_FLOAT;

				assert(0);
				return DXGI_FORMAT_R32_FLOAT;
			};
			if (materialFile.GetCustomData1Count() > 0)
			{
				decl[count].Format = getFormat(materialFile.GetCustomData1Count());
				decl[count].AlignedByteOffset = offset;
				decl[count].SemanticIndex = index;
				index++;
				count++;
				offset += sizeof(float) * materialFile.GetCustomData1Count();
			}

			if (materialFile.GetCustomData2Count() > 0)
			{
				decl[count].Format = getFormat(materialFile.GetCustomData2Count());
				decl[count].AlignedByteOffset = offset;
				decl[count].SemanticIndex = index;
				index++;
				count++;

				offset += sizeof(float) * materialFile.GetCustomData2Count();
			}

			shader = Shader::Create(renderer_.Get(),
									renderer_->GetGraphicsDevice()->CreateShaderFromBinary(
										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									"MaterialStandardRenderer",
									decl,
									count,
									true);
		}

		if (shader == nullptr)
			return false;

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

		D3D11_INPUT_ELEMENT_DESC decl[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		// compile
		std::string log;

		auto shader = Shader::Create(renderer_.Get(),
									 renderer_->GetGraphicsDevice()->CreateShaderFromBinary(
										 (uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]),
										 binary->GetVertexShaderSize(shaderTypesModel[st]),
										 (uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]),
										 binary->GetPixelShaderSize(shaderTypesModel[st])),
									 "MaterialStandardModelRenderer",
									 decl,
									 ARRAYSIZE(decl),
									 true);
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