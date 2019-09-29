#include "EffekseerRendererDX11.MaterialLoader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"
#include <d3dcompiler.h>
#include <iostream>

#include "../EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

namespace EffekseerRendererDX11
{

MaterialLoader::MaterialLoader(Renderer* renderer, ::Effekseer::FileInterface* fileInterface) : fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}

	renderer_ = renderer;
	ES_SAFE_ADDREF(renderer_);
}

MaterialLoader ::~MaterialLoader() { ES_SAFE_RELEASE(renderer_); }

::Effekseer::MaterialData* MaterialLoader::Load(const EFK_CHAR* path)
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

::Effekseer::MaterialData* MaterialLoader::LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary)
{
	auto materialData = new ::Effekseer::MaterialData();
	materialData->IsSimpleVertex = material.GetIsSimpleVertex();
	materialData->IsRefractionRequired = material.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (material.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		Shader* shader = nullptr;

		if (materialData->IsSimpleVertex)
		{
			// Pos(3) Color(1) UV(2)
			D3D11_INPUT_ELEMENT_DESC decl[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
									binary->GetVertexShaderSize(shaderTypes[st]),
									(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
									binary->GetPixelShaderSize(shaderTypes[st]),
									"MaterialStandardRenderer",
									decl,
									ARRAYSIZE(decl));
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

			};

			shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
									binary->GetVertexShaderSize(shaderTypes[st]),
									(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
									binary->GetPixelShaderSize(shaderTypes[st]),
									"MaterialStandardRenderer",
									decl,
									ARRAYSIZE(decl));
		}

		if (shader == nullptr)
			return false;

		// vs ps fixed
		int32_t vertexUniformSizeFixed = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;
		int32_t pixelUniformSizeFixed = sizeof(float) * 4;

		// vs ps shadermodel (light)
		int32_t vertexUniformSizeModel = 0;
		int32_t pixelUniformSizeModel = 0;

		// vs ps material common (predefined)
		int32_t vertexUniformSizeCommon = sizeof(float) * 4;
		int32_t pixelUniformSizeCommon = sizeof(float) * 4;

		int32_t vertexUniformSize = vertexUniformSizeFixed + vertexUniformSizeCommon + vertexUniformSizeModel;
		int32_t pixelUniformSize = pixelUniformSizeFixed + pixelUniformSizeCommon + pixelUniformSizeModel;

		vertexUniformSize += material.GetUniformCount() * 4 * sizeof(float);
		pixelUniformSize += material.GetUniformCount() * 4 * sizeof(float);

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			pixelUniformSize += sizeof(float) * 16;
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material.GetHasRefraction() && st == 1)
		{
			pixelUniformSize += sizeof(float) * 16;
		}

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetVertexRegisterCount(vertexUniformSize / (sizeof(float) * 4));

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		materialData->TextureCount = material.GetTextureCount();
		materialData->UniformCount = material.GetUniformCount();

		if (st == 0)
		{
			materialData->UserPtr = shader;
		}
		else
		{
			materialData->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		D3D11_INPUT_ELEMENT_DESC decl[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, sizeof(float) * 15, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		// compile
		std::string log;

		auto shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									 (uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]),
									 binary->GetVertexShaderSize(shaderTypesModel[st]),
									 (uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]),
									 binary->GetPixelShaderSize(shaderTypesModel[st]),
									 "MaterialStandardModelRenderer",
									 decl,
									 ARRAYSIZE(decl));
		if (shader == nullptr)
			return false;

		// vs ps fixed
		int32_t vertexUniformSizeFixed =
			sizeof(Effekseer::Matrix44) + (sizeof(Effekseer::Matrix44) + sizeof(float) * 4 + sizeof(float) * 4) * 40 + sizeof(float) * 4;
		int32_t pixelUniformSizeFixed = sizeof(float) * 4;

		// vs ps shadermodel (light)
		int32_t vertexUniformSizeModel = 0;
		int32_t pixelUniformSizeModel = 0;

		// vs ps material common (predefined)
		int32_t vertexUniformSizeCommon = sizeof(float) * 4;
		int32_t pixelUniformSizeCommon = sizeof(float) * 4;

		int32_t vertexUniformSize = vertexUniformSizeFixed + vertexUniformSizeCommon + vertexUniformSizeModel;
		int32_t pixelUniformSize = pixelUniformSizeFixed + pixelUniformSizeCommon + pixelUniformSizeModel;

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			pixelUniformSize += sizeof(float) * 16;
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		vertexUniformSize += material.GetUniformCount() * 4 * sizeof(float);
		pixelUniformSize += material.GetUniformCount() * 4 * sizeof(float);

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetVertexRegisterCount(vertexUniformSize / (sizeof(float) * 4));

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		if (st == 0)
		{
			materialData->ModelUserPtr = shader;
		}
		else
		{
			materialData->RefractionModelUserPtr = shader;
		}
	}

	materialData->TextureCount = material.GetTextureCount();
	materialData->UniformCount = material.GetUniformCount();
	materialData->ShadingModel = material.GetShadingModel();

	for (int32_t i = 0; i < materialData->TextureCount; i++)
	{
		materialData->TextureWrapTypes.at(i) = material.GetTextureWrap(i);
	}

	return materialData;
}

::Effekseer::MaterialData* MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
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
		Effekseer::Material material;
		material.Load((const uint8_t*)compiled.GetOriginalData().data(), compiled.GetOriginalData().size());
		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::DirectX11);

		return LoadAcutually(material, binary);
	}
	else
	{
		Effekseer::Material material;
		material.Load((const uint8_t*)data, size);
		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerDX11());
		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&material));

		return LoadAcutually(material, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialData* data)
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
	ES_SAFE_DELETE(data);
}

} // namespace EffekseerRendererDX11